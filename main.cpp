#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>

#include "algebra3.h"
#include "imageIO.h"
#include "Object.h"
#include "Light.h"
#include "Ray.h"
#include "BVH.h"

inline float clamp(const float &lo, const float &hi, const float &v) {
    return std::max(lo, std::min(hi, v)); 
} 

inline float deg2rad(const float &deg) { 
    return deg * M_PI / 180; 
}

struct Material {
    vec3 color;
    double Ka, Kd, Ks;
    double exp;
    double reflect;
};

struct Scene {
    vec3 eye_pos;
    vec3 vd;
    vec3 vu;
    double fov;
    struct {
        double w;
        double h;
    } resolution;

    /* viewport properties */
    const double d = 10;     // viewport distance from origin
    vec3 vv;          // viewport unit vertical vector
    double pixel_w;
    double pixel_h;
    vec3 upper_left;
};

class cmp {
public:
    int sortAxis;
    cmp(int axis) : sortAxis(axis) {};
    bool operator()(const BBox* lhs, const BBox* rhs){
        return (*lhs)[1][sortAxis] < (*rhs)[1][sortAxis];
    }    
};

double vec3SquareDistance(vec3 vec);
void initViewPort(Scene &scene);
void render(
    ColorImage &image,
    const Scene &scene,
    const Options &options, 
    const std::vector<std::shared_ptr<Object>> &objects,
    const std::vector<std::unique_ptr<Light>> &lights,
    const std::vector<BBox*> &boundingVolume);

int main() {
    std::string fname;
    std::string line;

    std::cout << "Please enter input file name: ";
    std::cin >> fname;
    std::ifstream fp(fname);

    ColorImage image;
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    // std::vector<std::shared_ptr<BBox>>  boundingVolumes;
    std::vector<BBox*>  boundingVolumes;
    Scene scene;
    Material material;
    Options options;
    char property;
    BBox wholeBox;

    if (fp.is_open()) {
        while (std::getline(fp, line)) {
            if (line.size() == 0) continue;

            std::istringstream ss(line);
            ss >> property;
            // std::cout<<property<<std::endl;
            switch (property) {
                case 'E': {
                    ss >> scene.eye_pos[0] 
                        >> scene.eye_pos[1]
                        >> scene.eye_pos[2];
                    break;
                }
                case 'V': {
                    ss >> scene.vd[0]
                        >> scene.vd[1]
                        >> scene.vd[2]
                        >> scene.vu[0]
                        >> scene.vu[1]
                        >> scene.vu[2];
                    break;
                }
                case 'F': {
                    ss >> scene.fov;
                    break;
                }
                case 'R': {
                    ss >> scene.resolution.w >> scene.resolution.h;
                    /* initialize viewport properties */
                    initViewPort(scene);
                    /* initialize ppm image */
                    image.init(scene.resolution.w, scene.resolution.h);
                    break;
                }
                case 'M': {
                    float r, g, b;
                    ss >> r >> g >> b
                        >> material.Ka
                        >> material.Kd
                        >> material.Ks
                        >> material.exp
                        >> material.reflect;
                    material.color = {r, g, b};
                    // boundingVolumes.push_back(std::make_unique<BBox>());
                    break;
                }
                case 'S': {
                    vec3 pos;
                    float r;
                    ss >> pos[0] >> pos[1] >> pos[2] >> r;
                    objects.push_back(std::make_shared<Sphere>(
                            material.color,
                            material.Ka,
                            material.Kd,
                            material.Ks,
                            material.exp,
                            material.reflect,
                            pos, r
                    ));
                    break;
                }
                case 'T': {
                    // std::cout<<"triangle!"<<std::endl;
                    std::array<vec3, 3> verts;
                    vec3 normal;
                    for(auto &it : verts){
                        ss >> it[0] >> it[1] >> it[2];
                    }
                    ss >> normal[0] >> normal[1] >> normal[2];
                    // ss >> verts[0][0]
                    //     >> verts[0][1]
                    //     >> verts[0][2]
                    //     >> verts[1][0]
                    //     >> verts[1][1]
                    //     >> verts[1][2]
                    //     >> verts[2][0]
                    //     >> verts[2][1]
                    //     >> verts[2][2]
                    //     >> normal[0]
                    //     >> normal[1]
                    //     >> normal[2];
                    objects.push_back(std::make_shared<Triangle>(
                        material.color,
                        material.Ka,
                        material.Kd,
                        material.Ks,
                        material.exp,
                        material.reflect,
                        verts[0], verts[1], verts[2],
                        normal
                    ));
                    // std::cout<<"++"<<std::endl;
                    boundingVolumes.push_back(new BBox);
                    boundingVolumes.back()->objl = objects.size() - 1;
                    boundingVolumes.back()->objr = objects.size();
                    
                    // boundingVolumes.back()->Objects().push_back(objects.back());
                    // boundingVolumes[boundingVolumes.size() - 1]->Objects().push_back(objects.back());

                    // boundingVolumes[boundingVolumes.size() - 1]->Objects().push_back(std::make_unique<Triangle>(
                    //     material.color,
                    //     material.Ka,
                    //     material.Kd,
                    //     material.Ks,
                    //     material.exp,
                    //     material.reflect,
                    //     verts[0], verts[1], verts[2],
                    //     normal
                    // ));
                    
                    for (int i = 0; i < verts.size(); ++i) {
                        wholeBox.extendBy(verts[i]);
                        boundingVolumes.back()->extendBy(verts[i]);
                        // boundingVolumes[boundingVolumes.size() - 1]->extendBy(verts[i]);
                    }

                    break;
                }
                case 'L': {
                    vec3 light_pos;
                    ss >> light_pos[0] >> light_pos[1] >> light_pos[2];
                    lights.push_back(std::make_unique<Light>(light_pos));
                    break;
                }
                default:
                    break;
            }
        }
    }
    int sortAxis = 0;
    if(wholeBox[1][0] - wholeBox[0][0] < wholeBox[1][1] - wholeBox[0][1]){
        if(wholeBox[1][1] - wholeBox[0][1] < wholeBox[1][2] - wholeBox[1][2]){
            sortAxis = 2;
        }
        else{
            sortAxis = 1;
        }
    }
    else{
        if(wholeBox[1][0] - wholeBox[0][0] < wholeBox[1][2] - wholeBox[1][2]){
            sortAxis = 2;
        }
        else{
            sortAxis = 0;
        }
    }

    sort(boundingVolumes.begin(), boundingVolumes.end(), cmp(sortAxis));
    // std::cout<<"OBJ size: "<<boundingVolumes.size()<<std::endl;
    render(image, scene, options, objects, lights, boundingVolumes);

    image.outputPPM("result.ppm");

    std::cout << "Image rendered: ./result.ppm" << std::endl;

    fp.close();
    return 0;
}

void initViewPort(Scene &scene) {
    scene.vu = (scene.vd ^ scene.vu).normalize();   // viewport unit horizontal vector
    scene.vv = (scene.vu ^ scene.vd).normalize();   // viewport unit vertical vector
    float aspectRatio = scene.resolution.w / scene.resolution.h;
    scene.pixel_w = 2 * std::tan(deg2rad(scene.fov / 2)) * scene.d / scene.resolution.w * aspectRatio;
    scene.pixel_h = 2 * std::tan(deg2rad(scene.fov / 2)) * scene.d / scene.resolution.h;

    scene.upper_left = scene.eye_pos 
        + scene.vd.normalize() * scene.d
        - (scene.resolution.w / 2) * scene.pixel_w * scene.vu
        + (scene.resolution.h / 2) * scene.pixel_h * scene.vv
        + (scene.pixel_w / 2) * scene.vu
        - (scene.pixel_h / 2) * scene.vv;
}

void render(
    ColorImage &image,
    const Scene &scene,
    const Options &options, 
    const std::vector<std::shared_ptr<Object>> &objects,
    const std::vector<std::unique_ptr<Light>> &lights,
    const std::vector<BBox*> &boundingVolumes)
{
    using namespace std;
    BVH bvh(boundingVolumes);
    bvh.build();
    auto timeStart = std::chrono::high_resolution_clock::now();
    for (int y = 0; y < scene.resolution.h; ++y) {
        for (int x = 0; x < scene.resolution.w; ++x) {
            vec3 cur_pos = scene.upper_left - scene.pixel_h * scene.vv * y + scene.pixel_w * scene.vu * x;
            vec3 ray_direction = (cur_pos - scene.eye_pos).normalize();

            // vec3 color = castRay(scene.eye_pos, ray_direction, objects, lights, boundingVolumes, options);
            // cout<<"!"<<endl;
            vec3 color = castRay(scene.eye_pos, ray_direction, objects, lights, bvh, options);
            // cout<<"*"<<endl;
            Pixel p = {
                (unsigned char)(clamp(0, 1, color[0]) * 255),
                (unsigned char)(clamp(0, 1, color[1]) * 255),
                (unsigned char)(clamp(0, 1, color[2]) * 255)
            };
            image.writePixel(x, y, p);

            fprintf(stderr, "\r%3d%c", uint32_t(y / (float)scene.resolution.h * 100), '%');
        }
    }
    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto diff = timeEnd - timeStart;
    auto passTime = std::chrono::duration<double, std::milli>(diff).count();
    fprintf(stderr, "\rDone: %.2f (sec)\n", passTime / 1000);
}
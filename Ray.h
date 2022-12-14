#ifndef RAY_H
#define RAY_H

#include <memory>
#include <cmath>
#include <iostream>

#include "Object.h"
#include "Light.h"
// #include "Utility.h"

static const vec3 kDefaultBackgroundColor = vec3(0);

inline vec3 mix(const vec3 &a, const vec3& b, const float &mixValue) { 
    return a * (1 - mixValue) + b * mixValue; 
} 

struct Options
{
    vec3 background_color = kDefaultBackgroundColor;
    float bias = 0.0001;
    uint32_t max_depth = 5;
};

enum RayType { kPrimaryRay, kShadowRay };

struct IsectInfo
{
    const Object *hitObject = nullptr;
    float tNear = kInfinity;
};

bool trace(
    const vec3 &origin, 
    const vec3 &dir, 
    const std::vector<std::unique_ptr<Object>> &objects,
    IsectInfo &isect,
    RayType ray_type=kPrimaryRay)
{
    isect.hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNear = kInfinity;
        if (objects[k]->intersect(origin, dir, tNear) && tNear < isect.tNear) {
            isect.hitObject = objects[k].get();
            isect.tNear = tNear;
        }
    }

    return (isect.hitObject != nullptr);
}

bool trace(
    const vec3 &origin, 
    const vec3 &dir, 
    const std::vector<std::unique_ptr<Object>> &objects,
    const std::vector<std::unique_ptr<BBox>> &boundingVolumes,
    IsectInfo &isect,
    RayType ray_type=kPrimaryRay)
{
    isect.hitObject = nullptr;
    const vec3 invDir = 1 / dir;
    const std::vector<bool> sign{ dir[0] < 0, dir[1] < 0, dir[2] < 0 };
    float t = kInfinity;

    // if (!boundingVolume.intersect(origin, invDir, sign, t)) {
    //     return false;
    // }

    for (uint32_t i = 0; i < boundingVolumes.size(); ++i) {
        float t = kInfinity;
        if (boundingVolumes[i]->intersect(origin, invDir, sign, t)) {
            for (uint32_t k = 0; k < boundingVolumes[i]->Objects().size(); ++k) {
                float tNear = kInfinity;
                if (boundingVolumes[i]->Objects()[k]->intersect(origin, dir, tNear) && tNear < isect.tNear) {
                    isect.hitObject = boundingVolumes[i]->Objects()[k].get();
                    isect.tNear = tNear;
                }
            }
        }
    }


    return (isect.hitObject != nullptr);
}

vec3 reflect(const vec3 &I, const vec3 &N)
{
    return I - 2 * (I * N) * N;
}

vec3 castRay(
    const vec3 &origin, const vec3 &dir,
    const std::vector<std::unique_ptr<Object>> &objects,
    const std::vector<std::unique_ptr<Light>> &lights,
    const std::vector<std::unique_ptr<BBox>> &boundingVolumes,
    const Options &options,
    const uint32_t &depth=0)
{
    if (depth > options.max_depth) return options.background_color;
    
    vec3 hitColor = 0;
    IsectInfo isect;
    if (trace(origin, dir, objects, boundingVolumes, isect)) {
        vec3 hitPoint = origin + dir * isect.tNear;
        vec3 hitNormal;
        isect.hitObject->getSurfaceProperties(hitPoint, dir, hitNormal);
        
        bool outside = (dir * hitNormal < 0);
        vec3 bias = options.bias * hitNormal;
        vec3 surface_color = isect.hitObject->Color();
        vec3 diffuse = 0, specular = 0;
        for (uint32_t i = 0; i < lights.size(); ++i) {
            vec3 lightDir, lightIntensity;
            IsectInfo isectShadow;
            lights[i]->illuminate(hitPoint, lightDir, lightIntensity, isectShadow.tNear);

            bool visible = !trace(hitPoint + bias, -lightDir, objects, isectShadow, kShadowRay);

            if (isect.hitObject->Reflect() > 0) {
                vec3 reflectionDir = reflect(dir, hitNormal).normalize();
                vec3 reflectionOrigin = outside ? hitPoint + bias : hitPoint - bias;
                vec3 reflectionColor = castRay(reflectionOrigin, reflectionDir, objects, lights, boundingVolumes, options, depth + 1);
                surface_color = mix(surface_color, reflectionColor, isect.hitObject->Reflect());
            }

            // compute the diffuse component
            diffuse += visible * surface_color * std::max(0.f, (-lightDir) * hitNormal);

            // compute the specular light
            vec3 R = reflect(lightDir, hitNormal).normalize();
            specular += visible * std::pow(std::max(0.f, (R * -dir)), isect.hitObject->Exp());
        }

        hitColor += surface_color * isect.hitObject->Ka() + diffuse * isect.hitObject->Kd() + specular * isect.hitObject->Ks();
    }
    else {
        hitColor = options.background_color;
    }

    return hitColor;
}

#endif
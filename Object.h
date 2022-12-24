#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <vector>
#include <memory>

#include "algebra3.h"
#include "Utility.h"

void printVec3(const vec3 &vec);

class Object
{
private:
    vec3 color;
    float ka;
    float kd;
    float ks;
    float exp;
    float reflect;
public:
    Object(
        const vec3 &color, 
        const float &ka, 
        const float &kd, 
        const float &ks, 
        const float &exp, 
        const float &reflect)
    : color(color), ka(ka), kd(kd), ks(ks), exp(exp), reflect(reflect) {}
    virtual ~Object() {}
    virtual bool intersect(const vec3 &origin, const vec3 &dir, float &tNear) const = 0;
    virtual void getSurfaceProperties(const vec3 &hitPoint, const vec3 &viewDir, vec3 &hitNormal) const = 0;
    vec3 Color() const { return color; }
    float Ka() const { return ka; }
    float Kd() const { return kd; }
    float Ks() const { return ks; }
    float Exp() const { return exp; }
    float Reflect() const { return reflect; }
};

bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);

class Sphere : public Object
{
private:
    float radius;
    vec3 center;
public:
    Sphere(const vec3 &color, 
        const float &ka, 
        const float &kd, 
        const float &ks, 
        const float &exp, 
        const float &reflect,
        const vec3 &pos, const float &r) : Object(color, ka, kd, ks, exp, reflect), center(pos), radius(r) {}
    bool intersect(const vec3 &origin, const vec3 &dir, float &tNear) const;
    void getSurfaceProperties(
        const vec3 &hitPoint, 
        const vec3 &viewDir,
        vec3 &hitNormal) const;
};

bool rayTriangleIntersect(
    const vec3 &origin, const vec3 &dir, 
    const vec3 &v0, const vec3 &v1, const vec3 &v2,
    float &t);

class Triangle : public Object
{
private:
    std::vector<vec3> verts;
    vec3 normal;
public:
    Triangle(
        const vec3 &color, 
        const float &ka, 
        const float &kd, 
        const float &ks, 
        const float &exp, 
        const float &reflect,
        const vec3 &v0,
        const vec3 &v1,
        const vec3 &v2,
        const vec3 normal) : Object(color, ka, kd, ks, exp, reflect), normal(normal)
    {
        verts.insert(verts.end(), {v0, v1, v2});
    }
    bool intersect(const vec3 &origin, const vec3 &dir, float &tNear) const;
    void getSurfaceProperties(
        const vec3 &hitPoint,
        const vec3 &viewDir,
        vec3 &hitNormal) const;
    std::vector<vec3> Verts() const { return verts; }
    vec3 Normal() const { return normal; }
};

class BBox
{
private:
    vec3 bounds[2] = { kInfinity, -kInfinity };
    std::vector<std::shared_ptr<Object>> objects;
public:
    BBox() {}
    BBox(vec3 min, vec3 max) : bounds{min, max} {};
    BBox& extendBy(const vec3 &p);
    std::vector<std::shared_ptr<Object>> &Objects() { return objects; }
    bool intersect(const vec3 &origin, const vec3 &invDir, const std::vector<bool> &sign, float &tHit) const;
    const vec3 centroid() const { return (bounds[0] + bounds[1]) * 0.5; }
    vec3 &operator [] (bool i) { return bounds[i]; }
    const vec3 operator [] (bool i) const { return bounds[i]; }
    BBox operator + (const BBox &rhs) const {
        BBox ret;
        ret.extendBy(bounds[0]);
        ret.extendBy(bounds[1]);
        ret.extendBy(rhs[0]);
        ret.extendBy(rhs[1]);
        ret.objects = objects;
        ret.objects.insert(ret.objects.end(), rhs.objects.begin(), rhs.objects.end());
        return ret;
    }
};

#endif
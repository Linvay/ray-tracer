#include <algorithm>
#include <iostream>

#include "Object.h"

void printVec3(const vec3 &vec) {
    vec3 v = vec;
    std::cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
}

bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
    float discr = (b * b) - (4 * a * c);
    if (discr < 0) return false;
    else if (discr == 0) {
        x0 = x1 = -0.5 * b / a;
    }
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }

    return true;
}

bool Sphere::intersect(const vec3 &origin, const vec3 &dir, float &tNear) const
{
    float t0, t1;   // solutions for t if the ray intersects
    // analytic solution
    vec3 L = origin - center;
    float a = dir * dir;
    float b = 2 * (dir * L);
    float c = (L * L) - pow(radius, 2);
    if (!solveQuadratic(a, b, c, t0, t1)) return false;

    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) return false;
    }

    tNear = t0;

    return true;
}

void Sphere::getSurfaceProperties(
    const vec3 &hitPoint, 
    const vec3 &viewDir,
    vec3 &hitNormal) const
{
    hitNormal = (hitPoint - center).normalize();
}

bool rayTriangleIntersect(
    const vec3 &origin, const vec3 &dir, 
    const vec3 &v0, const vec3 &v1, const vec3 &v2,
    float &t)
{
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    vec3 pvec = dir ^ v0v2;
    float det = v0v1 * pvec;

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;

    float invDet = 1 / det;

    vec3 tvec = origin - v0;
    float u = (tvec * pvec) * invDet;
    if (u < 0 || u > 1) return false;

    vec3 qvec = tvec ^ v0v1;
    float v = (dir * qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = (v0v2 * qvec) * invDet;

    return (t > 0) ? true : false;
}

bool Triangle::intersect(const vec3 &origin, const vec3 &dir, float &tNear) const
{
    bool isect = false;
    float t = kInfinity;
    if (rayTriangleIntersect(origin, dir, verts[0], verts[1], verts[2], t) && t < tNear) {
        tNear = t;
        isect = true;
    }

    return isect;
}

void Triangle::getSurfaceProperties(
    const vec3 &hitPoint,
    const vec3 &viewDir,
    vec3 &hitNormal) const
{
    hitNormal = normal;
    hitNormal.normalize();
}

BBox& BBox::extendBy(const vec3 &p)
{
    if (p[0] < bounds[0][0]) bounds[0][0] = p[0];
    if (p[1] < bounds[0][1]) bounds[0][1] = p[1];
    if (p[2] < bounds[0][2]) bounds[0][2] = p[2];
    if (p[0] > bounds[1][0]) bounds[1][0] = p[0];
    if (p[1] > bounds[1][1]) bounds[1][1] = p[1];
    if (p[2] > bounds[1][2]) bounds[1][2] = p[2];

    return *this;
}

bool BBox::intersect(const vec3 &origin, const vec3 &invDir, const std::vector<bool> &sign, float &tHit) const
{
    using namespace std;
    // cout<<"intersect: "<<endl;
    if(bounds == nullptr){
        cout<<"CRASH: intersect bound"<<endl;
        return false;
    }
    // ((vec3)(bounds[0])).print(stdout, "bound 0");
    // ((vec3)(bounds[1])).print(stdout, "bound 1");
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (bounds[sign[0]][0] - origin[0]) * invDir[0];
    tmax = (bounds[1 - sign[0]][0] - origin[0]) * invDir[0];
    tymin = (bounds[sign[1]][1] - origin[1]) * invDir[1];
    tymax = (bounds[1 - sign[1]][1] - origin[1]) * invDir[1];
    if ((tmin > tymax) || (tymin > tymax)) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    tzmin = (bounds[sign[2]][2] - origin[2]) * invDir[2];
    tzmax = (bounds[1 - sign[2]][2] - origin[2]) * invDir[2];

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    tHit = tmin;

    return true;
}
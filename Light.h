#ifndef LIGHT_H
#define LIGHT_H

#include "algebra3.h"

class Light
{
private:
    vec3 pos;
    vec3 color;
    float intensity;
public:
    Light(const vec3 &pos, const vec3 &color=1, const float &intensity=1)
    : pos(pos), color(color), intensity(intensity) {}
    void illuminate(const vec3 &P, vec3 &light_dir, vec3 &light_intensity, float &distance) const
    {
        light_dir = (P - pos).normalize();
        distance = light_dir.length();
        light_intensity = color * intensity;
    }
    vec3 Position() const { return pos; }
    vec3 Color() const { return color; }
    vec3 Intensity() const { return intensity; }
};

#endif
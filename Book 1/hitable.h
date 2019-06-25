#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

class Material;

struct hitRecord
{
    float t;
    vec3 p;
    vec3 normal;
    Material *matPtr;
};

class Hitable
{
public:
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const = 0;
};

#endif
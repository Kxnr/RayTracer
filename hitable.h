#ifndef HITABLEH
#define HITABLEH

#include "ray.h"
#include "path.h"

class Material;

struct hitRecord
{
    float t;
    arma::vec3 p;
    arma::vec3 normal;
    Material *matPtr;
};

class Hitable
{
public:
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const = 0;
};

#endif
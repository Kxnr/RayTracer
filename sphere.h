#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "path.h"

# TODO: resource management

class Sphere : public Hitable
{
public:
    Sphere(){};
    Sphere(Path* _path, float r, Material *mat) : radius(r), material(mat), path(_path){animated = true;};
    Sphere(vec3 origin, float r, Material *mat) : origin0(origin), radius(r), material(mat){animated = false;};
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const;
    virtual bool boundingBox(float t0, float t1, AABB& box) const;


    vec3 origin(float t = 0) const;
    vec3 rotation(float t = 0) const { return vec3(0, 0, 0); }
    vec3 scale(float t = 0) const { return vec3(1, 1, 1); }

private:
    bool animated;
    Path *path;
    vec3 origin0;
    float radius;
    Material *material;
};

#endif
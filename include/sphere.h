#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "path.h"

class Sphere : public Hitable
{
public:
    Sphere(Path* _path, float r, Material *mat) : radius(r), material(mat), path(_path){animated = true;};
    Sphere(arma::vec3 cen, float r, Material *mat) : center0(cen), radius(r), material(mat){animated = false;};
    ~Sphere(){};

    bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const override;
    bool boundingBox(float t0, float t1, AABB& box) const override;
    arma::vec3 center(float t = 0) const;

    bool animated;
    Path *path;
    arma::vec3 center0;
    float radius;
    Material *material;
};
#endif

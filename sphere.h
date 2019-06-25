#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "path.h"

class Sphere : public Hitable
{
public:
    Sphere(){};
    Sphere(arma::vec3 cen, float r, Material *mat, Path *_path) : center0(cen), radius(r), material(mat), path(_path){animated = true;};
    Sphere(arma::vec3 cen, float r, Material *mat) : center0(cen), radius(r), material(mat){animated = false;};
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const;

    bool animated;
    Path *path;
    arma::vec3 center0;
    arma::vec3 center(float t = 0) const;
    float radius;
    Material *material;
};

arma::vec3 Sphere::center(float t) const {
    if(!animated) {
        return center0;
    }

    else {
        return path->position(t);
    }
}

bool Sphere::hit(const ray &r, float tmin, float tmax, hitRecord &rec) const
{
    // tmin and tmax describe thresholds for deciding if a hit has occurred
    // factor of 2 in b and discriminant cancels with denominator
    arma::vec3 oc = r.origin() - center(r.time());
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;

    if (discriminant > 0)
    {
        float temp = (-b - sqrt(discriminant)) / a;
        if (temp < tmax && temp > tmin)
        {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.matPtr = material;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < tmax && temp > tmin)
        {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.matPtr = material;
            return true;
        }
    }
    return false;
}
#endif
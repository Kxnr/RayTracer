#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "path.h"

class Sphere : public Hitable
{
public:
    Sphere(){};
    Sphere(Path* _path, float r, Material *mat) : radius(r), material(mat), path(_path){animated = true;};
    Sphere(arma::vec3 cen, float r, Material *mat) : center0(cen), radius(r), material(mat){animated = false;};
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const;
    virtual bool boundingBox(float t0, float t1, aabb& box) const;

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

bool Sphere::boundingBox(float t0, float t1, aabb& box) const {
    // TODO: can't handle splines
    box = aabb::boundingBox(aabb(center(t0) - arma::vec3({radius, radius, radius}),
                                center(t1) + arma::vec3({radius, radius, radius})),
                            aabb(center(t0) - arma::vec3({radius, radius, radius}),
                                center(t1) + arma::vec3({radius, radius, radius})));

    return true;
}

#endif
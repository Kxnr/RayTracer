#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class Sphere : public Hitable
{
public:
    Sphere(){};
    Sphere(vec3 cen, float r, Material *mat) : center(cen), radius(r), material(mat){};
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const;
    vec3 center;
    float radius;
    Material *material;
};

bool Sphere::hit(const ray &r, float tmin, float tmax, hitRecord &rec) const
{
    // factor of 2 in b and discriminant cancels with denominator
    vec3 oc = r.origin() - center;
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
            rec.normal = (rec.p - center) / radius;
            rec.matPtr = material;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < tmax && temp > tmin)
        {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.matPtr = material;
            return true;
        }
    }
    return false;
}
#endif
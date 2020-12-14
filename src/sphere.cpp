#include "sphere.h"

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

    if (discriminant > 0) {
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

bool Sphere::boundingBox(float t0, float t1, AABB& box) const {
    // TODO: can't handle splines
    box = AABB::boundingBox(AABB(center(t0) - arma::vec3({radius, radius, radius}),
                                center(t1) + arma::vec3({radius, radius, radius})),
                            AABB(center(t0) - arma::vec3({radius, radius, radius}),
                                center(t1) + arma::vec3({radius, radius, radius})));

    return true;
}

#include "sphere.h"

vec3 Sphere::origin(float t) const
{
    if (!animated)
    {
        return origin0;
    }

    else
    {
        return path->position(t);
    }
}

bool Sphere::hit(const ray &r, float tmin, float tmax, hitRecord &rec) const
{
    // tmin and tmax describe thresholds for deciding if a hit has occurred
    // factor of 2 in b and discriminant cancels with denominator
    vec3 oc = r.origin() - origin(r.time());
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
            rec.normal = (rec.p - origin(r.time())) / radius;
            rec.matPtr = material;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < tmax && temp > tmin)
        {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - origin(r.time())) / radius;
            rec.matPtr = material;
            return true;
        }
    }
    return false;
}

bool Sphere::boundingBox(float t0, float t1, AABB &box) const
{
    // TODO: can't handle splines, can use spline derivatives to find extrema
    if (animated)
    {
        box = AABB::boundingBox(AABB(origin(t0) - vec3(radius, radius, radius),
                                     origin(t1) + vec3(radius, radius, radius)),
                                AABB(origin(t0) - vec3(radius, radius, radius),
                                     origin(t1) + vec3(radius, radius, radius)));
    }
    else
    {
        box = AABB(origin() - vec3(radius, radius, radius), origin() + vec3(radius, radius, radius));
    }

    return true;
}
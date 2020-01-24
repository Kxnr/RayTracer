#ifndef MATERIALH
#define MATERIALH
#include "hitable.h"
#include "ray.h"
#include <iostream>

float schlick(float cosine, float IOR);

struct materialProperties
{
    // these vectors are colors
    vec3 reflectance = vec3(1.0, 1.0, 1.0);
    vec3 absorption = vec3(0.0, 0.0, 0.0);

    float transparency = 0;
    float roughness = 0;
    float IOR = 1.0;
};

vec3 randomInUnitSphere()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3((float) drand48(), (float) drand48(), (float) drand48()) - vec3(1, 1, 1);
    } while (p.squaredLength() >= 1.0);
    return p;
}

class Material
{
public:
    Material(){};
    Material(const materialProperties props) { matProps = props; };
    Material(const vec3 ref, const vec3 absorp, const float transp, const float rough, const float ior)
    {
        matProps.absorption = absorp;
        matProps.transparency = transp;
        matProps.reflectance = ref;
        matProps.roughness = rough;
        matProps.IOR = ior;
    };
    bool scatter(const ray &rin, const hitRecord &rec, vec3 &attenuation, ray &scattered) const;
    vec3 reflect(const vec3 &v, const vec3 &n) const;
    bool refract(const vec3 &v, const vec3 &n, vec3 &refracted) const;

    materialProperties matProps;
};

bool Material::scatter(const ray &rin, const hitRecord &rec, vec3 &attenuation, ray &scattered) const
{

    float nint;
    vec3 reflected = reflect(unitVector(rin.direction()), rec.normal);
    scattered = ray(rec.p, reflected * (1 - matProps.roughness) + (randomInUnitSphere() + rec.normal) * matProps.roughness);
    attenuation = matProps.reflectance;

    float reflectProb;
    vec3 refracted;

    bool ref = refract(rin.direction(), rec.normal, refracted);
    if (((float) drand48() < matProps.transparency) && ref)
    {
        scattered = ray(rec.p, refracted * (1 - matProps.roughness) + (randomInUnitSphere() + rec.normal) * matProps.roughness);
        attenuation = vec3(1, 1, 1) - matProps.absorption;
        return true;
    }
    return (dot(scattered.direction(), rec.normal) > 0);
}

vec3 Material::reflect(const vec3 &v, const vec3 &n) const
{
    return v - 2 * dot(v, n) * n;
}

bool Material::refract(const vec3 &v, const vec3 &n, vec3 &refracted) const
{
    vec3 outwardNormal;
    float nint;
    float cosine;
    if (dot(v, n) > 0)
    {
        outwardNormal = -n;
        nint = matProps.IOR;
        cosine = nint * dot(v, n) / v.length();
    }
    else
    {
        outwardNormal = n;
        nint = 1.0 / matProps.IOR;
        cosine = -dot(v, n) / v.length();
    }
    vec3 uv = unitVector(v);
    float dt = dot(uv, outwardNormal);
    float discriminant = 1.0 - nint * nint * (1 - dt * dt);
    float reflectProb = schlick(cosine, matProps.IOR);
    if ((discriminant > 0) && ((float) drand48() > reflectProb))
    {
        refracted = nint * (uv - outwardNormal * dt) - outwardNormal * sqrt(discriminant);
        return true;
    }
    return false;
}

//approximation for Fresnel
float schlick(float cosine, float IOR)
{
    float r0 = (1 - IOR) / (1 + IOR);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

#endif
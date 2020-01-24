#include "material.h"

bool Material::scatter(const ray &rin, const hitRecord &rec, vec3 &attenuation, ray &scattered) const
{
    materialRecord matRec = matProps.setValuesAtPosition(rec.p);

    float nint;
    vec3 reflected = reflect(unitVector(rin.direction()), rec.normal);
    scattered = ray(rec.p, reflected * (1 - matRec.roughness) + (randomInUnitSphere() + rec.normal) * matRec.roughness, rin.time());
    attenuation = matRec.reflectance;

    float reflectProb;
    vec3 refracted;

    bool ref = refract(rin.direction(), rec.normal, matRec, refracted);
    if ((drand48() < matRec.transparency) && ref)
    {
        scattered = ray(rec.p, refracted * (1 - matRec.roughness) + (randomInUnitSphere() + rec.normal) * matRec.roughness, rin.time());
        attenuation = vec3(1, 1, 1) - matRec.absorption;
        return true;
    }
    return (dot(scattered.direction(), rec.normal) > 0);
}

vec3 Material::reflect(const vec3 &v, const vec3 &n) const
{
    return v - 2 * dot(v, n) * n;
}

bool Material::refract(const vec3 &v, const vec3 &n, const materialRecord matRec, vec3 &refracted) const
{

    vec3 outwardNormal;
    float nint;
    float cosine;
    if (dot(v, n) > 0)
    {
        outwardNormal = -n;
        nint = matRec.IOR;
        cosine = nint * dot(v, n) / v.length();
    }
    else
    {
        outwardNormal = n;
        nint = 1.0 / matRec.IOR;
        cosine = -dot(v, n) / v.length();
    }
    vec3 uv = v / v.length();
    float dt = dot(uv, outwardNormal);
    float discriminant = 1.0 - nint * nint * (1 - dt * dt);
    float reflectProb = schlick(cosine, matRec.IOR);
    if ((discriminant > 0) && (drand48() > reflectProb))
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

float fresnel() {

}
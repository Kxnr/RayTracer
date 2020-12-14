#include "material.h"

arma::vec3 randomInUnitSphere()
{
    arma::vec3 p;
    do
    {
        p = 2.0 * arma::vec3({drand48(), drand48(), drand48()}) - arma::vec3({1, 1, 1});
    } while (arma::norm(p) >= 1.0);
    return p;
}

//approximation for Fresnel
float schlick(float cosine, float IOR)
{
    float r0 = (1 - IOR) / (1 + IOR);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

bool Material::scatter(const ray &rin, const hitRecord &rec, arma::vec3 &attenuation, ray &scattered) const
{

    float nint;
    arma::vec3 reflected = reflect(arma::normalise(rin.direction()), rec.normal);
    scattered = ray(rec.p, reflected * (1 - matProps.roughness) + (randomInUnitSphere() + rec.normal) * matProps.roughness, rin.time());
    attenuation = matProps.reflectance;

    float reflectProb;
    arma::vec3 refracted;

    bool ref = refract(rin.direction(), rec.normal, refracted);
    if ((drand48() < matProps.transparency) && ref)
    {
        scattered = ray(rec.p, refracted * (1 - matProps.roughness) + (randomInUnitSphere() + rec.normal) * matProps.roughness, rin.time());
        attenuation = arma::vec3({1, 1, 1}) - matProps.absorption;
        return true;
    }
    return (arma::dot(scattered.direction(), rec.normal) > 0);
}

arma::vec3 Material::reflect(const arma::vec3 &v, const arma::vec3 &n) const
{
    return v - 2 * arma::dot(v, n) * n;
}

bool Material::refract(const arma::vec3 &v, const arma::vec3 &n, arma::vec3 &refracted) const
{
    arma::vec3 outwardNormal;
    float nint;
    float cosine;
    if (dot(v, n) > 0)
    {
        outwardNormal = -n;
        nint = matProps.IOR;
        cosine = nint * dot(v, n) / arma::norm(v);
    }
    else
    {
        outwardNormal = n;
        nint = 1.0 / matProps.IOR;
        cosine = -dot(v, n) / arma::norm(v);
    }
    arma::vec3 uv = v / arma::norm(v);
    float dt = arma::dot(uv, outwardNormal);
    float discriminant = 1.0 - nint * nint * (1 - dt * dt);
    float reflectProb = schlick(cosine, matProps.IOR);
    if ((discriminant > 0) && (drand48() > reflectProb))
    {
        refracted = nint * (uv - outwardNormal * dt) - outwardNormal * sqrt(discriminant);
        return true;
    }
    return false;
}

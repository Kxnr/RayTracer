#ifndef MATERIALH
#define MATERIALH
#include "hitable.h"
#include <iostream>

float schlick(float cosine, float IOR);

struct materialProperties
{
    // these vectors are colors
    arma::vec3 reflectance = {1.0f, 1.0f, 1.0f};
    arma::vec3 absorption = {0.0f, 0.0f, 0.0f};

    float transparency = 0;
    float roughness = 0;
    float IOR = 1.0;
};

class Material
{
public:
    Material(){};
    Material(const materialProperties props) { matProps = props; };
    Material(const arma::vec3 ref, const arma::vec3 absorp, const float transp, const float rough, const float ior)
    {
        matProps.absorption = absorp;
        matProps.transparency = transp;
        matProps.reflectance = ref;
        matProps.roughness = rough;
        matProps.IOR = ior;
    };
    bool scatter(const ray &rin, const hitRecord &rec, arma::vec3 &attenuation, ray &scattered) const;
    arma::vec3 reflect(const arma::vec3 &v, const arma::vec3 &n) const;
    bool refract(const arma::vec3 &v, const arma::vec3 &n, arma::vec3 &refracted) const;

    materialProperties matProps;
};
#endif

#ifndef MATERIALH
#define MATERIALH
#include "hitable.h"
#include "basicTextures.h"
#include <iostream>

// TODO: make sure that energy conservation is obeyed

struct materialRecord {
        // color dependent
        vec3 reflectance;
        vec3 absorption;

        // non color dependent
        float transparency;
        float roughness;
        float IOR;
};

// TODO: make coordinate transforms occur here, rather than in texture
// could also fit into the object, since many are object based

class MaterialProperties 
{
    public:
        MaterialProperties() {}

        // TODO: add transforms, make these setters and vec3 settable in addition to constructor
        MaterialProperties(Texture* _ref, Texture* _abs, Texture* _trans, Texture* _rough, Texture* _ior) {
            reflectance = _ref;
            absorption = _abs;
            transparency = _trans;
            roughness = _rough;
            IOR = _ior;
        }

        const Texture* reflectance;
        const Texture* absorption;
        const Texture* transparency;
        const Texture* roughness;
        const Texture* IOR;

        materialRecord setValuesAtPosition(const vec3& p) const {
            materialRecord matRec;

            // color types
            matRec.reflectance = reflectance->value(p);
            matRec.absorption = absorption->value(p);

            // float types
            matRec.transparency = transparency->value(p)[0];
            matRec.roughness = roughness->value(p)[0]; 
            matRec.IOR = IOR->value(p)[0];
            return matRec;
        }

    ~MaterialProperties() {
      delete reflectance;
      delete absorption;
      delete transparency;
      delete roughness;
      delete IOR;
    }
};

vec3 randomInUnitSphere()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (p.length() >= 1.0);
    return p;
}

class Material
{
public:
    Material(){};
    Material(const MaterialProperties props) { matProps = props; };


    // TODO: clarify how these are constructed
    Material(const vec3 ref, const vec3 absorp, const float transp, const float rough, const float ior)
    {
        matProps.absorption = new ConstantTexture(absorp);
        matProps.transparency = new ConstantTexture(transp);
        matProps.reflectance = new ConstantTexture(ref);
        matProps.roughness = new ConstantTexture(rough);
        matProps.IOR = new ConstantTexture(ior);
    };

    Material(const Texture* ref, const vec3 absorp, const float transp, const float rough, const float ior)
    {
        matProps.absorption = new ConstantTexture(absorp);
        matProps.transparency = new ConstantTexture(transp);
        matProps.reflectance = ref;
        matProps.roughness = new ConstantTexture(rough);
        matProps.IOR = new ConstantTexture(ior);
    };

    bool scatter(const ray &rin, const hitRecord &rec, vec3 &attenuation, ray &scattered) const;
    vec3 reflect(const vec3 &v, const vec3 &n) const;
    bool refract(const vec3 &v, const vec3 &n, const materialRecord matRec, vec3 &refracted) const;

    MaterialProperties matProps;
};

#endif

#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

class Camera
{
public:
    Camera(arma::vec3 lookFrom, arma::vec3 lookTo, arma::vec3 vup, float vfov, float aspect, float aperture, float focusDist);
    ray getRay(float s, float t, float time);

    float time0;
    float time1;

    arma::vec3 origin;
    arma::vec3 lowerLeftCorner;
    arma::vec3 horizontal;
    arma::vec3 vertical;

    arma::vec3 u, v, w;
    float lensRadius;
};

#endif

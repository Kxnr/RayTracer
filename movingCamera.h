#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

arma::vec3 randomInUnitDisk()
{
    arma::vec3 p;
    do
    {
        p = 2.0 * arma::vec3({drand48(), drand48(), 0}) - arma::vec3({1, 1, 0});
    } while (dot(p, p) >= 1.0);
    return p;
}
class Camera
{
public:
    Camera(arma::vec3 lookFrom, arma::vec3 lookTo, arma::vec3 vup, float vfov, float aspect, float aperture, float focusDist)
    {
        //TODO: express orientation in a more logical way, ie rotation matrix
        lensRadius = aperture / 2;
        float halfHeight = tan(vfov / 2);
        float halfWidth = aspect * halfHeight;

        origin = lookFrom;

        w = arma::normalise(lookFrom - lookTo);
        u = arma::normalise(arma::cross(vup, w));
        v = arma::cross(w, u);

        lowerLeftCorner = origin - halfWidth * focusDist * u - halfHeight * focusDist * v - focusDist * w;
        horizontal = 2 * halfWidth * focusDist * u;
        vertical = 2 * halfHeight * focusDist * v;
    }

    ray getRay(float s, float t, float time)
    {
        arma::vec3 rd = lensRadius * randomInUnitDisk();
        arma::vec3 offset = u * rd(0) + v * rd(1);
        return ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset, time);
    }

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
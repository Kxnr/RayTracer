#include "camera.h"

arma::vec3 randomInUnitDisk()
{
    arma::vec3 p;
    do
    {
        p = 2.0 * arma::vec3({drand48(), drand48(), 0}) - arma::vec3({1, 1, 0});
    } while (dot(p, p) >= 1.0);
    return p;
}

Camera::Camera(arma::vec3 lookFrom, arma::vec3 lookTo, arma::vec3 vup, float vfov, float aspect, float aperture, float focusDist)
{
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

ray Camera::getRay(float s, float t, float time)
{
  arma::vec3 rd = lensRadius * randomInUnitDisk();
  arma::vec3 offset = u * rd(0) + v * rd(1);
  return ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset, time);
}


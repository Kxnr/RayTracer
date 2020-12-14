#include "hitable.h"

bool AABB::hit(const ray& r, float tmin, float tmax) const {

  for(int a = 0; a < 3; a++) {
    double invD = 1.0f / r.direction()[a];
    double t0 = (min()[a] - r.origin()[a]) * invD;
    double t1 = (max()[a] - r.origin()[a]) * invD;

    if(invD < 0.0f) {
      std::swap(t0, t1);
    }

    tmin = t0 > tmin ? t0 : tmin;
    tmax = t1 < tmax ? t1 : tmax;

    if(tmax <= tmin) {
      return false;
    }
  }
  return true;
}

AABB AABB::boundingBox(AABB box0, AABB box1) {
  arma::vec3 small( {std::fmin(box0.min()[0], box1.min()[0]), 
      std::fmin(box0.min()[1], box1.min()[1]),
      std::fmin(box0.min()[2], box1.min()[2])} ); 

  arma::vec3 big( {std::fmax(box0.max()[0], box1.max()[0]), 
      std::fmax(box0.max()[1], box1.max()[1]),
      std::fmax(box0.max()[2], box1.max()[2])} ); 

  return AABB(small, big);
}

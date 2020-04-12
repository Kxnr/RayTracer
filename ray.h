#ifndef RAYH
#define RAYH

#include "vec3.h"

class ray
{
public:
    ray(){};
    ray(const vec3 &a, const vec3 &b, float time = 0)
    {
        A = a;
        B = b;
        _t = time;
    }

    vec3 origin() const { return A; };
    vec3 direction() const { return B; };
    vec3 pointAtParameter(float t) const { return A + t * B; };
    float time() const {return _t;};

    vec3 A;
    vec3 B;
    float _t;
};

#endif
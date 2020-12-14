#ifndef RAYH
#define RAYH
#include <armadillo>

class ray
{
public:
    ray(){};
    ray(const arma::vec3 &a, const arma::vec3 &b, float time = 0)
    {
        A = a;
        B = b;
        _t = time;
    }

    inline arma::vec3 origin() const { return A; };
    inline arma::vec3 direction() const { return B; };
    inline arma::vec3 pointAtParameter(float t) const { return A + t * B; };
    inline float time() const {return _t;};

    arma::vec3 A;
    arma::vec3 B;
    float _t;
};

#endif

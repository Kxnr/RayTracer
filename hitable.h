#ifndef HITABLEH
#define HITABLEH

#include "ray.h"
#include "path.h"

class Material;

struct hitRecord
{
    float t;
    arma::vec3 p;
    arma::vec3 normal;
    Material *matPtr;
};

class aabb {
    public:
        aabb() {}
        aabb(const arma::vec3& a, const arma::vec3& b) {_min = a; _max = b; }

        bool hit(const ray& r, float tmin, float tmax) const {

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

        static aabb boundingBox(aabb box0, aabb box1) {
            arma::vec3 small( {std::fmin(box0.min()[0], box1.min()[0]), 
                                std::fmin(box0.min()[1], box1.min()[1]),
                                std::fmin(box0.min()[2], box1.min()[2])} ); 

            arma::vec3 big( {std::fmax(box0.max()[0], box1.max()[0]), 
                                std::fmax(box0.max()[1], box1.max()[1]),
                                std::fmax(box0.max()[2], box1.max()[2])} ); 
            
            return aabb(small, big);
        }

        arma::vec3 min() const { return _min; }
        arma::vec3 max() const { return _max; }

    private:
        // edges of slab
        arma::vec3 _min;
        arma::vec3 _max;

};

class Hitable
{
public:
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const = 0;
    virtual bool boundingBox(float t0, float t1, aabb& box) const = 0;
};

#endif
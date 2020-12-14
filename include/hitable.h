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

// axis aligned bounding box
class AABB {
    public:
        AABB() {}
        AABB(const arma::vec3& a, const arma::vec3& b) {_min = a; _max = b; }

        bool hit(const ray& r, float tmin, float tmax) const;

        static AABB boundingBox(AABB box0, AABB box1);

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
    virtual bool boundingBox(float t0, float t1, AABB& box) const = 0;
};

#endif

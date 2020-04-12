#ifndef HITABLEH
#define HITABLEH

// TODO: make primitive geometry mesh based

#include "ray.h"
#include "path.h"

class Material;

struct hitRecord
{
    float t;
    vec3 p;
    vec3 normal;
    Material *matPtr;
};

class AABB {
    public:
        AABB() {}
        AABB(const vec3& a, const vec3& b) {_min = a; _max = b; }

        bool hit(const ray &r, float tmin, float tmax) const; 

        static AABB boundingBox(AABB box0, AABB box1);

        vec3 min() const { return _min; }
        vec3 max() const { return _max; }

    private:
        // edges of slab
        vec3 _min;
        vec3 _max;
};

class Hitable
{
public:
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const = 0;
    virtual bool boundingBox(float t0, float t1, AABB& box) const = 0;

    static bool compareX(const Hitable* a, const Hitable* b);
    static bool compareY(const Hitable* a, const Hitable* b);
    static bool compareZ(const Hitable* a, const Hitable* b);

    vec3 origin(const float t) const {return vec3(0,0,0);};
    vec3 scale(const float t) const {return vec3(1,1,1);}
    vec3 rotation(const float t) const {return vec3(0,0,0);}
};

#endif
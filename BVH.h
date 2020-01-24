#ifndef BVH_H
#define BVH_H

#include "hitableList.h"
#include <random>
#include <algorithm>

class BVHNode : public Hitable {
    public:
        BVHNode() {}
        BVHNode(Hitable** l, int n, float t0, float t1);
        ~BVHNode();

        virtual bool hit(const ray& r, float tmin, float tmax, hitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, aabb& b) const;

        Hitable * left;
        Hitable * right;

        aabb box;
        bool isLeaf;
};

#endif
#ifndef BVH_H
#define BVH_H

#include "hitableList.h"
#include <random>

class BVHNode : public HitableList {
    public:
        BVHNode() {}
        BVHNode(Hitable** l, int n, float t0, float t1);
        ~BVHNode();

        virtual bool hit(const ray& r, float tmin, float tmax, hitRecord& rec) const;
        virtual bool boundingBox(float t0, float t1, AABB& b) const;

        // tree children, most likely to be a binary tree
        HitableList* children; 
        int numChildren;
        AABB box;

        bool isLeaf;
};
#endif

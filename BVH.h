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
        virtual bool boundingBox(float t0, float t1, aabb& b) const;

        // tree children, most likely to be a binary tree
        HitableList* children; 
        int numChildren;
        aabb box;

        bool isLeaf;
};

BVHNode::BVHNode(Hitable** l, int n, float t0, float t1) {
    int axis = int(3*drand48());

    if(n <= 2) {
        isLeaf = true;
        children = new HitableList(l, n);
    }
    else {
        isLeaf = false;
        Hitable** list = new Hitable*[2];
        list[0] = new BVHNode(l, n/2, t0, t1);
        list[1] = new BVHNode(l + n/2, n - n/2, t0, t1);
        children = new HitableList(list, 2);
    }

    bool valid = children->boundingBox(t0, t1, box);
    if(!valid) {
        std::cerr << "Non boundable object in BVH" << std::endl;
        exit(0);
    }
}

bool BVHNode::boundingBox(float t0, float t1, aabb& b) const {
    bool valid = children->boundingBox(t0, t1, b);
    if(!valid) {
        std::cerr << "Non boundable object in BVH" << std::endl;
        exit(0);
    }
    return true;
}

BVHNode::~BVHNode() {
    if(!isLeaf) {
        for(int i = 0; i < numChildren; i++) {
            delete &children[i];
        }
    }
    delete [] children;
}

bool BVHNode::hit(const ray& r, float tmin, float tmax, hitRecord& rec) const {
    if (box.hit(r, tmin, tmax)) {
        if(children->hit(r, tmin, tmax, rec)) {
            return true;
        }
    }
    return false;
}


#endif
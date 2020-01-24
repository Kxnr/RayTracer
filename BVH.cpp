#include "BVH.h"

BVHNode::BVHNode(Hitable** l, int n, float t0, float t1) {
    int axis = int(3*(float) drand48());

    if(axis == 0) { 
        std::sort(l, l + n, Hitable::compareX);
    }
    else if(axis == 1) {
        std::sort(l, l + n, Hitable::compareY);
    }
    else {
        std::sort(l, l + n, Hitable::compareZ);
    }

    if(n == 2) {
        isLeaf = true;

        left = l[0];
        right = l[1];
    }
    else if(n == 1) {
        isLeaf = true;
        left = right = l[0];
    }
    else {
        isLeaf = false;
        left = new BVHNode(l, n/2, t0, t1);
        right = new BVHNode(l + n/2, n - n/2, t0, t1);
    }

    aabb leftBox;
    bool leftValid = left->boundingBox(t0, t1, leftBox);
    aabb rightBox;
    bool rightValid = right->boundingBox(t0, t1, rightBox);

    if(!leftValid || !rightValid) {
        std::cerr << "Non boundable object in BVH" << std::endl;
        exit(0);
    }
    
    box = aabb::boundingBox(leftBox, rightBox);
}


BVHNode::~BVHNode() {
    if(!isLeaf) {
        if(left != right)
            delete left;
        delete right;
    }
}


bool BVHNode::boundingBox(float t0, float t1, aabb &b) const {
    b = box;
    return true;
}


bool BVHNode::hit(const ray &r, float tmin, float tmax, hitRecord& rec) const {
    if (box.hit(r, tmin, tmax)) {
        hitRecord leftRec, rightRec;
        bool hitLeft = left->hit(r, tmin, tmax, leftRec);
        bool hitRight = right->hit(r, tmin, tmax, rightRec);

        if(hitLeft && hitRight) {
            rec = leftRec.t < rightRec.t ? leftRec : rightRec;
            return true;
        }
        else if(hitLeft) {
            rec = leftRec;
            return true;
        }
        else if(hitRight) {
            rec = rightRec;
            return true;
        }
    }
    return false;
}
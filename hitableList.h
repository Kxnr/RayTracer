#ifndef HITABLELISTH
#define HITABLELISTH

// bass ackwards, code into cpp

#include "hitable.h"
#include <algorithm>

class HitableList : public Hitable
{
public:
    HitableList(){};
    HitableList(Hitable **l, int n)
    {
        list = l;
        listSize = n;
    };

    virtual bool boundingBox(float t0, float t1, AABB& box) const override;
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const override;
    void sort(int axis);
    Hitable* &operator[] (int ind);

    Hitable **list;
    int listSize;
};

#endif
#ifndef HITABLELISTH
#define HITABLELISTH

#include "hitable.h"

class HitableList : public Hitable
{
public:
    HitableList(){};
    HitableList(Hitable **l, int n)
    {
        list = l;
        listSize = n;
    };

    bool boundingBox(float t0, float t1, AABB& box) const override;
    bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const override;
    void sort(int axis);
    Hitable* &operator[] (int ind);

    Hitable **list;
    int listSize;

private:
    static bool compareX(const Hitable* a, const Hitable* b);
    static bool compareY(const Hitable* a, const Hitable* b);
    static bool compareZ(const Hitable* a, const Hitable* b);
};
#endif

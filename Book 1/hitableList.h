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
    virtual bool hit(const ray &r, float tmin, float tmax, hitRecord &rec) const;
    Hitable **list;
    int listSize;
};

bool HitableList::hit(const ray &r, float tmin, float tmax, hitRecord &rec) const
{
    hitRecord tempRec;
    bool hitAnything = false;
    double closest = tmax;
    for (int i = 0; i < listSize; i++)
    {
        if (list[i]->hit(r, tmin, closest, tempRec))
        {
            hitAnything = true;
            closest = tempRec.t;
            rec = tempRec;
        }
    }
    return hitAnything;
}

#endif
#include "hitableList.h"

void HitableList::sort(int axis)
{

    if (axis == 0)
    {
        std::sort(list, list + listSize, HitableList::compareX);
    }
    else if (axis == 1)
    {
        std::sort(list, list + listSize, HitableList::compareY);
    }
    else if (axis == 2)
    {
        std::sort(list, list + listSize, HitableList::compareZ);
    }
    else
    {
        std::cerr << "Invalid sort axis" << std::endl;
        exit(0);
    }
}

Hitable *&HitableList::operator[](int index)
{
    if (index >= listSize || index < 0)
    {
        std::cerr << "List index out of bounds" << std::endl;
        exit(0);
    }
    return list[index];
}

bool HitableList::hit(const ray &r, float tmin, float tmax, hitRecord &rec) const
{
    hitRecord tempRec;
    bool hitAnything = false;
    float closest = tmax;
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

bool HitableList::boundingBox(float t0, float t1, AABB &box) const
{
    if (listSize < 1)
    {
        return false;
    }

    AABB tempBox;
    bool firstTrue = list[0]->boundingBox(t0, t1, tempBox);

    if (!firstTrue)
    {
        return false;
    }
    else
    {
        box = tempBox;
    }

    for (int i = 1; i < listSize; i++)
    {
        if (list[i]->boundingBox(t0, t1, tempBox))
        {
            box = AABB::boundingBox(box, tempBox);
        }
        else
        {
            return false;
        }
    }

    return true;
}

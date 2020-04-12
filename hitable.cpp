#include "hitable.h"

bool AABB::hit(const ray &r, float tmin, float tmax) const {
    for(int a = 0; a < 3; a++) {
        float invD = 1.0 / r.direction()[a];
        float t0 = (min()[a] - r.origin()[a]) * invD;
        float t1 = (max()[a] - r.origin()[a]) * invD;

        if(invD < 0.0f) {
            std::swap(t0, t1);
        }

        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;

        if(tmax <= tmin) {
            return false;
        }
    }
    return true;
}

AABB AABB::boundingBox(AABB box0, AABB box1) {
    vec3 small( std::fmin(box0.min()[0], box1.min()[0]), 
                        std::fmin(box0.min()[1], box1.min()[1]),
                        std::fmin(box0.min()[2], box1.min()[2]) ); 

    vec3 big( std::fmax(box0.max()[0], box1.max()[0]), 
                        std::fmax(box0.max()[1], box1.max()[1]),
                        std::fmax(box0.max()[2], box1.max()[2]) ); 
    
    return AABB(small, big);
}

bool Hitable::compareX(const Hitable *a, const Hitable *b)
{
    AABB box0, box1;
    if (!a->boundingBox(0, 0, box0) || !b->boundingBox(0, 0, box1))
    {
        std::cerr << "Un boundable object in HitableList sort" << std::endl;
        exit(0);
    }

    if (box0.min()[0] - box1.min()[0] < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Hitable::compareY(const Hitable *a, const Hitable *b)
{
    AABB box0, box1;
    if (!a->boundingBox(0, 0, box0) || !b->boundingBox(0, 0, box1))
    {
        std::cerr << "Un boundable object in HitableList sort" << std::endl;
        exit(0);
    }

    if (box0.min()[1] - box1.min()[1] < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Hitable::compareZ(const Hitable *a, const Hitable *b)
{
    AABB box0, box1;
    if (!a->boundingBox(0, 0, box0) || !b->boundingBox(0, 0, box1))
    {
        std::cerr << "Un boundable object in HitableList sort" << std::endl;
        exit(0);
    }

    if (box0.min()[2] - box1.min()[2] < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
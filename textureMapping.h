#ifndef TEXTUREMAPPINGH
#define TEXTUREMAPPINGH

#include "vec3.h"
#include "hitable.h"

class TextureMap {
    public:
        virtual vec3 map(const vec3& worldSpace, const float t) const = 0;
};

class WorldSpace : public TextureMap {
    public:
        virtual vec3 map(const vec3 &worldSpace, const float t) const
        {
            return worldSpace;
        }
};

class ObjectSpace : public TextureMap {
    public:
        ObjectSpace(const Hitable* object) : origin(object) {}

        virtual vec3 map(const vec3 &worldSpace, const float t) const
        {
            vec3 localSpace = worldSpace - origin->origin(t);
            return localSpace.unrotate(origin->rotation(t)) / origin->scale(t);
        }

    private:
        const Hitable *origin; // world space coordinate
};

class PolarSpace : public TextureMap {
    // 
};

class UVSpace : public TextureMap {
    // map object space coordinates to two dimensional space
    public:
        virtual vec3 map(const vec3 &worldSpace, const float t) const
        {
            // TODO: Use triplanar projection as default
            // until I figure out how to make an actual
            // UV editor
            return worldSpace;
        }
};

class BoundingSpace : public TextureMap {

    public:
        BoundingSpace(const Hitable* object) : origin(object) {}

        virtual vec3 map(const vec3 &worldSpace, const float t) const
        {
            AABB box;
            bool possible = origin->boundingBox(t, t, box);

            if(!possible) {
                std::cerr << "Invalid object for bounding coordinates" << std::endl;
                exit(0);
            }

            return vec3(1.0f, 1.0f, 1.0f) - ((box.max() - worldSpace) / (box.max() - box.min()));
        }

    private:
        const Hitable *origin; // world space coordinate
};

class CameraSpace : public TextureMap {

    public:
        CameraSpace(const Camera* cam) : camera(cam) {}

        virtual vec3 map(const vec3 &worldSpace, const float t) const
        {
            // TODO:
            return vec3(0,0,0);
        }
    private:
        const Camera *camera;
};

#endif
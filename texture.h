#ifndef TEXTUREH
#define TEXTUREH

#include "vec3.h"
#include "textureMapping.h"

class Texture {
    public:
        Texture() { mapping = new WorldSpace(); } // define a default mapping
        Texture(const TextureMap *texmap) : mapping(texmap) {}

        ~Texture() {
          delete mapping;
        }

        virtual vec3 value(const vec3& p, float t) const = 0;
        virtual vec3 value(const vec3 &p) const {
          return value(p, 0);
        };

        const TextureMap* mapping;
};

#endif

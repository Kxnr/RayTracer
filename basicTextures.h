#ifndef BASICTEXTURESH 
#define BASICTEXTURESH
// TODO: this got all bass-ackwards with the code in the .h, change that

#include "texture.h"
#include <SOIL.h> // texture loading

class ConstantTexture : public Texture {
    public:
        ConstantTexture(const vec3 c) : color(c) {}
        ConstantTexture(float c) : color(vec3(c, c, c)) {}
        ConstantTexture(float a, float b, float c) : color(vec3(a, b, c)) {}

        virtual vec3 value(const vec3 &p, float t) const override;
        
        vec3 color;
};

class CheckerTexture : public Texture {
    public:
        CheckerTexture(Texture *t0, Texture *t1, float scl) : even(t0), odd(t1), scale(scl) {}
        CheckerTexture(vec3 t0, vec3 t1, float scl) : even(new ConstantTexture(t0)), odd(new ConstantTexture(t1)), scale(scl) {}

        virtual vec3 value(const vec3& p, float t) const override;

        Texture *odd;
        Texture *even;
        float scale;

        ~CheckerTexture() {
          delete even;
          delete odd;
        }
};

class NoiseTexture : public Texture {
    NoiseTexture() {

    }

    ~NoiseTexture() {

    }

    virtual vec3 value(const vec3 &p, float t) const;
};

class ImageTexture : public Texture {
    ImageTexture(std::string source) {
        // TODO: this can error, handle it
        // load all images as RGB, as alpha is not supported by materials engine
        image = SOIL_load_image(source.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
    }

    ~ImageTexture() {
        // delete image
        SOIL_free_image_data(image);
    }
    
    virtual vec3 value(const vec3& p, float t) const;

    unsigned char* image;
    int width; int height; int channels;
};

#endif

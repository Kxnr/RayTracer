#include "basicTextures.h"

vec3 ConstantTexture::value(const vec3 &p, float t) const {
    return color;
}

vec3 CheckerTexture::value(const vec3 &p, float t) const {
    vec3 mapped = mapping->map(p, t);
    float sines = sin(scale*mapped[0])*sin(scale*mapped[1])*sin(scale*mapped[2]);
    if(sines < 0) {
        return odd->value(mapped);
    }
    else {
        return even->value(mapped);
    }
}

vec3 NoiseTexture::value(const vec3 &p, float t) const {
    return vec3();
}

vec3 ImageTexture::value(const vec3&p, float t) const {
    // discard z value, clamp to 0->1
    int col = std::round(p[0]*width);
    int row = std::round(p[1]*height);

    if(col >= width) {
        col = width - 1;
    }
    else if(col < 0) {
        col = 0;
    }

    if(row >= height) {
        row = height - 1;
    }
    else if(row < 0) {
        row = 0;
    }

    // images striped as r1, g1, b1, r2, b2, c2
    int pixel_index =  row * width + col;

    return vec3(image[pixel_index], image[pixel_index + 1], image[pixel_index + 2]);
}

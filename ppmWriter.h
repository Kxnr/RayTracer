#ifndef PPMWRITERH
#define PPMWRITERH

#include <iostream>
#include <string>
#include "vec3.h"

class PPMImage
{
public:
    PPMImage(int w, int h) : nx{w}, ny{h} { blankImage(); };
    ~PPMImage() { delete [] image; };

    void write(double gamma = 1);

    void blankImage();

    void colorVec(int x, int y, vec3 c);

    inline int idx(int x, int y, int c)
    {
        return 3 * (y * nx + x) + c;
    };

    inline double &operator()(int i, int j, int k)
    {
        return image[idx(i, j, k)];
    };

    int nx, ny;
    double *image;
};

#endif
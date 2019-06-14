#include <iostream>
#include "ppmWriter.h"

//TODO: constants for color channels
//TODO: modify ppmImage to be compatible with vec3's
int main()
{
    int nx = 200;
    int ny = 100;
    PPMImage image = PPMImage(nx, ny);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float r = float(i) / float(nx);
            float g = float(j) / float(ny);
            float b = .2;
            image(i, j, 0) = r;
            image(i, j, 1) = g;
            image(i, j, 2) = b;
        }
    }
    image.write();
}
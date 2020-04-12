#include "ppmWriter.h"

void PPMImage::write(double gamma = 1) {
    // writes ppm image file to std out
    std::cout << "P3\n"
                << nx << " " << ny << "\n65535\n";
    for (int y = ny - 1; y >= 0; y--)
    {
        for (int x = 0; x < nx; x++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (gamma != 1)
                {
                    image[idx(x, y, c)] = std::pow(image[idx(x, y, c)], 1 / gamma);
                }
                std::cout << int(65535.99 * image[idx(x, y, c)]) << " ";
            };
            std::cout << "\n";
        };
    };
}

void PPMImage::blankImage()
{
    image = new double[nx * ny * 3];

    for (int i = 0; i < nx; i++)
    {
        for (int j = 0; j < ny; j++)
        {
            for (int c = 0; c < 3; c++)
            {
                image[idx(i, j, c)] = 0;
            }
        }
    }
};

void PPMImage::colorVec(int x, int y, vec3 c)
{
    for (int i = 0; i < 3; i++)
    {
        image[idx(x, y, i)] = c[i];
    }
}
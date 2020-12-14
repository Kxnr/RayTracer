#ifndef PPMWRITERH
#define PPMWRITERH

#include <iostream>
#include <string>
#include <armadillo>

class PPMImage
{
public:
    PPMImage(int w, int h) : nx{w}, ny{h} { blankImage(); };
    int nx, ny;
    float *image;

    inline void write(float gamma = 1)
    {
        // writes ppm image file to std out
        std::cout << "P3\n"
                  << nx << " " << ny << "\n255\n";
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
                    std::cout << int(255.99 * image[idx(x, y, c)]) << " ";
                };
                std::cout << "\n";
            };
        };
    }

    int idx(int x, int y, int c)
    {
        return 3 * (y * nx + x) + c;
    }

    void deleteImage();

    void blankImage();

    void colorVec(int x, int y, arma::vec3 c);

    inline float &operator()(int i, int j, int k)
    {
        return image[idx(i, j, k)];
    }
};

void PPMImage::blankImage()
{
    image = new float[nx * ny * 3];

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
}

void PPMImage::deleteImage()
{
    delete image;
}

void PPMImage::colorVec(int x, int y, arma::vec3 c)
{
    for (int i = 0; i < 3; i++)
    {
        image[idx(x, y, i)] = c(i);
    }
}

#endif

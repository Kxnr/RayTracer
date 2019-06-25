
#include <iostream>
#include <random>
#include "ray.h"
#include "sphere.h"
#include "hitableList.h"
#include "ppmWriter.h"
#include "movingCamera.h"
#include "material.h"
#include "path.h"

// TODO: textuing
// TODO: properly freeing memory

arma::vec3 color(const ray &r, Hitable *world, int depth)
{
    hitRecord rec;
    // reject values really close to the surface of the material
    if (world->hit(r, .0001, MAXFLOAT, rec))
    {
        ray scattered;
        arma::vec3 attenuation;
        if (depth < 50 && rec.matPtr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation % color(scattered, world, depth + 1);
        }
        else
        {
            return arma::vec3({0, 0, 0});
        }
    }
    else
    {
        //sky
        arma::vec3 unitDirection = arma::normalise(r.direction());
        float t = 0.5 * (unitDirection(1) + 1.0);
        return (1.0 - t) * arma::vec3({1.0, 1.0, 1.0}) + t * arma::vec3({0.5, 0.7, 1.0});
    }
}

int main()
{
    int nx = 800;
    int ny = 400;
    int ns = 100;
    PPMImage image(nx, ny);

    arma::vec3 lowerLeftCorner = {-2.0, -1.0, -1.0};
    arma::vec3 horizontal = {4.0, 0.0, 0.0};
    arma::vec3 vertical = {0.0, 2.0, 0.0};
    arma::vec3 origin = {0.0, 0.0, 0.0};

    Material gray = Material(arma::vec3({0.4, 0.4, 0.4}), arma::vec3({1.0, 1.0, 1.0}), 0.0, 1.0, 1.3);
    Material red = Material(arma::vec3({0.6, 0.2, 0.2}), arma::vec3({1.0, 1.0, 1.0}), 0.0, 1.0, 1.3);

    Line *traj = new Line(arma::vec3({0,0,0}), arma::vec3({.5,0,0}));

    Hitable *list[2];
    list[0] = new Sphere(arma::vec3({0, 0, -1}), 0.5, &red, traj);
    list[1] = new Sphere(arma::vec3({0, -100.5, -1}), 100, &gray);
    Hitable *world = new HitableList(list, 2);

    Camera cam(arma::vec3({-2, 2, 1}), arma::vec3({0, 0, -1}), arma::vec3({0, 1, 0}), .5, float(nx) / float(ny), .1, 3.4);
    float t0 = 0;
    float exposure = 1;
    float t;

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            arma::vec3 col = {0, 0, 0};
            // antialiasing
            for (int s = 0; s < ns; s++)
            {

                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                t = t0 + drand48()*exposure;

                ray r = cam.getRay(u, v, t);
                col += color(r, world, 0);
            }
            col /= float(ns);
            image.colorVec(i, j, col);
        }
    }

    // cleanup
    image.write(2.0);

    // TODO //
    // delete list
}

#include <iostream>
#include <random>
#include "ray.h"
#include "sphere.h"
#include "hitableList.h"
#include "ppmWriter.h"
#include "camera.h"
#include "material.h"

vec3 color(const ray &r, Hitable *world, int depth)
{
    hitRecord rec;
    // reject values really close to the surface of the material
    if (world->hit(r, .0001, MAXFLOAT, rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.matPtr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            return vec3(0, 0, 0);
        }
    }
    else
    {
        //sky
        vec3 unitDirection = unitVector(r.direction());
        float t = 0.5 * (unitDirection.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
    int nx = 1920;
    int ny = 1080;
    int ns = 200;
    PPMImage image(nx, ny);

    vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    Material gray = Material(vec3(0.4, 0.4, 0.4), vec3(1.0, 1.0, 1.0), 0.0, 1.0, 1.3);
    Material red = Material(vec3(0.6, 0.2, 0.2), vec3(1.0, 1.0, 1.0), 0.0, 1.0, 1.3);
    Material polishedMetal = Material(vec3(.8, .6, .2), vec3(1.0, 1.0, 1.0), 0.0, .001, 1.0);
    Material glass = Material(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), .9, .01, 1.3);

    Hitable *list[5];
    list[0] = new Sphere(vec3(0, 0, -1), 0.5, &red);
    list[1] = new Sphere(vec3(0, -100.5, -1), 100, &gray);
    list[2] = new Sphere(vec3(1, 0, -1), 0.5, &polishedMetal);
    list[3] = new Sphere(vec3(-1, 0, -1), 0.5, &glass);
    Hitable *world = new HitableList(list, 4);

    Camera cam(vec3(-2, 2, 1), vec3(0, 0, -1), vec3(0, 1, 0), .5, float(nx) / float(ny), .1, 3.4);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col = vec3(0, 0, 0);
            // antialiasing
            for (int s = 0; s < ns; s++)
            {

                float u = float(i + (float) drand48()) / float(nx);
                float v = float(j + (float) drand48()) / float(ny);
                ray r = cam.getRay(u, v);
                col += color(r, world, 0);
            }
            col /= float(ns);
            image.colorVec(i, j, col);
        }
    }

    // cleanup
    image.write(2.0);
}
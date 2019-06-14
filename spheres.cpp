#include <iostream>
#include <random>
#include "ray.h"
#include "sphere.h"
#include "hitableList.h"
#include "ppmWriter.h"
#include "camera.h"

vec3 color(const ray &r, Hitable *world)
{
    hitRecord rec;
    if (world->hit(r, 0.0, MAXFLOAT, rec))
    {
        return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
    }
    else
    {
        vec3 unitDirection = unitVector(r.direction());
        float t = 0.5 * (unitDirection.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
    int nx = 200;
    int ny = 100;
    int ns = 100;
    PPMImage image(nx, ny);

    vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    Hitable *list[2];
    list[0] = new Sphere(vec3(0, 0, -1), 0.5);
    list[1] = new Sphere(vec3(0, -100.5, -1), 100);
    Hitable *world = new HitableList(list, 2);

    Camera cam;

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col = vec3(0, 0, 0);
            // antialiasing
            for (int s = 0; s < ns; s++)
            {

                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.getRay(u, v);
                col += color(r, world) * 255.99;
            }
            col /= float(ns);
            image.colorVec(i, j, col);
        }
    }
    image.write();
}

#include <iostream>
#include <iomanip>
#include <random>
#include "vec3.h"
#include "ray.h"
#include "BVH.h"
#include "sphere.h"
#include "ppmWriter.h"
#include "movingCamera.h"
#include "material.h"
#include "path.h"
#include "texture.h"
#include "basicTextures.h"

// TODO: scene class, with camera, objects, and materials and a single render function to allow renderer to be swapped out
// TODO: texturing
// TODO: properly freeing memory
// TODO: improve color depth
// TODO: integrate over paths to create multiple frames (animation wrapper?)
// TODO: spline plotter
// TODO: finish splines

// BIG TODO: make into spectral raytracer

vec3 color(const ray &r, Hitable *world, int depth)
{
    hitRecord rec;
    // reject values really close to the surface of the material
    if (world->hit(r, .0001, MAXFLOAT, rec))
    {
        ray scattered;
        vec3 attenuation;
        // TODO: shadow rays, multiple samples at child rays
        // TODO: rays can be produce both scattbxdfer and refraction rays
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
        float t = 0.5 * (unitDirection[1] + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
    std::clock_t start, end;
    start = std::clock();
    srand48(std::time(NULL));

    int nx = 400;
    int ny = 200;
    int ns = 100;
    PPMImage image(nx, ny);

    vec3 lowerLeftCorner = {-1.0, -1.0, -1.0};
    vec3 horizontal = {4.0, 0.0, 0.0};
    vec3 vertical = {0.0, 2.0, 0.0};
    vec3 origin = {0.0, 0.0, 0.0};

    std::cerr << "Setting up materials" << std::endl;

    // make floor checkered
    Material background = Material(new CheckerTexture(vec3(.1, .1, .1), vec3(.9, .9, .9), 5), vec3(1.0, 1.0, 1.0), 0.0, 1.0, 1.3);
    Material ball = Material(vec3(.8, .8, .8), vec3(.1, .1, .1), 0.0, .3, 1.3);
    //Line *traj = new Line(vec3({0,0,-1}), vec3({0,0,-1.05}));

    std::cerr << "Creating objects" << std::endl;

    Hitable *list[2];

    Camera cam(vec3(-5, 2, 0.0), vec3(0, 1.0, 0.0), vec3(0, 1, 0), .5, float(nx) / float(ny), .01, 3.4);
    float t0 = 0;
    float exposure = 1;
    float t;

    list[0] = new Sphere(vec3(0, -100, -1), 100, &background);
    list[1] = new Sphere(vec3(0, 1.0, 0), 1.0, &ball);

    //Hitable* world = new HitableList(list, NUM_OBJECTS + 1);
    Hitable* world = new BVHNode(list, 2, t0, t0 + exposure);
    //Hitable* world = new BVHNode(list, NUM_OBJECTS+1, t0, t0 + exposure);

    std::cerr << "Tracing rays" << std::endl;

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col = {0, 0, 0};
            // antialiasing
            for (int s = 0; s < ns; s++)
            {

                float u = float(i + (float) drand48()) / float(nx);
                float v = float(j + (float) drand48()) / float(ny);
                t = t0 + (float) drand48()*exposure;
                

                ray r = cam.getRay(u, v, t);
                col += color(r, world, 0); // ERROR:

            }
            col /= float(ns);
            image.colorVec(i, j, col);
        }
    }

    std::cerr << "Saving Image" << std::endl;

    // cleanup
    image.write(1.5);

    std::cerr << "Cleaning up" << std::endl;
    for(int i = 0; i < 2; i++) {
        delete list[i];
    }

    delete world;
    // delete traj;

    end = std::clock();
    
    std::cerr << "Time elapsed: " << std::endl;
    std::cerr << std::fixed << double(end-start) / double(CLOCKS_PER_SEC) << std::setprecision(5) << std::endl;
}
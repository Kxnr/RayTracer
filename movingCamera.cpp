
#include <iostream>
#include <iomanip>
#include <random>
#include "ray.h"
#include "BVH.h"
#include "sphere.h"
#include "ppmWriter.h"
#include "movingCamera.h"
#include "material.h"
#include "path.h"

#define NUM_OBJECTS 40
#define NUM_MATS 7

// TODO: scene class, with camera, objects, and materials and a single render function to allow renderer to be swapped out
// TODO: texturing
// TODO: properly freeing memory
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
        if (depth < 50 && rec.matPtr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation % color(scattered, world, depth + 1);
        }
        else
        {
            return vec3({0, 0, 0});
        }
    }
    else
    {
        //sky
        vec3 unitDirection = arma::normalise(r.direction());
        float t = 0.5 * (unitDirection[1] + 1.0);
        return (1.0 - t) * vec3({1.0, 1.0, 1.0}) + t * vec3({0.5, 0.7, 1.0});
    }
}

void randomScene(int numObjects, float minSize, float maxSize, Camera cam, Material** mat, int numMats, Hitable* list[]){
    // get random unit vector in camera view, translate until z is 0 + radius + epsilon
    // check if spheres intersect
    vec3 origin = cam.origin;
    double epsilon = 0;
    int m;
    double u, v;
    bool valid;
    double radius, intersectionTime, magnitude;
    vec3 center;
    for(int i = 0; i < numObjects; i++) {
        valid = false;
        while(!valid) {
            u = (float) drand48();
            v = (float) drand48();
            radius = (float) drand48() * maxSize;

            ray pose = cam.getRay(u, v, 0);
            intersectionTime = (pose.origin()(1)) / pose.direction()(1);

            magnitude = arma::norm(pose.origin() - pose.pointAtParameter(intersectionTime));

            if(magnitude < 8*radius) {
                continue;
            }

            magnitude -= (magnitude * (radius + epsilon)) / pose.origin()(1);
            center = pose.origin() + arma::normalise(pose.direction()) * magnitude;


            valid  = true;
            for(int j = 0; j < i; j++) {
                magnitude = arma::norm(center - ((Sphere*)list[j])->center()); 
                if(magnitude < 1.5 * (radius + ((Sphere*)list[j])->radius)) {
                    valid = false;
                    break;
                }
            } 
        }
        m = int((float) drand48() * numMats);
        list[i] = new Sphere(center, radius, mat[m]);
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

    vec3 lowerLeftCorner = {-2.0, -1.0, -1.0};
    vec3 horizontal = {4.0, 0.0, 0.0};
    vec3 vertical = {0.0, 2.0, 0.0};
    vec3 origin = {0.0, 0.0, 0.0};

    std::cerr << "Setting up materials" << std::endl;

    Material *mats[NUM_MATS];
    Material background = Material(vec3({0.4, 0.4, 0.4}), vec3({1.0, 1.0, 1.0}), 0.0, 1.0, 1.3);
    
    mats[0] = new Material(vec3({0.6, 0.2, 0.2}), vec3({1.0, 1.0, 1.0}), 0.0, .9, 1.3);
    mats[1] = new Material(vec3({0.2, 0.6, 0.2}), vec3({1.0, 1.0, 1.0}), 0.0, 1.0, 1.3);
    mats[2] = new Material(vec3({0.2, 0.2, 0.6}), vec3({1.0, 1.0, 1.0}), 0.0, .2, 1.3);
    mats[3] = new Material(vec3({0.6, 0.2, 0.6}), vec3({1.0, 1.0, 1.0}), 0.0, .6, 1.3);
    mats[4] = new Material(vec3({0.6, 0.2, 0.6}), vec3({.7, .7, .5}), 0.0, .4, 1.4);
    mats[5] = new Material(vec3({0.4, 0.3, 0.6}), vec3({.5, .5, .5}), .95, .2, 1.4);
    mats[6] = new Material(vec3({0.05, .01, .01}), vec3({0.0, 0.0, .05}), .95, .02, 1.3);

    //Line *traj = new Line(vec3({0,0,-1}), vec3({0,0,-1.05}));

    std::cerr << "Creating objects" << std::endl;

    Hitable *list[NUM_OBJECTS+1];

    Camera cam(vec3({-2, .5, -1}), vec3({0, .2, -1}), vec3({0, 1, 0}), .5, float(nx) / float(ny), .01, 3.4);
    float t0 = 0;
    float exposure = 1;
    float t;

    list[NUM_OBJECTS] = new Sphere(vec3({0, -100.1, -1}), 100, &background);
    randomScene(NUM_OBJECTS, .1, .5, cam, mats, NUM_MATS, list);

    //Hitable* world = new HitableList(list, NUM_OBJECTS + 1);
    Hitable* world = new BVHNode(list, NUM_OBJECTS + 1, t0, t0 + exposure);
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
    for(int i = 0; i < NUM_OBJECTS; i++) {
        delete list[i];
    }

    delete world;
    // delete traj;

    end = std::clock();
    
    std::cerr << "Time elapsed: " << std::endl;
    std::cerr << std::fixed << double(end-start) / double(CLOCKS_PER_SEC) << std::setprecision(5) << std::endl;
}
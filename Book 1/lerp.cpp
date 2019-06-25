#include <iostream>
#include "ray.h"
#include "ppmWriter.h"

float hitSphere(const vec3 &center, float radius, const ray &r)
{
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

vec3 color(const ray &r)
{
    float t = hitSphere(vec3(0, 0, -1), 0.5, r);
    if (t > 0.0)
    {
        vec3 N = unitVector(r.pointAtParameter(t) - vec3(0, 0, -1));
        return 0.5 * vec3(N.x() + 1, N.y() + 1, N.z() + 1);
    }
    vec3 unitDirection = unitVector(r.direction());
    t = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main()
{
    int nx = 200;
    int ny = 100;
    PPMImage image(nx, ny);

    vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lowerLeftCorner + u * horizontal + v * vertical);
            vec3 col = color(r);

            image.colorVec(i, j, col);
        }
    }
    image.write();
}
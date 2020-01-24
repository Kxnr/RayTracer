#ifndef PATH_H
#define PATH_H

#include "ray.h"
#include "vec3.h"
#include <armadillo>

class Path
{
public:
    virtual vec3 position(float t) const = 0;
};

class Spline : public Path
{
public: 
    Spline(arma::vec3 *_pts, float *_times, int _num_pts)
    {
        // points interpolated pairwise with
        // quadratic spline, defined as 1 time
        // unit apart

        // first and last points are used to
        // determine slopes at end points
        // with an adjacent point
        pts = _pts;
        times = _times; // assert that they're sorted
        n = _num_pts;

        if (n < 4)
        {
            std::cerr << "Not enough points to define spline" << std::endl;
            throw "Too few points";
        }

        get_params();
    }

    virtual vec3 position(float t) const override;

    arma::vec3 *pts;
    float *times;
    int n;
    arma::mat params;

private:
    void get_params();
};


class Line : public Path
{
public: 
    Line(vec3 _or, vec3 _vel)
    {
        // magnitude of _vel determines velocity
        line = ray(_or, _vel);
    }
    ray line;

    virtual vec3 position(float t) const {  return line.pointAtParameter(t); };
};

#endif
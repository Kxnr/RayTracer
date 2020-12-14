#ifndef PATH_H
#define PATH_H

#include "ray.h"

class Path
{
public:
    virtual arma::vec3 position(float t) const = 0;
};

class Spline : public Path
{
public: 
    Spline(arma::vec3 *_pts, float *_times, int _num_pts);

    virtual arma::vec3 position(float t) const;

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
    Line(arma::vec3 _or, arma::vec3 _vel)
    {
        // magnitude of _vel determines velocity
        line = ray(_or, _vel);
    }
    ray line;

    virtual arma::vec3 position(float t) const {  return line.pointAtParameter(t); };
};

#endif

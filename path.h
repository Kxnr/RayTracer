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

    virtual arma::vec3 position(float t) const;

    arma::vec3 *pts;
    float *times;
    int n;
    arma::mat params;

private:
    void get_params();
};

void Spline::get_params()
{
    arma::cube M = arma::cube(4 * (n - 1), 4 * (n - 1), 3);
    arma::mat x = arma::mat(4 * (n - 1), 3);

    M.fill(0);
    x.fill(0);

    for (arma::uword r = 0; r < 4 * n - 2; r++)
    {
        arma::uword part = r / 4;
        arma::uword cond = r % 4;

        if (cond == 0)
        {
            // C0(ui)
            // assign vector
            x.row(r) = pts[r];

            // assign matrix
            M.tube(r, part)   = 1;
            M.tube(r, part+1) = times[part];
            M.tube(r, part+2) = times[part]*times[part];
            M.tube(r, part+3) = times[part]*times[part]*times[part];

        }
        else if (cond == 1) {
            // C0(ui+1)
            // assign vector with 
            x.row(r) = pts[r+1];

            // assign matrix
            M.tube(r, part)     = 1;
            M.tube(r, part + 1) = times[part +1];
            M.tube(r, part + 2) = times[part+1] * times[part+1];
            M.tube(r, part + 3) = times[part+1] * times[part+1] * times[part+1];
        }
        else if (cond == 2)
        {
            // C1(ui+1)
            // vector initialized to 0

            // assign matrix
            arma::vec3 dx0 = pts[part+1] - pts[part];
            float du0 = times[part+1] - times[part];

            arma::vec3 dx1 = pts[part+2] - pts[part+1];
            float du1 = times[part+2] - times[part+1];

            M.tube(r, part+1) = (dx1/ du1);
            M.tube(r, part+2) = 2*times[part+1] * (dx1/ du1);
            M.tube(r, part+3) = 3*times[part+1]*times[part+1] *(dx1 / du1);
            M.tube(r, part+5) = -1*(dx0/du0);
            M.tube(r, part+6) = -2*times[part+1] * (dx0/du0);
            M.tube(r, part+7) = -3*times[part+1] *times[part+1]* (dx0 / du0);
        }
        else
        {
            // C2(ui+1)
            // vector initialized to 0

            // assign matrix
            arma::vec3 dx0 = pts[part + 1] - pts[part];
            float du0 = times[part + 1] - times[part];

            arma::vec3 dx1 = pts[part + 2] - pts[part + 1];
            float du1 = times[part + 2] - times[part + 1];

            M.tube(r, part + 2) = 2 *(dx1 / du1)*(dx1 / du1);
            M.tube(r, part + 3) = 6 * times[part + 1] * (dx1 / du1)*(dx1 / du1);
            M.tube(r, part + 6) = -2 * (dx0 / du0)*(dx1 / du1);
            M.tube(r, part + 7) = -6* times[part + 1] * (dx0 / du0)*(dx1 / du1);
        }
    }

    // TODO: // slope constraints

    arma::mat params = arma::mat(4*n-4, 3);
    params.col(0) = arma::dot(arma::inv(M.slice(0)), x.col(0));  
    params.col(1) = arma::dot(arma::inv(M.slice(0)), x.col(0));  
    params.col(2) = arma::dot(arma::inv(M.slice(0)), x.col(0));  
}

arma::vec3 Spline::position(float t) const {
    // use value of i-1 at loop exit as partition,
    // clipped to 0 and n-2
    int i;
    for(i = 0; i<n-1; i++) {
        if(t < times[i])
            break;
    }

    int part = i - 1;
    part = (part < 0) ? 0 : (part > n-2) ? n-2  : part;

    // turn partition into index
    part = part*4;


    return params.row(part) + params.row(part+1)*t + params.row(part+2)*t*t + params.row(part+3)*t*t*t;
}

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
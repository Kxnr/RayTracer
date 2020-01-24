#include "path.h"

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
            M.tube(r, part) = 1;
            M.tube(r, part + 1) = times[part];
            M.tube(r, part + 2) = times[part] * times[part];
            M.tube(r, part + 3) = times[part] * times[part] * times[part];
        }
        else if (cond == 1)
        {
            // C0(ui+1)
            // assign vector with
            x.row(r) = pts[r + 1];

            // assign matrix
            M.tube(r, part) = 1;
            M.tube(r, part + 1) = times[part + 1];
            M.tube(r, part + 2) = times[part + 1] * times[part + 1];
            M.tube(r, part + 3) = times[part + 1] * times[part + 1] * times[part + 1];
        }
        else if (cond == 2)
        {
            // C1(ui+1)
            // vector initialized to 0

            // assign matrix
            arma::vec3 dx0 = pts[part + 1] - pts[part];
            float du0 = times[part + 1] - times[part];

            arma::vec3 dx1 = pts[part + 2] - pts[part + 1];
            float du1 = times[part + 2] - times[part + 1];

            M.tube(r, part + 1) = (dx1 / du1);
            M.tube(r, part + 2) = 2 * times[part + 1] * (dx1 / du1);
            M.tube(r, part + 3) = 3 * times[part + 1] * times[part + 1] * (dx1 / du1);
            M.tube(r, part + 5) = -1 * (dx0 / du0);
            M.tube(r, part + 6) = -2 * times[part + 1] * (dx0 / du0);
            M.tube(r, part + 7) = -3 * times[part + 1] * times[part + 1] * (dx0 / du0);
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

            M.tube(r, part + 2) = 2 * (dx1 / du1) * (dx1 / du1);
            M.tube(r, part + 3) = 6 * times[part + 1] * (dx1 / du1) * (dx1 / du1);
            M.tube(r, part + 6) = -2 * (dx0 / du0) * (dx1 / du1);
            M.tube(r, part + 7) = -6 * times[part + 1] * (dx0 / du0) * (dx1 / du1);
        }
    }

    // TODO: // slope constraints

    arma::mat params = arma::mat(4 * n - 4, 3);
    params.col(0) = arma::dot(arma::inv(M.slice(0)), x.col(0));
    params.col(1) = arma::dot(arma::inv(M.slice(0)), x.col(0));
    params.col(2) = arma::dot(arma::inv(M.slice(0)), x.col(0));
}

vec3 Spline::position(float t) const {
    // use value of i-1 at loop exit as partition,
    // clipped to 0 and n-2
    int i;
    for (i = 0; i < n - 1; i++)
    {
        if (t < times[i])
            break;
    }

    int part = i - 1;
    part = (part < 0) ? 0 : (part > n - 2) ? n - 2 : part;

    // turn partition into index
    part = part * 4;

    arma::vec3 out = params.row(part) + params.row(part+1)*t + params.row(part+2)*t*t + params.row(part+3)*t*t*t;
    return vec3(out[0], out[1], out[2]);
}
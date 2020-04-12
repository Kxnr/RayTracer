#include "vec3.h"

#define STRETCH_CONSTANT_4D (std::sqrt(5.0f) - 1.0f) / 4.0f
#define SQUISH_CONSTANT_4D (1.0f / (std::sqrt(5.0f) - 1.0f)) / 4.0f
#define NORM_CONSTANT_4D 30

class Noise
{
    public:
        Noise() {}
        ~Noise() {}

        virtual float Noise2D(float x, float y);
        virtual float Noise3D(float x, float y, float z);
        virtual float Noise4D(float x, float y, float z, float w);
};

class Simplex : Noise
{
public:
    Simplex(long seed)
    {
        int source[256];
        for(int i =0; i < 256; i++) {
            source[i] = i;
        }

        // Not really sure what these magic numbers are
        seed = seed * 6364136223846793005l + 1442695040888963407l;
		seed = seed * 6364136223846793005l + 1442695040888963407l;
		seed = seed * 6364136223846793005l + 1442695040888963407l;

        for (int i = 255; i >= 0; i--) {
            seed = seed * 6364136223846793005l + 1442695040888963407l;
            int r = (int)((seed + 31) % (i+1));
            if(r < 0) {
                r += (i+1);
            } 

            perm[i] = source[r];
            source[r] = source[i];
        }
        // generate noise
    }

    ~Simplex()
    {
    }

    virtual float Noise2D(float x, float y) {
        return GetValue(vec3(x, y, 0), 0);
    }

    virtual float Noise3D(float x, float y, float z) {
        return GetValue(vec3(x, y, z), 0);
    }

    virtual float Noise4D(float x, float y, float z, float w) {
        return GetValue(vec3(x, y, z), w);
    }

private:
    int perm[256];

    const int8_t gradients4D[16*16] = {
        3,  1,  1,  1,      1,  3,  1,  1,      1,  1,  3,  1,      1,  1,  1,  3,
        -3,  1,  1,  1,     -1,  3,  1,  1,     -1,  1,  3,  1,     -1,  1,  1,  3,
        3, -1,  1,  1,      1, -3,  1,  1,      1, -1,  3,  1,      1, -1,  1,  3,
        -3, -1,  1,  1,     -1, -3,  1,  1,     -1, -1,  3,  1,     -1, -1,  1,  3,
        3,  1, -1,  1,      1,  3, -1,  1,      1,  1, -3,  1,      1,  1, -1,  3,
        -3,  1, -1,  1,     -1,  3, -1,  1,     -1,  1, -3,  1,     -1,  1, -1,  3,
        3, -1, -1,  1,      1, -3, -1,  1,      1, -1, -3,  1,      1, -1, -1,  3,
        -3, -1, -1,  1,     -1, -3, -1,  1,     -1, -1, -3,  1,     -1, -1, -1,  3,
        3,  1,  1, -1,      1,  3,  1, -1,      1,  1,  3, -1,      1,  1,  1, -3,
        -3,  1,  1, -1,     -1,  3,  1, -1,     -1,  1,  3, -1,     -1,  1,  1, -3,
        3, -1,  1, -1,      1, -3,  1, -1,      1, -1,  3, -1,      1, -1,  1, -3,
        -3, -1,  1, -1,     -1, -3,  1, -1,     -1, -1,  3, -1,     -1, -1,  1, -3,
        3,  1, -1, -1,      1,  3, -1, -1,      1,  1, -3, -1,      1,  1, -1, -3,
        -3,  1, -1, -1,     -1,  3, -1, -1,     -1,  1, -3, -1,     -1,  1, -1, -3,
        3, -1, -1, -1,      1, -3, -1, -1,      1, -1, -3, -1,      1, -1, -1, -3,
        -3, -1, -1, -1,     -1, -3, -1, -1,     -1, -1, -3, -1,     -1, -1, -1, -3,
    };

    double extrapolate(int xsb, int ysb, int zsb, int wsb, double dx, double dy, double dz, double dw)
    {
        int index = perm[(perm[(perm[(perm[xsb & 0xFF] + ysb) & 0xFF] + zsb) & 0xFF] + wsb) & 0xFF] & 0xFC;
        return gradients4D[index] * dx + gradients4D[index + 1] * dy + gradients4D[index + 2] * dz + gradients4D[index + 3] * dw;
    }

    // https://gist.github.com/KdotJPG/b1270127455a94ac5d19
    float GetValue(const vec3 pos, const float t)
    {
        float x = pos.x;
        float y = pos.y;
        float z = pos.z;

        // skew coordinates to rectilinear
        float F = (std::sqrt(5.0f) - 1.0f) / 4.0f;
        float stretchOffset = (x + y + z + t) * F;
        float xs = x * F;
        float ys = y * F;
        float zs = z * F;
        float ts = t * F;

        // get coordinates of simplectic honeycomb
        int xsb = (int)xs;
        int ysb = (int)ys;
        int zsb = (int)zs;
        int tsb = (int)ts; // time simplex?

        // transform into simplectic space
        float Finv = (xsb + ysb + zsb + tsb) / F;
        int xb = xsb + Finv;
        int yb = ysb + Finv;
        int zb = zsb + Finv;
        int tb = tsb + Finv;

        // find relative position within simplex
        float xins = xs - xsb;
        float yins = ys - ysb;
        float zins = zs - zsb;
        float tins = ts - tsb;
        float inSum = xins + yins + zins + tins;

        // position relative to origin
        float dx0 = x - xb;
        float dy0 = y - yb;
        float dz0 = z - zb;
        float dt0 = x - xb;

        // variable declarations for following block
        float dx_ext0, dy_ext0, dz_ext0, dw_ext0;
        float dx_ext1, dy_ext1, dz_ext1, dw_ext1;
        float dx_ext2, dy_ext2, dz_ext2, dw_ext2;
        int xsv_ext0, ysv_ext0, zsv_ext0, wsv_ext0;
        int xsv_ext1, ysv_ext1, zsv_ext1, wsv_ext1;
        int xsv_ext2, ysv_ext2, zsv_ext2, wsv_ext2;

        int i = 0; // single loop index;
        float value = 0;
        if (inSum <= 1)
        { //We're inside the pentachoron (4-Simplex) at (0,0,0,0)

            //Determine which two of (0,0,0,1), (0,0,1,0), (0,1,0,0), (1,0,0,0) are closest.
            uint8_t aPoint = 0x01;
            float aScore = xins;
            uint8_t bPoint = 0x02;
            float bScore = yins;
            if (aScore >= bScore && zins > bScore)
            {
                bScore = zins;
                bPoint = 0x04;
            }
            else if (aScore < bScore && zins > aScore)
            {
                aScore = zins;
                aPoint = 0x04;
            }
            if (aScore >= bScore && tins > bScore)
            {
                bScore = tins;
                bPoint = 0x08;
            }
            else if (aScore < bScore && tins > aScore)
            {
                aScore = tins;
                aPoint = 0x08;
            }

            //Now we determine the three lattice points not part of the pentachoron that may contribute.
            //This depends on the closest two pentachoron vertices, including (0,0,0,0)
            float uins = 1 - inSum;
            if (uins > aScore || uins > bScore)
            {                                                    //(0,0,0,0) is one of the closest two pentachoron vertices.
                uint8_t c = (bScore > aScore ? bPoint : aPoint); //Our other closest vertex is the closest out of a and b.
                if ((c & 0x01) == 0)
                {
                    xsv_ext0 = xsb - 1;
                    xsv_ext1 = xsv_ext2 = xsb;
                    dx_ext0 = dx0 + 1;
                    dx_ext1 = dx_ext2 = dx0;
                }
                else
                {
                    xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb + 1;
                    dx_ext0 = dx_ext1 = dx_ext2 = dx0 - 1;
                }

                if ((c & 0x02) == 0)
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
                    dy_ext0 = dy_ext1 = dy_ext2 = dy0;
                    if ((c & 0x01) == 0x01)
                    {
                        ysv_ext0 -= 1;
                        dy_ext0 += 1;
                    }
                    else
                    {
                        ysv_ext1 -= 1;
                        dy_ext1 += 1;
                    }
                }
                else
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
                    dy_ext0 = dy_ext1 = dy_ext2 = dy0 - 1;
                }

                if ((c & 0x04) == 0)
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
                    dz_ext0 = dz_ext1 = dz_ext2 = dz0;
                    if ((c & 0x03) != 0)
                    {
                        if ((c & 0x03) == 0x03)
                        {
                            zsv_ext0 -= 1;
                            dz_ext0 += 1;
                        }
                        else
                        {
                            zsv_ext1 -= 1;
                            dz_ext1 += 1;
                        }
                    }
                    else
                    {
                        zsv_ext2 -= 1;
                        dz_ext2 += 1;
                    }
                }
                else
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
                    dz_ext0 = dz_ext1 = dz_ext2 = dz0 - 1;
                }

                if ((c & 0x08) == 0)
                {
                    wsv_ext0 = wsv_ext1 = tsb;
                    wsv_ext2 = tsb - 1;
                    dw_ext0 = dw_ext1 = dt0;
                    dw_ext2 = dt0 + 1;
                }
                else
                {
                    wsv_ext0 = wsv_ext1 = wsv_ext2 = tsb + 1;
                    dw_ext0 = dw_ext1 = dw_ext2 = dt0 - 1;
                }
            }
            else
            {                                           //(0,0,0,0) is not one of the closest two pentachoron vertices.
                uint8_t c = (uint8_t)(aPoint | bPoint); //Our three extra vertices are determined by the closest two.

                if ((c & 0x01) == 0)
                {
                    xsv_ext0 = xsv_ext2 = xsb;
                    xsv_ext1 = xsb - 1;
                    dx_ext0 = dx0 - 2 * SQUISH_CONSTANT_4D;
                    dx_ext1 = dx0 + 1 - SQUISH_CONSTANT_4D;
                    dx_ext2 = dx0 - SQUISH_CONSTANT_4D;
                }
                else
                {
                    xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb + 1;
                    dx_ext0 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dx_ext1 = dx_ext2 = dx0 - 1 - SQUISH_CONSTANT_4D;
                }

                if ((c & 0x02) == 0)
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
                    dy_ext0 = dy0 - 2 * SQUISH_CONSTANT_4D;
                    dy_ext1 = dy_ext2 = dy0 - SQUISH_CONSTANT_4D;
                    if ((c & 0x01) == 0x01)
                    {
                        ysv_ext1 -= 1;
                        dy_ext1 += 1;
                    }
                    else
                    {
                        ysv_ext2 -= 1;
                        dy_ext2 += 1;
                    }
                }
                else
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
                    dy_ext0 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dy_ext1 = dy_ext2 = dy0 - 1 - SQUISH_CONSTANT_4D;
                }

                if ((c & 0x04) == 0)
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
                    dz_ext0 = dz0 - 2 * SQUISH_CONSTANT_4D;
                    dz_ext1 = dz_ext2 = dz0 - SQUISH_CONSTANT_4D;
                    if ((c & 0x03) == 0x03)
                    {
                        zsv_ext1 -= 1;
                        dz_ext1 += 1;
                    }
                    else
                    {
                        zsv_ext2 -= 1;
                        dz_ext2 += 1;
                    }
                }
                else
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
                    dz_ext0 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dz_ext1 = dz_ext2 = dz0 - 1 - SQUISH_CONSTANT_4D;
                }

                if ((c & 0x08) == 0)
                {
                    wsv_ext0 = wsv_ext1 = tsb;
                    wsv_ext2 = tsb - 1;
                    dw_ext0 = dt0 - 2 * SQUISH_CONSTANT_4D;
                    dw_ext1 = dt0 - SQUISH_CONSTANT_4D;
                    dw_ext2 = dt0 + 1 - SQUISH_CONSTANT_4D;
                }
                else
                {
                    wsv_ext0 = wsv_ext1 = wsv_ext2 = tsb + 1;
                    dw_ext0 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dw_ext1 = dw_ext2 = dt0 - 1 - SQUISH_CONSTANT_4D;
                }
            }

            //Contribution (0,0,0,0)
            float attn0 = 2 - dx0 * dx0 - dy0 * dy0 - dz0 * dz0 - dt0 * dt0;
            if (attn0 > 0)
            {
                attn0 *= attn0;
                value += attn0 * attn0 * extrapolate(xsb + 0, ysb + 0, zsb + 0, tsb + 0, dx0, dy0, dz0, dt0);
            }

            //Contribution (1,0,0,0)
            float dx1 = dx0 - 1 - SQUISH_CONSTANT_4D;
            float dy1 = dy0 - 0 - SQUISH_CONSTANT_4D;
            float dz1 = dz0 - 0 - SQUISH_CONSTANT_4D;
            float dw1 = dt0 - 0 - SQUISH_CONSTANT_4D;
            float attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1;
            if (attn1 > 0)
            {
                attn1 *= attn1;
                value += attn1 * attn1 * extrapolate(xsb + 1, ysb + 0, zsb + 0, tsb + 0, dx1, dy1, dz1, dw1);
            }

            //Contribution (0,1,0,0)
            float dx2 = dx0 - 0 - SQUISH_CONSTANT_4D;
            float dy2 = dy0 - 1 - SQUISH_CONSTANT_4D;
            float dz2 = dz1;
            float dw2 = dw1;
            float attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2;
            if (attn2 > 0)
            {
                attn2 *= attn2;
                value += attn2 * attn2 * extrapolate(xsb + 0, ysb + 1, zsb + 0, tsb + 0, dx2, dy2, dz2, dw2);
            }

            //Contribution (0,0,1,0)
            float dx3 = dx2;
            float dy3 = dy1;
            float dz3 = dz0 - 1 - SQUISH_CONSTANT_4D;
            float dw3 = dw1;
            float attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3;
            if (attn3 > 0)
            {
                attn3 *= attn3;
                value += attn3 * attn3 * extrapolate(xsb + 0, ysb + 0, zsb + 1, tsb + 0, dx3, dy3, dz3, dw3);
            }

            //Contribution (0,0,0,1)
            float dx4 = dx2;
            float dy4 = dy1;
            float dz4 = dz1;
            float dw4 = dt0 - 1 - SQUISH_CONSTANT_4D;
            float attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4;
            if (attn4 > 0)
            {
                attn4 *= attn4;
                value += attn4 * attn4 * extrapolate(xsb + 0, ysb + 0, zsb + 0, tsb + 1, dx4, dy4, dz4, dw4);
            }
        }
        else if (inSum >= 3)
        { //We're inside the pentachoron (4-Simplex) at (1,1,1,1)
            //Determine which two of (1,1,1,0), (1,1,0,1), (1,0,1,1), (0,1,1,1) are closest.
            uint8_t aPoint = 0x0E;
            float aScore = xins;
            uint8_t bPoint = 0x0D;
            float bScore = yins;
            if (aScore <= bScore && zins < bScore)
            {
                bScore = zins;
                bPoint = 0x0B;
            }
            else if (aScore > bScore && zins < aScore)
            {
                aScore = zins;
                aPoint = 0x0B;
            }
            if (aScore <= bScore && tins < bScore)
            {
                bScore = tins;
                bPoint = 0x07;
            }
            else if (aScore > bScore && tins < aScore)
            {
                aScore = tins;
                aPoint = 0x07;
            }

            //Now we determine the three lattice points not part of the pentachoron that may contribute.
            //This depends on the closest two pentachoron vertices, including (0,0,0,0)
            float uins = 4 - inSum;
            if (uins < aScore || uins < bScore)
            {                                                    //(1,1,1,1) is one of the closest two pentachoron vertices.
                uint8_t c = (bScore < aScore ? bPoint : aPoint); //Our other closest vertex is the closest out of a and b.

                if ((c & 0x01) != 0)
                {
                    xsv_ext0 = xsb + 2;
                    xsv_ext1 = xsv_ext2 = xsb + 1;
                    dx_ext0 = dx0 - 2 - 4 * SQUISH_CONSTANT_4D;
                    dx_ext1 = dx_ext2 = dx0 - 1 - 4 * SQUISH_CONSTANT_4D;
                }
                else
                {
                    xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb;
                    dx_ext0 = dx_ext1 = dx_ext2 = dx0 - 4 * SQUISH_CONSTANT_4D;
                }

                if ((c & 0x02) != 0)
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
                    dy_ext0 = dy_ext1 = dy_ext2 = dy0 - 1 - 4 * SQUISH_CONSTANT_4D;
                    if ((c & 0x01) != 0)
                    {
                        ysv_ext1 += 1;
                        dy_ext1 -= 1;
                    }
                    else
                    {
                        ysv_ext0 += 1;
                        dy_ext0 -= 1;
                    }
                }
                else
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
                    dy_ext0 = dy_ext1 = dy_ext2 = dy0 - 4 * SQUISH_CONSTANT_4D;
                }

                if ((c & 0x04) != 0)
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
                    dz_ext0 = dz_ext1 = dz_ext2 = dz0 - 1 - 4 * SQUISH_CONSTANT_4D;
                    if ((c & 0x03) != 0x03)
                    {
                        if ((c & 0x03) == 0)
                        {
                            zsv_ext0 += 1;
                            dz_ext0 -= 1;
                        }
                        else
                        {
                            zsv_ext1 += 1;
                            dz_ext1 -= 1;
                        }
                    }
                    else
                    {
                        zsv_ext2 += 1;
                        dz_ext2 -= 1;
                    }
                }
                else
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
                    dz_ext0 = dz_ext1 = dz_ext2 = dz0 - 4 * SQUISH_CONSTANT_4D;
                }

                if ((c & 0x08) != 0)
                {
                    wsv_ext0 = wsv_ext1 = tsb + 1;
                    wsv_ext2 = tsb + 2;
                    dw_ext0 = dw_ext1 = dt0 - 1 - 4 * SQUISH_CONSTANT_4D;
                    dw_ext2 = dt0 - 2 - 4 * SQUISH_CONSTANT_4D;
                }
                else
                {
                    wsv_ext0 = wsv_ext1 = wsv_ext2 = tsb;
                    dw_ext0 = dw_ext1 = dw_ext2 = dt0 - 4 * SQUISH_CONSTANT_4D;
                }
            }
            else
            {                                           //(1,1,1,1) is not one of the closest two pentachoron vertices.
                uint8_t c = (uint8_t)(aPoint & bPoint); //Our three extra vertices are determined by the closest two.

                if ((c & 0x01) != 0)
                {
                    xsv_ext0 = xsv_ext2 = xsb + 1;
                    xsv_ext1 = xsb + 2;
                    dx_ext0 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dx_ext1 = dx0 - 2 - 3 * SQUISH_CONSTANT_4D;
                    dx_ext2 = dx0 - 1 - 3 * SQUISH_CONSTANT_4D;
                }
                else
                {
                    xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb;
                    dx_ext0 = dx0 - 2 * SQUISH_CONSTANT_4D;
                    dx_ext1 = dx_ext2 = dx0 - 3 * SQUISH_CONSTANT_4D;
                }

                if ((c & 0x02) != 0)
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
                    dy_ext0 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dy_ext1 = dy_ext2 = dy0 - 1 - 3 * SQUISH_CONSTANT_4D;
                    if ((c & 0x01) != 0)
                    {
                        ysv_ext2 += 1;
                        dy_ext2 -= 1;
                    }
                    else
                    {
                        ysv_ext1 += 1;
                        dy_ext1 -= 1;
                    }
                }
                else
                {
                    ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
                    dy_ext0 = dy0 - 2 * SQUISH_CONSTANT_4D;
                    dy_ext1 = dy_ext2 = dy0 - 3 * SQUISH_CONSTANT_4D;
                }

                if ((c & 0x04) != 0)
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
                    dz_ext0 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dz_ext1 = dz_ext2 = dz0 - 1 - 3 * SQUISH_CONSTANT_4D;
                    if ((c & 0x03) != 0)
                    {
                        zsv_ext2 += 1;
                        dz_ext2 -= 1;
                    }
                    else
                    {
                        zsv_ext1 += 1;
                        dz_ext1 -= 1;
                    }
                }
                else
                {
                    zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
                    dz_ext0 = dz0 - 2 * SQUISH_CONSTANT_4D;
                    dz_ext1 = dz_ext2 = dz0 - 3 * SQUISH_CONSTANT_4D;
                }

                if ((c & 0x08) != 0)
                {
                    wsv_ext0 = wsv_ext1 = tsb + 1;
                    wsv_ext2 = tsb + 2;
                    dw_ext0 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dw_ext1 = dt0 - 1 - 3 * SQUISH_CONSTANT_4D;
                    dw_ext2 = dt0 - 2 - 3 * SQUISH_CONSTANT_4D;
                }
                else
                {
                    wsv_ext0 = wsv_ext1 = wsv_ext2 = tsb;
                    dw_ext0 = dt0 - 2 * SQUISH_CONSTANT_4D;
                    dw_ext1 = dw_ext2 = dt0 - 3 * SQUISH_CONSTANT_4D;
                }
            }

            //Contribution (1,1,1,0)
            float dx4 = dx0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float dy4 = dy0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float dz4 = dz0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float dw4 = dt0 - 3 * SQUISH_CONSTANT_4D;
            float attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4;
            if (attn4 > 0)
            {
                attn4 *= attn4;
                value += attn4 * attn4 * extrapolate(xsb + 1, ysb + 1, zsb + 1, tsb + 0, dx4, dy4, dz4, dw4);
            }

            //Contribution (1,1,0,1)
            float dx3 = dx4;
            float dy3 = dy4;
            float dz3 = dz0 - 3 * SQUISH_CONSTANT_4D;
            float dw3 = dt0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3;
            if (attn3 > 0)
            {
                attn3 *= attn3;
                value += attn3 * attn3 * extrapolate(xsb + 1, ysb + 1, zsb + 0, tsb + 1, dx3, dy3, dz3, dw3);
            }

            //Contribution (1,0,1,1)
            float dx2 = dx4;
            float dy2 = dy0 - 3 * SQUISH_CONSTANT_4D;
            float dz2 = dz4;
            float dw2 = dw3;
            float attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2;
            if (attn2 > 0)
            {
                attn2 *= attn2;
                value += attn2 * attn2 * extrapolate(xsb + 1, ysb + 0, zsb + 1, tsb + 1, dx2, dy2, dz2, dw2);
            }

            //Contribution (0,1,1,1)
            float dx1 = dx0 - 3 * SQUISH_CONSTANT_4D;
            float dz1 = dz4;
            float dy1 = dy4;
            float dw1 = dw3;
            float attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1;
            if (attn1 > 0)
            {
                attn1 *= attn1;
                value += attn1 * attn1 * extrapolate(xsb + 0, ysb + 1, zsb + 1, tsb + 1, dx1, dy1, dz1, dw1);
            }

            //Contribution (1,1,1,1)
            dx0 = dx0 - 1 - 4 * SQUISH_CONSTANT_4D;
            dy0 = dy0 - 1 - 4 * SQUISH_CONSTANT_4D;
            dz0 = dz0 - 1 - 4 * SQUISH_CONSTANT_4D;
            dt0 = dt0 - 1 - 4 * SQUISH_CONSTANT_4D;
            float attn0 = 2 - dx0 * dx0 - dy0 * dy0 - dz0 * dz0 - dt0 * dt0;
            if (attn0 > 0)
            {
                attn0 *= attn0;
                value += attn0 * attn0 * extrapolate(xsb + 1, ysb + 1, zsb + 1, tsb + 1, dx0, dy0, dz0, dt0);
            }
        }
        else if (inSum <= 2)
        { //We're inside the first dispentachoron (Rectified 4-Simplex)
            float aScore;
            uint8_t aPoint;
            bool aIsBiggerSide = true;
            float bScore;
            uint8_t bPoint;
            bool bIsBiggerSide = true;

            //Decide between (1,1,0,0) and (0,0,1,1)
            if (xins + yins > zins + tins)
            {
                aScore = xins + yins;
                aPoint = 0x03;
            }
            else
            {
                aScore = zins + tins;
                aPoint = 0x0C;
            }

            //Decide between (1,0,1,0) and (0,1,0,1)
            if (xins + zins > yins + tins)
            {
                bScore = xins + zins;
                bPoint = 0x05;
            }
            else
            {
                bScore = yins + tins;
                bPoint = 0x0A;
            }

            //Closer between (1,0,0,1) and (0,1,1,0) will replace the further of a and b, if closer.
            if (xins + tins > yins + zins)
            {
                float score = xins + tins;
                if (aScore >= bScore && score > bScore)
                {
                    bScore = score;
                    bPoint = 0x09;
                }
                else if (aScore < bScore && score > aScore)
                {
                    aScore = score;
                    aPoint = 0x09;
                }
            }
            else
            {
                float score = yins + zins;
                if (aScore >= bScore && score > bScore)
                {
                    bScore = score;
                    bPoint = 0x06;
                }
                else if (aScore < bScore && score > aScore)
                {
                    aScore = score;
                    aPoint = 0x06;
                }
            }

            //Decide if (1,0,0,0) is closer.
            float p1 = 2 - inSum + xins;
            if (aScore >= bScore && p1 > bScore)
            {
                bScore = p1;
                bPoint = 0x01;
                bIsBiggerSide = false;
            }
            else if (aScore < bScore && p1 > aScore)
            {
                aScore = p1;
                aPoint = 0x01;
                aIsBiggerSide = false;
            }

            //Decide if (0,1,0,0) is closer.
            float p2 = 2 - inSum + yins;
            if (aScore >= bScore && p2 > bScore)
            {
                bScore = p2;
                bPoint = 0x02;
                bIsBiggerSide = false;
            }
            else if (aScore < bScore && p2 > aScore)
            {
                aScore = p2;
                aPoint = 0x02;
                aIsBiggerSide = false;
            }

            //Decide if (0,0,1,0) is closer.
            float p3 = 2 - inSum + zins;
            if (aScore >= bScore && p3 > bScore)
            {
                bScore = p3;
                bPoint = 0x04;
                bIsBiggerSide = false;
            }
            else if (aScore < bScore && p3 > aScore)
            {
                aScore = p3;
                aPoint = 0x04;
                aIsBiggerSide = false;
            }

            //Decide if (0,0,0,1) is closer.
            float p4 = 2 - inSum + tins;
            if (aScore >= bScore && p4 > bScore)
            {
                bScore = p4;
                bPoint = 0x08;
                bIsBiggerSide = false;
            }
            else if (aScore < bScore && p4 > aScore)
            {
                aScore = p4;
                aPoint = 0x08;
                aIsBiggerSide = false;
            }

            //Where each of the two closest points are determines how the extra three vertices are calculated.
            if (aIsBiggerSide == bIsBiggerSide)
            {
                if (aIsBiggerSide)
                { //Both closest points on the bigger side
                    uint8_t c1 = (uint8_t)(aPoint | bPoint);
                    uint8_t c2 = (uint8_t)(aPoint & bPoint);
                    if ((c1 & 0x01) == 0)
                    {
                        xsv_ext0 = xsb;
                        xsv_ext1 = xsb - 1;
                        dx_ext0 = dx0 - 3 * SQUISH_CONSTANT_4D;
                        dx_ext1 = dx0 + 1 - 2 * SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        xsv_ext0 = xsv_ext1 = xsb + 1;
                        dx_ext0 = dx0 - 1 - 3 * SQUISH_CONSTANT_4D;
                        dx_ext1 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    }

                    if ((c1 & 0x02) == 0)
                    {
                        ysv_ext0 = ysb;
                        ysv_ext1 = ysb - 1;
                        dy_ext0 = dy0 - 3 * SQUISH_CONSTANT_4D;
                        dy_ext1 = dy0 + 1 - 2 * SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        ysv_ext0 = ysv_ext1 = ysb + 1;
                        dy_ext0 = dy0 - 1 - 3 * SQUISH_CONSTANT_4D;
                        dy_ext1 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    }

                    if ((c1 & 0x04) == 0)
                    {
                        zsv_ext0 = zsb;
                        zsv_ext1 = zsb - 1;
                        dz_ext0 = dz0 - 3 * SQUISH_CONSTANT_4D;
                        dz_ext1 = dz0 + 1 - 2 * SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        zsv_ext0 = zsv_ext1 = zsb + 1;
                        dz_ext0 = dz0 - 1 - 3 * SQUISH_CONSTANT_4D;
                        dz_ext1 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    }

                    if ((c1 & 0x08) == 0)
                    {
                        wsv_ext0 = tsb;
                        wsv_ext1 = tsb - 1;
                        dw_ext0 = dt0 - 3 * SQUISH_CONSTANT_4D;
                        dw_ext1 = dt0 + 1 - 2 * SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        wsv_ext0 = wsv_ext1 = tsb + 1;
                        dw_ext0 = dt0 - 1 - 3 * SQUISH_CONSTANT_4D;
                        dw_ext1 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    }

                    //One combination is a permutation of (0,0,0,2) based on c2
                    xsv_ext2 = xsb;
                    ysv_ext2 = ysb;
                    zsv_ext2 = zsb;
                    wsv_ext2 = tsb;
                    dx_ext2 = dx0 - 2 * SQUISH_CONSTANT_4D;
                    dy_ext2 = dy0 - 2 * SQUISH_CONSTANT_4D;
                    dz_ext2 = dz0 - 2 * SQUISH_CONSTANT_4D;
                    dw_ext2 = dt0 - 2 * SQUISH_CONSTANT_4D;
                    if ((c2 & 0x01) != 0)
                    {
                        xsv_ext2 += 2;
                        dx_ext2 -= 2;
                    }
                    else if ((c2 & 0x02) != 0)
                    {
                        ysv_ext2 += 2;
                        dy_ext2 -= 2;
                    }
                    else if ((c2 & 0x04) != 0)
                    {
                        zsv_ext2 += 2;
                        dz_ext2 -= 2;
                    }
                    else
                    {
                        wsv_ext2 += 2;
                        dw_ext2 -= 2;
                    }
                }
                else
                { //Both closest points on the smaller side
                    //One of the two extra points is (0,0,0,0)
                    xsv_ext2 = xsb;
                    ysv_ext2 = ysb;
                    zsv_ext2 = zsb;
                    wsv_ext2 = tsb;
                    dx_ext2 = dx0;
                    dy_ext2 = dy0;
                    dz_ext2 = dz0;
                    dw_ext2 = dt0;

                    //Other two points are based on the omitted axes.
                    uint8_t c = (uint8_t)(aPoint | bPoint);

                    if ((c & 0x01) == 0)
                    {
                        xsv_ext0 = xsb - 1;
                        xsv_ext1 = xsb;
                        dx_ext0 = dx0 + 1 - SQUISH_CONSTANT_4D;
                        dx_ext1 = dx0 - SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        xsv_ext0 = xsv_ext1 = xsb + 1;
                        dx_ext0 = dx_ext1 = dx0 - 1 - SQUISH_CONSTANT_4D;
                    }

                    if ((c & 0x02) == 0)
                    {
                        ysv_ext0 = ysv_ext1 = ysb;
                        dy_ext0 = dy_ext1 = dy0 - SQUISH_CONSTANT_4D;
                        if ((c & 0x01) == 0x01)
                        {
                            ysv_ext0 -= 1;
                            dy_ext0 += 1;
                        }
                        else
                        {
                            ysv_ext1 -= 1;
                            dy_ext1 += 1;
                        }
                    }
                    else
                    {
                        ysv_ext0 = ysv_ext1 = ysb + 1;
                        dy_ext0 = dy_ext1 = dy0 - 1 - SQUISH_CONSTANT_4D;
                    }

                    if ((c & 0x04) == 0)
                    {
                        zsv_ext0 = zsv_ext1 = zsb;
                        dz_ext0 = dz_ext1 = dz0 - SQUISH_CONSTANT_4D;
                        if ((c & 0x03) == 0x03)
                        {
                            zsv_ext0 -= 1;
                            dz_ext0 += 1;
                        }
                        else
                        {
                            zsv_ext1 -= 1;
                            dz_ext1 += 1;
                        }
                    }
                    else
                    {
                        zsv_ext0 = zsv_ext1 = zsb + 1;
                        dz_ext0 = dz_ext1 = dz0 - 1 - SQUISH_CONSTANT_4D;
                    }

                    if ((c & 0x08) == 0)
                    {
                        wsv_ext0 = tsb;
                        wsv_ext1 = tsb - 1;
                        dw_ext0 = dt0 - SQUISH_CONSTANT_4D;
                        dw_ext1 = dt0 + 1 - SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        wsv_ext0 = wsv_ext1 = tsb + 1;
                        dw_ext0 = dw_ext1 = dt0 - 1 - SQUISH_CONSTANT_4D;
                    }
                }
            }
            else
            { //One point on each "side"
                uint8_t c1, c2;
                if (aIsBiggerSide)
                {
                    c1 = aPoint;
                    c2 = bPoint;
                }
                else
                {
                    c1 = bPoint;
                    c2 = aPoint;
                }

                //Two contributions are the bigger-sided point with each 0 replaced with -1.
                if ((c1 & 0x01) == 0)
                {
                    xsv_ext0 = xsb - 1;
                    xsv_ext1 = xsb;
                    dx_ext0 = dx0 + 1 - SQUISH_CONSTANT_4D;
                    dx_ext1 = dx0 - SQUISH_CONSTANT_4D;
                }
                else
                {
                    xsv_ext0 = xsv_ext1 = xsb + 1;
                    dx_ext0 = dx_ext1 = dx0 - 1 - SQUISH_CONSTANT_4D;
                }

                if ((c1 & 0x02) == 0)
                {
                    ysv_ext0 = ysv_ext1 = ysb;
                    dy_ext0 = dy_ext1 = dy0 - SQUISH_CONSTANT_4D;
                    if ((c1 & 0x01) == 0x01)
                    {
                        ysv_ext0 -= 1;
                        dy_ext0 += 1;
                    }
                    else
                    {
                        ysv_ext1 -= 1;
                        dy_ext1 += 1;
                    }
                }
                else
                {
                    ysv_ext0 = ysv_ext1 = ysb + 1;
                    dy_ext0 = dy_ext1 = dy0 - 1 - SQUISH_CONSTANT_4D;
                }

                if ((c1 & 0x04) == 0)
                {
                    zsv_ext0 = zsv_ext1 = zsb;
                    dz_ext0 = dz_ext1 = dz0 - SQUISH_CONSTANT_4D;
                    if ((c1 & 0x03) == 0x03)
                    {
                        zsv_ext0 -= 1;
                        dz_ext0 += 1;
                    }
                    else
                    {
                        zsv_ext1 -= 1;
                        dz_ext1 += 1;
                    }
                }
                else
                {
                    zsv_ext0 = zsv_ext1 = zsb + 1;
                    dz_ext0 = dz_ext1 = dz0 - 1 - SQUISH_CONSTANT_4D;
                }

                if ((c1 & 0x08) == 0)
                {
                    wsv_ext0 = tsb;
                    wsv_ext1 = tsb - 1;
                    dw_ext0 = dt0 - SQUISH_CONSTANT_4D;
                    dw_ext1 = dt0 + 1 - SQUISH_CONSTANT_4D;
                }
                else
                {
                    wsv_ext0 = wsv_ext1 = tsb + 1;
                    dw_ext0 = dw_ext1 = dt0 - 1 - SQUISH_CONSTANT_4D;
                }

                //One contribution is a permutation of (0,0,0,2) based on the smaller-sided point
                xsv_ext2 = xsb;
                ysv_ext2 = ysb;
                zsv_ext2 = zsb;
                wsv_ext2 = tsb;
                dx_ext2 = dx0 - 2 * SQUISH_CONSTANT_4D;
                dy_ext2 = dy0 - 2 * SQUISH_CONSTANT_4D;
                dz_ext2 = dz0 - 2 * SQUISH_CONSTANT_4D;
                dw_ext2 = dt0 - 2 * SQUISH_CONSTANT_4D;
                if ((c2 & 0x01) != 0)
                {
                    xsv_ext2 += 2;
                    dx_ext2 -= 2;
                }
                else if ((c2 & 0x02) != 0)
                {
                    ysv_ext2 += 2;
                    dy_ext2 -= 2;
                }
                else if ((c2 & 0x04) != 0)
                {
                    zsv_ext2 += 2;
                    dz_ext2 -= 2;
                }
                else
                {
                    wsv_ext2 += 2;
                    dw_ext2 -= 2;
                }
            }

            //Contribution (1,0,0,0)
            float dx1 = dx0 - 1 - SQUISH_CONSTANT_4D;
            float dy1 = dy0 - 0 - SQUISH_CONSTANT_4D;
            float dz1 = dz0 - 0 - SQUISH_CONSTANT_4D;
            float dw1 = dt0 - 0 - SQUISH_CONSTANT_4D;
            float attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1;
            if (attn1 > 0)
            {
                attn1 *= attn1;
                value += attn1 * attn1 * extrapolate(xsb + 1, ysb + 0, zsb + 0, tsb + 0, dx1, dy1, dz1, dw1);
            }

            //Contribution (0,1,0,0)
            float dx2 = dx0 - 0 - SQUISH_CONSTANT_4D;
            float dy2 = dy0 - 1 - SQUISH_CONSTANT_4D;
            float dz2 = dz1;
            float dw2 = dw1;
            float attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2;
            if (attn2 > 0)
            {
                attn2 *= attn2;
                value += attn2 * attn2 * extrapolate(xsb + 0, ysb + 1, zsb + 0, tsb + 0, dx2, dy2, dz2, dw2);
            }

            //Contribution (0,0,1,0)
            float dx3 = dx2;
            float dy3 = dy1;
            float dz3 = dz0 - 1 - SQUISH_CONSTANT_4D;
            float dw3 = dw1;
            float attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3;
            if (attn3 > 0)
            {
                attn3 *= attn3;
                value += attn3 * attn3 * extrapolate(xsb + 0, ysb + 0, zsb + 1, tsb + 0, dx3, dy3, dz3, dw3);
            }

            //Contribution (0,0,0,1)
            float dx4 = dx2;
            float dy4 = dy1;
            float dz4 = dz1;
            float dw4 = dt0 - 1 - SQUISH_CONSTANT_4D;
            float attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4;
            if (attn4 > 0)
            {
                attn4 *= attn4;
                value += attn4 * attn4 * extrapolate(xsb + 0, ysb + 0, zsb + 0, tsb + 1, dx4, dy4, dz4, dw4);
            }

            //Contribution (1,1,0,0)
            float dx5 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dy5 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dz5 = dz0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dw5 = dt0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float attn5 = 2 - dx5 * dx5 - dy5 * dy5 - dz5 * dz5 - dw5 * dw5;
            if (attn5 > 0)
            {
                attn5 *= attn5;
                value += attn5 * attn5 * extrapolate(xsb + 1, ysb + 1, zsb + 0, tsb + 0, dx5, dy5, dz5, dw5);
            }

            //Contribution (1,0,1,0)
            float dx6 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dy6 = dy0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dz6 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dw6 = dt0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float attn6 = 2 - dx6 * dx6 - dy6 * dy6 - dz6 * dz6 - dw6 * dw6;
            if (attn6 > 0)
            {
                attn6 *= attn6;
                value += attn6 * attn6 * extrapolate(xsb + 1, ysb + 0, zsb + 1, tsb + 0, dx6, dy6, dz6, dw6);
            }

            //Contribution (1,0,0,1)
            float dx7 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dy7 = dy0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dz7 = dz0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dw7 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float attn7 = 2 - dx7 * dx7 - dy7 * dy7 - dz7 * dz7 - dw7 * dw7;
            if (attn7 > 0)
            {
                attn7 *= attn7;
                value += attn7 * attn7 * extrapolate(xsb + 1, ysb + 0, zsb + 0, tsb + 1, dx7, dy7, dz7, dw7);
            }

            //Contribution (0,1,1,0)
            float dx8 = dx0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dy8 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dz8 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dw8 = dt0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float attn8 = 2 - dx8 * dx8 - dy8 * dy8 - dz8 * dz8 - dw8 * dw8;
            if (attn8 > 0)
            {
                attn8 *= attn8;
                value += attn8 * attn8 * extrapolate(xsb + 0, ysb + 1, zsb + 1, tsb + 0, dx8, dy8, dz8, dw8);
            }

            //Contribution (0,1,0,1)
            float dx9 = dx0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dy9 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dz9 = dz0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dw9 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float attn9 = 2 - dx9 * dx9 - dy9 * dy9 - dz9 * dz9 - dw9 * dw9;
            if (attn9 > 0)
            {
                attn9 *= attn9;
                value += attn9 * attn9 * extrapolate(xsb + 0, ysb + 1, zsb + 0, tsb + 1, dx9, dy9, dz9, dw9);
            }

            //Contribution (0,0,1,1)
            float dx10 = dx0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dy10 = dy0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dz10 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dw10 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float attn10 = 2 - dx10 * dx10 - dy10 * dy10 - dz10 * dz10 - dw10 * dw10;
            if (attn10 > 0)
            {
                attn10 *= attn10;
                value += attn10 * attn10 * extrapolate(xsb + 0, ysb + 0, zsb + 1, tsb + 1, dx10, dy10, dz10, dw10);
            }
        }
        else
        { //We're inside the second dispentachoron (Rectified 4-Simplex)
            float aScore;
            uint8_t aPoint;
            bool aIsBiggerSide = true;
            float bScore;
            uint8_t bPoint;
            bool bIsBiggerSide = true;

            //Decide between (0,0,1,1) and (1,1,0,0)
            if (xins + yins < zins + tins)
            {
                aScore = xins + yins;
                aPoint = 0x0C;
            }
            else
            {
                aScore = zins + tins;
                aPoint = 0x03;
            }

            //Decide between (0,1,0,1) and (1,0,1,0)
            if (xins + zins < yins + tins)
            {
                bScore = xins + zins;
                bPoint = 0x0A;
            }
            else
            {
                bScore = yins + tins;
                bPoint = 0x05;
            }

            //Closer between (0,1,1,0) and (1,0,0,1) will replace the further of a and b, if closer.
            if (xins + tins < yins + zins)
            {
                float score = xins + tins;
                if (aScore <= bScore && score < bScore)
                {
                    bScore = score;
                    bPoint = 0x06;
                }
                else if (aScore > bScore && score < aScore)
                {
                    aScore = score;
                    aPoint = 0x06;
                }
            }
            else
            {
                float score = yins + zins;
                if (aScore <= bScore && score < bScore)
                {
                    bScore = score;
                    bPoint = 0x09;
                }
                else if (aScore > bScore && score < aScore)
                {
                    aScore = score;
                    aPoint = 0x09;
                }
            }

            //Decide if (0,1,1,1) is closer.
            float p1 = 3 - inSum + xins;
            if (aScore <= bScore && p1 < bScore)
            {
                bScore = p1;
                bPoint = 0x0E;
                bIsBiggerSide = false;
            }
            else if (aScore > bScore && p1 < aScore)
            {
                aScore = p1;
                aPoint = 0x0E;
                aIsBiggerSide = false;
            }

            //Decide if (1,0,1,1) is closer.
            float p2 = 3 - inSum + yins;
            if (aScore <= bScore && p2 < bScore)
            {
                bScore = p2;
                bPoint = 0x0D;
                bIsBiggerSide = false;
            }
            else if (aScore > bScore && p2 < aScore)
            {
                aScore = p2;
                aPoint = 0x0D;
                aIsBiggerSide = false;
            }

            //Decide if (1,1,0,1) is closer.
            float p3 = 3 - inSum + zins;
            if (aScore <= bScore && p3 < bScore)
            {
                bScore = p3;
                bPoint = 0x0B;
                bIsBiggerSide = false;
            }
            else if (aScore > bScore && p3 < aScore)
            {
                aScore = p3;
                aPoint = 0x0B;
                aIsBiggerSide = false;
            }

            //Decide if (1,1,1,0) is closer.
            float p4 = 3 - inSum + tins;
            if (aScore <= bScore && p4 < bScore)
            {
                bScore = p4;
                bPoint = 0x07;
                bIsBiggerSide = false;
            }
            else if (aScore > bScore && p4 < aScore)
            {
                aScore = p4;
                aPoint = 0x07;
                aIsBiggerSide = false;
            }

            //Where each of the two closest points are determines how the extra three vertices are calculated.
            if (aIsBiggerSide == bIsBiggerSide)
            {
                if (aIsBiggerSide)
                { //Both closest points on the bigger side
                    uint8_t c1 = (uint8_t)(aPoint & bPoint);
                    uint8_t c2 = (uint8_t)(aPoint | bPoint);

                    //Two contributions are permutations of (0,0,0,1) and (0,0,0,2) based on c1
                    xsv_ext0 = xsv_ext1 = xsb;
                    ysv_ext0 = ysv_ext1 = ysb;
                    zsv_ext0 = zsv_ext1 = zsb;
                    wsv_ext0 = wsv_ext1 = tsb;
                    dx_ext0 = dx0 - SQUISH_CONSTANT_4D;
                    dy_ext0 = dy0 - SQUISH_CONSTANT_4D;
                    dz_ext0 = dz0 - SQUISH_CONSTANT_4D;
                    dw_ext0 = dt0 - SQUISH_CONSTANT_4D;
                    dx_ext1 = dx0 - 2 * SQUISH_CONSTANT_4D;
                    dy_ext1 = dy0 - 2 * SQUISH_CONSTANT_4D;
                    dz_ext1 = dz0 - 2 * SQUISH_CONSTANT_4D;
                    dw_ext1 = dt0 - 2 * SQUISH_CONSTANT_4D;
                    if ((c1 & 0x01) != 0)
                    {
                        xsv_ext0 += 1;
                        dx_ext0 -= 1;
                        xsv_ext1 += 2;
                        dx_ext1 -= 2;
                    }
                    else if ((c1 & 0x02) != 0)
                    {
                        ysv_ext0 += 1;
                        dy_ext0 -= 1;
                        ysv_ext1 += 2;
                        dy_ext1 -= 2;
                    }
                    else if ((c1 & 0x04) != 0)
                    {
                        zsv_ext0 += 1;
                        dz_ext0 -= 1;
                        zsv_ext1 += 2;
                        dz_ext1 -= 2;
                    }
                    else
                    {
                        wsv_ext0 += 1;
                        dw_ext0 -= 1;
                        wsv_ext1 += 2;
                        dw_ext1 -= 2;
                    }

                    //One contribution is a permutation of (1,1,1,-1) based on c2
                    xsv_ext2 = xsb + 1;
                    ysv_ext2 = ysb + 1;
                    zsv_ext2 = zsb + 1;
                    wsv_ext2 = tsb + 1;
                    dx_ext2 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dy_ext2 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dz_ext2 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    dw_ext2 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
                    if ((c2 & 0x01) == 0)
                    {
                        xsv_ext2 -= 2;
                        dx_ext2 += 2;
                    }
                    else if ((c2 & 0x02) == 0)
                    {
                        ysv_ext2 -= 2;
                        dy_ext2 += 2;
                    }
                    else if ((c2 & 0x04) == 0)
                    {
                        zsv_ext2 -= 2;
                        dz_ext2 += 2;
                    }
                    else
                    {
                        wsv_ext2 -= 2;
                        dw_ext2 += 2;
                    }
                }
                else
                { //Both closest points on the smaller side
                    //One of the two extra points is (1,1,1,1)
                    xsv_ext2 = xsb + 1;
                    ysv_ext2 = ysb + 1;
                    zsv_ext2 = zsb + 1;
                    wsv_ext2 = tsb + 1;
                    dx_ext2 = dx0 - 1 - 4 * SQUISH_CONSTANT_4D;
                    dy_ext2 = dy0 - 1 - 4 * SQUISH_CONSTANT_4D;
                    dz_ext2 = dz0 - 1 - 4 * SQUISH_CONSTANT_4D;
                    dw_ext2 = dt0 - 1 - 4 * SQUISH_CONSTANT_4D;

                    //Other two points are based on the shared axes.
                    uint8_t c = (uint8_t)(aPoint & bPoint);

                    if ((c & 0x01) != 0)
                    {
                        xsv_ext0 = xsb + 2;
                        xsv_ext1 = xsb + 1;
                        dx_ext0 = dx0 - 2 - 3 * SQUISH_CONSTANT_4D;
                        dx_ext1 = dx0 - 1 - 3 * SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        xsv_ext0 = xsv_ext1 = xsb;
                        dx_ext0 = dx_ext1 = dx0 - 3 * SQUISH_CONSTANT_4D;
                    }

                    if ((c & 0x02) != 0)
                    {
                        ysv_ext0 = ysv_ext1 = ysb + 1;
                        dy_ext0 = dy_ext1 = dy0 - 1 - 3 * SQUISH_CONSTANT_4D;
                        if ((c & 0x01) == 0)
                        {
                            ysv_ext0 += 1;
                            dy_ext0 -= 1;
                        }
                        else
                        {
                            ysv_ext1 += 1;
                            dy_ext1 -= 1;
                        }
                    }
                    else
                    {
                        ysv_ext0 = ysv_ext1 = ysb;
                        dy_ext0 = dy_ext1 = dy0 - 3 * SQUISH_CONSTANT_4D;
                    }

                    if ((c & 0x04) != 0)
                    {
                        zsv_ext0 = zsv_ext1 = zsb + 1;
                        dz_ext0 = dz_ext1 = dz0 - 1 - 3 * SQUISH_CONSTANT_4D;
                        if ((c & 0x03) == 0)
                        {
                            zsv_ext0 += 1;
                            dz_ext0 -= 1;
                        }
                        else
                        {
                            zsv_ext1 += 1;
                            dz_ext1 -= 1;
                        }
                    }
                    else
                    {
                        zsv_ext0 = zsv_ext1 = zsb;
                        dz_ext0 = dz_ext1 = dz0 - 3 * SQUISH_CONSTANT_4D;
                    }

                    if ((c & 0x08) != 0)
                    {
                        wsv_ext0 = tsb + 1;
                        wsv_ext1 = tsb + 2;
                        dw_ext0 = dt0 - 1 - 3 * SQUISH_CONSTANT_4D;
                        dw_ext1 = dt0 - 2 - 3 * SQUISH_CONSTANT_4D;
                    }
                    else
                    {
                        wsv_ext0 = wsv_ext1 = tsb;
                        dw_ext0 = dw_ext1 = dt0 - 3 * SQUISH_CONSTANT_4D;
                    }
                }
            }
            else
            { //One point on each "side"
                uint8_t c1, c2;
                if (aIsBiggerSide)
                {
                    c1 = aPoint;
                    c2 = bPoint;
                }
                else
                {
                    c1 = bPoint;
                    c2 = aPoint;
                }

                //Two contributions are the bigger-sided point with each 1 replaced with 2.
                if ((c1 & 0x01) != 0)
                {
                    xsv_ext0 = xsb + 2;
                    xsv_ext1 = xsb + 1;
                    dx_ext0 = dx0 - 2 - 3 * SQUISH_CONSTANT_4D;
                    dx_ext1 = dx0 - 1 - 3 * SQUISH_CONSTANT_4D;
                }
                else
                {
                    xsv_ext0 = xsv_ext1 = xsb;
                    dx_ext0 = dx_ext1 = dx0 - 3 * SQUISH_CONSTANT_4D;
                }

                if ((c1 & 0x02) != 0)
                {
                    ysv_ext0 = ysv_ext1 = ysb + 1;
                    dy_ext0 = dy_ext1 = dy0 - 1 - 3 * SQUISH_CONSTANT_4D;
                    if ((c1 & 0x01) == 0)
                    {
                        ysv_ext0 += 1;
                        dy_ext0 -= 1;
                    }
                    else
                    {
                        ysv_ext1 += 1;
                        dy_ext1 -= 1;
                    }
                }
                else
                {
                    ysv_ext0 = ysv_ext1 = ysb;
                    dy_ext0 = dy_ext1 = dy0 - 3 * SQUISH_CONSTANT_4D;
                }

                if ((c1 & 0x04) != 0)
                {
                    zsv_ext0 = zsv_ext1 = zsb + 1;
                    dz_ext0 = dz_ext1 = dz0 - 1 - 3 * SQUISH_CONSTANT_4D;
                    if ((c1 & 0x03) == 0)
                    {
                        zsv_ext0 += 1;
                        dz_ext0 -= 1;
                    }
                    else
                    {
                        zsv_ext1 += 1;
                        dz_ext1 -= 1;
                    }
                }
                else
                {
                    zsv_ext0 = zsv_ext1 = zsb;
                    dz_ext0 = dz_ext1 = dz0 - 3 * SQUISH_CONSTANT_4D;
                }

                if ((c1 & 0x08) != 0)
                {
                    wsv_ext0 = tsb + 1;
                    wsv_ext1 = tsb + 2;
                    dw_ext0 = dt0 - 1 - 3 * SQUISH_CONSTANT_4D;
                    dw_ext1 = dt0 - 2 - 3 * SQUISH_CONSTANT_4D;
                }
                else
                {
                    wsv_ext0 = wsv_ext1 = tsb;
                    dw_ext0 = dw_ext1 = dt0 - 3 * SQUISH_CONSTANT_4D;
                }

                //One contribution is a permutation of (1,1,1,-1) based on the smaller-sided point
                xsv_ext2 = xsb + 1;
                ysv_ext2 = ysb + 1;
                zsv_ext2 = zsb + 1;
                wsv_ext2 = tsb + 1;
                dx_ext2 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
                dy_ext2 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
                dz_ext2 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
                dw_ext2 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
                if ((c2 & 0x01) == 0)
                {
                    xsv_ext2 -= 2;
                    dx_ext2 += 2;
                }
                else if ((c2 & 0x02) == 0)
                {
                    ysv_ext2 -= 2;
                    dy_ext2 += 2;
                }
                else if ((c2 & 0x04) == 0)
                {
                    zsv_ext2 -= 2;
                    dz_ext2 += 2;
                }
                else
                {
                    wsv_ext2 -= 2;
                    dw_ext2 += 2;
                }
            }

            //Contribution (1,1,1,0)
            float dx4 = dx0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float dy4 = dy0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float dz4 = dz0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float dw4 = dt0 - 3 * SQUISH_CONSTANT_4D;
            float attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4;
            if (attn4 > 0)
            {
                attn4 *= attn4;
                value += attn4 * attn4 * extrapolate(xsb + 1, ysb + 1, zsb + 1, tsb + 0, dx4, dy4, dz4, dw4);
            }

            //Contribution (1,1,0,1)
            float dx3 = dx4;
            float dy3 = dy4;
            float dz3 = dz0 - 3 * SQUISH_CONSTANT_4D;
            float dw3 = dt0 - 1 - 3 * SQUISH_CONSTANT_4D;
            float attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3;
            if (attn3 > 0)
            {
                attn3 *= attn3;
                value += attn3 * attn3 * extrapolate(xsb + 1, ysb + 1, zsb + 0, tsb + 1, dx3, dy3, dz3, dw3);
            }

            //Contribution (1,0,1,1)
            float dx2 = dx4;
            float dy2 = dy0 - 3 * SQUISH_CONSTANT_4D;
            float dz2 = dz4;
            float dw2 = dw3;
            float attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2;
            if (attn2 > 0)
            {
                attn2 *= attn2;
                value += attn2 * attn2 * extrapolate(xsb + 1, ysb + 0, zsb + 1, tsb + 1, dx2, dy2, dz2, dw2);
            }

            //Contribution (0,1,1,1)
            float dx1 = dx0 - 3 * SQUISH_CONSTANT_4D;
            float dz1 = dz4;
            float dy1 = dy4;
            float dw1 = dw3;
            float attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1;
            if (attn1 > 0)
            {
                attn1 *= attn1;
                value += attn1 * attn1 * extrapolate(xsb + 0, ysb + 1, zsb + 1, tsb + 1, dx1, dy1, dz1, dw1);
            }

            //Contribution (1,1,0,0)
            float dx5 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dy5 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dz5 = dz0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dw5 = dt0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float attn5 = 2 - dx5 * dx5 - dy5 * dy5 - dz5 * dz5 - dw5 * dw5;
            if (attn5 > 0)
            {
                attn5 *= attn5;
                value += attn5 * attn5 * extrapolate(xsb + 1, ysb + 1, zsb + 0, tsb + 0, dx5, dy5, dz5, dw5);
            }

            //Contribution (1,0,1,0)
            float dx6 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dy6 = dy0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dz6 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dw6 = dt0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float attn6 = 2 - dx6 * dx6 - dy6 * dy6 - dz6 * dz6 - dw6 * dw6;
            if (attn6 > 0)
            {
                attn6 *= attn6;
                value += attn6 * attn6 * extrapolate(xsb + 1, ysb + 0, zsb + 1, tsb + 0, dx6, dy6, dz6, dw6);
            }

            //Contribution (1,0,0,1)
            float dx7 = dx0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dy7 = dy0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dz7 = dz0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dw7 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float attn7 = 2 - dx7 * dx7 - dy7 * dy7 - dz7 * dz7 - dw7 * dw7;
            if (attn7 > 0)
            {
                attn7 *= attn7;
                value += attn7 * attn7 * extrapolate(xsb + 1, ysb + 0, zsb + 0, tsb + 1, dx7, dy7, dz7, dw7);
            }

            //Contribution (0,1,1,0)
            float dx8 = dx0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dy8 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dz8 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dw8 = dt0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float attn8 = 2 - dx8 * dx8 - dy8 * dy8 - dz8 * dz8 - dw8 * dw8;
            if (attn8 > 0)
            {
                attn8 *= attn8;
                value += attn8 * attn8 * extrapolate(xsb + 0, ysb + 1, zsb + 1, tsb + 0, dx8, dy8, dz8, dw8);
            }

            //Contribution (0,1,0,1)
            float dx9 = dx0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dy9 = dy0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dz9 = dz0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dw9 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float attn9 = 2 - dx9 * dx9 - dy9 * dy9 - dz9 * dz9 - dw9 * dw9;
            if (attn9 > 0)
            {
                attn9 *= attn9;
                value += attn9 * attn9 * extrapolate(xsb + 0, ysb + 1, zsb + 0, tsb + 1, dx9, dy9, dz9, dw9);
            }

            //Contribution (0,0,1,1)
            float dx10 = dx0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dy10 = dy0 - 0 - 2 * SQUISH_CONSTANT_4D;
            float dz10 = dz0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float dw10 = dt0 - 1 - 2 * SQUISH_CONSTANT_4D;
            float attn10 = 2 - dx10 * dx10 - dy10 * dy10 - dz10 * dz10 - dw10 * dw10;
            if (attn10 > 0)
            {
                attn10 *= attn10;
                value += attn10 * attn10 * extrapolate(xsb + 0, ysb + 0, zsb + 1, tsb + 1, dx10, dy10, dz10, dw10);
            }
        }

        //First extra vertex
        float attn_ext0 = 2 - dx_ext0 * dx_ext0 - dy_ext0 * dy_ext0 - dz_ext0 * dz_ext0 - dw_ext0 * dw_ext0;
        if (attn_ext0 > 0)
        {
            attn_ext0 *= attn_ext0;
            value += attn_ext0 * attn_ext0 * extrapolate(xsv_ext0, ysv_ext0, zsv_ext0, wsv_ext0, dx_ext0, dy_ext0, dz_ext0, dw_ext0);
        }

        //Second extra vertex
        float attn_ext1 = 2 - dx_ext1 * dx_ext1 - dy_ext1 * dy_ext1 - dz_ext1 * dz_ext1 - dw_ext1 * dw_ext1;
        if (attn_ext1 > 0)
        {
            attn_ext1 *= attn_ext1;
            value += attn_ext1 * attn_ext1 * extrapolate(xsv_ext1, ysv_ext1, zsv_ext1, wsv_ext1, dx_ext1, dy_ext1, dz_ext1, dw_ext1);
        }

        //Third extra vertex
        float attn_ext2 = 2 - dx_ext2 * dx_ext2 - dy_ext2 * dy_ext2 - dz_ext2 * dz_ext2 - dw_ext2 * dw_ext2;
        if (attn_ext2 > 0)
        {
            attn_ext2 *= attn_ext2;
            value += attn_ext2 * attn_ext2 * extrapolate(xsv_ext2, ysv_ext2, zsv_ext2, wsv_ext2, dx_ext2, dy_ext2, dz_ext2, dw_ext2);
        }

        return value / NORM_CONSTANT_4D;
    }
};
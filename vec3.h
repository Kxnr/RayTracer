#ifndef VEC3
#define VEC3

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec3
{
public:
    vec3() {}
    vec3(float e0, float e1, float e2)
    {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }

    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    inline const vec3 &operator+() const { return *this; }
    inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    inline float operator[](int i) const { return e[i]; }
    inline float &operator[](int i) { return e[i]; };

    inline vec3 &operator+=(const vec3 &v2);
    inline vec3 &operator-=(const vec3 &v2);
    inline vec3 &operator*=(const vec3 &v2);
    inline vec3 &operator/=(const vec3 &v2);
    inline vec3 &operator*=(const float t);
    inline vec3 &operator/=(const float t);

    inline float length() const
    {
        return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }

    inline float squaredLength() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
    inline void makeUnitVector();
    inline vec3 rotate(const vec3 angles) const;
    inline vec3 rotate(const float theta, const float psi, const float phi) const;
    inline vec3 unrotate(const vec3 angles) const;
    inline vec3 unrotate(const float theta, const float psi, const float phi) const;

    float e[3];
};

// operator definitions
inline std::istream &operator>>(std::istream &is, vec3 &t)
{
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

inline std::ostream &operator<<(std::ostream &os, const vec3 &t)
{
    os << t.e[0] << " " << t.e[1] << " " << t.e[2];
    return os;
}

inline void vec3::makeUnitVector()
{
    float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(float t, const vec3 &v)
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator/(float t, const vec3 &v)
{
    return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline vec3 operator*(const vec3 &v, float t)
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator/(const vec3 &v, float t)
{
    return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}


inline float dot(const vec3 &v1, const vec3 &v2)
{
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2)
{
    return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
                (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
                (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

inline vec3 &vec3::operator+=(const vec3 &v)
{
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

inline vec3 &vec3::operator*=(const vec3 &v)
{
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
}

inline vec3 &vec3::operator/=(const vec3 &v)
{
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
}

inline vec3 &vec3::operator-=(const vec3 &v)
{
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

inline vec3 &vec3::operator*=(const float t)
{
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline vec3 &vec3::operator/=(const float t)
{
    float k = 1.0 / t;

    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
}

inline vec3 vec3::rotate(const vec3 angles) const {
    return rotate(angles[0], angles[1], angles[2]);
}

inline vec3 vec3::rotate(const float theta, const float psi, const float phi) const {
    // TODO: quaternions would make this faster and solve
    //       any gimble lock issues down stream
    
    float sintheta = sin(theta);
    float sinpsi = sin(psi);
    float sinphi = sin(phi);
    float costheta = cos(theta);
    float cospsi = cos(psi);
    float cosphi = cos(phi);

    vec3 newVec = vec3();
    
    newVec[0] = e[0] * cospsi*costheta
                + e[1] * (sinpsi*sinphi*costheta - sintheta*cospsi) 
                + e[2] * (-sinpsi*cosphi*costheta - sinphi*sintheta);

    newVec[1] = e[0] * sintheta*cospsi 
                + e[1] * (sinpsi*sinphi*sintheta + cosphi*costheta)
                + e[2] * (-sinpsi*sintheta*cosphi + sinphi*costheta);
    
    newVec[2] = e[0] * sinpsi
                + e[1] * (-sinphi*cospsi)
                + e[2] * (cospsi*cosphi);
    
    return newVec;
}

inline vec3 vec3::unrotate(const vec3 angles) const {
    return unrotate(angles[0], angles[1], angles[2]);
}

inline vec3 vec3::unrotate(const float theta, const float psi, const float phi) const {

    float sintheta = sin(theta);
    float sinpsi = sin(psi);
    float sinphi = sin(phi);
    float costheta = cos(theta);
    float cospsi = cos(psi);
    float cosphi = cos(phi);

    vec3 newVec = vec3();

    newVec[0] = e[0] * cospsi*costheta
                + e[1] * sintheta*cospsi
                + e[2] * sinpsi;
                

    newVec[1] = e[0] * (sinpsi*sinphi*costheta - sintheta*cosphi) 
                + e[1] * (sinpsi*sinphi*sintheta + cosphi*costheta)
                + e[2] * -sinphi*cospsi; 


    newVec[2] = e[0] * (-sinpsi*cosphi*costheta - sinphi*sintheta) 
                + e[1] * (-sinpsi * sintheta *cosphi + sinphi * costheta) 
                + e[2] * cospsi*cosphi;

    return newVec;
}

inline vec3 unitVector(vec3 v)
{
    return v / v.length();
}

#endif
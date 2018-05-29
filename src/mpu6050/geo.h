#ifndef _GEO_H_
#define _GEO_H_

#include <math.h>

struct quatf {
public:

    float w;
    float x;
    float y;
    float z;

    quatf() {
        w = 1.0;
        x = 0;
        y = 0;
        z = 0;
    }

    quatf(float w, float x, float y, float z) {
        this->w = w;
        this->x = x;
        this->y = y;
        this->z = z;
    }

    quatf operator*(quatf rhs) {
        return quatf(
                (w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z),
                (w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y),
                (w*rhs.y - x*rhs.z + y*rhs.w + z*rhs.x),
                (w*rhs.z + x*rhs.y - y*rhs.x + z*rhs.w));
    }

    void operator=(quatf rhs) {
        w=rhs.w;
        x=rhs.x;
        y=rhs.y;
        z=rhs.z;
    }

    quatf conj() {
        return quatf(w, -x, -y, -z);
    }

    float length() {
        return sqrt(w*w + x*x + y*y + z*z);
    }

    quatf normalize() {
        float n = length();
        return quatf(w/n, x/n, y/n, z/n);
    }
};


struct vec3f {
public:
    vec3f() {
        x = 0;
        y = 0;
        z = 0;
    };

    vec3f(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    //binary operators
    bool operator==(vec3f rhs)  {
        return(x == rhs.x && y == rhs.y && z == rhs.z);
    }

    vec3f operator+(vec3f rhs) {
        return vec3f(x + rhs.x,
                    y + rhs.y,
                    z + rhs.z);
    }

    vec3f operator-(vec3f rhs) {
        return vec3f(x - rhs.x,
                    y - rhs.y,
                    z - rhs.z);
    }

    vec3f operator*(float scalar) {
        return vec3f(x*scalar, y*scalar, z*scalar);
    }

    vec3f operator/(float scalar) {
       return vec3f(x/scalar, y/scalar, z/scalar);
    }

    //unary operators
    vec3f operator-() {
       return vec3f(-x, -y, -z);
    }

    vec3f cross(vec3f rhs) {
        return vec3f(y * rhs.z - z * rhs.y,
                    x * rhs.z - z * rhs.x,
                    x * rhs.y - y * rhs.x);
    }

    float dot(vec3f rhs) {
        return (x * rhs.x +
                y * rhs.y +
                z * rhs.z);
    }

    float length() {
        return (float)sqrt(x*x + y*y + z*z);
    }

    vec3f normalize() {
        vec3f v(x, y, z);
        return v/v.length();
    }

    float angleBetween(vec3f v) {
       return acos(this->dot(v)/(this->length()*v.length()));
    }

    vec3f rotate(vec3f k, float t) {
        // Rodriques' Formula: vr = vcos(t) + (k x v)sin(t) + k(k*v)(1-cos(t)))
        // where v is the vector to be rotated, vr is the rotated vector, k is the axis of rotation unit vector, t is the angle to rotate, following the right hand rule.
        vec3f v(x, y, z);
        return v*cos(t)+(k.cross(v))*sin(t)+(k*(k.dot(v))*(1-cos(t)));
    }

    vec3f rotate(quatf q) {

        quatf p(0, x, y, z);
        quatf pq = (q*p)*q.conj();
        return vec3f(pq.x, pq.y, pq.z);
    }

    float x;
    float y;
    float z;

};

struct vec2f {
public:
    vec2f() {
        x = 0;
        y = 0;
    }

    vec2f(float x, float y) {
        this->x = x;
        this->y = y;
    }

    bool operator==(vec2f rhs) {
        return(x == rhs.x && y == rhs.y);
    }

    vec2f operator+(vec2f rhs) {
        return vec2f(x + rhs.x,
                    y + rhs.y);
    }

    vec2f operator-(vec2f rhs) {
        return vec2f(x - rhs.x,
                    y - rhs.y);
    }

    vec2f operator*(float scalar) {
        return vec2f(x*scalar, y*scalar);
    }

    vec2f operator/(float scalar) {
        return vec2f(x/scalar, y/scalar);
    }

    float dot(vec2f rhs) {
        return (x * rhs.x +
                y * rhs.y);
    }

    float length() {
        return float(sqrt(x*x + y*y));
    }

    vec2f normalize() {
        vec2f v(x, y);
        return v/v.length();
    }

    float x;
    float y;
};

#endif

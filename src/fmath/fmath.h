#ifndef _FMATH_H
#define _FMATH_H

/**
 * this library contains a bunch of functions and structs for doing fixed point math.
 *
 */

#include <stdint.h>

#define F16_ONE (1<<16)
#define PI      (f16_t)((int32_t)205887) // 3.1415926*(2^16)

#define f16_mul(n, m) (int32_t)((((int64_t)n)*m)>>16)
#define f16_div(n, m) (int32_t)((((int64_t)n)<<16)/(m))

#define f16(n) (int32_t)((n*F16_ONE))
#define f16_f(n) (n/(float)F16_ONE)

// if calling code is C++, this lets the linker work
#ifdef __cplusplus
extern "C" {
#endif
/**
 * this calculates the sqrt of a uint32_t number, then converts it to a f16.16
 * found the algorithm somewhere online.
 */
int32_t isqrt(int32_t x);

#ifdef __cplusplus
}
#endif


// define the nice struct with operators if it's C++, otherwise just give the typedef for use with the above functions.
#ifdef __cplusplus
struct f16_t {
public:

    inline f16_t() {
        x = 0;
    }

    inline f16_t(float a) {
        x = f16(a);
    }

    inline f16_t(int32_t a) {
        x = a;
    }

    inline void operator=(f16_t rhs) {
        x = rhs.x;
    }

    inline bool operator==(f16_t rhs) {
        return(x == rhs.x);
    }

    inline f16_t operator+(f16_t rhs) {
        return x+rhs.x;
    }

    inline f16_t operator-(f16_t rhs) {
        return x-rhs.x;
    }

    // if multiplying/dividing by an integer, it can should be done directly (a.x*n, a.x/n instead of a*n, a/n)
    // doing the latter will assume n is a fixed point and treat it as being 65536 times smaller and you'll get the wrong result.
    // or, convert n to a float and then to a f16_t, but that will take more time.
    inline f16_t operator*(f16_t rhs) {
        return f16_mul(x, rhs.x);
    }

    inline f16_t operator/(f16_t rhs) {
        return f16_div(x, rhs.x);
    }

    inline void operator*=(f16_t rhs) {
        x = f16_mul(x, rhs.x);
    }

    inline operator float() {
        return f16_f(x);
    }

    inline f16_t sqrt() {
        return isqrt(x);
    }

    inline f16_t floor() {
        return f16_t(x & ((F16_ONE - 1) << 16));
    }

    int32_t x;
};

struct vec3f16 {
public:
    vec3f16() {
        x = 0;
        y = 0;
        z = 0;
    };

    vec3f16(f16_t nx, f16_t ny, f16_t nz) {
        x = nx;
        y = ny;
        z = nz;
    }

    void operator=(vec3f16 rhs) {
        x=rhs.x;
        y=rhs.y;
        z=rhs.z;
    }

    //binary operators
    bool operator==(vec3f16 rhs)  {
        return(x == rhs.x && y == rhs.y && z == rhs.z);
    }

    vec3f16 operator+(vec3f16 rhs) {
        return vec3f16(x + rhs.x,
                    y + rhs.y,
                    z + rhs.z);
    }

    vec3f16 operator-(vec3f16 rhs) {
        return vec3f16(x - rhs.x,
                    y - rhs.y,
                    z - rhs.z);
    }

    vec3f16 operator*(f16_t scalar) {
        return vec3f16(x*scalar, y*scalar, z*scalar);
    }

    vec3f16 operator/(f16_t scalar) {
       return vec3f16(x/scalar, y/scalar, z/scalar);
    }

    //unary operators
    vec3f16 operator-() {
       return vec3f16(-x, -y, -z);
    }

    vec3f16 cross(vec3f16 rhs) {
        return vec3f16((y * rhs.z) - (z * rhs.y),
                    (z * rhs.x) - (x * rhs.z),
                    (x * rhs.y) - (y * rhs.x));
    }

    f16_t dot(vec3f16 rhs) {
        return (x * rhs.x +
                y * rhs.y +
                z * rhs.z);
    }

    f16_t length() {
        return (x*x + y*y + z*z).sqrt();
    }

    vec3f16 normalize() {
        f16_t l = this->length();
        return vec3f16(x/l, y/l, z/l);
    }

    // float angleBetween(vec3f16 v) {
    //    return acos(this->dot(v)/(this->length()*v.length()));
    // }

    // vec3f16 rotate(vec3f16 k, float t) {
    //     // Rodriques' Formula: vr = vcos(t) + (k x v)sin(t) + k(k*v)(1-cos(t)))
    //     // where v is the vector to be rotated, vr is the rotated vector, k is the axis of rotation unit vector, t is the angle to rotate, following the right hand rule.
    //     vec3f16 v(x, y, z);
    //     return v*cos(t)+(k.cross(v))*sin(t)+(k*(k.dot(v))*(1-cos(t)));
    // }


    f16_t x;
    f16_t y;
    f16_t z;

};

struct quatf16 {
public:

    f16_t w;
    f16_t x;
    f16_t y;
    f16_t z;

    quatf16() {
        w = F16_ONE;
        x = 0;
        y = 0;
        z = 0;
    }

    quatf16(f16_t nw, f16_t nx, f16_t ny, f16_t nz) {
        w = nw;
        x = nx;
        y = ny;
        z = nz;
    }

    /*
     * computes the small angle delta quat based on axis-angle vector v
     */
    quatf16(vec3f16 v) {
        x.x = v.x.x/2;
        y.x = v.y.x/2;
        z.x = v.z.x/2;
        w.x = F16_ONE - (x*x + y*y + z*z).x/2; // 1 - 0.5*(x^2 + y^2 + z^2)
    }

    quatf16 operator*(quatf16 rhs) {
        return quatf16(
                (w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z),
                (w * rhs.x + z * rhs.y - y * rhs.z + x * rhs.w),
                (w * rhs.y + x * rhs.z + y * rhs.w - z * rhs.x),
                (y * rhs.x - x * rhs.y + w * rhs.z + z * rhs.w));
    }

    void operator=(quatf16 rhs) {
        w=rhs.w;
        x=rhs.x;
        y=rhs.y;
        z=rhs.z;
    }

    quatf16 conj() {
        return quatf16(w, -x, -y, -z);
    }

    f16_t length() {
        return (w*w + x*x + y*y + z*z).sqrt();
    }

    quatf16 normalize() {
        f16_t n = length();
        return quatf16(w/n, x/n, y/n, z/n);
    }

    vec3f16 up() {
        vec3f16 v;
        v.x = (x*z - w*y); // 2*(q.x*q.z + q.w*q.y)
        v.x.x *= 2;
        v.y = (y*z + w*x); // 2*(q.y*q.z - q.w*q.x)
        v.y.x *= 2;
        v.z = w*w - x*x - y*y + z*z; // (q.w^2 - q.x^2 - q.y^2 + q.z^2)
        return v;
    }

    vec3f16 rotate(vec3f16 v) {

        vec3f16 qv = vec3f16(x, y, z);
        vec3f16 t = qv.cross(v);
        t.x.x *= 2;
        t.y.x *= 2;
        t.z.x *= 2;

        return v + (t*w) + (qv.cross(t));

        /*

        quatf16 p(0, v.x, v.y, v.z);
        quatf16 q(w, x, y, z);
        quatf16 pq = q*p*q.conj();
        return vec3f16(pq.x, pq.y, pq.z);*/
    }
};


#else
typedef int32_t f16_t;
#endif

#endif
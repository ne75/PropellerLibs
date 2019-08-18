#ifndef _QUADRATURE_H_
#define _QUADRATURE_H_

#ifdef LIB_COMPILE
    #include "fmath/fmath.h"
#else
    #include "proplibs/fmath.h"
#endif

#include "quad_cog.h"

class Quadrature {
    unsigned stack[QUADRATURE_STACK_SIZE + 1];
    uint32_t calc_stack[64];

    f16_t cpr;
    f16_t pll_pos; // compute this in counts
    f16_t pll_vel; // compute this in counts/s
    f16_t Kp;
    f16_t Ki; // Ki = 1/4*Kp^2

    static void calc(void *par);

public:

    /*
     * main struct that holds all parameters. They should be accessed through the setters/getters,
     * but can also be accessed here for convenience
     */
    volatile quadrature_mb enc_mb;

    f16_t t; // angle, counts
    f16_t w; // angular velocity, counts/s
    f16_t a; // angular acceleration, counts/s^2, currently not computed
    uint32_t loop_time;
    f16_t dp;

    /*
     * empty constructor
     */
    Quadrature();

    /*
     * pa: first pin of encoder. pins MUST be sequential in the order A, B, X.
     * cpr: counts per revolution of the encoder.
     */
    Quadrature(uint8_t pa, uint16_t cpr);

    /*
     * start the Quadrature counter cog
     */
    void init();

    /*
     * start the calculation cog. This will compute angle, angular velocity, and angular acceleration using a tracking loop/PLL
     * Ki is computed from Kp, so no need to specify both. Kp = 2*loop_bandwidth
     */
    void init_calc(f16_t Kp);

    /*
     * return the current count of the encoder
     */
    int32_t get_count();

    /*
     * clears the count to 0 and resets the PLL position to 0.
     */
    void zero();

};

#endif
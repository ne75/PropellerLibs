#ifndef _QUADRATURE_H_
#define _QUADRATURE_H_

#include "quad_cog.h"

class Quadrature {
    unsigned stack[QUADRATURE_STACK_SIZE + 1];
    uint32_t calc_stack[64];

    int32_t cpr;
    int32_t Kp;
    int32_t Ki; // Ki = 1/4*Kp^2

    static void calc(void *par);

public:

    /*
     * main struct that holds all parameters. They should be accessed through the setters/getters,
     * but can also be accessed here for convenience
     */
    quadrature_mb enc_mb;

    int32_t *t; // angle, counts
    int32_t *w; // angular velocity, counts/s
    uint32_t loop_time;

    /*
     * empty constructor
     */
    Quadrature();

    /*
     * pa: first pin of encoder. pins MUST be sequential in the order A, B, X.
     * cpr: counts per revolution of the encoder.
     */
    Quadrature(uint8_t pa, uint16_t cpr, int32_t Ki);

    /*
     * start the Quadrature counter cog
     */
    void init();


    //  * start the calculation cog. This will compute angle, angular velocity, and angular acceleration using a tracking loop/PLL
    //  * Ki is computed from Kp, so no need to specify both. Kp = 2*loop_bandwidth

    // void init_calc(f16_t Kp);

    /*
     * clears the count to 0 and resets the PLL position to 0.
     */
    void zero();

};

#endif
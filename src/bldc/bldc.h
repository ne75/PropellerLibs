#ifndef _BLDC_WRAPPER_H_
#define _BLDC_WRAPPER_H_

#include "bldc_cog.h"

class BLDC {
    unsigned stack_bldc[BLDC_STACK_SIZE + 1];

public:
    /*
     * main struct that holds all parameters. They should be accessed through the setters/getters,
     * but can also be accessed here for convenience
     */
    volatile struct bldc_mb m;

    BLDC(uint8_t id, uint8_t pa, uint8_t pb, uint8_t pc, uint8_t poles);

    /*
     * start the BLDC COG program
     */
    void init();

    /*
     * enable or disable power to the motor phases
     */
    void enable(bool en);

    /*
     * Steps the motor angle by a angle units.
     * Angle unites are 1/10 of an electrical degree
     */
    void step(int32_t a);

    /*
     * Sets the velocity of the motor (number of angle units to step per PWM cycle)
     * Units are arbitrary, but some math from the headers can show the exact unit
     * in relation to electric angle.
     */
    void set_velocity(int32_t v);

    /*
     * set the the motor power between 0 (off) and 255 (max)
     */
    void set_power(uint8_t p);

};

#endif
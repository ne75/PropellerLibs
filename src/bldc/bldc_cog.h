/* bldc.h */

#ifndef _BLDC_H_
#define _BLDC_H_

#include <stdint.h>
#include <stdbool.h>

#define BLDC_STACK_SIZE 300
#define MAX_ANGLE       3600000
#define MAX_VEL         6000
#define P_MAX           1000

#define PWM_FREQ        16000
#define PWM_PERIOD      (CLKFREQ/PWM_FREQ)
#define SS_DIV          1
#define NCO_SINGLE      (0x04 << 26)

typedef struct bldc_mb {

    // Remember to set these mailbox variables.
    uint8_t id;

    uint8_t pin1;
    uint8_t pin2;
    uint8_t pin3;
    uint8_t poles;

    int16_t power; // power it out of 1000, negative powers are allowed
    int32_t velocity;

    int32_t mech_angle;     // elec_angle/poles
    int32_t elec_angle;     // 0-3600000
    uint8_t zone;
    uint32_t zone_phase;

    bool en;
    bool soft_start;

    uint32_t test_val;

} bldc_mb;

void calc_pwm(bldc_mb *m, uint32_t* fall, uint32_t* rise);

#endif
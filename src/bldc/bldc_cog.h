/* bldc.h */

#ifndef _BLDC_H_
#define _BLDC_H_

#include <stdint.h>
#include <stdbool.h>

#define BLDC_STACK_SIZE 256

#define PWM_FREQ        18000
#define PWM_PERIOD      (CLKFREQ/PWM_FREQ)
#define MAX_ANGLE       3600000
#define SS_DIV          1
#define NCO_SINGLE      (0x04 << 26)

typedef struct bldc_mb {

    // Remember to set these mailbox variables.
    uint8_t id;        // 0 = pitch, 1 = roll
    uint8_t pin1;
    uint8_t pin2;
    uint8_t pin3;
    uint8_t poles;
    uint8_t power;
    int32_t velocity;

    int32_t mech_angle;     // 0-3600000
    int32_t elec_angle;     // (poles/2)*mech_angle
    uint8_t zone;
    uint32_t zone_phase;

    bool en;
    bool soft_start;

}bldc_mb;

uint32_t bldc_calc_pwm (bldc_mb *m);

#endif
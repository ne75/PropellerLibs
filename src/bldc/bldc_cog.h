/* bldc.h */

#ifndef _BLDC_H_
#define _BLDC_H_

#include <stdint.h>
#include <stdbool.h>

#define BLDC_STACK_SIZE 300
#define MAX_ANGLE       3600000
#define P_MAX           1000

#define PWM_FREQ        15000
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

    // phase control gains (not used if encoder_count below is null)
    int32_t pKp;
    int32_t pKi;
    int32_t *encoder_count;
    uint16_t enc_cpr;
    int32_t phase;          // drive angle - measured rotor angle.

    int16_t power;
    int16_t max_power;      // maximum power out of 1000
    int32_t velocity;

    int32_t elec_angle;     // 0-3600000
    uint8_t zone;
    uint32_t zone_phase;

    bool en;
    bool en_phase_ctrl;

    uint32_t exec_time;

} bldc_mb;

void calc_pwm(bldc_mb *m, uint32_t* fall, uint32_t* rise);

#endif
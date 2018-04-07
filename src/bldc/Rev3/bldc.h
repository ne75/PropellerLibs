/**
  * NOTES ON CHANGES:
  * (1) The original init function has been cut down.  Don't see the ned to return address.
  */

//bldc.h
#ifndef _BLDC_H
#define _BLDC_H

#include <stdint.h>
#include <stdbool.h>

#define BLDC_STACK_SIZE 64

#define PWM_FREQ        18000
#define PWM_PERIOD      (CLKFREQ/PWM_FREQ)
#define MAX_ANGLE       3600000
#define SS_DIV          1
#define NCO_SINGLE      (0x04 << 26)

typedef struct bldc_mb {
    uint8_t poles; 			// number of poles in the motor
    int32_t mech_angle; 	// 0-3600000
    int32_t velocity;	
    int32_t elec_angle;

    uint8_t zone;
    uint32_t zone_phase;
    uint32_t pwm_time;

    uint8_t pin1;
    uint8_t pin2;
    uint8_t pin3;

    uint8_t motorNo;		// TODO Consider the functionality of having a motor number. 

    bool en;
    bool soft_start;
    uint32_t ss_cntr;
    uint8_t ss_div;

    int32_t testCount;      // TODO Variable for test counting.

}bldc_mb;

void bldc_init(bldc_mb *m, uint8_t npoles, uint8_t pin1, uint8_t pin2,uint8_t pin3);

void drivePin(bldc_mb *m, uint8_t hi,uint8_t lo, uint8_t pwm);

#endif


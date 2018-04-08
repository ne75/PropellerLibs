/**
  * NOTES ON CHANGES:
  * (1) The original init function has been cut down.  Don't see the ned to return address.
  */

//bldc.h
#ifndef _BLDC_H
#define _BLDC_H

#include <stdint.h>
#include <stdbool.h>

#define BLDC_STACK_SIZE 256

#define PWM_FREQ        16000
#define PWM_PERIOD      (CLKFREQ/PWM_FREQ)
#define MAX_ANGLE       3600000
#define SS_DIV          1
#define NCO_SINGLE      (0x04 << 26)

typedef struct bldc_mb {
    uint8_t poles; 			// number of poles in the motor
    
    uint8_t ExtPin1;
    uint8_t ExtPin2;
    uint8_t ExtPin3;

    int32_t mech_angle; 	// 0-3600000
    int32_t velocity;	
    int32_t elec_angle;
    uint8_t zone;
    uint32_t zone_phase;

    uint8_t motorNo;		// TODO Consider the functionality of having a motor number. 

    bool en;
    bool soft_start;

}bldc_mb;

void bldc_init(bldc_mb *m, uint8_t npoles);

uint32_t bldc_calc_pwm (bldc_mb *m);

void bldc_set_velocity(bldc_mb *m, int32_t v);

void bldc_step_angle(bldc_mb *m, int32_t a);

#endif


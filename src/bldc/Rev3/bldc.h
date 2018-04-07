//bldc.h
#ifndef _BLDC_H
#define _BLDC_H

#include <stdint.h>
#include <stdbool.h>

#define BLDC_STACK_SIZE 128

#define PWM_FREQ        18000
#define PWM_PERIOD      (CLKFREQ/PWM_FREQ)
#define MAX_ANGLE       3600000


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


#endif

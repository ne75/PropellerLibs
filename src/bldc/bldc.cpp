#include "bldc.h"

#include <cplib>
#include <cstdio>
#include <cstdlib>

#include "global.h"
#include "timeutil.h"

uint32_t bldc_time = 0;

volatile bldc::bldc(uint8_t n, uint8_t npoles, uint8_t pin1, uint8_t pin2,uint8_t pin3) {
	poles = npoles;
    soft_start = true;
    ss_cntr = 0;
    en = true;
    SS_DIV = 1;		// Something to be said about whether we actually need SS_DIV.
    this -> pin1 = pin1;
    this -> pin2 = pin2;
	this -> pin3 = pin3;

}

// The initialization will occur on the cog of choice.
void bldc_pwm_init(){
	// Set the counter settings.
	PHSA = 0;	
	PHSB = 0;
	FRQA = 1;	// We take the highest resolution for both counters.
	FRQB = 1;

	// Configure counter to NCO mode on both cog count.
	CTRA = NCO_SINGLE;
	CTRB = NCO_SINGLE;

	// Direction out. Output low.
	DIRA |= 1 << pin1 | pin2 | pin3;
	OUTA &= ~(1 << pin1 | pin2 | pin3);

	// Compare mode - not a propeller thing, but just remember that it was not implemented.
}
	
void bldc_set_velocity(int32_t v) {
	velocity = v;
}

void bldc_step_angle(int32_t a) {
	elec_angle += (a*1000) % MAX_ANGLE;
}

// Math element, direct port from old code.

inline void bldc_calc_pwm (){
	elec_angle += velocity;
	mech_angle = elec_angle / poles;
	elec_angle = elec_angle % MAX_ANGLE;

	uint32_t a; = (elec_angle < 0) ? MAX_ANGLE + elec_angle : elec_angle;

	zone = a/(MAX_ANGLE/6);
	zone_phase = a % (MAX_ANGLE/6);

	zone_phase= (zone & 1) ? (MAX_ANGLE/6) - zone_phase : zone_phase;
	pwm_time = zone_phase*PWM_PERIOD/(MAX_ANGLE/6);
}


void setDrive(hi, lo, pwm)[
		OUTA &= ~(1 << lo);
		OUTA |= 1 << hi;
		CTRA = NCO_SINGLE | pwm;
		PHSA = -pwm_time*(PWM_PERIOD/1000);

	]

void cycle(){
	if (motors[BLDC_MOTOR_PITCH].en) {
    	if (motors[BLDC_MOTOR_PITCH].soft_start) {
    		motors[BLDC_MOTOR_PITCH].ss_cntr++;
            motors[BLDC_MOTOR_PITCH].pwm_time = (motors[BLDC_MOTOR_PITCH].ss_cntr*PWM_PERIOD)/(PWM_FREQ/SS_DIV);

            OUTA &= ~(1 << pin1 | pin2 | pin3);


            PPSOutput(1, RPB3, OC1);
            PPSOutput(1, RPB4, NULL);
            PPSOutput(1, RPB7, NULL);

    	}
	}

}


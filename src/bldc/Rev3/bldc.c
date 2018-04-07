#include "bldc.h"

#include "propeller.h"
#include <stdio.h>
#include <stdlib.h>

_NAKED int main(struct bldc_mb **ppmailbox){

	struct bldc_mb *par = *ppmailbox;

	bldc_step_angle(par, 3600);
	while(1){
		par->testCount=par->velocity;
		DIRA |= 1 << 16;

		while (par->testCount > 0) {
			OUTA ^= 1 << 16;
			waitcnt(CNT + CLKFREQ/2);
			OUTA ^= 1 << 16;
			waitcnt(CNT + CLKFREQ/2);
			par->testCount--;
		}
		waitcnt(CNT + CLKFREQ);
	}

}

// Init
void bldc_pwm_init(){


}


// Zone calculation Function
void bldc_calc_pwm (bldc_mb *m){
	m->elec_angle += m->velocity;
	m->mech_angle = m->elec_angle / m->poles;
	m->elec_angle = m->elec_angle % MAX_ANGLE;

    uint32_t a = (m->elec_angle < 0) ? MAX_ANGLE+m->elec_angle : m->elec_angle;

    m->zone = a/(MAX_ANGLE/6);
    m->zone_phase = a % (MAX_ANGLE/6);

    m->zone_phase = (m->zone & 1) ? (MAX_ANGLE/6) - m->zone_phase : m->zone_phase;
    m->pwm_time = (m->zone_phase*PWM_PERIOD)/(MAX_ANGLE/6); 
}

void bldc_set_velocity(bldc_mb *m, int32_t v) {
    m->velocity = v;
}

void bldc_step_angle(bldc_mb *m, int32_t a) {
    m->elec_angle += (a*1000) % MAX_ANGLE;
}


// Base Case: Blink test.
/**
	DIRA |= 1 << 16;

	while (1) {
	OUTA ^= 1 << 16;
		waitcnt(CNT + CLKFREQ/2);
	}
*/
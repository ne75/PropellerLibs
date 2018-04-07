#include "bldc.h"

#include "propeller.h"
#include <stdio.h>
#include <stdlib.h>

_NAKED int main(struct bldc_mb **ppmailbox){

	struct bldc_mb *par = *ppmailbox;

	bldc_init(par,12,16,17,18);

	// Set the counter settings.
	PHSA = 0;	
	PHSB = 0;
	FRQA = 1;	
	FRQB = 1;

	// Configure counter to NCO mode on both cog count.
	CTRA = NCO_SINGLE;
	CTRB = NCO_SINGLE;

	// Can be used to set (select) register pins. Ease of use.
	uint32_t allPin = 1 << par->pin1 | 1 << par->pin2 | 1 << par->pin3;

	// Direction out. Output lo to start.
	DIRA |= allPin;
	OUTA &= ~(allPin);


	bldc_set_velocity (par, 400);

	bldc_step_angle(par, 240000);


	//OUTA |= allPin;

	uint32_t t = CNT;

	while(1){
		if (par-> en){
			if (par->soft_start) {
				par->ss_cntr++;														// Inc. SS counter.
				par->pwm_time = (par->ss_cntr*PWM_PERIOD)/(PWM_FREQ/SS_DIV); 		// Determine pwm time.
				OUTA &= ~(allPin);													// Write selected pins to 0.

				CTRA = 	CTRA | par->pin1;
				t = CNT;	// Set time here. Now set outputs;

				PHSA = -par->pwm_time*(PWM_PERIOD/1000);
				OUTA |= 0 << par->pin2;
				OUTA |= 0 << par->pin3;

				waitcnt(t += PWM_PERIOD);

				if (par->ss_cntr >= PWM_FREQ/SS_DIV) {
	                par->soft_start = false;
	            }									
			}

			else {
				bldc_calc_pwm(par);

				/*DIRA |= 1 << 22;
				t = CNT;
				OUTA |= 1 << 22;
				waitcnt(t += CLKFREQ);
				OUTA |= 0 << 22;*/

				switch (par->zone){
					case 0:
						drivePin(par, par->pin1, par->pin3, par->pin2);
						break;
					case 1:
						drivePin(par, par->pin2, par->pin3, par->pin1);
						break;
					case 2:
						drivePin(par, par->pin2, par->pin1, par->pin3);
						break;
					case 3:
						drivePin(par, par->pin3, par->pin1, par->pin2);
						break;
					case 4:
						drivePin(par, par->pin3, par->pin2, par->pin1);
						break;
					case 5:
						drivePin(par, par->pin1, par->pin2, par->pin3);
						break;
				}
			}
		}
		else {
			OUTA &= ~(allPin);
		}
	}

	/**
	NOTE: Old code to check fields. Why look at LEDs? Because I'm lazy as shit.
	while(1){
		par->testCount=par->pin3;
		DIRA |= 1 << 16;

		while (par->testCount > 0) {
			OUTA ^= 1 << 16;
			waitcnt(CNT + CLKFREQ/2);
			OUTA ^= 1 << 16;
			waitcnt(CNT + CLKFREQ/2);
			par->testCount--;
		}
		waitcnt(CNT + CLKFREQ*2);
	}
	*/
	
}

// I like the idea of carrying out varying process w/ drivePin.   
void drivePin(bldc_mb *m, uint8_t hi,uint8_t lo, uint8_t pwm){
		// Clear.
		uint32_t allPin = 1 << hi | 1 << lo | 1 << pwm; 

		OUTA &= ~(allPin);												
		
		CTRA = NCO_SINGLE | pwm;

		uint32_t t = CNT;
		PHSA = -(m->pwm_time)*(PWM_PERIOD/1000);
		OUTA |= 1 << hi;
		waitcnt(t += PWM_PERIOD);
}

// Initialize the struct to the appropriate values
void bldc_init(bldc_mb *m, uint8_t npoles, uint8_t pin1, uint8_t pin2,uint8_t pin3){
	m->poles = npoles;
	
	// Soft Start Variables
	m->soft_start = true;
	m->ss_cntr = 0;
	m->en = true;

	// Pin Definitions
	m->pin1 = pin1;
	m->pin2 = pin2;
	m->pin3 = pin3;

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


// Basic Test: Blink test.
/**
	DIRA |= 1 << 16;

	while (1) {
	OUTA ^= 1 << 16;
		waitcnt(CNT + CLKFREQ/2);
	}
*/

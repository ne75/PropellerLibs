#include "bldc.h"
#include "propeller.h"
#include <stdio.h>
#include <stdlib.h>



_NAKED int main(struct bldc_mb **ppmailbox){

	struct bldc_mb *par = *ppmailbox;

	bldc_init(par,12);

	// Set the counter settings.
	PHSA = 0;	
	PHSB = 0;
	FRQA = 1;	
	FRQB = 1;

	// Configure counter to NCO mode on both cog count.
	CTRA = NCO_SINGLE;
	CTRB = NCO_SINGLE;


	// External Trigger.
	DIRA |= 1 << 21; 

	// Pins 
	uint8_t pin1 = 16;
    uint8_t pin2 = 17;
    uint8_t pin3 = 18;

    // Regarding the PWM total

    uint8_t power = 0;
 	uint32_t hiLevel = 0;
    power = 50; 

	// Current state Hi-Lo-PWM-Pins 
	uint8_t pinHi;
    uint8_t pinLo;
    uint8_t pinPWM;
    uint32_t allPin;

    // Regarding pwm-time.
    uint32_t pwm_time;


	// Can be used to set (select) register pins. Ease of use.
	allPin = 1 << pin1 | 1 << pin2 | 1 << pin3;

	// Direction out. Output lo to start.
	DIRA |= allPin;
	OUTA &= ~(allPin);

	bldc_set_velocity (par, 5000);
	bldc_step_angle(par, 1);


	uint32_t t = CNT;

	while(1){
		OUTA ^= 1 << 21;

		if (par-> en){
			// Standard Output.
			OUTA &= ~(allPin);

			CTRA = NCO_SINGLE | pinPWM;
			CTRB = NCO_SINGLE | pinHi;

			PHSA = -(pwm_time);
			PHSB = -(hiLevel);
	
			hiLevel = (PWM_PERIOD*power)/100;
			pwm_time = bldc_calc_pwm(par);

			switch (par->zone){
				case 0:
					pinHi = pin1;
					pinLo = pin3;
					pinPWM = pin2;
					break;
				case 1:
					pinHi = pin2;
					pinLo = pin3;
					pinPWM = pin1;
					break;
				case 2:
					pinHi = pin2;
					pinLo = pin1;
					pinPWM = pin3;
					break;
				case 3:
					pinHi = pin3;
					pinLo = pin1;
					pinPWM = pin2;
					break;
				case 4:
					pinHi = pin3;
					pinLo = pin2;
					pinPWM = pin1;
					break;
				case 5:
					pinHi = pin1;
					pinLo = pin2;
					pinPWM = pin3;
					break;
			}

		} else {
			OUTA &= ~(allPin);
		}
		
		waitcnt(t += PWM_PERIOD);

	}
	
}

// Initialize the struct to the appropriate values
void bldc_init(bldc_mb *m, uint8_t npoles){
	m->poles = npoles;
	// Soft Start Variables
	m->soft_start = true;
	m->en = true;

}

// Zone calculation Function
uint32_t bldc_calc_pwm (bldc_mb *m){
	m->elec_angle += m->velocity;
	m->mech_angle = m->elec_angle / m->poles;
	m->elec_angle = m->elec_angle % MAX_ANGLE;

    uint32_t a = (m->elec_angle < 0) ? MAX_ANGLE+m->elec_angle : m->elec_angle;


    m->zone = a/(MAX_ANGLE/6);
    m->zone_phase = (a % (MAX_ANGLE/6))*50/100;

    m->zone_phase = (m->zone & 1) ? (MAX_ANGLE/6)*50/100 - m->zone_phase : m->zone_phase;
   	return (m->zone_phase*PWM_PERIOD)/(MAX_ANGLE/6); 
}

//PWM Time should be locals

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

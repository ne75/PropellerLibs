/* bldc.c */

#include <propeller.h>

#include "bldc_cog.h"

int main(struct bldc_mb **ppmailbox){

	struct bldc_mb *par = *ppmailbox;

	// Localize Variables.
	uint8_t pin1 = par->pin1;
    uint8_t pin2 = par->pin2;
    uint8_t pin3 = par->pin3;

	// State Hi-Lo-PWM-Pins.
	uint8_t pinHi;
    uint8_t pinLo;
    uint8_t pinPWM;

    // Variable to access all active motor pins.
	uint32_t allPin = 1 << pin1 | 1 << pin2 | 1 << pin3;

    uint32_t pwm_time;

	// Set the counter settings.
	PHSA = 0;
	PHSB = 0;
	FRQA = 1;
	FRQB = 1;

	// Configure counter to NCO mode on both cog count.
	CTRA = NCO_SINGLE;
	CTRB = NCO_SINGLE;

    // Configure the motor power level (currently out of 100).
 	uint8_t power = par->power;
 	uint32_t hiLevel = 0;

	// Direction out. Output lo to start.
	DIRA |= allPin;
	OUTA &= ~(allPin);

	uint32_t t = CNT;

	while(1){
		if (par->en){
			// Standard Output.
			OUTA &= ~(allPin);

			CTRA = NCO_SINGLE | pinPWM;
			CTRB = NCO_SINGLE | pinHi;

			PHSA = -(pwm_time);
			PHSB = -(hiLevel);

			power = par->power;
			hiLevel = (PWM_PERIOD>>7)*power;
			pwm_time = bldc_calc_pwm(par);		// TODO: Optimize the bldc_calc_pwm.

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
		}

		else {
			OUTA &= ~(allPin);
		}

		waitcnt(t += PWM_PERIOD);
	}
}

// Zone calculation Function
uint32_t bldc_calc_pwm (bldc_mb *m){
	uint8_t power = m->power;

	m->elec_angle += m->velocity;
	m->mech_angle = m->elec_angle / m->poles;
	m->elec_angle = m->elec_angle % MAX_ANGLE;

    uint32_t a = (m->elec_angle < 0) ? MAX_ANGLE+m->elec_angle : m->elec_angle;

    uint32_t subd_max = (MAX_ANGLE/6);

    m->zone = a/(subd_max);
    m->zone_phase = (a % (subd_max));

    m->zone_phase = (m->zone & 1) ? subd_max - m->zone_phase : m->zone_phase;

   	return ((m->zone_phase*PWM_PERIOD)/(subd_max)*power)>>7; // scale the pwm time by the power
}

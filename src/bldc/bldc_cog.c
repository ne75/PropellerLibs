/* bldc.c */

#include <propeller.h>

#include "bldc_cog.h"
#include "bldc_lut.h"

int main(struct bldc_mb **ppmailbox){

	struct bldc_mb *par = *ppmailbox;

	// Localize Variables.
	uint8_t pin1 = par->pin1;
    uint8_t pin2 = par->pin2;
    uint8_t pin3 = par->pin3;

	// State Hi-Lo-PWM-Pins.
	uint8_t pin_fall;
    uint8_t pin_rise;
    uint8_t pin_off;

    // Variable to access all active motor pins.
	uint32_t allPin = 1 << pin1 | 1 << pin2 | 1 << pin3;

	// Set the counter settings.
	PHSA = 0;
	PHSB = 0;
	FRQA = 1;
	FRQB = 1;

	// Configure counter to NCO mode on both cog count.
	CTRA = NCO_SINGLE;
	CTRB = NCO_SINGLE;

    // Configure the motor power level (currently out of 100).
 	uint8_t power = 0;
 	uint32_t a_period = 0;
 	uint32_t b_period = 0;

	// Direction out. Output lo to start.
	DIRA |= allPin;
	OUTA &= ~(allPin);

	DIRA |= 1<<3;

	uint32_t t = CNT;
	while(1) {
		OUTA ^= 1<<3;
		if (par->en) {
			CTRA = NCO_SINGLE | pin_fall;
			CTRB = NCO_SINGLE | pin_rise;
			PHSA = -(a_period); // "falling" phase
			PHSB = -(b_period); // "rising" phase

			calc_pwm(par, &a_period, &b_period);

			switch (par->zone){
				case 0:
					pin_fall = pin1;
					pin_rise = pin2;
					pin_off = pin3;
					break;
				case 1:
					pin_fall = pin2;
					pin_rise = pin3;
					pin_off = pin1;
					break;
				case 2:
					pin_fall = pin3;
					pin_rise = pin1;
					pin_off = pin2;
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
void calc_pwm (bldc_mb *m, uint32_t* fall, uint32_t* rise){
	uint8_t power = m->power;

	m->elec_angle += m->velocity;
	m->mech_angle = m->elec_angle / m->poles;
	m->elec_angle = m->elec_angle % MAX_ANGLE;

    uint32_t a = (m->elec_angle < 0) ? MAX_ANGLE+m->elec_angle : m->elec_angle;

    m->zone = a/(MAX_ANGLE/3);
    m->zone_phase = (a % (MAX_ANGLE/3));

    *fall = (((((uint32_t)waveform_lut[m->zone_phase/1000])*PWM_PERIOD)>>16)*power)/100;
    *rise = (((((uint32_t)waveform_lut[(MAX_ANGLE/3 - m->zone_phase)/1000])*PWM_PERIOD)>>16)*power)/100;

    // m->zone = a/subd_max;
    // m->zone_phase = (a % subd_max);

    // m->zone_phase = (m->zone & 1) ? subd_max - m->zone_phase : m->zone_phase;

    // return (((((uint32_t)waveform_lut[m->zone_phase/1000])*PWM_PERIOD)>>16)*power)/100;

   	// return (((m->zone_phase*PWM_PERIOD)/subd_max)*power)/100; // scale the pwm time by the power

}

/* define address where this code lives. this is needed for library use (some chicken and egg linker thing from https://forums.parallax.com/discussion/158190/cant-get-cogc-files-to-compile/p2) */
extern unsigned int _load_start_bldc_cog[];
const unsigned int *bldc_code = _load_start_bldc_cog;

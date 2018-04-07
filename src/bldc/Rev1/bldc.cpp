/**
 * Volatile: Do we still need to prevent compiler optimizations on prop.
 * bldc_time: what is going on here?
 * ss_cntr: Softstart?  Could you explain the mechanism for softstart.
 * why define SS_DIV if it's just one.
 * compare mode...erm/
 */


#include "bldc.h"

#include <cplib>
#include <cstdio>
#include <cstdlib>

#include "global.h"
#include "timeutil.h"

uint32_t bldc_time = 0;	// TODO: Fragment from before.

volatile bldc::bldc(uint8_t n, uint8_t npoles, uint8_t pin1, uint8_t pin2,uint8_t pin3) {
	poles = npoles;
    soft_start = true;
    ss_cntr = 0;
    en = true;
    SS_DIV = 1;				// Something to be said about whether we actually need SS_DIV.
    this -> pin1 = pin1;
    this -> pin2 = pin2;
	this -> pin3 = pin3;

}

// The initialization will occur on the cog of choice.
void bldc_pwm_init(){
	// Set the counter settings.
	PHSA = 0;	
	PHSB = 0;
	FRQA = 1;	
	FRQB = 1;

	// Configure counter to NCO mode on both cog count.
	CTRA = NCO_SINGLE;
	CTRB = NCO_SINGLE;

	// Direction out. Output lo to start.
	DIRA |= 1 << pin1 | pin2 | pin3;
	OUTA &= ~(1 << pin1 | pin2 | pin3);

	// Compare mode - not a propeller thing, but just remember that it was not implemented.
}

// This is sort of similar, excecpt that 
void drive(hi, lo, pwm){
		// Clear.
		OUTA &= ~(1 << lo);
		PHSA = -pwm_time*(PWM_PERIOD/1000);

		uint32_t t = CNT;

		CTRA = NCO_SINGLE | pwm;
		OUTA |= 1 << hi;
		waitcnt(t + PWM_PERIOD);
}

// This is the BLDC for pitch.
void pitchCycle () {
	uint32_t t = CNT;
	
	while(signal && motorNo == 1){		//Exists because I'm not sure how to stop loop.
			if (soft_start) {
				ss_cntr++;
				
				pwm_time = (ss_cntr*PWM_PERIOD)/(PWM_FREQ/SS_DIV);
				// Clear the out register.
				OUTA &= ~(1 << (pin1 | pin2 | pin3);

				CTRA = NCO_SINGLE | pin1;

				t = CNT;	// Set time here. Now set outputs;
				PHSA = -pwm_time*(PWM_PERIOD/1000);
				OUTA |= 0 << pin2;
				OUTA |= 0 << pin3;

				waitcnt(t + PWM_PERIOD);

				if (ss_cntr >= PWM_FREQ/SS_DIV) {
					soft_start = false;
				}
			}
			else {
				bldc_calc_pwm;
				switch (zone){
					case 0:
						drive(pin1, pin3, pin2);
						break;
					case 1:
						drive(pin2, pin3, pin1);
						break;
					case 2:
						drive(pin2, pin1, pin3);
						break;
					case 3:
						drive(pin3, pin1, pin2);
						break;
					case 4:
						drive(pin3, pin2, pin1);
						break;
					case 5:
						drive(pin1, pin2, pin3);
						break;
					}
			}
		else {
			OUTA &= ~(1 << (pin1 | pin2 | pin3);d
		}
	}
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
	
void bldc_set_velocity(int32_t v) {
	velocity = v;
}

void bldc_step_angle(int32_t a) {
	elec_angle += (a*1000) % MAX_ANGLE;
}





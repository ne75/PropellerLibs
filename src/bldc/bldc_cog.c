/* bldc.c */

#include <propeller.h>

#include "bldc_cog.h"
#include "bldc_lut.h"

int main(struct bldc_mb **ppmailbox){

	struct bldc_mb *par = *ppmailbox;

	// Localize Variables.
	const uint8_t pin1 = par->pin1;
    const uint8_t pin2 = par->pin2;
    const uint8_t pin3 = par->pin3;
    int32_t *count = par->encoder_count; //encoder count variable for phase control.
    const int32_t encoder_offset = par->encoder_offset;
    const uint8_t poles = par->poles;
    const uint16_t cpr = par->enc_cpr;

	// State Hi-Lo-PWM-Pins.
	uint8_t pin_fall;
    uint8_t pin_rise;
    uint8_t pin_off;

    // pre-compute some stuff
    const int32_t count2phase = poles*(MAX_ANGLE/cpr);

    // phase control variables.
    // const int32_t phase_sp = MAX_ANGLE/4;   // out of BLDCs MAX_ANGLE. Constant at 90 degrees.
    int32_t phase_fb;

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

 	uint32_t a_period = 0;
 	uint32_t b_period = 0;

	// Direction out. Output low to start.
	DIRA |= allPin;
	OUTA &= ~(allPin);

	uint32_t t = CNT;
    volatile uint32_t time = 0;
	while(1) {
        time = CNT;

        // compute needed electrical angle to maintain 90deg phase.

		if (par->en) {
			CTRA = NCO_SINGLE | pin_fall;
			CTRB = NCO_SINGLE | pin_rise;
			PHSA = -(a_period); // "falling" phase
			PHSB = -(b_period); // "rising" phase
		} else {
			OUTA &= ~(allPin);
		}

        if (par->en_phase_ctrl) {
            int32_t m_angle = par->elec_angle; // current drive angle, 0 to 360
            int32_t phase_angle = ((*count + encoder_offset)*count2phase) % MAX_ANGLE; // compute the current phase angle, 0 to 360
            phase_angle = (phase_angle < 0) ? MAX_ANGLE+phase_angle : phase_angle;

            // phase_fb = m_angle-phase_angle;
            // if (abs(phase_fb) > (MAX_ANGLE/2)) {
            //     // if the phase error is very large, then assume we wrapped around in set point
            //     phase_fb += MAX_ANGLE;
            // }

            //par->elec_angle += (MAX_ANGLE/4 - phase_fb);

            par->elec_angle = (MAX_ANGLE/4 + phase_angle) % MAX_ANGLE;
            par->phase = phase_angle;
        }

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

        par->exec_time = CNT-time;
		waitcnt(t += PWM_PERIOD);
	}
}

// Zone calculation Function
inline void calc_pwm (bldc_mb *m, uint32_t* fall, uint32_t* rise){
	int16_t power = m->power;

	//m->elec_angle += m->velocity;
    int32_t a = m->elec_angle % MAX_ANGLE;
    a = (a < 0) ? MAX_ANGLE+a : a;
    m->elec_angle = a;

    if (power < 0) {
        power *= -1;
        a = (a + MAX_ANGLE/2)%MAX_ANGLE;
    }

    m->zone = a/(MAX_ANGLE/3);
    m->zone_phase = (a % (MAX_ANGLE/3));

    *fall = (((((uint32_t)waveform_lut[m->zone_phase/1000])*PWM_PERIOD)>>16)*power)/P_MAX;
    *rise = (((((uint32_t)waveform_lut[(MAX_ANGLE/3 - m->zone_phase)/1000])*PWM_PERIOD)>>16)*power)/P_MAX;
}

/* define address where this code lives. this is needed for library use (some chicken and egg linker thing from https://forums.parallax.com/discussion/158190/cant-get-cogc-files-to-compile/p2) */
extern unsigned int _load_start_bldc_cog[];
const unsigned int *bldc_code = _load_start_bldc_cog;

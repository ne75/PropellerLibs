/* bldc.c */

#include <propeller.h>

#include "quad_cog.h"

_NAKED int main(struct quadrature_mb **ppmailbox){

	struct quadrature_mb *encoder = *ppmailbox;

	// Localize Variables.
	uint8_t pA = encoder->pin;
    DIRA &= ~(0x3 << pA);

    const uint8_t enc_states[] = {0, 1, 3, 2}; // converts encoder value to a state index
    const int8_t enc_lut[] = {0, -1, 0, 1};
    const int32_t kp = encoder->kp/10;
    const int32_t ki = encoder->ki/10;

    encoder->err_cnt = 0;

    uint8_t enc_last = 0;
    int32_t pll_p_hr = 0;
    int32_t pll_v_hr = 0;
    int32_t pll_p = 0;
    int32_t pll_v = 0;

    volatile uint32_t t = CNT;
    while(1) {
        t = CNT;

        /* sample encoder */
        uint8_t enc = (INA >> pA) & 0x3;

        int8_t d = enc_lut[(enc_states[enc] - enc_states[enc_last]) & 3];
        encoder->count += d;

        // if the state changed but didn't increment or decrement the count, must have skipped states so there's have an error.
        if (enc != enc_last && d == 0) {
        	encoder->err_cnt++;
        }

        enc_last = enc;

        /* velocity tracking loop */
        int32_t p = encoder->count*16384;

        pll_p_hr += (pll_v_hr/SAMPLE_FREQ);
        int32_t dp = p - (pll_p_hr & ~16383);

        pll_p_hr += ((kp*dp)/SAMPLE_FREQ)*10;
        pll_v_hr += ((ki*dp)/SAMPLE_FREQ)*10;

        pll_p = pll_p_hr/16384;
        pll_v = pll_v_hr/16384;

        encoder->vel = pll_v;

        encoder->loop_time = CNT - t;
        waitcnt(t + SAMPLE_PERIOD);
    }
}

/* define address where this code lives. this is needed for library use (some chicken and egg linker thing from https://forums.parallax.com/discussion/158190/cant-get-cogc-files-to-compile/p2) */
extern unsigned int _load_start_quad_cog[];
const unsigned int *quad_cog = _load_start_quad_cog;

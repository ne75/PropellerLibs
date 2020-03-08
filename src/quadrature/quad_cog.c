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

    encoder->err_cnt = 0;

    uint8_t enc_last = 0;
    uint32_t t_last = 0; // time of the last pulse

    uint32_t t = CNT;
    while(1) {
        t = CNT;
        uint8_t enc = (INA >> pA) & 0x3;

        int8_t d = enc_lut[(enc_states[enc] - enc_states[enc_last]) & 3];
        encoder->count += d;

        // if the state changed but didn't increment or decrement the count, must have skipped states so there's have an error.
        if (enc != enc_last && d == 0) {
        	encoder->err_cnt++;
        }

        enc_last = enc;

        waitcnt(t + SAMPLE_PERIOD);
    }
}

/* define address where this code lives. this is needed for library use (some chicken and egg linker thing from https://forums.parallax.com/discussion/158190/cant-get-cogc-files-to-compile/p2) */
extern unsigned int _load_start_quad_cog[];
const unsigned int *quad_cog = _load_start_quad_cog;

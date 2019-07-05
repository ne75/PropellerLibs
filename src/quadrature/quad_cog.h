/* quad_cog.h */

#ifndef _QUAD_COG_H_
#define _QUAD_COG_H_

#include <stdint.h>
#include <stdbool.h>

#define QUADRATURE_STACK_SIZE 64

#define SAMPLE_FREQ     50000
#define SAMPLE_PERIOD   (CLKFREQ/SAMPLE_FREQ)

typedef struct quadrature_mb {

    // first of the A, B, X encoder pins (must be sequential)
    uint8_t pin;

    int32_t count;

    uint32_t err_cnt;

} quadrature_mb;

#endif
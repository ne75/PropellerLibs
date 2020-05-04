/* quad_cog.h */

#ifndef _QUAD_COG_H_
#define _QUAD_COG_H_

#include <stdint.h>
#include <stdbool.h>

#define QUADRATURE_STACK_SIZE 80

#define SAMPLE_FREQ     10000
#define SAMPLE_PERIOD   (CLKFREQ/SAMPLE_FREQ)

typedef struct quadrature_mb {

    // first of the A, B, X encoder pins (must be sequential)
    uint8_t pin;
    int32_t kp;
    int32_t ki;

    int32_t count;
    int32_t vel;

    int32_t dbg;

    uint32_t err_cnt;
    uint32_t loop_time;

    uint8_t lock_id;

} quadrature_mb;

#endif
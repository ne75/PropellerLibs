#include "fmath.h"

// there are 2^11 + 1 samples starting at 0xe000
#define SINE_TABLE_ADDR     0xE000
#define SINE_TABLE_LENGTH   2049

#include <stdbool.h>

int32_t isqrt(int32_t x) {
    // this algorithm comes from https://github.com/chmike/fpsqrt/blob/master/fpsqrt.c

    uint32_t t, q, b, r;
    r = x;
    b = 0x40000000;
    q = 0;
    while( b > 0x40 )
    {
        t = q + b;
        if( r >= t )
        {
            r -= t;
            q = t + b; // equivalent to q += 2*b
        }
        r <<= 1;
        b >>= 1;
    }
    q >>= 8;
    return q;
}

int32_t isin(int32_t x) {
    // eventually, add interpolation to take advantage of the f16 resolution.

    uint16_t *table = (uint16_t*)SINE_TABLE_ADDR;
    x = x % (2*PI); // get the parameter to be between 0 and 2 pi

    uint32_t x_pos = 0;
    if (x < 0) {
        x_pos = x + (2*PI);
    } else {
        x_pos = x;
    }

    int32_t q = (2*x_pos)/(PI+1); // quadrant 0-3
    x_pos = x_pos%(PI/2 + 1); // +1 to include PI/2 in this range (since table is inclusive)


    // table offset needs ot convert 0 -> pi/2 to 0 -> SINE_TABLE_LENGTH
    uint32_t y = x_pos*SINE_TABLE_LENGTH;
    uint32_t x_table = (2*y)/PI;
    int32_t dx = y%(PI/2 + 1); // this is the substep between two table points, ranging from 0 to PI/2 (inclusive)

    // offset = offset in table, offset_1 = next point in table past offset (including when going backwards)
    int32_t offset = 0;

    // for quadrants 1 and 3 (bit 0 = 1), move from the end of the table instead of the begining
    if (q & 1) {
        offset = (SINE_TABLE_LENGTH-1) - x_table;
    } else {
        offset = x_table;
    }
    // look up the value. because the table is 16 bit and f16.16 is 17 bits to represent 1.0, we change the last value to F16_ONE
    // to make a true 1.0 for sin(pi/2)
    int32_t val;

    if (offset == SINE_TABLE_LENGTH-1) {
        val = F16_ONE;
    } else {
        val = table[offset];
    }

    // for quadrants 2 and 3 (bit 1 = 1), flip the value by -1
    if (q >> 1) {
        return -val;
    }
    return val;
}

int32_t icos(int32_t x) {
    return isin(x + PI/2); // cos(x) is just sin(x+pi/2)
}

int32_t iasin(int32_t x) {
    // binary search through the lookup table to find the result.
    // first, find the table index that gets just below and just above x
    // then, interpolate between those two points to get the subindex in radians
    // x is in range [-65536, 65536], result should be in range -PI to PI

    // not defined for values outide of [-1, 1]
    if (x > F16_ONE || x < -F16_ONE) return 0;

    uint16_t *table = (uint16_t*)SINE_TABLE_ADDR;

    int32_t x_val = x;
    if (x < 0) {
        x_val = -x_val;
    }

    bool found = false;
    int32_t min = 0;
    int32_t max = SINE_TABLE_LENGTH-1;
    int32_t i = 0;

    while (!found) {
        i = (max+min)/2;

        int32_t v = table[i];
        if (v < x_val) {
            min = i;
        } else if (v > x_val) {
            max = i;
        } else {
            // found an exact value.
            if (x < 0) {
                return -(i*PI)/(2*SINE_TABLE_LENGTH);
            } else {
                return (i*PI)/(2*SINE_TABLE_LENGTH);
            }
        }

        if (max-min == 1) {
            found = true;
        }
    }

    if (x < 0) {
        return -(min*PI)/(2*SINE_TABLE_LENGTH);
    } else {
        return (min*PI)/(2*SINE_TABLE_LENGTH);
    }

    // our result is somewhere between min and max. Linearly interpolate to find it.
    // not implementing this yet until interpolation in sin is implemented

    // int32_t v_lo = table[min];
    // int32_t v_hi = table[min+1];
    // int32_t Dx = v_hi - v_lo;
    // int32_t Dy = PI/(2*SINE_TABLE_LENGTH);
    // int32_t dx = x_val-v_lo;

    // int32_t dy = (Dy*dx)/Dx;
    // int32_t r = (min*PI)/(2*SINE_TABLE_LENGTH);

    // if (x < 0) {
    //     return -(r+dy);
    // } else {
    //     return r+dy;
    // }
}

int32_t iacos(int32_t x) {
    return PI/2 - iasin(x);
}
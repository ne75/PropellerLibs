#include "fmath.h"

// there are 2^11 + 1 samples starting at 0xe000
#define SINE_TABLE_ADDR     0xE000
#define SINE_TABLE_LENGTH   2049

int32_t isqrt(int32_t x) {
    if (x < 0) return 0;

    uint64_t op, res, one;

    op = ((int64_t)x)<<16; // add 16 bits so that the number is f*2^32, then sqrt(x) = sqrt(f)*2^16.
    res = 0;

    /* "one" starts at the highest power of four <= than the argument. */
    one = ((int64_t)1)<<44;  /* second-to-top bit set */
    while (one > op) one >>= 2;

    while (one != 0) {
        if (op >= res + one) {
            op -= res + one;
            res += one << 1;  // <-- faster than 2 * one
        }
        res >>= 1;
        one >>= 2;
    }

    return res;
}

int32_t isin(int32_t x) {
    // convert x to a lookup table value.
    // table values range from 0 to PI/2.
    //

    uint16_t *table = (uint16_t*)SINE_TABLE_ADDR;
    x = x % (2*PI); // get the parameter to be between 0 and 2 pi

    uint32_t x_pos = 0;
    if (x < 0) {
        x_pos = x + (2*PI);
    } else {
        x_pos = x;
    }

    // table offset needs ot convert 0 -> 2pi to 0 -> 4*SINE_TABLE_LENGTH
    // subtract 3 because the last entry in the table is 90deg,
    // which will overlap with 0 for the next quadrant.
    uint32_t x_table = x_pos * ((4*SINE_TABLE_LENGTH) -3) / (2*PI);

    uint32_t q = x_table/SINE_TABLE_LENGTH; // quadrant 0-3
    // for quadrants 1 and 3 (bit 0 = 1), move from the end of the table instead of the begining
    uint32_t offset = 0;

    if (q & 1) {
        offset = SINE_TABLE_LENGTH - (x_table % SINE_TABLE_LENGTH) - 1;
    } else {
        offset = x_table % SINE_TABLE_LENGTH;
    }

    // for quadrants 2 and 3 (bit 1 = 1), flip the value by -1
    if (q >> 1) {
        return -table[offset];
    }

    return table[offset];
}

int32_t icos(int32_t x) {
    return isin(x + PI/2); // cos(x) is just sin(x+pi/2)
}
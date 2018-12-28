#include "fmath.h"

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
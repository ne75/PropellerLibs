#include <propeller.h>
#include <stdio.h>
#include <math.h>

#include "fmath.h"

/*

performance notes:

isin(): (in counts). This is MUCH faster than floats, around 20x faster.
    min: 4688
    max: 5168
    avg: 4975

*/

#define PI_F 3.1415926536

int main() {

    int t_min = 10000000;
    int t_max = 0;
    int t_avg = 0;
    int cnt = 0;
    volatile int t = CNT;
    for (int i = 0; i < 2*PI; i+=50) {
        t = CNT;
        f16_t r = ftan(f16_t(i));
        t = CNT - t;

        if (t < t_min) t_min = t;
        if (t > t_max) t_max = t;
        t_avg += t;
        cnt++;
    }

    printf("%d, %d, %d/%d\n", t_min, t_max, t_avg, cnt);

    t_min = 10000000;
    t_max = 0;
    t_avg = 0;
    cnt = 0;
    t = CNT;

    for (int i = 0; i < 8000; i++) {
        float v = (2.0*PI_F*i)/8000.0;
        t = CNT;
        volatile float r = sin(v);
        t = CNT - t;

        if (t < t_min) t_min = t;
        if (t > t_max) t_max = t;
        t_avg += t;
        cnt++;
    }

    printf("%d, %d, %d/%d\n", t_min, t_max, t_avg, cnt);

    while (1) {

    }
}
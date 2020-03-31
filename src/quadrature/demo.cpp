#include <propeller.h>
#include <stdio.h>

#include "quadrature.h"

Quadrature encoder(15, 1000, 200);

int main() {

    encoder.init();

    volatile int32_t t = CNT;
    while(1) {
        t = CNT;
        //printf("%d\n", encoder.enc_mb.count);
        printf("%d\n", *encoder.w);
        waitcnt(CLKFREQ/100 + t);
    }
}
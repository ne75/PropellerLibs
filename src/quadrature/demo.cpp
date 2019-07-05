#include <propeller.h>
#include <stdio.h>

#include "quadrature.h"

Quadrature encoder(21, 2000);

int main() {

    encoder.init();
    encoder.init_calc(20.0f);

    while(1) {
        printf("%d, %f\n", encoder.get_count(), (float)encoder.t);
        waitcnt(CLKFREQ/100 + CNT);
    }
}
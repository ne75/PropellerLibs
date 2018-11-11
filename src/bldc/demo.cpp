#include <propeller.h>
#include <stdio.h>

#include "bldc.h"

// motor #0, phases are 16, 17, 18, and 12 poles
// motor number for user reference only, not necessary for functionality
BLDC motor(0, 16, 17, 18, 12);

int main() {

	printf("\n**** BLDC Demo ****\n");
    motor.init();
    motor.enable(true);
    motor.set_power(255);

    while(1) {
        motor.step(1);
        waitcnt(CLKFREQ/1000 + CNT);
    }
}
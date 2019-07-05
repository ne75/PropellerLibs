#include <propeller.h>
#include <stdio.h>

#include "bldc.h"

// motor #0, phases are 16, 17, 18, and 12 poles
// motor number for user reference only, not necessary for functionality
BLDC motor(0, 13, 14, 15, 8);

int main() {

	printf("\n**** BLDC Demo ****\n");
    motor.init();
    motor.enable(true);
    motor.set_power(30);
    motor.set_velocity(0);

    uint32_t t = CNT;
    while(1) {
        t=CNT;
        //printf("cnt: %d\n", encoder.get_count());
        motor.step(10);
        waitcnt(t+CLKFREQ/100);
        //waitcnt(CNT+CLKFREQ/100);
    }
}
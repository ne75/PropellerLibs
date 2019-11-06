#include <propeller.h>
#include <stdio.h>
#include <stdlib.h>

#include "SerialPlot.h"
#include "../SerialUI/SerialUI.h"

/* Device Drivers */
extern _Driver _SimpleSerialDriver;
extern _Driver _FullDuplexSerialDriver;

_Driver *_driverlist[] = {
    &_FullDuplexSerialDriver,
    NULL
};

SerialPlot plt(stdout);

int main() {

    plt.init(50);
    plt.reset();

    f16_t v = 0;
    f16_t v2 = 0;
    plt.add(&v, "variable!");
    plt.add(&v2, "variable 2!");

    int ctr = 0;
    while(1) {
        ctr++;
        v.x++;
        v2.x+= 2;
        waitcnt(CLKFREQ + CNT);
        if (ctr == 10) {
            plt.clear();
            ctr = 0;
        }
    }

}

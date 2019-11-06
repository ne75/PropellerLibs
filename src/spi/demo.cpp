#include <propeller.h>
#include <stdio.h>

#include "spibus.h"

SPIBus spi(0, 1, 2, CLKFREQ/10000, 0);
int main() {

    SPIBus::spi_dev dev;

    dev.cs = 3;

    uint8_t test_data[] = {10, 1};

    DIRA |= 1 << 16;

	while (1) {
        OUTA ^= 1 << 16;

        DIRA |= 1 << 3;
        OUTA |= 1 << 3;
        spi.transfer(&dev, test_data, 2);
        DIRA |= 1 << 3;
        OUTA |= 1 << 3;

        test_data[0]++;

        waitcnt(CLKFREQ/10 + CNT);
	}
}
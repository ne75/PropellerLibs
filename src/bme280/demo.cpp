/**
 * This example builds for a 104MHz propeller.
 */

#include <math.h>
#include <propeller.h>
#include <stdio.h>

#include "bme280.h"

#define p_I2C_SCL           28
#define p_I2C_SDA           29
#define p_UART_TX           30
#define p_UART_RX           31

I2CBus i2cbus(p_I2C_SCL, p_I2C_SDA, 400000);
BME280 baro = BME280(&i2cbus);

int main() {

    if (!baro.setup()) {
        printf("# Error setting up barometer\n");
    }

    volatile int t = 0;
    volatile int time = 0;

    int32_t raw_p;
    int32_t raw_t;

    while(1) {
        raw_p = baro.getRawPressure();
        raw_t = baro.getRawTemperature();
        t = CNT;
        baro.compensateT(raw_t);
        baro.compensateP(raw_p);
        time = CNT - t;

        printf("t=%d; p=%d; T=%d\n", time/104, baro.p, baro.T);
        waitcnt(CLKFREQ/5 + CNT);
    }

}

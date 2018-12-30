#include <math.h>
#include <propeller.h>
#include <stdio.h>

#include "sam-m8q.h"

#define p_I2C_SCL           28
#define p_I2C_SDA           29
#define p_UART_TX           30
#define p_UART_RX           31

I2CBus i2cbus(p_I2C_SCL, p_I2C_SDA, 400000);
SAM_M8Q gps = SAM_M8Q(&i2cbus);

int main() {

    gps.setup();

    volatile int t = 0;
    volatile int time = 0;

    int prev_cnt=0;
    while(1) {
        t = CNT;
        gps.read(8);
        time = CNT - t;
        printf("t to read %d\n", time/104);

        if (gps.packet_count > prev_cnt) {
            printf("%f, %f, fix: %d, hdop: %f, alt: %f, gs: %f\n", (float)gps.lat, (float)gps.lon, gps.gps_quality, (float)gps.hdop, (float)gps.alt, (float)gps.geoidal_sep);
            printf("packet_count: %d\n", gps.packet_count);
        }
        prev_cnt = gps.packet_count;
        //waitcnt(CLKFREQ/10 + CNT);
        //printf("%s", gps.nmea_buf);
    }

}

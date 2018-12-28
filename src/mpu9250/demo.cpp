#include <math.h>
#include <propeller.h>
#include <stdio.h>

#include "mpu9250.h"

#define p_I2C_SCL           28
#define p_I2C_SDA           29
#define p_UART_TX           30
#define p_UART_RX           31

I2CBus i2cbus(p_I2C_SCL, p_I2C_SDA, 400000);
MPU9250 imu = MPU9250(&i2cbus, 1, MPU9250::GYRO_FS_1000, MPU9250::ACCEL_FS_2G);

int main() {
    if (!imu.setup()) {
        printf("# Error up IMU\n");
        while(1);
    }

    waitcnt(CLKFREQ/100+CNT);

    // the calibration should be constant, so just going to hard set it here. in implementations, store these calibrations in eeprom.
    // imu.calibrateMag();
    imu.h_scale = vec3f16(0.807173f, 1.176833f, 1.097251f);
    imu.h_offset = vec3f16(16.506165f, 6.624252f, -10.983040f);

    //imu.calibrateNorth();
    imu.north = vec3f16(-1.988083f, 10.420944f, 19.780182f);

    volatile int t = CNT;
    int time = 0;
    while(1) {
        t = CNT;
        imu.readData();
        time = (CNT - t)/80;
        if (imu.h_valid) {
            printf("time = %dus\n", time);
            printf("%f,%f,%f\n", (float)imu.h.x, (float)imu.h.y, (float)imu.h.z);
        } else {
            printf("mag invalid\n");
        }

        printf("%f,%f,%f\n", (float)imu.a.x, (float)imu.a.y, (float)imu.a.z);

        waitcnt(CLKFREQ/100 + CNT);
    }

}

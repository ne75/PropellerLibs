#include <propeller.h>
#include <stdio.h>

#include "mpu6050.h"

#define p_I2C_SCL           28
#define p_I2C_SDA           29
#define p_UART_TX           30
#define p_UART_RX           31

I2CBus i2cbus(p_I2C_SCL, p_I2C_SDA);
MPU6050 imu = MPU6050(&i2cbus, MPU6050::IMU_CAM, MPU6050::GYRO_FS_1000, MPU6050::ACCEL_FS_2G);

int main() {
    if (!imu.setup()) {
        printf("# Error setting up IMU\n");
        while(1);
    }

    printf("# KEEP IMU STILL\n");
    imu.measureGyroBias();
    printf("# CALIBRATION COMPLETE\n");

    waitcnt(CLKFREQ/100+CNT);

    volatile int t = CNT;

    while(1) {
    }

}

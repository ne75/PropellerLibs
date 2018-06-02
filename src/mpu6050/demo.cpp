#include <propeller.h>
#include <stdio.h>

#include "mpu6050.h"

#define p_I2C_SCL           28
#define p_I2C_SDA           29
#define p_UART_TX           30
#define p_UART_RX           31

I2CBus i2cbus(p_I2C_SCL, p_I2C_SDA);
MPU6050 imu = MPU6050(&i2cbus, MPU6050::IMU_CAM, MPU6050::GYRO_FS_1000, MPU6050::ACCEL_FS_2G, quatf(1,0,0,0));

int main() {
    if (!imu.setup()) {
        printf("# Error setting up IMU\n");
        while(1);
    }

    printf("# KEEP IMU STILL\n");
    imu.measureGyroBias();
    printf("# CALIBRATION COMPLETE\n");

    imu.initFilter(70,1); //currently, 70 Hz is the fastest it will run

    waitcnt(CLKFREQ/100+CNT);

    volatile int t = CNT;

    while(1) {
        //printf("%f, %f, %f, %f\n", imu.q.w,imu.q.x,imu.q.y,imu.q.z);
        float qArray[4] = {imu.q.w,imu.q.x,imu.q.y,imu.q.z};
        uint8_t *q = (uint8_t *) (qArray);
        putchar('$');
        for (int i = 0; i<16; i++){
            putchar(q[i]);
        }
        putchar('\n');
        waitcnt(CLKFREQ/100 + CNT);
    }

}

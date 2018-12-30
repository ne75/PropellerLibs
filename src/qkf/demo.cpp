/**
 * This example requires an overclocked propeller adjust makefile for it
 *
 * current example is 104MHz
 */

#pragma GCC diagnostic ignored "-fpermissive"


#include <math.h>
#include <propeller.h>
#include <stdio.h>

#ifdef LIB_COMPILE
    #include "mpu9250/mpu9250.h"
    #include "fmath/fmath.h"
    #include "sam-m8q/sam-m8q.h"
    #include "qkf.h"
#else
    #include "proplibs/mpu9250.h"
    #include "proplibs/fmath.h"
    #include "proplibs/sam-m8q.h"
    #include "proplibs/qkf.h"
#endif

#undef CLKFREQ
#define CLKFREQ (*(int *)0)

#define p_I2C_SCL           28
#define p_I2C_SDA           29
#define p_UART_TX           30
#define p_UART_RX           31

/* Global Objects */
I2CBus i2cbus(p_I2C_SCL, p_I2C_SDA);
MPU9250 imu(&i2cbus, 0, MPU9250::GYRO_FS_2000, MPU9250::ACCEL_FS_2G);
SAM_M8Q gps(&i2cbus);
QKF qkf;

/* Cog Mailboxes */

/* Cog Stacks */
unsigned reader_stack[100];

void data_reader(void *par) {

    while(1) {
        imu.readData();

        while(!lockset(qkf.in_lock_id));
        qkf.a = (imu.a*(f16_t(F16_ONE)-qkf.a_beta)) + (qkf.a*qkf.a_beta);
        qkf.w = imu.w;
        qkf.a_valid = true;
        qkf.w_valid = true;

        if (imu.h_valid) {
            qkf.h = (imu.h.normalize()*(f16_t(F16_ONE)-qkf.h_beta)) + (qkf.h*qkf.h_beta);
            qkf.h_valid = true;
        }
        lockclr(qkf.in_lock_id);

        //gps.read(16);
    }
}

void calibrate_mag() {
    // the calibration should be constant, so just going to hard set it here. in implementations, store these calibrations in eeprom.
    //printf("starting calibration\n");
    //imu.calibrateMag();
    //printf("scale: %d %d %d\n", imu.h_scale.x, imu.h_scale.y, imu.h_scale.z);
    //printf("offset: %d %d %d\n", imu.h_offset.x, imu.h_offset.y, imu.h_offset.z);

    imu.h_scale = vec3f16(1.00700f, 0.982025f, 1.011459f);
    imu.h_offset = vec3f16(6.239731f, 13.576202f, 19.529541f);

    // imu.calibrateNorth();
    // printf("north is: %d %d %d; %d\n", imu.north.x, imu.north.y, imu.north.z, imu.north_mag);

    // while(1);

    imu.north = vec3f16(3.654465f, 9.550613f, -17.477462f);
    imu.north_norm = imu.north.normalize();
    imu.north_mag = imu.north.length();
}

void _setup() {

    DIRA |= 1 << 22;

    if (!imu.setup()) {
        printf("# Error setting up IMU\n");
        while(1) {
            OUTA ^= 1 << 22;
            waitcnt(CLKFREQ/10 + CNT);
        }
    }

    gps.setup();

    calibrate_mag();

    qkf.Q = 0.055f;
    qkf.R1 = 0.005f;  // variance of the gyro ish
    qkf.R2 = 3.0f;  // this should be the variance of the accel, though it will be much larger to reduce influence of linear accel.
                    // probably best to take a large sample from the environment (such as with motors running or a plane flying in manual)
                    // and estimate from there
    qkf.R3 = 1.0f;  // ditto. but for mag.

    qkf.a_beta = 0.125f;
    qkf.h_beta = 0.75f;

    qkf.q.w = F16_ONE;
    qkf.q.x = 0;
    qkf.q.y = 0;
    qkf.q.z = 0;

    qkf.north = imu.north_norm;

    cogstart(data_reader, NULL, reader_stack, sizeof(reader_stack));
    qkf.start();
}

int main() {

    _setup();

    int t = 0;
    while(1) {
        while(!lockset(qkf.out_lock_id));
        quatf16 q;
        memcpy((void*)&q, (void*)&qkf.q, sizeof(quatf16));
        lockclr(qkf.out_lock_id);
        //printf("t=%d;\tP=%d; K1=%d; K2=%d; K3=%d\n", qkf.exec_time/104, qkf.P, qkf.K1, qkf.K2, qkf.K3);
        //printf("%d %d %d\n", qkf.x.x, qkf.x.y, qkf.x.z);
        printf("%d %d %d %d\n", q.w.x, q.x.x, q.y.x, q.z.x);
        //printf("fix: %d; nsats: %d\n", gps.gps_quality, gps.n_sats);
        waitcnt(CLKFREQ/50 + CNT);
    }
}
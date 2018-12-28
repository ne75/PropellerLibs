#ifndef _MPU6050_H
#define _MPU6050_H

/**
 *
 * This library needs to be re-built using fixed point math. float point version should be built on top of the fix version.
 * MPU9250 will be made with fixed point, and once that works, this should be based on it.
 *
 */

#include <stdbool.h>
#include <stdfix.h>
#include <stdint.h>

#ifdef LIB_COMPILE
    #include "i2c/i2cbus.h"
    #include "i2c/i2cdev.h"
#else
    #include "proplibs/i2cbus.h"
    #include "proplibs/i2cdev.h"
#endif

#include "geo.h"

class MPU6050 : public I2CDevice {

private:

    uint8_t id;

    vec3f gyro_bias;        // Aka the constant drift.
    vec3f angle_offset;

    float gyro_fs;          // picking the appropriate FS value determines resolution. Tune.
    float accel_fs;

    float gs;
    float as;

public:
    vec3f gyro;
    vec3f accel;
    quatf q;
    uint16_t temp;          // not currently in use.

    enum {
        IMU_CAM = 0,
        IMU_FRAME,
        IMU_MAX
    };

    // Full scale gyro/accel values
    enum {
        GYRO_FS_250 = 0,
        GYRO_FS_500,
        GYRO_FS_1000,
        GYRO_FS_2000
    };

    enum {
        ACCEL_FS_2G = 0,
        ACCEL_FS_4G,
        ACCEL_FS_8G,
        ACCEL_FS_16G
    };


    MPU6050(I2CBus *bus, uint8_t id, uint8_t gfs, uint8_t afs);

     /*
     * write a register to the MPU6050
     */
    bool writeReg(uint8_t reg, uint8_t *d, uint8_t s);

    /*
     * read a register from the MPU6050
     */
    uint8_t readReg(uint8_t reg, uint8_t s);

    /*
     * reads raw data (xyz accel, xyz gyro, and temp) from IMU
     */
    uint8_t *readRawData(uint8_t *buffer);

    /*
     * Implementing virtual parent method
     */
    bool isConnected();

    bool setup();

    /*
     * updates IMU data
     */
    void readData();

    /**
     * calibrate the imu's gyro. IMU must be still while this is happening
     */
    void measureGyroBias();

};
#endif

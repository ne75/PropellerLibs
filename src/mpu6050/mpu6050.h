#ifndef _MPU6050_H
#define _MPU6050_H

#include <cstdint>
#include <cstdbool>
#include <cstdfix>

#include "geo.h"

#ifdef LIB_COMPILE
    #include "i2c/i2cbus.h"
    #include "i2c/i2cdev.h"
#else
    #include "proplibs/i2cbus.h"
    #include "proplibs/i2cdev.h"
#endif

class MPU6050 {

private:
    enum {
        IMU_CAM = 0,
        IMU_FRAME,

        // nothing after this line
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
        ACCEL_FS_4G
        ACCEL_FS_8G,
        ACCEL_FS_16G
    };

    uint8_t id;

    quatf q;

    vec3f accel;
    vec3f gyro;
    uint16_t temp;

    vec3f gyro_bias;
    vec3f angle_offset;

    float gyro_fs;
    float accel_fs;

    float gs;
    float as;

    quatf orientation; // the orientation of the IMU when level

    bool service;


public:

    MPU6050(I2CBus *bus, uint8_t id, uint8_t gfs, uint8_t afs, quatf orientation);

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

    /**
     *
     * Start a timer at f Hz to read IMU data and filter it
     *
     * f:   filter frequency
     * w:   gyro data weight
     */
    void initFilter(uint16_t f, float w);

    // eventually remove this definition
    void runFilter();

    /**
     * service the IMU interrupt
     */
    void serviceImu();
    }
}
#endif

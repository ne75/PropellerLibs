#ifndef _MPU9250_H
#define _MPU9250_H

#include <stdbool.h>
#include <stdfix.h>
#include <stdint.h>

#ifdef LIB_COMPILE
    #include "i2c/i2cbus.h"
    #include "i2c/i2cdev.h"
    #include "fmath/fmath.h"
#else
    #include "proplibs/i2cbus.h"
    #include "proplibs/i2cdev.h"
    #include "proplibs/fmath.h"
#endif

class MPU9250 : public I2CDevice {

private:

    uint8_t id;

    uint8_t gfs;
    uint8_t afs;

    f16_t gs;
    f16_t as;
    vec3f16 ms; // mag sensitivity adjustment*full scale value

    I2CBus::i2c_dev mag_dev;

public:
    vec3f16 w;
    vec3f16 a;
    vec3f16 h;
    uint16_t temp;          // not currently in use, but easy to read out

    bool h_valid;
    vec3f16 h_scale;
    vec3f16 h_offset;
    vec3f16 north;
    vec3f16 north_norm; // unit vector pointing to local magnetic north (after calibration)

    f16_t north_mag;


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

    /**
     * id sets the I2C address, should be equal to the AD0 pin of the chip.
     */
    MPU9250(I2CBus *bus, uint8_t id, uint8_t gfs, uint8_t afs);

    /*
     * write a register to the MPU6050
     */
    bool writeReg(uint8_t reg, uint8_t *d, uint8_t s);

    /*
     * read a register from the MPU6050
     */
    uint8_t readReg(uint8_t reg, uint8_t s);

    /*
     * write a register to the mag in the MPU6050
     */
    bool writeMagReg(uint8_t reg, uint8_t *d, uint8_t s);

    /*
     * read a register from the mag in the MPU6050
     */
    uint8_t readMagReg(uint8_t reg, uint8_t s);

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
     * set up the magnetometer
     */
    bool setupMag();

    /*
     * updates IMU data
     */
    void readData();

    /*
     * run magnetometer calibration
     */
    void calibrateMag();

    /*
     * measure static magnetic field to determine north
     */
    void calibrateNorth();

};
#endif

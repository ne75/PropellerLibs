#ifndef _MPU6050_H
#define _MPU6050_H

// #include <stdint.h>

#include "geo.h"

#ifdef LIB_COMPILE
    #include "i2c/i2cbus.h"
    #include "i2c/i2cdev.h"
#else
    #include "proplibs/i2cbus.h"
    #include "proplibs/i2cdev.h"
#endif

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
    ACCEL_FS_4G,
    ACCEL_FS_8G,
    ACCEL_FS_16G
};

class MPU6050 : public I2CDevice {

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

    uint8_t raw_data[14]; //maximum amount of data that will be read at a time

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
    bool mpu6050_read_raw(MPU6050 *imu);


    /*
     * parse and scale raw data into acclerometer, temperature, and gyro data
     *
     */
    void mpu6050_parse_data(MPU6050 *imu);


public:

    MPU6050(I2CBus *bus, uint8_t id, uint8_t gfs, uint8_t afs, quatf orientation);

    /*
     * Implementing virtual parent method
     */
    bool isConnected();
    bool setup();

    /*
     * updates IMU data
     */
    void mpu6050_read_data(MPU6050 *imu);



    // STUFF



    /**
     *
     * Start a timer at f Hz to read IMU data and filter it
     *
     * f:   filter frequency
     * w:   gyro data weight
     */
    void mpu6050_init_filter(uint16_t f, float w);

    // eventually remove this definition
    void mpu6050_run_filter(MPU6050 *imu);

    /**
     * service the IMU interrupt
     */
    void mpu6050_service_imu(MPU6050 *imu);

    /**
     * calibrate the imu's gyro. IMU must be still while this is happening
     */
    void mpu6050_measure_gyro_bias(MPU6050 *imu);
};
#endif

#include "mpu6050.h"
#include <math.h>
#include <stdio.h>

// Register list
#define MPU6050_BASE_ADDR   0x68
#define REG_SMPL_RATE_DIV   0x19
#define REG_GYRO_CONFIG     0x1B
#define REG_ACCEL_CONFIG    0x1C
#define REG_ACCEL_XOUT_H    0x3B
#define REG_PWR_MGMT_1      0x6B
#define REG_WHO_AM_I        0x75


// Globals used in the ISR.

vec3f start = vec3f(0,0,0);



// NOTE: In the I2CDevice constructor, be careful to shift the address (see i2c documentation as needed)!

MPU6050::MPU6050(I2CBus * bus, uint8_t id, uint8_t gfs, uint8_t afs, quatf orientation) : I2CDevice(bus, (MPU6050_BASE_ADDR + id)<< 1) {
    this->id = id;
    this->gyro_fs = gfs;
    this->accel_fs = afs;
    this->orientation = orientation;
    this->q = quatf(1.0,0.0,0.0,0.0);

}

bool MPU6050::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    //printf("# Writing to register address: %x\n",dev.data[0]);
    memcpy(&(dev.data[1]), d, s);
    //printf("# Copying Bytes: %d\n",dev.data[1]);
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t MPU6050::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}


bool MPU6050::isConnected() {
    return connected;
}

bool MPU6050::setup() {

    uint8_t d; // d pointer address
    uint8_t gfs = gyro_fs;
    uint8_t afs = accel_fs;


    // reset the IMU
    d = 1 << 7;
    if (!writeReg(REG_PWR_MGMT_1, &d, 1)) {
        return false;
    }


    waitcnt(CNT + CLKFREQ / 100);

    // write 1 to power management to enable the IMU and select the x-axis gyro as the clock source
    // (recommended by datasheet)
    d = 1;
    writeReg(REG_PWR_MGMT_1, &d, 1);

    float gfs_lut[] = { 4.3633, 8.7266, 17.4533, 34.9066 };
    float afs_lut[] = { 2.0, 4.0, 8.0, 16.0 };

    gyro_fs = gfs_lut[gfs];
    accel_fs = afs_lut[afs];

    gs = gyro_fs / (1 << 15);
    as = accel_fs / (1 << 15);


    // set gyro and accel full scale range
    d = gfs << 3;
    if (!writeReg(REG_GYRO_CONFIG, &d, 1)) {
        return false;
    }

    d = afs << 3;
    if (!writeReg(REG_ACCEL_CONFIG, &d, 1)) {
        return false;
    }

    d = 15; //hard code this for now.
    if (!writeReg(REG_SMPL_RATE_DIV, &d, 1)) {
        return false;
    }

    this->connected = true;
    return true;

}

uint8_t *MPU6050::readRawData(uint8_t *buffer) {
    readReg(REG_ACCEL_XOUT_H, 14);
    memcpy(buffer, dev.data, 14);
    return buffer;
}


void MPU6050::readData() {
    uint8_t rawData [14];
    this->readRawData(rawData);

    // Parse accelerometer data (registers 0-5)
    accel.x = (float)(int16_t)(rawData[0] << 8 | rawData[1]);
    accel.y = (float)(int16_t)(rawData[2] << 8 | rawData[3]);
    accel.z = (float)(int16_t)(rawData[4] << 8 | rawData[5]);

    // Parse temperature data (registers 6-7)
    temp = rawData[6] << 8 | rawData[7];
    temp = temp / 340.0 + 36.53; // Conversion from datasheet

    // Parse gyroscope data (registers 8-13)
    gyro.x = (float)(int16_t)(rawData[8]  << 8 | rawData[9]);
    gyro.y = (float)(int16_t)(rawData[10] << 8 | rawData[11]);
    gyro.z = (float)(int16_t)(rawData[12] << 8 | rawData[13]);

    // I'm finding that all three axes are flipped on the accel, leading to a left hand system
    accel.x *= -as;
    accel.y *= -as;
    accel.z *= -as;

    gyro.x *= gs;
    gyro.y *= gs;
    gyro.z *= gs;

    gyro.x += gyro_bias.x;
    gyro.y += gyro_bias.y;
    gyro.z += gyro_bias.z;
}

void MPU6050::measureGyroBias() {
    vec3f bias;
    waitcnt(CNT + CLKFREQ); // wait 1 second to let the gyro settle

    float i;
    int t = CNT;
    // take 1 second of measurements
    while (CNT < t + CLKFREQ) {
        i++;
        readData();

        bias.x -= gyro.x;
        bias.y -= gyro.y;
        bias.z -= gyro.z;
    }

    gyro_bias = bias / i;
}

void MPU6050::initFilter(uint16_t f, float w) {
    filter_freq = f;
    filter_weight = w;
    cogstart(&runFilter, (void*)this, filter_stack, sizeof(filter_stack));
}

void MPU6050::runFilter(void * par) {

    MPU6050 *imu = (MPU6050*)par;
    DIRA |= 1 << 16;

    volatile uint32_t t = CNT;

    while(1) {
        OUTA ^= 1 << 16;
        quatf q = imu->q;
        // instead of multiplying by a dynamic orientations, we should just define a mapping that will be applied in readData
        vec3f g = imu->gyro;//.rotate(imu->orientation);
        vec3f a = imu->accel;//.rotate(imu->orientation);

        //vec3f expected_a = vec3f(0,0,-1).rotate(q.conj());
        // We need to simplify this, can be about 2x faster
        vec3f expected_a = vec3f(2*(q.w*q.y - q.x*q.z), -2*(q.w*q.x+q.y*q.z), (-q.w*q.w+q.x*q.x+q.y*q.y-q.z*q.z));

        vec3f err = a.cross(expected_a);
        err.y *= -1;
        err = err*imu->filter_weight;

        g = g+err;
        quatf qgyro;
        qgyro.x = g.x/(imu->filter_freq*2);
        qgyro.y = g.y/(imu->filter_freq*2);
        qgyro.z = 0; //g.z/(imu->filter_freq*2);;
        qgyro.w = 1.0 - (qgyro.x*qgyro.x + qgyro.y*qgyro.y + qgyro.z*qgyro.z)/2;

        imu->q = (qgyro*q);

        imu->readData();

        imu->filt_time = CNT - t;
        waitcnt(t+= CLKFREQ/imu->filter_freq);
    }

}


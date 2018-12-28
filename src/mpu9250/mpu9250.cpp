#include "mpu9250.h"
#include <math.h>
#include <stdio.h>

// Register list
#define MPU9250_BASE_ADDR   0x68
#define AK8963_ADDR         0x0C

#define REG_SMPL_RATE_DIV   0x19
#define REG_GYRO_CONFIG     0x1B
#define REG_ACCEL_CONFIG    0x1C
#define REG_BYPASS_EN       0x37
#define REG_ACCEL_XOUT_H    0x3B
#define REG_USER_CTRL       0x6A
#define REG_PWR_MGMT_1      0x6B
#define REG_WHO_AM_I        0x75

#define REG_MAG_WHO_AM_I    0x00
#define REG_MAG_ST1         0x02
#define REG_MAG_HXL         0x03
#define REG_MAG_CNTL1       0x0A
#define REG_MAG_ASAX        0x10

#define MAG_CAL_SIZE        100
#define MAG_CAL_MIN_DOT     f16_t(0.95f)

// NOTE: In the I2CDevice constructor, be careful to shift the address (see i2c documentation as needed)!

MPU9250::MPU9250(I2CBus * bus, uint8_t id, uint8_t gfs, uint8_t afs) : I2CDevice(bus, (MPU9250_BASE_ADDR + id)<< 1) {
    this->id = id;
    this->gfs = gfs;
    this->afs = afs;

    h_offset = vec3f16(0, 0, 0);

    h_scale = vec3f16(F16_ONE, F16_ONE, F16_ONE);

    mag_dev.adr = AK8963_ADDR<<1;

    bus->pollDevice(&dev);
}

bool MPU9250::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    memcpy(&(dev.data[1]), d, s);
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t MPU9250::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}

bool MPU9250::writeMagReg(uint8_t reg, uint8_t *d, uint8_t s) {
    mag_dev.data[0] = reg;
    memcpy(&(mag_dev.data[1]), d, s);
    bool ret = bus->writeData(&mag_dev, mag_dev.data, s+1, true);

    return ret;
}

uint8_t MPU9250::readMagReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&mag_dev, reg, false);
    bus->readData(&mag_dev, s, true);

    return s;
}

bool MPU9250::isConnected() {
    return connected;
}

bool MPU9250::setup() {

    uint8_t d; // d pointer address

    // reset the IMU
    d = 1 << 7;
    if (!writeReg(REG_PWR_MGMT_1, &d, 1)) {
        return false;
    }

    waitcnt(CNT + CLKFREQ/100);

    // write 1 to power management to enable the IMU and select the x-axis gyro as the clock source
    // (recommended by datasheet)
    d = 1;
    writeReg(REG_PWR_MGMT_1, &d, 1);

    // disable I2C master mode
    d = 0;
    if (!writeReg(REG_USER_CTRL, &d, 1)) {
        return false;
    }

    // turn on I2C bypass in MPU6050
    d = 0x02;
    if (!writeReg(REG_BYPASS_EN, &d, 1)) {
        return false;
    }

    f16_t gfs_lut[] = { 4.3633f, 8.7266f, 17.4533f, 34.9066f };
    f16_t afs_lut[] = { 2.0f, 4.0f, 8.0f, 16.0f };

    gs = gfs_lut[gfs];
    as = afs_lut[afs];

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

    setupMag();

    this->connected = true;
    return true;

}

bool MPU9250::setupMag() {

    uint8_t d = 0;

    // set ROM to access mode.
    d = 0x1f;
    if (!writeMagReg(REG_MAG_CNTL1, &d, 1)) {
        return false;
    }

    waitcnt(CLKFREQ/10 + CNT);

    readMagReg(REG_MAG_ASAX, 3);

    // 4913.2 uT is the true full scale, though data is only for +/-32760 (+/-4912uT)
    ms.y = (((int32_t)dev.data[0]-128)*F16_ONE)/256 + F16_ONE; // sensor x
    ms.y *= 4913.2f;
    ms.x = (((int32_t)dev.data[1]-128)*F16_ONE)/256 + F16_ONE; // sensor y
    ms.x *= 4913.2f;
    ms.z = (((int32_t)dev.data[2]-128)*F16_ONE)/256 + F16_ONE; // sensor z
    ms.z *= 4913.2f;

    d = 0;
    if (!writeMagReg(REG_MAG_CNTL1, &d, 1)) {
        return false;
    }

    waitcnt(CLKFREQ/10 + CNT);

    d = 0x12; // 100hz mode
    if (!writeMagReg(REG_MAG_CNTL1, &d, 1)) {
        return false;
    }

    waitcnt(CLKFREQ/10 + CNT);

    return true;

}

uint8_t *MPU9250::readRawData(uint8_t *buffer) {
    readReg(REG_ACCEL_XOUT_H, 14);
    readMagReg(REG_MAG_ST1, 8);
    memcpy(buffer, dev.data, 14);
    memcpy(buffer+14, mag_dev.data, 8);
    return buffer;
}


void MPU9250::readData() {
    uint8_t rawData[22];
    this->readRawData(rawData);

    // Parse accelerometer data (registers 0-5)
    // hella casts. casts to int16 to capture proper negative numbers, then to int32_t to multiply properly by f16_one;
    // then shift by 15 to convert the integer data into float data (FS = -1 to 1).
    a.x = ((int32_t)((int16_t)(rawData[0] << 8 | rawData[1])))*(F16_ONE>>15);
    a.y = ((int32_t)((int16_t)(rawData[2] << 8 | rawData[3])))*(F16_ONE>>15);
    a.z = ((int32_t)((int16_t)(rawData[4] << 8 | rawData[5])))*(F16_ONE>>15);

    // // Parse temperature data (registers 6-7)
    // temp = rawData[6] << 8 | rawData[7];
    // temp = temp / 340.0 + 36.53; // Conversion from datasheet

    // // Parse gyroscope data (registers 8-13)
    w.x = ((int32_t)(int16_t)(rawData[8]  << 8 | rawData[9]))*(F16_ONE>>15);
    w.y = ((int32_t)(int16_t)(rawData[10] << 8 | rawData[11]))*(F16_ONE>>15);
    w.z = ((int32_t)(int16_t)(rawData[12] << 8 | rawData[13]))*(F16_ONE>>15);

    h.y = ((int32_t)(int16_t)(rawData[16]<<8 | rawData[15]))*(F16_ONE>>15); // sensor x
    h.x = ((int32_t)(int16_t)(rawData[18]<<8 | rawData[17]))*(F16_ONE>>15); // sensor y
    h.z = -((int32_t)(int16_t)(rawData[20]<<8 | rawData[19]))*(F16_ONE>>15); // sensor z

    a.x *= as;
    a.y *= as;
    a.z *= as;

    w.x *= gs;
    w.y *= gs;
    w.z *= gs;

    h.x *= ms.x;
    h.y *= ms.y;
    h.z *= ms.z;

    h.x = (h.x - h_offset.x)*h_scale.x;
    h.y = (h.y - h_offset.y)*h_scale.y;
    h.z = (h.z - h_offset.z)*h_scale.z;

    h_valid = (rawData[14] & 1) && !(rawData[21] & (1<<3)); // H is valid iff data was ready and it did not overflow.
}

void MPU9250::calibrateMag() {
    vec3f16 min;
    vec3f16 max;
    vec3f16 h_last(F16_ONE, 0, 0);
    int n_samples = MAG_CAL_SIZE;

    while(n_samples) {
        readData();
        vec3f16 h_norm = h.normalize();
        f16_t dist = h_last.dot(h_norm);
        if (dist < MAG_CAL_MIN_DOT) {
            // point is far enough away from the last, let's count it.
            n_samples--;
            // save it if it's the min or max
            if (h.x < min.x) min.x = h.x;
            if (h.y < min.y) min.y = h.y;
            if (h.z < min.z) min.z = h.z;
            if (h.x > max.x) max.x = h.x;
            if (h.y > max.y) max.y = h.y;
            if (h.z > max.z) max.z = h.z;
        }

        h_last = h_norm;
        waitcnt(CLKFREQ/10 + CNT);
    }

    h_offset = (max + min)/f16_t(2.0f);
    vec3f16 delta = (max - min)/f16_t(2.0f);
    f16_t avg_delta = (delta.x + delta.y + delta.z)/f16_t(3.0f);
    h_scale.x = avg_delta/delta.x;
    h_scale.y = avg_delta/delta.y;
    h_scale.z = avg_delta/delta.z;

}

void MPU9250::calibrateNorth() {
    vec3f16 h_tot(0, 0, 0);

    for (int i = 0; i < MAG_CAL_SIZE; i++) {
        readData();
        h_tot = h_tot + h;
        waitcnt(CLKFREQ/100 + CNT);
    }

    north = h_tot/f16_t((float)MAG_CAL_SIZE);
    north_norm = north.normalize();
    north_mag = north.length();
}

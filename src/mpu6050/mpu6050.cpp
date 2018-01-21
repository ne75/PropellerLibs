#include "mpu6050.h"

#include <math.h>
#include <stdio.h>

//Register list
#define MPU6050_BASE_ADDR   0x68

// incomplete register list
#define REG_SMPL_RATE_DIV   0x19
#define REG_GYRO_CONFIG     0x1B
#define REG_ACCEL_CONFIG    0x1C

#define REG_ACCEL_XOUT_H    0x3B

#define REG_PWR_MGMT_1      0x6B

#define REG_WHO_AM_I        0x75


// // Globals used in the ISR.
// MPU6050 imus[IMU_MAX] = { 0 };

MPU6050::MPU6050(I2CBus *bus, uint8_t id, uint8_t gfs, uint8_t afs, quatf orientation) : I2CDevice(bus, MPU6050_BASE_ADDR + id) {
    this->id = id;
    this->gyro_fs = gfs;
    this->accel_fs = afs;
    this->orientation = orientation;
}

bool MPU6050::isConnected() {
    return connected;
}
bool MPU6050::setup() {
    printf("# * Init IMU %d *\n", this->id);
    uint8_t d; // d pointer address
    uint8_t gfs = this->gyro_fs;
    uint8_t afs = this->accel_fs;

    // reset the IMU
    d = 1 << 7;
    writeReg(REG_PWR_MGMT_1, &d, 1);
    waitcnt(CNT + CLKFREQ / 100);

    // write 1 to power management to enable the IMU and sellect the x-axis gyro as the clock source
    // (recommended by datasheet)
    d = 1;
    writeReg(REG_PWR_MGMT_1, &d, 1);

    float gfs_lut[] = { 4.3633, 8.7266, 17.4533, 34.9066 };
    float afs_lut[] = { 2.0, 4.0, 8.0, 16.0 };

    this->gyro_fs = gfs_lut[gfs];
    this->accel_fs = afs_lut[afs];
    this->gs = this->gyro_fs / (1 << 15);
    this->as = this->accel_fs / (1 << 15);

    // set gyro and accel full scale range
    d = gfs << 3;
    writeReg(REG_GYRO_CONFIG, &d, 1);

    d = afs << 3;
    writeReg(REG_GYRO_CONFIG, &d, 1);

    d = 15;
    writeReg(REG_SMPL_RATE_DIV, &d, 1);

    if (this->q.length() < 0.9) {
        this->q = quatf(1.0,0.0,0.0,0.0);
    }

    this->init = true;

    return true;
}

bool MPU6050::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    memcpy(&(dev.data[1]), d, s);                   //
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t MPU6050::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}


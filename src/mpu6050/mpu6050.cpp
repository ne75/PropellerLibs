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
float filter_weight = 0;
uint16_t filter_freq = 0;
uint32_t filt_time = 0;


// MPU6050 imus[IMU_MAX] = { 0 };

MPU6050::MPU6050(I2CBus *bus, uint8_t id, uint8_t gfs, uint8_t afs, quatf orientation) : I2CDevice(bus, MPU6050_BASE_ADDR + id) {
    this->id = id;
    this->gyro_fs = gfs;
    this->accel_fs = afs;
    this->orientation = orientation;
}

bool MPU6050::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    memcpy(&(dev.data[1]), d, s);
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t MPU6050::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}

uint8_t *MPU6050::readRawData(uint8_t *buffer) {
    readReg(REG_ACCEL_XOUT_H, 14);
    memcpy(&(buffer), &(dev.data), 14);
    return buffer;
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
    if (!writeReg(REG_PWR_MGMT_1, &d, 1)) {
        return false;
    }
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
    if (!writeReg(REG_GYRO_CONFIG, &d, 1)) {
        return false;
    }

    d = afs << 3;
    if (!writeReg(REG_GYRO_CONFIG, &d, 1)) {
        return false;
    }

    d = 15; //hard code this for now.
    if (!writeReg(REG_SMPL_RATE_DIV, &d, 1)) {
        return false;
    }

    // if there isn't an orientation set already, set it to vertical
    // this is for if the IMU needs to be restarted, it doesn't jerk.
    if (this->q.length() < 0.9) {
        this->q = quatf(1.0,0.0,0.0,0.0);
    }

    this->connected = true;
    return true;
}

void MPU6050::readData() {
    uint8_t rawData [14];
    this->readRawData(rawData);

    // Parse accelerometer data (registers 0-5)
    this->accel.x = (float)(int16_t)(rawData[0] << 8 | rawData[1]);
    this->accel.y = (float)(int16_t)(rawData[2] << 8 | rawData[3]);
    this->accel.z = (float)(int16_t)(rawData[4] << 8 | rawData[5]);

    // Parse temperature data (registers 6-7)
    this->temp = rawData[6] << 8 | rawData[7];
    this->temp = this->temp / 340.0 + 36.53; // Conversion from datasheet

    // Parse gyroscope data (registers 8-13)
    this->gyro.x = (float)(int16_t)(rawData[8]  << 8 | rawData[9]);
    this->gyro.y = (float)(int16_t)(rawData[10] << 8 | rawData[11]);
    this->gyro.z = (float)(int16_t)(rawData[12] << 8 | rawData[13]);

    // I'm finding that all three axes are flipped on the accel, leading to a left hand system
    this->accel.x *= -this->as;
    this->accel.y *= -this->as;
    this->accel.z *= -this->as;

    this->gyro.x *= this->gs;
    this->gyro.y *= this->gs;
    this->gyro.z *= this->gs;

    this->gyro.x += this->gyro_bias.x;
    this->gyro.y += this->gyro_bias.y;
    this->gyro.z += this->gyro_bias.z;
}

void MPU6050::measureGyroBias() {
    vec3f bias;
    waitcnt(CNT + CLKFREQ); // wait 1 second to let the gyro settle

    float i;
    int t = CNT;
    // take 1 second of measurements
    while (CNT < t + CLKFREQ) {
        i++;
        this->readData();

        bias.x -= this->gyro.x;
        bias.y -= this->gyro.y;
        bias.z -= this->gyro.z;
    }

    this->gyro_bias = bias / t;
}

void MPU6050::initFilter(uint16_t f, float w) {
    this->filter_freq = f;
    this->filter_time = w;

}

void MPU6050::runFilter() {

    vec3f g = this->gyro.rotate(this->orientation);
    vec3f a = this->accel.rotate(this->orientation);


    vec3f tempVec = {0,0,1};
    vec3f expected_a = tempVec.rotate(this->q);

    vec3f err = a.cross(expected_a);
    err.x*= -1;  
    err = err*filter_weight;

    g = g+err;

    quatf gyro = {0};
    gyro.x = g.x/(filter_freq*2);
    gyro.y = g.y/(filter_freq*2);
    gyro.w = 1.0k - (gyro.x*gyro.x + gyro.y*gyro.y + gyro.z*gyro.z)/2;

    quatf normTemp = gyro*(this->q);
    this->q = normTemp.normalize();

}


#include "mpu6050.h"

MPU6050::MPU6050(I2CBus *bus, uint8_t adr) : I2CDevice(bus, adr) {
    this->bus = bus;
    dev.adr = adr;
    connected = false;
}

bool MPU6050::isConnected() {
    return connected;
}

bool MPU6050::setup() {
    // TODO: add startup stuff
    connected = true;
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


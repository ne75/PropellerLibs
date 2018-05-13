#include "I2CBus.h"

I2CBus::I2CBus(uint8_t scl, uint8_t sda) {
	this->scl = scl;
	this->sda = sda;
	i2cdev = i2cOpen(&cdev, scl, sda, 400000); //open a 400 khz bus
	lock_id = locknew();
	lockclr(lock_id);
}

I2CBus::I2CBus(uint8_t scl, uint8_t sda, uint32_t freq) {
	this->scl = scl;
	this->sda = sda;
	i2cdev = i2cOpen(&cdev, scl, sda, freq); //open a 400 khz bus
	lock_id = locknew();
	lockclr(lock_id);
}

void resetBusFreq(uint32_t freq) {
	i2cClose(cdev);
	i2cOpen(&cdev, scl, sda, freq);
}

bool I2CBus::getLineHeld() {
	return ((INA >> scl) & 1 == 0);
}

bool I2CBus::pollDevice(i2c_dev *dev) {
	return i2cWrite(i2cdev, dev->adr, 0, 0, false) == 0;
}

bool I2CBus::writeByte(i2c_dev *dev, uint8_t b, bool stop) {
	return i2cWrite(i2cdev, dev->adr, &b, 1, stop) == 0;
}

uint8_t I2CBus::readByte(i2c_dev *dev, bool stop) {
	i2cRead(i2cdev, dev->adr, dev->data, 1, stop);
	return dev->data[0];
}

bool I2CBus::writeData(i2c_dev *dev, uint8_t *data, uint8_t s, bool stop) {
	return i2cWrite(i2cdev, dev->adr, data, s, stop) == 0;
}

bool I2CBus::readData(i2c_dev *dev, uint8_t s, bool stop) {
	return i2cRead(i2cdev, dev->adr, dev->data, s, stop) == 0;
}

bool I2CBus::lock() {
	return lockset(lock_id);
}

void I2CBus::unlock() {
	lockclr(lock_id);
}
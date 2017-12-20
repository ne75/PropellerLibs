#include "i2cdev.h"

I2CDevice::I2CDevice(I2CBus *bus, uint8_t adr) {
	this->bus = bus;
	dev.adr = adr;
	connected = false;
}
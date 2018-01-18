#ifndef _I2C_DEV_H
#define _I2C_DEV_H

#include "i2cbus.h"

class I2CDevice {

protected:
	I2CBus::i2c_dev dev;
	I2CBus *bus;

	bool connected;

public:
	I2CDevice(I2CBus *bus, uint8_t adr);

	/*
	 * setup the device. Needs to be implemented by a child
	 */
	bool setup();

	/*
	 * check if the device is connected. Needs to be implemented by a child
	 */
	bool isConnected();
};

#endif
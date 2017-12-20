#ifndef _I2C_H
#define _I2C_H

#include <i2c.h>
#include <propeller.h>

class I2CBus {

	I2C_COGDRIVER cdev;
	I2C *i2cdev;

public:
	struct i2c_dev {
		uint8_t adr; 			// 7-bit address of the i2c device, shifted left by 1
		uint8_t data[32];		// where data from an i2c device is stored. Max 16 bytes (4 ints/floats) can be read per cycle
	};

	uint8_t scl;
	uint8_t sda;

	I2CBus(uint8_t scl, uint8_t sda);

	/*
	 * returns true if the SCL line is held low by the slave
	 */
	bool getLineHeld();

	/*
	 * poll an address to see if a device is connected --- UNTESTED ---
	 */
	bool pollDevice(i2c_dev *dev);

	/*
	 * write a byte to a device on the bus
	 */
	bool writeByte(i2c_dev *dev, uint8_t b, bool stop);

	/*
	 * read a byte from a device on the bus
	 */
	uint8_t readByte(i2c_dev *dev, bool stop);

	/*
	 * write several bytes to a device on the bus
	 */
	bool writeData(i2c_dev *dev, uint8_t *data, uint8_t s, bool stop);

	/*
	 * read several bytes from a device on the bus
	 */
	bool readData(i2c_dev *dev, uint8_t s, bool stop);

};

#endif
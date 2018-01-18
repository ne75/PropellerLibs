#ifndef _M24512_H
#define _M24512_H

#ifdef LIB_COMPILE
	#include "i2c/i2cbus.h"
	#include "i2c/i2cdev.h"
#else
	#include "proplibs/i2cbus.h"
	#include "proplibs/i2cdev.h"
#endif

class M24512 : public I2CDevice {


public:

	M24512(I2CBus *bus, uint8_t adr);

	/*
	 * Implementing virtual parent method
	 */
	bool isConnected();
	bool setup();

	/*
	 * write a byte to EEPROM
	 */
	void writeByte(uint8_t value, uint32_t addr);

	/*
	 * read a byte from EEPROM
	 */
	uint8_t readByte(uint32_t addr);

	/*
	 * write a series of bytes to EEPROM
	 */
	void writeData(uint8_t *data, uint32_t n, uint32_t addr);

	/*
	 * read series of bytes from EEPROM. returns point to start of data
	 */
	uint8_t *readData(uint8_t *data, uint32_t n, uint32_t addr);


};

#endif
#ifndef _SPI_H
#define _SPI_H

#include <propeller.h>

#define CPHA 1
#define CPOL (1 << 1)

class SPIBus {

	uint8_t lock_id;
	uint32_t half_period;

	uint8_t cpol;
	uint8_t cpha;

	void idle_sck();
	void assert_sck();

public:
	struct spi_dev {
		uint8_t cs; 			// CS pin of the device.
		uint8_t data[32];		// where data from an i2c device is stored. Max 16 bytes (4 ints/floats) can be read per cycle
	};

	uint8_t sck;
	uint8_t miso;
	uint8_t mosi;

	SPIBus(uint8_t sck, uint8_t miso, uint8_t mosi, uint32_t freq, uint8_t mode);

	/*
	 * change the frequency
	 */
	void setBusFreq(uint32_t freq);

	/*
	 * change the mode.
	 *
	 * 2 bit number: [ CPOL | CPHA ]
	 *
	 */
	void setMode(uint8_t mode);

	/*
	 * transfer data between master and slave. data is array of bytes to write, n is size of data.
	 * will simultaneously read n bytes into dev's data field. if data is null, MOSI will be tristated and nothing will be written to the slave
	 */
	void transfer(spi_dev *dev, uint8_t *data, uint8_t n);

	/*
	 * lock the bus so only one device at a time can push bytes. Returns
	 * false if the bus was successfully locked
	 */
	bool lock();

	/*
	 * unlock the bus
	 */
	void unlock();

};

#endif
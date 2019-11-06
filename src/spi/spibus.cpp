#include "SPIBus.h"

#include <stdio.h>

SPIBus::SPIBus(uint8_t sck, uint8_t miso, uint8_t mosi, uint32_t freq, uint8_t mode) {
	this->sck = sck;
	this->miso = miso;
	this->mosi = mosi;
	setMode(mode);

	half_period = CLKFREQ/(2*freq);

	lock_id = locknew();
	lockclr(lock_id);
}

void SPIBus::idle_sck() {
	if (cpol) {
		OUTA |= 1 << sck;
	} else {
		OUTA &= ~(1 << sck);
	}
}

void SPIBus::assert_sck() {
	if (cpol) {
		OUTA &= ~(1 << sck);
	} else {
		OUTA |= (1 << sck);
	}
}

void SPIBus::setBusFreq(uint32_t freq) {
	half_period = CLKFREQ/(2*freq);
}

void SPIBus::setMode(uint8_t mode) {
	cpha = mode & 1;
	cpol = mode >> 1;
}

void SPIBus::transfer(spi_dev *dev, uint8_t *data, uint8_t n) {
	if (n > 32) n = 32;

	OUTA |= 1 << dev->cs; 	// set CS high to start.
	OUTA &= ~(1 << mosi); 	// set the data pin low to start.
	idle_sck();				// idle clock to start

	// set as outputs once OUTA is configured
	DIRA |= 1 << sck;
	if (data != NULL) DIRA |= 1 << mosi; // only set to writing if data is not null
	DIRA |= 1 << dev->cs;
	DIRA &= ~(1 << miso);

	OUTA &= ~(1<<dev->cs); 	// turn on this device

	uint8_t prev_bit = 0;
	uint8_t bit = 0;

	volatile uint32_t t = CNT;
	idle_sck();				// half a cycle of clock idle
	waitcnt(t + half_period);

	for (int i = 0; i < n; i++) {
		// read/write a byte.
		dev->data[i] = 0;
		int j = 8;
		while (j-- > 0) {

			// assert the clock for half a cycle. if cpha is 1, also set the next bit of the byte.
			t = CNT;
			if (cpha)
				assert_sck();
			else
				idle_sck();

			// only write if data is not null
			if (data != NULL) {
				bit = (data[i] >> j) & 1;

				if (bit ^ prev_bit) { // only if this bit is different than the previous one, toggle mosi.
					OUTA ^= 1 << mosi;
					prev_bit = bit;
				}
			}

			dev->data[i] |= ((INA >> miso) & 1) << j;

			waitcnt(t + half_period);

			// idle the clock for half a cycle, if cpha is 0, also set the next bit of the byte.
			t = CNT;
			if (cpha)
				idle_sck();
			else
				assert_sck();

			waitcnt(t + half_period);
		}
	}

	// cs should have pullup. if not, the higher level function is responsible for keeping CS high when inactive
	OUTA |= (1<<dev->cs);
	DIRA &= ~(1<<dev->cs);
	DIRA &= ~(1 << sck);
	DIRA &= ~(1 << mosi);
}

bool SPIBus::lock() {
	return lockset(lock_id);
}

void SPIBus::unlock() {
	lockclr(lock_id);
}
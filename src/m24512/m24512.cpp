#include "m24512.h"

M24512::M24512(I2CBus *bus, uint8_t adr) : I2CDevice(bus, adr) {
	this->bus = bus;
	dev.adr = adr;
	connected = false;
}

bool M24512::isConnected() {
	return connected;
}

bool M24512::setup() {
	connected = true;
}

void M24512::writeByte(uint8_t value, uint32_t addr) {
	while(bus->lock());
	dev.data[0] = addr >> 8;
	dev.data[1] = addr & 0xFF;
	dev.data[2] = value;

	bus->writeData(&dev, dev.data, 3, true);
	while(!bus->pollDevice(&dev)); // wait for eeprom to finish storing the data
	bus->unlock();
}

uint8_t M24512::readByte(uint32_t addr){
	while(bus->lock());
	dev.data[0] = addr >> 8;
	dev.data[1] = addr & 0xFF;
	bus->writeData(&dev, dev.data, 2, false);
	uint8_t c = bus->readByte(&dev, true);
	bus->unlock();
	return c;
}

void M24512::writeData(uint8_t *data, uint32_t n, uint32_t addr) {
	for (int i = 0; i < n; i++) {
		writeByte(data[i], addr + i);
	}
}

uint8_t *M24512::readData(uint8_t *data, uint32_t n, uint32_t addr) {
	for (int i = 0; i < n; i++) {
		data[i] = readByte(addr + i);
	}

	return data;
}
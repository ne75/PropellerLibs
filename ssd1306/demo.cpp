#include <propeller.h>

#include "global.h"
#include "ssd1306.h"

I2CBus i2cbus(p_I2C_SCL, p_I2C_SDA);
SSD1306 oled(&i2cbus, SSD1306_ADDR, p_SSD1306_RST);

static uint8_t logo16_glcd_bmp[] = { 
	0b00000000, 0b11000000,
	0b00000001, 0b11000000,
	0b00000001, 0b11000000,
	0b00000011, 0b11100000,
	0b11110011, 0b11100000,
	0b11111110, 0b11111000,
	0b01111110, 0b11111111,
	0b00110011, 0b10011111,
	0b00011111, 0b11111100,
	0b00001101, 0b01110000,
	0b00011011, 0b10100000,
	0b00111111, 0b11100000,
	0b00111111, 0b11110000,
	0b01111100, 0b11110000,
	0b01110000, 0b01110000,
	0b00000000, 0b00110000 
};

int main() {
	waitcnt(CLKFREQ + CNT);
	oled.setup();

	oled.clear();

	oled.string(16, 3, "Hello\rWorld!", 1);
	while (1) {

	}
}
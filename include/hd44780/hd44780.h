#ifndef _HD44780_H
#define _HD44780_H

#include <stdint.h>

class HD44780 {

	void _busy(void);
	void _inst8(uint8_t lcd_data);
	void _char(uint8_t lcd_data);

	uint8_t p_rs;
	uint8_t p_rw;
	uint8_t p_e;
	uint8_t p_db0;
	uint8_t p_db7;

public:

	HD44780(uint8_t p_rs, uint8_t p_rw, uint8_t p_e, uint8_t p_db0);

	void init(void);
	void clear(void);
	void move(int32_t x, int32_t y);
	void str(const char *string_ptr);
	int32_t dec(int32_t VALUE);
	void hex(int32_t VALUE, int32_t DIGITS);
	void bin(int32_t VALUE, int32_t DIGITS);
};

#endif
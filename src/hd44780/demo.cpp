#include <propeller.h>

#include "global.h"
#include "hd44780.h"

HD44780 lcd(p_RS, p_RW, p_E, p_DB0);

int main() {
	
	lcd.init();

	

	while (1) {
		lcd.move(3, 1);
		lcd.clear();
		lcd.str("Hello World!");
		waitcnt(CLKFREQ + CNT);
		lcd.move(1, 1);
		lcd.clear();
		lcd.dec(255);
		waitcnt(CLKFREQ + CNT);
		lcd.move(1, 1);
		lcd.clear();
		lcd.hex(255, 2);
		waitcnt(CLKFREQ + CNT);
		lcd.move(1, 1);
		lcd.clear();
		lcd.bin(255, 8);
		waitcnt(CLKFREQ + CNT);
	}
}
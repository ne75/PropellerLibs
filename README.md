# PropellerLibs
Libraries for Propeller Microcontroller

Run ```make``` from the top level and it will compile all libraries and copy the libraries and headers into lib and include, respectively

## Current Libraries:
- hd44780: Driver for hd44780-style serial-parallel character LCD displays
- i2c: A generic I2C driver
- m24512: Generic I2C-based EEPROM driver, specificly for M24512, but should conform to any standard EEPROM. Requires i2c
- mcp2515: Driver for MCP2515 SPI-CAN interface. Currently bit-bangs the SPI, should be ported to use an SPI driver created by this library
- ssd1306: Driver for ssd1306 OLED drivers. Requires i2c
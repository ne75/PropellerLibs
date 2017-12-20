PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar

MMODEL = cmm

LIBDIR = $(MMODEL)
LIBTARGET = libi2c

OBJDIR = build

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions

LIBS = -lhd44780
LFLAGS = -L./$(MMODEL)/

PORT = /dev/cu.usbserial-*
LOAD_MODE = load_eeprom
LOADFLAGS = -Dreset=dtr -S5

.PHONY: all setup clean

all: setup $(LIBDIR)/$(LIBTARGET).a

setup: 
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/i2cbus.o $(OBJDIR)/i2cdev.o
		$(AR) rcs $@ $^

$(OBJDIR)/i2cbus.o: i2cbus.cpp i2cbus.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $< 

$(OBJDIR)/i2cdev.o: i2cdev.cpp i2cdev.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $< 

clean:
	rm -rf $(OBJDIR)/
	rm -rf $(LIBDIR)/
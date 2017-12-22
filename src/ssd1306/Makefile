PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar

MMODEL = cmm
BOARD = QUICKSTART

TARGET = ssd1306-example
LIBDIR = $(MMODEL)
LIBTARGET = libssd1306
LIBNAME = ssd1306
INCLFILES = ssd1306.h

OBJDIR = build

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions -I../

LIBS = -li2c
LFLAGS = -L./$(MMODEL)/ -L../i2c/cmm/

PORT = /dev/cu.usbserial-*
LOAD_MODE = load_eeprom
LOADFLAGS = -Dreset=dtr -S5

.PHONY: all setup lib load load_eeprom clean

all: setup $(LIBDIR)/$(LIBTARGET).a $(OBJDIR)/$(TARGET).elf $(LOAD_MODE)

lib: setup $(LIBDIR)/$(LIBTARGET).a

setup: 
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/ssd1306.o
		$(AR) rcs $@ $<

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o $(OBJDIR)/ssd1306.o
		$(CC) $(INCLUDES) $(LFLAGS) -o $@ -m$(MMODEL) $(CFLAGS) $^ $(LIBS)
		$(PROPGCC)/bin/propeller-elf-size $(OBJDIR)/$(TARGET).elf

$(OBJDIR)/$(TARGET).o: demo.cpp global.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $< 	

$(OBJDIR)/ssd1306.o: ssd1306.cpp ssd1306.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $< 

load: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -r -v $<

load_eeprom: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -e -r -v $<

install: lib
	mkdir -p ../../include/$(LIBNAME)
	mkdir -p ../../lib/$(MMODEL)
	cp $(LIBDIR)/$(LIBTARGET).a ../../lib/$(LIBDIR)/$(LIBTARGET).a
	cp $(INCLFILES) ../../include/$(LIBNAME)/

clean:
	rm -rf $(OBJDIR)/
	rm -rf $(LIBDIR)/
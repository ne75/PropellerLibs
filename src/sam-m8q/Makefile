PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar

MMODEL ?= cmm
BOARD = QUICKSTART

TARGET = sam-m8q-example
LIBDIR = $(MMODEL)
LIBTARGET = libsam-m8q
LIBNAME = sam-m8q
INCLFILES = sam-m8q.h

OBJDIR = build_$(MMODEL)

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions -I../ -DLIB_COMPILE

LIBS = -lm -lsam-m8q -li2c -lfmath
LFLAGS = -L./$(MMODEL)/ -L../i2c/$(MMODEL)/ -L../fmath/$(MMODEL)

PORT = /dev/cu.usbserial-*
LOAD_MODE = load
LOADFLAGS = -Dreset=dtr -S5 -t -Dclkfreq=104000000

.PHONY: all setup lib load load_eeprom clean

all: clean setup $(LIBDIR)/$(LIBTARGET).a $(OBJDIR)/$(TARGET).elf $(LOAD_MODE)

lib: setup $(LIBDIR)/$(LIBTARGET).a

setup:
	$(shell pkill propeller-load)
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/sam-m8q.o
		$(AR) rcs $@ $<

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(INCLUDES) $(LFLAGS) -o $@ -m$(MMODEL) $(CFLAGS) $^ $(LIBS)
		$(PROPGCC)/bin/propeller-elf-size $(OBJDIR)/$(TARGET).elf

$(OBJDIR)/$(TARGET).o: demo.cpp
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

$(OBJDIR)/sam-m8q.o: sam-m8q.cpp sam-m8q.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

load: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -r -v $<

load_eeprom: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -e -r -v $<

install: lib
	mkdir -p ../../lib/$(MMODEL)
	cp $(LIBDIR)/$(LIBTARGET).a ../../lib/$(LIBDIR)/$(LIBTARGET).a
	cp $(INCLFILES) ../../include/proplibs/

clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIBDIR)

PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar

MMODEL ?= cmm

TARGET = spi-demo
LIBDIR = $(MMODEL)
LIBTARGET = libspi
LIBNAME = spi
INCLFILES = spibus.h

OBJDIR = build_$(MMODEL)

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions

LFLAGS = -L./$(MMODEL)/
LIBS = -lspi

PORT = /dev/cu.usbserial-*
LOAD_MODE = load
LOADFLAGS = -Dreset=dtr -S5 -Dclkfreq=104000000

.PHONY: all setup lib clean

all: clean setup $(LIBDIR)/$(LIBTARGET).a $(OBJDIR)/$(TARGET).elf $(LOAD_MODE)

lib: clean setup $(LIBDIR)/$(LIBTARGET).a

setup:
	$(shell pkill propeller-load)
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/spibus.o
		$(AR) rcs $@ $^

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(INCLUDES) $(LFLAGS) -o $@ -m$(MMODEL) $(CFLAGS) $^ $(LIBS)
		$(PROPGCC)/bin/propeller-elf-size $(OBJDIR)/$(TARGET).elf

$(OBJDIR)/$(TARGET).o: demo.cpp
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

$(OBJDIR)/spibus.o: spibus.cpp spibus.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

load: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -p $(PORT) -I $(PROPGCC)/propeller-load -r -v $<

load_eeprom: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -p $(PORT) -I $(PROPGCC)/propeller-load -e -r -v $<

install: lib
	mkdir -p ../../lib/$(MMODEL)
	cp $(LIBDIR)/$(LIBTARGET).a ../../lib/$(LIBDIR)/$(LIBTARGET).a
	cp $(INCLFILES) ../../include/proplibs/

clean:
	rm -rf ./$(OBJDIR)
	rm -rf ./$(LIBDIR)
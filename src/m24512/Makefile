PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar

MMODEL ?= cmm
BOARD = QUICKSTART

LIBDIR = $(MMODEL)
LIBTARGET = libm24512
LIBNAME = m24512
INCLFILES = m24512.h

OBJDIR = build_$(MMODEL)

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions -I../ -DLIB_COMPILE

LIBS = -li2c
LFLAGS = -L./$(MMODEL)/ -L../i2c/cmm/

PORT = /dev/cu.usbserial-*
LOAD_MODE = load_eeprom
LOADFLAGS = -Dreset=dtr -S5

.PHONY: all setup lib load load_eeprom clean

all: setup $(LIBDIR)/$(LIBTARGET).a

lib: setup $(LIBDIR)/$(LIBTARGET).a

setup:
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/m24512.o
		$(AR) rcs $@ $<

$(OBJDIR)/m24512.o: m24512.cpp m24512.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

install: lib
	mkdir -p ../../lib/$(MMODEL)
	cp $(LIBDIR)/$(LIBTARGET).a ../../lib/$(LIBDIR)/$(LIBTARGET).a
	cp $(INCLFILES) ../../include/proplibs/

clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIBDIR)
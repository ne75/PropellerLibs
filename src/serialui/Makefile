PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar

MMODEL ?= cmm
BOARD = QUICKSTART

LIBDIR = $(MMODEL)
LIBTARGET = libserialui
LIBNAME = serialui
INCLFILES = SerialUI.h

OBJDIR = build_$(MMODEL)

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions -I../ -DLIB_COMPILE -Dprintf=__simple_printf

LIBS = -lserialui
LFLAGS = -L./$(MMODEL)/

PORT = /dev/cu.usbserial-*
LOAD_MODE = load_eeprom
LOADFLAGS = -Dreset=dtr -S5

.PHONY: all setup lib load load_eeprom clean

all: setup $(LIBDIR)/$(LIBTARGET).a

lib: setup $(LIBDIR)/$(LIBTARGET).a

setup:
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/SerialUI.o
		$(AR) rcs $@ $<

$(OBJDIR)/SerialUI.o: SerialUI.cpp SerialUI.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

install: lib
	mkdir -p ../../lib/$(MMODEL)
	cp $(LIBDIR)/$(LIBTARGET).a ../../lib/$(LIBDIR)/$(LIBTARGET).a
	cp $(INCLFILES) ../../include/proplibs/

clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIBDIR)
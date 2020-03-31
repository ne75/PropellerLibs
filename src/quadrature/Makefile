PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
OBJCOPY = $(PROPGCC)/bin/propeller-elf-objcopy
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar
NM = $(PROPGCC)/bin/propeller-elf-nm

MMODEL ?= cmm
BOARD = QUICKSTART

TARGET = quadrature-example
LIBDIR = $(MMODEL)
LIBTARGET = libquadrature
LIBNAME = quadrature
INCLFILES = quadrature.h quad_cog.h

OBJDIR = build_$(MMODEL)

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions -I../ -DLIB_COMPILE
LIBS = -lquadrature -lm
LFLAGS = -L./$(MMODEL)/

PORT = /dev/cu.usbserial-*
LOAD_MODE = load
LOADFLAGS = -Dreset=dtr -S5 -t #-Dclkfreq=100000000 -Dclkmode=xinput

.PHONY: all config load clean

all: lib $(OBJDIR)/$(TARGET).elf $(LOAD_MODE)

lib: setup $(LIBDIR)/$(LIBTARGET).a

setup:
	$(shell pkill propeller-load)
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/quad.cog $(OBJDIR)/quadrature.o
		$(AR) rs $@ $^

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(INCLUDES) $(LFLAGS) -o $@ -m$(MMODEL) $(CFLAGS) $^ $(LIBS)
		$(PROPGCC)/bin/propeller-elf-size $@

$(OBJDIR)/$(TARGET).o: demo.cpp
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

$(OBJDIR)/quadrature.o: quadrature.cpp quadrature.h
		$(CC) -c -r $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $<

# COG targets

$(OBJDIR)/quad.cog: quad_cog.c quad_cog.h
		$(CC) -r $(INCLUDES) $(LFLAGS) $(CFLAGS) -mcog -o $@ $<
		$(OBJCOPY) --localize-text --rename-section .text=$(notdir $@) $@

# Loading

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

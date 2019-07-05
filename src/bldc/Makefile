PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
OBJCOPY = $(PROPGCC)/bin/propeller-elf-objcopy
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar
NM = $(PROPGCC)/bin/propeller-elf-nm

MMODEL ?= cmm
BOARD = QUICKSTART

CLKFREQ = 100MHz
CLKMODE = xinput # use external clock source

TARGET = bldc-example
LIBDIR = $(MMODEL)
LIBTARGET = libbldc
LIBNAME = bldc
INCLFILES = bldc.h bldc_cog.h

OBJDIR = build_$(MMODEL)

CFLAGS = -Os -n -m32bit-doubles -mfcache -fno-exceptions
LIBS = -lbldc
LFLAGS = -L./$(MMODEL)/

PORT = /dev/cu.usbserial-*
LOAD_MODE = load
LOADFLAGS = -Dreset=dtr -S5 -t -Dclkfreq=$(CLKFREQ) -Dclkmode=$(CLKMODE)

.PHONY: all config load clean

all: lib $(OBJDIR)/$(TARGET).elf $(LOAD_MODE)

lib: setup $(LIBDIR)/$(LIBTARGET).a

setup:
	$(shell pkill propeller-load)
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/bldc.cog $(OBJDIR)/bldc.o
		$(AR) rs $@ $^

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(INCLUDES) $(LFLAGS) -o $@ -m$(MMODEL) $(CFLAGS) $^ $(LIBS)
		$(PROPGCC)/bin/propeller-elf-size $(OBJDIR)/$(TARGET).elf

$(OBJDIR)/$(TARGET).o: demo.cpp
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

$(OBJDIR)/bldc.o: bldc.cpp bldc.h
		$(CC) -c -r $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $<

# COG targets

$(OBJDIR)/bldc.cog: bldc_cog.c bldc_cog.h
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

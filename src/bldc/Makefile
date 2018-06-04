PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
OBJCOPY = $(PROPGCC)/bin/propeller-elf-objcopy
LOAD = $(PROPGCC)/bin/propeller-load
AR = $(PROPGCC)/bin/propeller-elf-ar

MMODEL = cmm

CUAIRPROPLIBS = /opt/PropellerLibraries

#OBJDIR is hardcoded.
BOARD = QUICKSTART
OBJDIR = ./obj

TARGET = bldc-example
LIBDIR = $(MMODEL)
LIBTARGET = libbldc
LIBNAME = bldc
INCLFILES = bldc.h

PORT = /dev/cu.usb*

#Loaded into the EEPROM.
LOAD_MODE = load

DEADCODESTRIP = -Wl,-static -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s

CFLAGS =  -Os -n -m32bit-doubles -fno-exceptions -I$(CUAIRPROPLIBS)/include -DCLKFREQ=80000000
LFLAGS = -L./$(MMODEL)/
LOADFLAGS = -Dreset=dtr -S5 -t

 LIBS = -lbldc

.PHONY: all config load clean

all: lib $(OBJDIR)/$(TARGET).elf $(LOAD_MODE)

lib: setup $(LIBDIR)/$(LIBTARGET).a

setup:
	$(shell pkill propeller-load)
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBTARGET).a: $(OBJDIR)/bldc_cog.o $(OBJDIR)/bldc.o
		$(AR) rcs $@ $^

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(INCLUDES) $(LFLAGS) -o $@ -m$(MMODEL) $(CFLAGS) $^ $(LIBS)
		$(PROPGCC)/bin/propeller-elf-size $(OBJDIR)/$(TARGET).elf

$(OBJDIR)/$(TARGET).o: demo.cpp
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

$(OBJDIR)/bldc.o: bldc.cpp bldc.h
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $<


# COG functions

$(OBJDIR)/bldc_cog.o: bldc_cog.c bldc_cog.h
		$(CC) -r $(INCLUDES) $(LFLAGS) $(CFLAGS) -mcog -o $@ $<
		$(OBJCOPY) --localize-text --rename-section .text=bldc.cog $@

# Loading

load: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -r -v $<

load_eeprom: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -e -r -v $<

clean:
	rm -rf $(OBJDIR)/
	rm -rf $(LIBDIR)/

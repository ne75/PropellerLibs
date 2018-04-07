PROPGCC = /opt/parallax
CC = $(PROPGCC)/bin/propeller-elf-gcc
OBJCOPY = $(PROPGCC)/bin/propeller-elf-objcopy
LOAD = $(PROPGCC)/bin/propeller-load

MMODEL = cmm
BOARD = QUICKSTART

TARGET = main

PORT = /dev/ttyUSB0
OBJDIR = ../../build/$(TARGET)

LOAD_MODE = load_eeprom

CFLAGS =  -Os -n -m32bit-doubles -fno-exceptions

LOADFLAGS = -Dreset=dtr -S50 

.PHONY: all config load clean

all: config $(OBJDIR)/$(TARGET).elf load clean 



$(OBJDIR)/$(TARGET).elf: bldc.cog $(OBJDIR)/$(TARGET).o
		$(CC) $(INCLUDES) $(LFLAGS) -o $@ -m$(MMODEL) $(CFLAGS) $^ $(LIBS)
		$(PROPGCC)/bin/propeller-elf-size $(OBJDIR)/$(TARGET).elf

$(OBJDIR)/$(TARGET).o:  $(TARGET).cpp
		$(CC) -c $(INCLUDES) -o $@ -m$(MMODEL) $(CFLAGS) $(DEADCODESTRIP) $<

bldc.cog: bldc.c bldc.h
		$(CC) -r $(INCLUDES) $(LFLAGS) $(CFLAGS) -mcog -o $@ $< 
		$(OBJCOPY) --localize-text --rename-section .text=$@ $@	

load: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -r -v $<

load_eeprom: $(OBJDIR)/$(TARGET).elf
		$(LOAD) $(LOADFLAGS) -b $(BOARD) -p $(PORT) -I $(PROPGCC)/propeller-load -e -r -v $<

clean:
	rm -f $(OBJDIR)/*.o;

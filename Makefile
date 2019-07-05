# directory names of every library in src. List in the order they should be built
LIBI2C = i2c
LIBFMATH = fmath
LIBHD44770 = hd44780
LIBM24512 = m24512
LIBMPU6050 = mpu6050
LIBSSD1306 = ssd1306
LIBMCP2515 = mcp2515
LIBMPU9250 = mpu9250
LIBSAM_M8Q = sam-m8q
LIBBLDC = bldc
LIBQKF = qkf
LIBQUADRATURE = quadrature
LIBSERIALUI = serialui

PREFIX = /opt/PropellerLibraries # where to install the libraries

LIBOUTDIR = lib
INCLDIR = include
PLDIR = proplibs
SRCDIR = src
MMODEL ?= cmm # by default build using CMM

export

.PHONY: all setup install $(LIBI2C) $(LIBHD44770) $(LIBM24512) $(LIBMPU6050) $(LIBMPU9250) $(LIBSAM_M8Q) $(LIBSSD1306) $(LIBMCP2515) $(LIBBLDC) $(LIBFMATH) $(LIBQKF) $(LIBQUADRATURE) $(LIBSERIALUI) clean

all: setup $(LIBI2C) $(LIBHD44770) $(LIBM24512) $(LIBMPU6050) $(LIBMPU9250) $(LIBSAM_M8Q) $(LIBSSD1306) $(LIBMCP2515) $(LIBBLDC) $(LIBFMATH) $(LIBQKF) $(LIBQUADRATURE) $(LIBSERIALUI)

setup:
	mkdir -p $(LIBOUTDIR)
	mkdir -p $(INCLDIR)/$(PLDIR)

install: all
	mkdir -p $(PREFIX)
	cp -r $(LIBOUTDIR) $(PREFIX)
	cp -r $(INCLDIR) $(PREFIX)

$(LIBI2C):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBHD44770):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBM24512):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBMPU6050):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBMPU9250):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBSAM_M8Q):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBSSD1306):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBMCP2515):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBBLDC):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBFMATH):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBQKF):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBQUADRATURE):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBSERIALUI):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

clean:
	rm -rf ./$(LIBOUTDIR)
	rm -rf ./$(INCLDIR)
	cd $(SRCDIR)/$(LIBI2C) && make clean
	cd $(SRCDIR)/$(LIBHD44770) && make clean
	cd $(SRCDIR)/$(LIBM24512) && make clean
	cd $(SRCDIR)/$(LIBMPU6050) && make clean
	cd $(SRCDIR)/$(LIBMPU9250) && make clean
	cd $(SRCDIR)/$(LIBSSD1306) && make clean
	cd $(SRCDIR)/$(LIBMCP2515) && make clean
	cd $(SRCDIR)/$(LIBBLDC) && make clean
	cd $(SRCDIR)/$(LIBFMATH) && make clean
	cd $(SRCDIR)/$(LIBSAM_M8Q) && make clean
	cd $(SRCDIR)/$(LIBQKF) && make clean
	cd $(SRCDIR)/$(LIBQUADRATURE) && make clean
	cd $(SRCDIR)/$(LIBSERIALUI) && make clean


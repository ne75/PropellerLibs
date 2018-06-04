# directory names of every library in src. List in the order they should be built
LIBI2C = i2c

LIBHD44770 = hd44780
LIBM24512 = m24512
LIBMPU6050 = mpu6050
LIBSSD1306 = ssd1306
LIBMCP2515 = mcp2515
LIBBLDC = bldc

PREFIX = /opt/PropellerLibraries # where to install the libraries

LIBDIR = lib
INCLDIR = include
PLDIR = proplibs
SRCDIR = src

.PHONY: all setup install $(LIBI2C) $(LIBHD44770) $(LIBM24512) $(LIBMPU6050) $(LIBSSD1306) $(LIBMCP2515) $(LIBBLDC) clean

all: setup $(LIBI2C) $(LIBHD44770) $(LIBM24512) $(LIBMPU6050)  $(LIBSSD1306) $(LIBMCP2515) $(LIBBLDC)

setup:
	mkdir -p $(LIBDIR)
	mkdir -p $(INCLDIR)/$(PLDIR)

install: all
	mkdir -p $(PREFIX)
	cp -r $(LIBDIR) $(PREFIX)
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

$(LIBSSD1306):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBMCP2515):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBBLDC):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

clean:
	rm -rf $(LIBDIR)/
	rm -rf $(INCLDIR)/
	cd $(SRCDIR)/$(LIBI2C) && make clean
	cd $(SRCDIR)/$(LIBHD44770) && make clean
	cd $(SRCDIR)/$(LIBM24512) && make clean
	cd $(SRCDIR)/$(LIBMPU6050) && make clean
	cd $(SRCDIR)/$(LIBSSD1306) && make clean
	cd $(SRCDIR)/$(LIBMCP2515) && make clean
	cd $(SRCDIR)/$(LIBBLDC) && make clean

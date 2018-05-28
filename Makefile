# directory names of every library in src
LIBI2C = i2c

LIBHD44770 = hd44780
LIBM24512 = m24512
LIBMPU6050 = mpu6050
LIBSSD1306 = ssd1306

PREFIX = /opt/PropellerLibraries

LIBDIR = lib
INCLDIR = include
PLDIR = proplibs
SRCDIR = src

.PHONY: all setup install $(LIBI2C) $(LIBHD44770) $(LIBM24512) $(LIBMPU6050)$(LIBSSD1306) clean

## removed $(LIBMPU6050) to temporarily fix the lib build. Please put back when MPU6050 will compile successfully (should branch to work on)
all: setup $(LIBI2C) $(LIBHD44770) $(LIBM24512)  $(LIBSSD1306)

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

clean:
	rm -rf $(LIBDIR)/
	rm -rf $(INCLDIR)/
	cd $(SRCDIR)/$(LIBI2C) && make clean
	cd $(SRCDIR)/$(LIBHD44770) && make clean
	cd $(SRCDIR)/$(LIBM24512) && make clean
	cd $(SRCDIR)/$(LIBMPU6050) && make clean
	cd $(SRCDIR)/$(LIBSSD1306) && make clean

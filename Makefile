# directory names of every library in src
LIBHD44770 = hd44780
LIBI2C = i2c
LIBSSD1306 = ssd1306
LIBM24512 = M24512

LIBDIR = lib
INCLDIR = include
PLDIR = proplibs
SRCDIR = src

.PHONY: all setup install $(LIBHD44770) $(LIBI2C) $(LIBSSD1306) $(LIBM24512) clean

all: setup $(LIBHD44770) $(LIBI2C) $(LIBSSD1306) $(LIBM24512)

setup:
	mkdir -p $(LIBDIR)
	mkdir -p $(INCLDIR)/$(PLDIR)

$(LIBHD44770):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBI2C):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBSSD1306):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

$(LIBM24512):
	cd $(SRCDIR)/$@ && make lib
	cd $(SRCDIR)/$@ && make install

clean:
	rm -rf $(LIBDIR)/
	rm -rf $(INCLDIR)/
	cd $(SRCDIR)/$(LIBHD44770) && make clean
	cd $(SRCDIR)/$(LIBI2C) && make clean
	cd $(SRCDIR)/$(LIBSSD1306) && make clean
	cd $(SRCDIR)/$(LIBM24512) && make clean
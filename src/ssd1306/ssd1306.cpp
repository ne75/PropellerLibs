#include "ssd1306.h"
#include "font_5x7.c"

SSD1306::SSD1306(I2CBus *bus, uint8_t adr, uint8_t p_rst, uint8_t fps) : I2CDevice(bus, adr) {
    this->p_rst = p_rst;
    this->fps = fps;
}

bool SSD1306::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    memcpy(&(dev.data[1]), d, s);
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t SSD1306::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}

void SSD1306::writeCommand(uint8_t c) {
    writeReg(0, &c, 1);
}

void SSD1306::drawPixel(uint8_t x, uint8_t y, uint8_t c) {
    fb[x + (y/8)*SSD1306_WIDTH] &= ~(1 << (y&7)); // clear the bit
    fb[x + (y/8)*SSD1306_WIDTH] |= (c << (y&7)); // set it if c is high
}

void SSD1306::_char(uint8_t x, uint8_t y, uint8_t c, bool inv) {
    uint8_t byte = 0;

    uint8_t *d = &(font5x7[5*c]);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            if (j & 7) {
                byte >>= 1;
            } else {
                byte = d[i + j/8];
            }
            drawPixel(x+i, y+j, inv ^ (byte & 1));
        }
    }

    drawVLine(x-1, y, 8, inv);
    drawVLine(x+5, y, 8, inv);
    drawHLine(x-1, y-1, 7, inv);
}

void SSD1306::string(uint8_t x, uint8_t y, const char *s, bool inv) {
    uint8_t c_x = x, c_y = y;

    for (int i = 0; i < strlen(s); i++) {
        uint8_t ch = s[i];
        c_x += 6;
        if (ch == '\n' | ch == '\r') {
            c_x = x;
            c_y += 8;
        } else {
            _char(c_x - 6, c_y, ch, inv);
        }
    }
}

void SSD1306::drawBitmap(uint8_t x, uint8_t y, uint8_t *d, uint8_t w, uint8_t h) {

    uint8_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int j = 0; j<h; j++) {
        for(int i = 0; i<w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = d[j * byteWidth + i/8];
            }
            drawPixel(x+i, y+j, byte >> 7);
        }
    }
}

void SSD1306::drawHLine(uint8_t x, uint8_t y, uint8_t l, uint8_t c) {
    for (int i = 0; i < l; i++) {
        drawPixel(x + i, y, c);
    }
}

void SSD1306::drawVLine(uint8_t x, uint8_t y, uint8_t l, uint8_t c) {
    for (int i = 0; i < l; i++) {
        drawPixel(x, y+i, c);
    }
}

void SSD1306::clear() {
    memset(fb, 0, (SSD1306_WIDTH*SSD1306_HEIGHT/8));
}

void SSD1306::clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    for (int i = y; i < y+h; i++) {
        for (int j = x; j < x+w; j++) {
            drawPixel(j, i, 0);
        }
    }
}

bool SSD1306::setup() {

    // reset the chip
    DIRA |= 1 << p_rst;
    OUTA |= 1 << p_rst;
    waitcnt(CLKFREQ/1000 + CNT);
    OUTA &= ~(1 << p_rst);
    waitcnt(CLKFREQ/100 + CNT);
    OUTA |= 1 << p_rst;

    while(bus->lock());

    writeCommand(SSD1306_DISPLAYOFF);                       // 0xAE
    writeCommand(SSD1306_SETDISPLAYCLOCKDIV);               // 0xD5
    writeCommand(0x80);                                     // the suggested ratio 0x80

    writeCommand(SSD1306_SETMULTIPLEX);                     // 0xA8
    writeCommand(SSD1306_HEIGHT - 1);

    writeCommand(SSD1306_SETDISPLAYOFFSET);                 // 0xD3
    writeCommand(0x0);                                      // no offset
    writeCommand(SSD1306_SETSTARTLINE | 0x0);               // line #0
    writeCommand(SSD1306_CHARGEPUMP);                       // 0x8D

    writeCommand(0x14);
    writeCommand(SSD1306_MEMORYMODE);                       // 0x20
    writeCommand(0x00);                                     // 0x0 act like ks0108
    writeCommand(SSD1306_SEGREMAP | 0x1);
    writeCommand(SSD1306_COMSCANDEC);

    writeCommand(SSD1306_SETCOMPINS);                       // 0xDA
    writeCommand(0x02);
    writeCommand(SSD1306_SETCONTRAST);                      // 0x81
    writeCommand(0x8F);

    writeCommand(SSD1306_SETPRECHARGE);                     // 0xd9
    writeCommand(0xF1);
    writeCommand(SSD1306_SETVCOMDETECT);                    // 0xDB
    writeCommand(0x40);

    writeCommand(SSD1306_DISPLAYALLON_RESUME);              // 0xA4
    writeCommand(SSD1306_NORMALDISPLAY);                    // 0xA6
    writeCommand(SSD1306_DEACTIVATE_SCROLL);

    writeCommand(SSD1306_DISPLAYON);                        //--turn on oled panel
    bus->unlock();

    cogstart(SSD1306::draw, (void*)this, stack, sizeof(stack));

    return isConnected();
}

bool SSD1306::isConnected() {
    // check connectivity here
    connected = bus->pollDevice(&dev);
    return connected;
}

void SSD1306::draw(void *par) {

    SSD1306 *oled = (SSD1306*)par;

    while (1) {
        while(oled->bus->lock());
        oled->writeCommand(SSD1306_COLUMNADDR);
        oled->writeCommand(0);   // Column start address (0 = reset)
        oled->writeCommand(SSD1306_WIDTH-1); // Column end address (127 = reset)

        oled->writeCommand(SSD1306_PAGEADDR);
        oled->writeCommand(0); // Page start address (0 = reset)
        oled->writeCommand(3); // Page end address

        for (uint16_t i = 0; i < (SSD1306_WIDTH*SSD1306_HEIGHT/8); i+=16) {
            // send a bunch of data in one xmission
            oled->writeReg(0x40, &(oled->fb[i]), 16);
        }
        oled->bus->unlock();

        waitcnt(CLKFREQ/oled->fps + CNT);
    }
}
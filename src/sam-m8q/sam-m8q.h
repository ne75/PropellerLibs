#ifndef _SAM_M8Q_H
#define _SAM_M8Q_H

#include <stdbool.h>
#include <stdint.h>

#ifdef LIB_COMPILE
    #include "i2c/i2cbus.h"
    #include "i2c/i2cdev.h"
    #include "fmath/fmath.h"
#else
    #include "proplibs/i2cbus.h"
    #include "proplibs/i2cdev.h"
    #include "proplibs/fmath.h"
#endif

class SAM_M8Q : public I2CDevice {

private:

    unsigned parser_stack[100];
    uint8_t parser_bytes_available;
    uint8_t nmea_buf[100];
    uint8_t nmea_buf_ptr;

    void parseGGA(uint8_t *dat);
    void parseNMEASentence();
    void parseTime(uint8_t *dat);
    void parseLatitude(uint8_t *dat, uint8_t *unit);
    void parseLongitude(uint8_t *dat, uint8_t *unit);
    void parseGPSQuality(uint8_t *dat);
    void parseNSats(uint8_t *dat);
    void parseHDOP(uint8_t *dat);
    void parseAltitude(uint8_t *dat, uint8_t *unit);
    void parseGeoidalSep(uint8_t *dat, uint8_t *unit);

    static void parser(void *par);

    uint8_t _cutFirstComma(uint8_t *dat);

public:

    f16_t lat; // north
    f16_t lon; // east
    f16_t alt;
    f16_t geoidal_sep;

    uint8_t h;
    uint8_t m;
    uint8_t s;
    uint8_t ds;
    uint32_t t_s;
    uint32_t t_ms;

    uint8_t gps_quality;
    uint8_t n_sats;
    f16_t hdop;

    uint8_t packet_count;

    enum {
        GPS_NO_FIX = 0,
        GPS_FIX,
        GPS_DIFF_FIX
    };

    int test;

    /*
     *
     */
    SAM_M8Q(I2CBus *bus);

    /*
     * write a register to the GPS module
     */
    bool writeReg(uint8_t reg, uint8_t *d, uint8_t s);

    /*
     * read a register from the GPS module
     */
    uint8_t readReg(uint8_t reg, uint8_t s);

    /*
     * write a register to the mag in the MPU6050
     */
    bool writeMagReg(uint8_t reg, uint8_t *d, uint8_t s);

    /*
     * Implementing virtual parent method
     */
    bool isConnected();
    bool setup();

    /*
     * read n bytes from teh GPS
     *
     * returns false if the parser is still processing the previous bytes
     */
    bool read(uint8_t n);

    /*
     * process the latest bytes that were read in by read.
     *
     * returns false if the entire packet was empty.
     */
    bool process(uint8_t n);

};
#endif

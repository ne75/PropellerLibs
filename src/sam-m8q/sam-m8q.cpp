#include "sam-m8q.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Register list
#define SAM_M8Q_BASE_ADDR   0x42

#define REG_NBYTES_H        0xFD // 2 byte
#define REG_DAT_STREAM      0xFF

// NOTE: In the I2CDevice constructor, be careful to shift the address (see i2c documentation as needed)!

SAM_M8Q::SAM_M8Q(I2CBus *bus) : I2CDevice(bus, (SAM_M8Q_BASE_ADDR)<< 1) {
    connected = false;
    nmea_buf_ptr = 0;
}

bool SAM_M8Q::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    memcpy(&(dev.data[1]), d, s);
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t SAM_M8Q::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}

bool SAM_M8Q::isConnected() {
    return connected;
}

bool SAM_M8Q::setup() {
    // no setup to do
    this->connected = true;
    return true;

}

/*
 * helper function sets the first comma out to a zero and returns the offset into the buffer of the next byte
 */
uint8_t SAM_M8Q::_cutFirstComma(uint8_t *dat) {
    uint8_t dat_start = 0;
    for (int i = 0; i < strlen((char*)dat); i++) {
        if (dat[i] == ',') {
            dat[i] = 0; // clear the first comma
            dat_start = i+1;
            break;
        }
    }

    return dat_start;
}

void SAM_M8Q::parseGGA(uint8_t *dat) {

    // split out the data fields into separate strings
    uint8_t *t_str = dat;
    uint8_t next = _cutFirstComma(dat);

    uint8_t *lat_str = &(dat[next]);
    next = _cutFirstComma(lat_str);

    uint8_t *lat_dir_str = &(lat_str[next]);
    next = _cutFirstComma(lat_dir_str);

    uint8_t *lon_str = &(lat_dir_str[next]);
    next = _cutFirstComma(lon_str);

    uint8_t *lon_dir_str = &(lon_str[next]);
    next = _cutFirstComma(lon_dir_str);

    uint8_t *gps_qual_str = &(lon_dir_str[next]);
    next = _cutFirstComma(gps_qual_str);

    uint8_t *n_sats_str = &(gps_qual_str[next]);
    next = _cutFirstComma(n_sats_str);

    uint8_t *hdop_str = &(n_sats_str[next]);
    next = _cutFirstComma(hdop_str);

    uint8_t *alt_str = &(hdop_str[next]);
    next = _cutFirstComma(alt_str);

    uint8_t *alt_unit_str = &(alt_str[next]);
    next = _cutFirstComma(alt_unit_str);

    uint8_t *gs_str = &(alt_unit_str[next]);
    next = _cutFirstComma(gs_str);

    uint8_t *gs_unit_str = &(gs_str[next]);
    next = _cutFirstComma(gs_unit_str);

    parseTime(t_str);
    parseLatitude(lat_str, lat_dir_str);
    parseLongitude(lon_str, lon_dir_str);
    parseGPSQuality(gps_qual_str);
    parseNSats(n_sats_str);
    parseHDOP(hdop_str);
    parseAltitude(alt_str, alt_unit_str);
    parseGeoidalSep(gs_str, gs_unit_str);

}

void SAM_M8Q::parseNMEASentence() {
    updated = true;
    nmea_buf[nmea_buf_ptr-1] = 0; // remove extra new lines
    nmea_buf[nmea_buf_ptr-2] = 0;
    //printf("NMEA Sentance: %s\n", nmea_buf);

    uint8_t dat_start = _cutFirstComma(nmea_buf);

    if (strcmp((char*)nmea_buf, "$GNGGA") == 0) {
        parseGGA(&(nmea_buf[dat_start]));
    }

    memset(nmea_buf, 0, 82);
}

bool SAM_M8Q::process(uint8_t n) {
    uint8_t nodatctr = 0;

    for (int i = 0; i < n; i++) {
        if (dev.data[i] != 0xff) {
            if (dev.data[i] == '$') {
                nmea_buf_ptr = 0;
            }
            nmea_buf[nmea_buf_ptr++] = dev.data[i];
            if (dev.data[i] == '\n') {
                parseNMEASentence();
            }
        } else {
            nodatctr++;
        }
    }

    return nodatctr == n;
}



bool SAM_M8Q::update() {
    updated = false;

    // keep reading until we read out an enitre packet of 0xff
    do {
        readReg(REG_DAT_STREAM, 32);
    } while(!process(32));

    return updated;
}

void SAM_M8Q::parseTime(uint8_t *dat) {
    if (dat[0] == 0) return;

    uint8_t t[7] = {0};
    uint8_t ds[3] = {0};

    memcpy(t, &dat[0], 6);
    memcpy(ds, &dat[7], 2);

    int32_t t_int = atoi((char*)t);
    int32_t ds_int = atoi((char*)ds);

    h = t_int/10000; // get the hours from the integer form of the time
    m = (t_int-h)/100; // get minutes
    s = (t_int-h-m);
    this->ds = ds_int;
    t_s = (h*3600) + (m*60) + s;
    t_ms = this->ds*10;
}

void SAM_M8Q::parseLatitude(uint8_t *dat, uint8_t *unit) {
    if (dat[0] == 0) return;
    if (unit[0] == 0) return;

    uint8_t lat_int_str[5] = {0};
    uint8_t lat_dec_str[6] = {0};

    memcpy(lat_int_str, &dat[0], 4);
    memcpy(lat_dec_str, &dat[5], 5);

    int32_t lat_int = atoi((char*)lat_int_str);
    int32_t lat_dec = atoi((char*)lat_dec_str);

    lat = f16_t(F16_ONE*(lat_int/100)) + f16_t(F16_ONE*(lat_int%100)/60) + f16_t(lat_dec/100000.0f);
    if (unit[0] == 'S') lat.x *= -1;
}

void SAM_M8Q::parseLongitude(uint8_t *dat, uint8_t *unit) {
    if (dat[0] == 0) return;
    if (unit[0] == 0) return;

    uint8_t lon_int_str[6] = {0};
    uint8_t lon_dec_str[6] = {0};

    memcpy(lon_int_str, &dat[0], 5);
    memcpy(lon_dec_str, &dat[6], 5);

    int32_t lon_int = atoi((char*)lon_int_str);
    int32_t lon_dec = atoi((char*)lon_dec_str);

    lon = f16_t(F16_ONE*(lon_int/100)) + f16_t(F16_ONE*(lon_int%100)/60) + f16_t(lon_dec/100000.0f);
    if (unit[0] == 'W') lon.x *= -1;

}

void SAM_M8Q::parseGPSQuality(uint8_t *dat) {
    if (dat[0] == 0) return;
    gps_quality = atoi((char*)dat);
}

void SAM_M8Q::parseNSats(uint8_t *dat) {
    if (dat[0] == 0) return;
    n_sats = atoi((char*)dat);
}

void SAM_M8Q::parseHDOP(uint8_t *dat) {
    if (dat[0] == 0) return;
    hdop = f16_t((float)atof((char*)dat));
}

void SAM_M8Q::parseAltitude(uint8_t *dat, uint8_t *unit) {
    if (dat[0] == 0) return;
    if (unit[0] == 0) return;

    alt = f16_t((float)atof((char*)dat));
}

void SAM_M8Q::parseGeoidalSep(uint8_t *dat, uint8_t *unit) {
    if (dat[0] == 0) return;
    if (unit[0] == 0) return;
    geoidal_sep = f16_t((float)atof((char*)dat));
}
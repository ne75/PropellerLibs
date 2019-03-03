#ifndef _BME280_H
#define _BME280_H

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

class BME280 : public I2CDevice {

private:

    int32_t t_fine;

    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;

    uint8_t runMode;
    uint8_t tStandby;
    uint8_t filter;
    uint8_t tempOverSample;
    uint8_t pressOverSample;
    uint8_t humidOverSample;

public:

    int32_t p;
    int32_t T;

    /*
     *
     */
    BME280(I2CBus *bus);

    /*
     * write a register to the GPS module
     */
    bool writeReg(uint8_t reg, uint8_t *d, uint8_t s);

    /*
     * read a register from the GPS module
     */
    uint8_t readReg(uint8_t reg, uint8_t s);

    /*
     * Implementing virtual parent method
     */
    bool isConnected();
    bool setup();

    /*
     * read out the raw ADC pressure value
     */
    int32_t getRawPressure();

    /*
     * read out the raw ADC temp value
     */
    int32_t getRawTemperature();

    /*
     * compensate raw ADC reading to Pa pressure reading
     */
    uint32_t compensateP(int32_t adc_P);

    /*
     * compensate raw ADC reading to 1/100 degC temperature reading.
     */
    int32_t compensateT(int32_t adc_T);

};
#endif

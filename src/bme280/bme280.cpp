#include "bme280.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Register list
#define BME280_BASE_ADDR         0x77

#define REG_DIG_T1_LSB           0x88
#define REG_DIG_T1_MSB           0x89
#define REG_DIG_T2_LSB           0x8A
#define REG_DIG_T2_MSB           0x8B
#define REG_DIG_T3_LSB           0x8C
#define REG_DIG_T3_MSB           0x8D
#define REG_DIG_P1_LSB           0x8E
#define REG_DIG_P1_MSB           0x8F
#define REG_DIG_P2_LSB           0x90
#define REG_DIG_P2_MSB           0x91
#define REG_DIG_P3_LSB           0x92
#define REG_DIG_P3_MSB           0x93
#define REG_DIG_P4_LSB           0x94
#define REG_DIG_P4_MSB           0x95
#define REG_DIG_P5_LSB           0x96
#define REG_DIG_P5_MSB           0x97
#define REG_DIG_P6_LSB           0x98
#define REG_DIG_P6_MSB           0x99
#define REG_DIG_P7_LSB           0x9A
#define REG_DIG_P7_MSB           0x9B
#define REG_DIG_P8_LSB           0x9C
#define REG_DIG_P8_MSB           0x9D
#define REG_DIG_P9_LSB           0x9E
#define REG_DIG_P9_MSB           0x9F
#define REG_DIG_H1               0xA1
#define REG_CHIP_ID              0xD0 //Chip ID
#define REG_RST                  0xE0 //Softreset Reg
#define REG_DIG_H2_LSB           0xE1
#define REG_DIG_H2_MSB           0xE2
#define REG_DIG_H3               0xE3
#define REG_DIG_H4_MSB           0xE4
#define REG_DIG_H4_LSB           0xE5
#define REG_DIG_H5_MSB           0xE6
#define REG_DIG_H6               0xE7
#define REG_CTRL_HUMIDITY        0xF2 //Ctrl Humidity Reg
#define REG_STAT                 0xF3 //Status Reg
#define REG_CTRL_MEAS            0xF4 //Ctrl Measure Reg
#define REG_CONFIG               0xF5 //Configuration Reg
#define REG_PRESSURE_MSB         0xF7 //Pressure MSB
#define REG_PRESSURE_LSB         0xF8 //Pressure LSB
#define REG_PRESSURE_XLSB        0xF9 //Pressure XLSB
#define REG_TEMPERATURE_MSB      0xFA //Temperature MSB
#define REG_TEMPERATURE_LSB      0xFB //Temperature LSB
#define REG_TEMPERATURE_XLSB     0xFC //Temperature XLSB
#define REG_HUMIDITY_MSB         0xFD //Humidity MSB
#define REG_HUMIDITY_LSB         0xFE //Humidity LSB

// NOTE: In the I2CDevice constructor, be careful to shift the address (see i2c documentation)!

BME280::BME280(I2CBus *bus) : I2CDevice(bus, (BME280_BASE_ADDR)<< 1) {
    connected = false;

    runMode = 0x11;
    tStandby = 0;
    filter = 0;
    tempOverSample = 1;
    pressOverSample = 1;
    humidOverSample = 1;
}

bool BME280::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    memcpy(&(dev.data[1]), d, s);
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t BME280::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}

bool BME280::isConnected() {
    return connected;
}

bool BME280::setup() {

    readReg(REG_CHIP_ID, 1);
    if (dev.data[0] != 0x60) {
        return false; // don't see the BME280 on the bus
    }

    readReg(REG_DIG_T1_LSB, 24);

    dig_T1 = (uint16_t)((dev.data[1] << 8) | dev.data[0]);
    dig_T2 = (int16_t)((dev.data[3] << 8) | dev.data[2]);
    dig_T3 = (int16_t)((dev.data[5] << 8) | dev.data[4]);

    dig_P1 = (uint16_t)((dev.data[7] << 8) | dev.data[6]);
    dig_P2 = (int16_t)((dev.data[9] << 8) | dev.data[8]);
    dig_P3 = (int16_t)((dev.data[11] << 8) | dev.data[10]);
    dig_P4 = (int16_t)((dev.data[13] << 8) | dev.data[12]);
    dig_P5 = (int16_t)((dev.data[15] << 8) | dev.data[14]);
    dig_P6 = (int16_t)((dev.data[17] << 8) | dev.data[16]);
    dig_P7 = (int16_t)((dev.data[19] << 8) | dev.data[18]);
    dig_P8 = (int16_t)((dev.data[21] << 8) | dev.data[20]);
    dig_P9 = (int16_t)((dev.data[23] << 8) | dev.data[22]);

    readReg(REG_DIG_H1, 1);

    dig_H1 = (uint8_t)dev.data[0];

    readReg(REG_DIG_H2_LSB, 7);

    dig_H2 = (int16_t)((dev.data[1] << 8) + dev.data[0]);
    dig_H3 = (uint8_t)(dev.data[2]);
    dig_H4 = (int16_t)((dev.data[4] << 4) + (dev.data[3] & 0x0F));
    dig_H5 = (int16_t)((dev.data[5]  << 4) + ((dev.data[3] >> 4) & 0x0F));
    dig_H6 = (int8_t)dev.data[6];

    uint8_t ctrl = (tempOverSample<<5) | (pressOverSample<<5) | runMode;
    writeReg(REG_CTRL_MEAS, &ctrl, 1);

    this->connected = true;
    return true;
}

int32_t BME280::getRawPressure() {
    readReg(REG_PRESSURE_MSB, 3);

    return ((uint32_t)dev.data[0] << 12) | ((uint32_t)dev.data[1] << 4) | ((dev.data[2] >> 4) & 0x0F);
}

int32_t BME280::getRawTemperature() {
    readReg(REG_TEMPERATURE_MSB, 3);

    return ((uint32_t)dev.data[0] << 12) | ((uint32_t)dev.data[1] << 4) | ((dev.data[2] >> 4) & 0x0F);
}

uint32_t BME280::compensateP(int32_t adc_P) {
    int32_t var1, var2;

    var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
    var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
    var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
    var1 =((((32768+var1))*((int32_t)dig_P1))>>15);

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }

    p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;

    if (p < 0x80000000) {
        p = (p << 1) / ((uint32_t)var1);
    } else {
        p = (p / (uint32_t)var1) * 2;
    }

    var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
    return p;
}

int32_t BME280::compensateT(int32_t adc_T) {
    int32_t var1, var2;
    var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *
    ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;

    return T;
}
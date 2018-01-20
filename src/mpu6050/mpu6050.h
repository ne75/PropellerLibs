#ifndef _MPU6050_H
#define _MPU6050_H

#ifdef LIB_COMPILE
    #include "i2c/i2cbus.h"
    #include "i2c/i2cdev.h"
#else
    #include "proplibs/i2cbus.h"
    #include "proplibs/i2cdev.h"
#endif

class MPU6050 : public I2CDevice {
public:
    MPU6050(I2CBus *bus, uint8_t adr);

    /*
     * Implementing virtual parent method
     */
    bool isConnected();
    bool setup();

    /*
     * write a register to the MPU6050
     */
    bool writeReg(uint8_t reg, uint8_t *d, uint8_t s);

    /*
     * read a register from the MPU6050
     */
    uint8_t readReg(uint8_t reg, uint8_t s);
};
#endif

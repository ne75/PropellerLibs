#ifndef _MPU6050_H
#define _MPU6050_H

/*#include "quatq.h"
#include "vec3.h"*/
#include <stdint.h>
#include <iostream>
#include <stdbool.h>


#ifdef LIB_COMPILE
    #include "i2c/i2cbus.h"
    #include "i2c/i2cdev.h"
#else
    #include "proplibs/i2cbus.h"
    #include "proplibs/i2cdev.h"
#endif

class MPU6050 : public I2CDevice {
public:

	// KE: I've added these. I'm assuming these eventually need to be used.
	bool init;
	uint8_t id;

	quatq orientation;
	quatq q;

	vec3 gyro;
	
	float gs;
	float as;
	
	float accel_fs;
	float gyro_fs;

	uint8_t adr;
	uint8_t errno;


    MPU6050(I2CBus *bus, uint8_t adr);

    /*
     * Implementing virtual parent method
     */
    bool isConnected();
    bool setup(uint8_t gfs, uint8_t afs, quatq orientation);

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

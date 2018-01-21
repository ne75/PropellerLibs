#include "mpu6050.h"

// KE: Added declaration for constants.

//Register list
#define MPU6050_BASE_ADDR	0x68
#define REG_PWR_MGMT_1		0x6B
#define REG_GYRO_CONFIG		0x1B
#define REG_SMPL_RATE_DIV	0x19

// Globals used in the ISR.
MPU6050 imus[IMU_MAX] = { 0 };



// KE: Added libraries
#include <math.h>
#include <plib.h>
#include <stdio.h>

#include "global.h"
#include "timeutil.h"


using namespace std;


MPU6050::MPU6050(I2CBus *bus, uint8_t id) : I2CDevice(bus, adr) {
    this->bus = bus;
	this->id = id;
    dev.adr = MPU6050_BASE_ADDR + id;
    connected = false;


}

bool MPU6050::isConnected() {
    return connected;
}



// The original setup() shell.
/*bool MPU6050::setup() {
    // TODO: add startup stuff
    connected = true;
}*/


/* 

 * KE: First try at porting over code.
 * mpu6050.h changed to include arguments in declaration.
 * imus needs to be created.
 * imu is missing a couple of fields: id, orientation, gyro, gs, as, accel_fs,gyro_fs,q
 * Check to see if writeReg is right.
 * Do we need wait CNT. 
*/

/*
 * Personal notes: id determines which IMU we are using (1 of 2).
*/


bool MPU6050::setup(uint8_t gfs, uint8_t afs, quatq orientation) {
	cout << "# * Init IMU *" << id << endl;	//printf
	this->id = id;									// id is a missing field.		
	uint8_t d = 1 << 7;

	// reset the IMU
	writeReg(REG_PWR_MGMT_1, &d, 1);				// d pointer address, s number of bytes.
	
	// Do we still need the wait CNT?
	// Yes

	d = 1;
	writeReg(REG_PWR_MGMT_1, &d, 1);

	float gfs_lut[] = { 4.3633k, 8.7266k, 17.4533k, 34.9066k };
	float afs_lut[] = { 2k, 4k, 8k, 16k }; 

	imu->gyro_fs = gfs_lut[gfs];
	imu->accel_fs = afs_lut[afs];
	imu->gs = imu->gyro_fs / (1 << 15);
	imu->as = imu->accel_fs / (1 << 15);

	
	// set gyro and accel full scale range
	d = gfs << 3;	
	writeReg(REG_GYRO_CONFIG, &d, 1);

	d = afs << 3;
	writeReg(REG_GYRO_CONFIG, &d, 1);

	d = 15; 
	writeReg(REG_SMPL_RATE_DIV, &d, 1);


	if (quatq_abs(imu->q) < 0.9) {
		imu->q = (quatq) { 1.0k, 0, 0, 0 };

	}

	imu->orientation = orientation; 
	imu->init = true;
	imu->errno = I2C_NOERR;					

	return imu;

}



bool MPU6050::writeReg(uint8_t reg, uint8_t *d, uint8_t s) {
    dev.data[0] = reg;
    memcpy(&(dev.data[1]), d, s);					//
    bool ret = bus->writeData(&dev, dev.data, s+1, true);

    return ret;
}

uint8_t MPU6050::readReg(uint8_t reg, uint8_t s) {
    bus->writeByte(&dev, reg, false);
    bus->readData(&dev, s, true);

    return s;
}


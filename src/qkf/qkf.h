#ifndef _QKF_H
#define _QKF_H

#include <stdint.h>

#ifdef LIB_COMPILE
    #include "fmath/fmath.h"
#else
    #include "proplibs/fmath.h"
#endif

class QKF {

	uint32_t a_t;
	uint32_t w_t;
    uint32_t h_t;

    vec3f16 x;

    unsigned estimator_stack[164];

	static void estimator(void *par);

public:

	f16_t a_beta; // accel LPF constant.
    f16_t h_beta; // mag LPF constant.

	// tunable values
	f16_t Q; 	// process variance
    f16_t R1; 	// measurement variance of the gyro.
    f16_t R2; 	// measurement variance of the accel.
    f16_t R3; 	// measurement variance of the mag.

    // outputs
    f16_t P;
    f16_t K1;
    f16_t K2;
    f16_t K3;

    quatf16 q; 	// the current estimate of orientation

    // inputs to be updated each time step
 	vec3f16 a;
    vec3f16 w;
    vec3f16 h;
    vec3f16 north;

    // set true when new data is available
    bool a_valid;
    bool w_valid;
    bool h_valid;

    // locks for data input and output
    uint8_t out_lock_id; // data lock for the quaternion.
    uint8_t in_lock_id; // data lock for the imu data

    uint32_t exec_time; // stores the number of clock cycles it took to execute the filter loop, not counting the wait time


	QKF();

	void start();

};

#endif
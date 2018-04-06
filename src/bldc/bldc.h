//bldc.h
#ifndef _BLDC_H
#define _BLDC_H

#include <cstdint>
#include <cstdbool>
#include "io_utils.h"						//TODO What is io_utils.h supposed to be?


const PWM_FREQ = 18000;
const PWM_PERIOD = (CLKFREQ/PWM_FREQ); 		//TODO SysFreq = Clkfreq?
const MAX_ANGLE = 3600000;

// Definition for class bldc.
class bldc {

//TODO: Permissions level for the code.

private:

	uint8_t poles;			// number of poles in the motor.
	int32_t mech_angle;		// 0-3600000
	int32_t velocity;


	int32_t elec_angle;
	uint8_t zone;
    uint32_t zone_phase;
    uint32_t pwm_time;

    uint8_t pin1;
    uint8_t pin2;
    uint8_t pin3;

    uint8_t SS_DIV;

    uint32_t duty;

    bool en;
    bool soft_start;
    uint32_t ss_cntr;


	// Anonymous enumeration maintained from before.  Check in on this.
	enum {
		BLDC_MOTOR_PITCH = 0,
		BLDC_MOTOR_ROLL,
		BLDC_MOTOR_MAX
	};


public:
	
	/**
	 *	Class constructor.
	 *	n:			Which motor to initialize (0 or 1).
	 *	npoles: 	Number of poles. 
	 *	Do not optimize this method
	 */
	volatile bldc(uint8_t n, uint8_t npoles);


	/**
	 *
	 */
	inline void bldc_calc_pwm ();

	/** 
	 *  TODO: Determine the use of this.
	 */
	void bldc_pwm_init();

	/** 
	 *  Sets the velocity of the motor in arbitrary units: angle/10 * PWM_FREQ.
	 */
	void bldc_set_velocity(int32_t v);

	/** 
	 *  Steps the position of the motor in electrical angles (10th of degrees)
	 */
	void bldc_step_angle(int32_t a);

}

#endif

#ifndef _DHT11_H
#define _DHT11_H

#include <propeller.h>
#include <stdint.h>

#ifdef LIB_COMPILE
    #include "fmath/fmath.h"
#else
    #include "proplibs/fmath.h"
#endif

class DHT11 {

	uint8_t pin;

	bool start();
    void low();
    void high();
	uint8_t get_bit();

public:

    f16_t T;
    f16_t RH;

	DHT11(uint8_t pin);

	bool read();
};

#endif
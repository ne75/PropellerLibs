#include "dht11.h"

#define T_START     (CLKFREQ/50) // 20ms
#define T_HIGH      (CLKFREQ/20000) // 50us

DHT11::DHT11(uint8_t pin) {
    this->pin = pin;
    DIRA &= ~(1 << pin);
    OUTA &= ~(1 << pin);
}

void DHT11::low() {
    DIRA |= 1 << pin;
}

void DHT11::high() {
    DIRA &= ~(1 << pin);
}

bool DHT11::start() {
    // send 20ms low and wait for response.
    low();
    waitcnt(CNT + T_START);
    high();

    uint32_t t = CNT;
    while(INA & (1<<pin)) {
        // if 1ms passes and no low signal, return false
        if (CNT - t > CLKFREQ/1000) {
            return false;
        }
    }

    // wait for device release the line
    waitpeq(1, 1<<pin);

    return true;
}

inline uint8_t DHT11::get_bit() {
    waitpeq(0, 1<<pin);
    waitpeq(1, 1<<pin);
    uint32_t t_start = CNT;
    waitpeq(0, 1<<pin);
    uint32_t dt = CNT - t_start;
    if (dt > T_HIGH) {
        return 1;
    } else {
        return 0;
    }

}

bool DHT11::read() {
    uint8_t dat[5] = {0};
    uint8_t bits[40] = {0};

    if (start()) {
        for (int i = 0; i < 40; i++) {
            bits[i] = get_bit();
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 8; j++) {
                dat[i] |= bits[8*i + j] << (7-j);
            }
        }

        uint8_t check = dat[0] + dat[1] + dat[2] + dat[3];
        if (check != dat[4]) return false; // checksum failed, there was a read issue.

        T = f16(dat[2]) + f16(dat[3])/10;
        RH = f16(dat[0]);
    } else {
        return false;
    }

    return true;

}
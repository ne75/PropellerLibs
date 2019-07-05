#include <propeller.h>

#include "bldc.h"
#include "stdio.h"

BLDC::BLDC(uint8_t id, uint8_t pa, uint8_t pb, uint8_t pc, uint8_t poles) {
    stack_bldc[BLDC_STACK_SIZE] = (unsigned)&m;

    m.pin1 = pa;
    m.pin2 = pb;
    m.pin3 = pc;
    m.id = id;
    m.poles = poles;
    m.velocity = 0;
}

void BLDC::init() {
    extern unsigned int *bldc_code;
    cognew(bldc_code, &stack_bldc[BLDC_STACK_SIZE]);
}

void BLDC::enable(bool en) {
    m.en = en;
}

void BLDC::step(int32_t a) {
    m.elec_angle += (a*1000) % MAX_ANGLE;
}

void BLDC::set_velocity(int32_t v) {
    m.velocity = v;
}

void BLDC::set_power(uint8_t p) {
    m.power = p;
}
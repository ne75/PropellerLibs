#include <propeller.h>

#include "quadrature.h"

Quadrature::Quadrature() {}

Quadrature::Quadrature(uint8_t pa, uint16_t cpr, int32_t Kp) {
    stack[QUADRATURE_STACK_SIZE] = (unsigned)&enc_mb;

    enc_mb.pin = pa;
    enc_mb.kp = Kp;
    enc_mb.ki = Kp*Kp/4;
    this->cpr = cpr;

    this->t = &(enc_mb.count);
    this->w = &(enc_mb.vel);
}

void Quadrature::init() {
    extern uint32_t *quad_cog;
    zero();
    cognew(quad_cog, &stack[QUADRATURE_STACK_SIZE]);
}

void Quadrature::zero() {
    enc_mb.count = 0;
    enc_mb.vel = 0;
}
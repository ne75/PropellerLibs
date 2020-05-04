#include <propeller.h>

#include "quadrature.h"

Quadrature::Quadrature() {}

Quadrature::Quadrature(uint8_t pa, uint16_t cpr, int32_t Kp) {
    stack[QUADRATURE_STACK_SIZE] = (unsigned)&enc_mb;

    enc_mb.pin = pa;
    enc_mb.kp = Kp;
    enc_mb.ki = Kp*Kp/4;
    this->cpr = cpr;
    enc_mb.count = 0;
    enc_mb.vel = 0;

    this->t = &(enc_mb.count);
    this->w = &(enc_mb.vel);

    enc_mb.lock_id = locknew();
    cogid = -2;
}

void Quadrature::init() {
    extern uint32_t *quad_cog;
    zero();
    cogid = cognew(quad_cog, &stack[QUADRATURE_STACK_SIZE]);
}

void Quadrature::zero() {
    while(lockset(enc_mb.lock_id));
    enc_mb.count = 0;
    enc_mb.vel = 0;
    lockclr(enc_mb.lock_id);
}
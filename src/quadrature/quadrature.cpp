#include <propeller.h>

#include "quadrature.h"

#define CALC_FREQ       1000
#define CALC_PER        (CLKFREQ/CALC_FREQ)

Quadrature::Quadrature() {}

Quadrature::Quadrature(uint8_t pa, uint16_t cpr) {
    stack[QUADRATURE_STACK_SIZE] = (unsigned)&enc_mb;

    enc_mb.pin = pa;

    this->cpr = f16(cpr);
    pll_pos = 0;
    pll_vel = 0;
}

void Quadrature::init() {
    extern uint32_t *quad_cog;
    cognew(quad_cog, &stack[QUADRATURE_STACK_SIZE]);
}

void Quadrature::init_calc(f16_t Kp) {
    this->Kp = Kp;
    this->Ki = 0.25f*(Kp*Kp);
    cogstart(Quadrature::calc, (void*)this, calc_stack, sizeof(calc_stack));
}

int32_t Quadrature::get_count() {
    return enc_mb.count;
}

void Quadrature::zero() {
    enc_mb.count = 0;
    pll_pos = 0;
}

void Quadrature::calc(void *par) {
    Quadrature *encoder = (Quadrature*)par;

    f16_t pll_kp = encoder->Kp;
    f16_t pll_ki = encoder->Ki;

    const f16_t dt = f16(1.0f/(CALC_FREQ));

    uint32_t t = CNT;
    while(1) {
        t = CNT;
        // angle = 2*pi*count/(cpr)
        f16_t c = f16(encoder->get_count());

        encoder->pll_pos = encoder->pll_pos + encoder->pll_vel*dt;
        f16_t dp = (c - encoder->pll_pos.floor());

        encoder->pll_pos = encoder->pll_pos + pll_kp*dp*dt;
        encoder->pll_vel = encoder->pll_vel + pll_ki*dp*dt;

        encoder->t = encoder->pll_pos;
        encoder->w = encoder->pll_vel;

        encoder->loop_time = CNT-t;

        waitcnt(t + CALC_PER);
    }
}
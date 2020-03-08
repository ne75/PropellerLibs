#include <propeller.h>

#include "quadrature.h"

#define CALC_FREQ       1000
#define CALC_PER        (CLKFREQ/CALC_FREQ)

Quadrature::Quadrature() {}

Quadrature::Quadrature(uint8_t pa, uint16_t cpr) {
    stack[QUADRATURE_STACK_SIZE] = (unsigned)&enc_mb;

    enc_mb.pin = pa;

    this->cpr = f16(cpr);
}

void Quadrature::init() {
    extern uint32_t *quad_cog;
    cognew(quad_cog, &stack[QUADRATURE_STACK_SIZE]);
}

void Quadrature::init_calc(f16_t Kp) {
    this->Kp = Kp;
    this->Ki = f16_t(0.25f)*(Kp*Kp);
    cogstart(Quadrature::calc, (void*)this, calc_stack, sizeof(calc_stack));
}

int32_t Quadrature::get_count() {
    return enc_mb.count;
}

void Quadrature::zero() {
    enc_mb.count = 0;
    t = 0;
    w = 0;
}

void Quadrature::calc(void *par) {
    Quadrature *encoder = (Quadrature*)par;

    f16_t pll_kp = encoder->Kp;
    f16_t pll_ki = encoder->Ki;

    //const f16_t dt = f16(1.0f/CALC_FREQ);
    f16_t pll_pos; // compute this in counts
    f16_t pll_vel; // compute this in counts/s
    f16_t c;

    uint32_t t = CNT;
    while(1) {
        t = CNT;
        // angle = 2*pi*count/(cpr)
        f16_t c = f16(encoder->get_count());
        pll_pos = encoder->t;
        pll_vel = encoder->w;

        pll_pos.x = pll_pos.x + pll_vel.x/CALC_FREQ;
        //f16_t dp = (c - pll_pos.floor());
        f16_t dpdt = f16_t((int32_t)((c - pll_pos.floor()).x/CALC_FREQ));
        pll_pos = pll_pos + pll_kp*dpdt;
        pll_vel = pll_vel + pll_ki*dpdt;

        encoder->t = pll_pos;
        encoder->w = pll_vel;

        encoder->loop_time = CNT-t;

        waitcnt(t + CALC_PER);
    }
}
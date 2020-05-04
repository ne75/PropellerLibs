#include "qkf.h"

#include <propeller.h>

QKF::QKF() {
    out_lock_id = locknew();
    in_lock_id = locknew();
    lockclr(out_lock_id);
    lockclr(in_lock_id);

    setup_complete = false;
}

void QKF::setup(vec3f16 g, vec3f16 h) {


    setup_complete = true;
}

bool QKF::start() {
    if (!setup_complete) return false;
    int cogid = cogstart(estimator, (void*)this, estimator_stack, sizeof(estimator_stack));
    return cogid != -1;
}

void QKF::estimator(void *par) {

    DIRA |= 1 << 18;

    QKF *qkf = (QKF*)par;

    volatile int sample_time_a = 0;
    volatile int sample_time_w = 0;
    volatile int sample_time_h = 0;
    volatile int t = 0;

    // kalman variables
    f16_t P = F16_ONE/100;

    f16_t Q = qkf->Q;
    f16_t R1 = qkf->R1;
    f16_t R2 = qkf->R2;
    f16_t R3 = qkf->R3;
    f16_t K1 = 0;
    f16_t K2 = 0;
    f16_t K3 = 0;

    vec3f16 a;
    vec3f16 w;
    vec3f16 h;

    int32_t df_a;
    int32_t df_w;
    int32_t df_h;

    vec3f16 x(0, 0, 0);
    vec3f16 z1(0, 0, 0); // gyro measurement = w*dt
    vec3f16 z2(0, 0, 0); // accel measurement = dtheta between accel vector and current up estimate
    vec3f16 z3(0, 0, 0); // mag measurement = dtheta between mag vector and current local north estimate

    vec3f16 a_exp(0, 0, 0); // estimated accel vector
    vec3f16 n_exp(0, 0, 0); // estimated local north vector (normalized)
    quatf16 dq;

    while (1) {
        while(lockset(qkf->in_lock_id));
        w = qkf->w;
        a = qkf->a;
        h = qkf->h;
        lockclr(qkf->in_lock_id);

        OUTA ^= 1 << 18;

        t = CNT;
        df_a = CLKFREQ/(t - sample_time_a) + 1;
        df_w = CLKFREQ/(t - sample_time_w) + 1;
        df_h = CLKFREQ/(t - sample_time_h) + 1;

        if (qkf->a_valid) {
            sample_time_a = t; // accel/gyro is always valid.
            qkf->a_valid = false;
        }

        if (qkf->w_valid) {
            sample_time_w = t;
            qkf->w_valid = false;
        }

        if (qkf->h_valid) {
            sample_time_h = t;
            qkf->h_valid = false;
        }

        // 1. calculate Delta theta for quaternion integration
        z1.x.x = w.x.x/df_w;
        z1.y.x = w.y.x/df_w;
        z1.z.x = w.z.x/df_w;

        // 2. compute expected gravity vector and expected north vector
        a_exp = qkf->q.up(); // need rederve the up formula
        n_exp = qkf->q.conj().rotate(qkf->north);

        // 3. compute angle error, z. a and a_exp have magnitude 1, so ||a x a_exp|| = sin(err) ~= err
        // add in the error due to mag. might need to change around the axis, but hopefully not.
        z2 = a.cross(a_exp);
        z3 = h.cross(n_exp);

        // 4. update predict P, compute K, update P
        R1.x = qkf->R1.x/df_w;
        R2.x = qkf->R2.x/df_a;
        R3.x = qkf->R3.x/df_h;

        P.x = P.x + Q.x/df_w;
        // P doesn't seem to be calculated correctly, as it always gives 0/1, which is tiny.
        // K here is also approimate. Computing the full inverse of (H*P*H^T + R) is nasty, and the result doesn't seem to make that much sense.

        K1 = P/(P+R1);
        K2 = P/(P+R2);
        K3 = P/(P+R3);

        P = P*(f16_t(F16_ONE) - (K1+K2+K3));

        // 5. apply K to x and y = z-x
        x = x + (z1-x)*K1 + (z2-x)*K2 + (z3-x)*K3;

        // 6. create the delta quaternion from x
        dq = quatf16(x);

        // 7. multiply dq*q to get the new orientation.
        while(lockset(qkf->out_lock_id));
        qkf->q = (dq*qkf->q).normalize();
        qkf->K1 = K1;
        qkf->K2 = K2;
        qkf->K3 = K3;
        qkf->P = P;
        qkf->x = x;
        lockclr(qkf->out_lock_id);

        qkf->exec_time = CNT - t;
        waitcnt(t + CLKFREQ/100);
    }
}

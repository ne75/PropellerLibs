import matplotlib.pyplot as plt
import numpy as np
from math import *

plt.style.use('gadfly')

def get_encoder_samples(t, p):
    '''
    t: time array
    s: encoder position array

    returns array of reported encoder positions and velocities at each time in t
    '''

    # use a PLL to estimate velocity
    length = len(t)

    #p = np.floor(p)
    pll_p = length*[0.0]
    pll_v = length*[0.0]
    pll_dp = length*[0.0]

    kp = 200
    ki = kp*kp/4

    for i in range(1, length):
        dt = t[i]-t[i-1];

        pll_p[i] = pll_p[i-1] + (pll_v[i-1]*dt)

        dp = float(p[i] - (pll_p[i]))
        pll_dp[i] = dp;

        pll_p[i] += kp*dp*dt;
        pll_v[i] = pll_v[i-1] + ki*dp*dt;

    return (np.floor(p), np.floor(pll_v), np.floor(pll_p), np.array(pll_dp))

def get_encoder_samples_v2(t, p):
    '''
    t: time array
    s: encoder position array

    returns array of reported encoder positions and velocities at each time in t
    '''

    # use a PLL to estimate velocity
    length = len(t)

    p = np.floor(p)
    pll_p = length*[0.0]
    pll_v = length*[0.0]
    pll_v_int = length*[0.0]
    pll_dp = length*[0.0]

    kp = 200
    ki = kp*kp/4

    for i in range(1, length):
        dt = t[i]-t[i-1];

        pll_p[i] = pll_p[i-1] + (pll_v[i-1]*dt)

        dp = float(p[i] - floor(pll_p[i]))
        pll_dp[i] = dp

        #pll_p[i] += kp*dp*dt
        pll_v_int[i] = pll_v_int[i-1] + ki*dp*dt
        pll_v[i] = kp*dp + pll_v_int[i]

    return (np.floor(p), np.floor(pll_v), np.floor(pll_v_int), np.array(pll_dp))

def create_motion_curves(dt):
    '''

    returns an ideal array of positions and velocities at each time t
    '''
    K = 1; # scale the results by K so simulate changing the cpr
    t = [0]
    v = [0]
    p = [0]
    i = 1

    # will do a slow ramp to a constant v, a few small jerks in v, then a slow ramp down, then repeat with a fast ramp

    # basic ramp first
    a = 0.01
    for k in range(1, 10000):
        t.append(i*dt)
        v.append(v[i-1] + a)

        i+=1

    for k in range(0, 1000):
        t.append(i*dt)
        v.append(v[i-1])

        i+=1

    for k in range(0, 10000):
        t.append(i*dt)
        v.append(v[i-1] - a)

        i+=1

    for k in range(0, 10000):
        t.append(i*dt)
        v.append(v[i-1])

        i+=1

    # now add some small peaks to the velocity
    for k in range(12000, 28000, 2000):
        s = 0.25
        w = 80
        for j in range (0, w):
            v[k+j] += j*s
        for j in range(0, 2*w):
            v[k+j+w] += (w-j)*s
        for j in range (0, w):
            v[k+j+(3*w)] += (-w+j)*s

    # now a fast ramp
    for k in range(1, 2000):
        t.append(i*dt)
        v.append(v[i-1] + 0.05)

        i+=1

    for k in range(0, 5000):
        t.append(i*dt)
        v.append(v[i-1])

        i+=1

    for k in range(0, 2000):
        t.append(i*dt)
        v.append(v[i-1] - 0.05)

        i+=1

    # now compute position by integrating velocity
    i = 1
    for k in v[1:]:
        p.append(p[i-1] + k*dt)
        i+=1

    return (np.array(t), np.array(p)*K, np.array(v)*K)


def main():
    t, p, v = create_motion_curves(1.0/10000)
    enc_p, enc_v, enc_p_int, dp = get_encoder_samples(t, p)
    enc_p2, enc_v2, enc_v_int2, dp2 = get_encoder_samples_v2(t, p)

    p *= (2*pi/1000)*8.0899
    v *= (2*pi/1000)*8.0899

    enc_p *= (2*pi/1000)*8.0899
    enc_v *= (2*pi/1000)*8.0899
    enc_p_int *= (2*pi/1000)*8.0899

    enc_p2 *= (2*pi/1000)*8.0899
    enc_v2 *= (2*pi/1000)*8.0899
    enc_v_int2 *= (2*pi/1000)*8.0899

    plt.subplot(3,1,1)
    plt.plot(t, p, label='true')
    plt.plot(t, enc_p, '-', markersize=4, markeredgewidth=0.5, markerfacecolor='None', label='measured')
    plt.plot(t, enc_p_int, '-', markersize=4, markeredgewidth=0.5, markerfacecolor='None', label='integrator')
    plt.legend(loc='upper left')
    plt.subplot(3,1,2)
    plt.plot(t, v, label='true')
    plt.plot(t, enc_v, '-', markersize=4, markeredgewidth=0.5, markerfacecolor='None', label='measured')
    plt.legend(loc='upper left')
    plt.subplot(3,1,3)
    plt.plot(t, dp)

    plt.figure()

    plt.subplot(3,1,1)
    plt.plot(t, p, label='true')
    plt.plot(t, enc_p2, '-', markersize=4, markeredgewidth=0.5, markerfacecolor='None', label='measured')
    plt.legend(loc='upper left')
    plt.subplot(3,1,2)
    plt.plot(t, v, label='true')
    plt.plot(t, enc_v2, '-', markersize=4, markeredgewidth=0.5, markerfacecolor='None', label='measured')
    plt.plot(t, enc_v_int2, '-', markersize=4, markeredgewidth=0.5, markerfacecolor='None', label='integrator')
    plt.legend(loc='upper left')
    plt.subplot(3,1,3)
    plt.plot(t, dp2)

    plt.show()

if __name__ == "__main__":
    main()
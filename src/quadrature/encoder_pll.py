import matplotlib.pyplot as plt
from math import *
from statistics import mean, stdev

def sim_encoder():

    dt = 0.0001 #100 us time step
    f_sim = 1/dt
    sim_length = int(10/dt) # 10 second sim
    f_s = 500 # sample freq

    t = [i*dt for i in range(sim_length)]
    v_true = sim_length*[0.0] # counts per second
    p_true = sim_length*[0.0] # counts
    e_p = [] # quantized counts
    e_v = sim_length*[0.0] # velocity as computed by dp/dt

    # generate a velocity sequence
    for i in range(1, int(2/dt)):
        v_true[i] = v_true[i-1] + 0.002

    for i in range(int(2/dt), int(4/dt)):
        v_true[i] = v_true[i-1]

    for i in range(int(4/dt), int(7/dt)):
        v_true[i] = v_true[i-1]*.9999

    for i in range(int(7/dt), sim_length):
        v_true[i] = v_true[i-1] - 0.0001

    # compute position based on v sequence
    for i in range(1, sim_length):
        p_true[i] = p_true[i-1] + dt*v_true[i-1]

    e_p = [floor(p) for p in p_true];

    p_last = 0;
    for i in range(1, sim_length):
        if (i % (f_sim/f_s)) == 0:
            e_v[i] = (e_p[i] - p_last)*f_s
            p_last = e_p[i]
        else:
            e_v[i] = e_v[i-1]

    pll_p = sim_length*[0.0]
    pll_v = sim_length*[0.0]
    kp = 100
    ki = kp*kp/4

    for i in range(1, sim_length):
        if (i % (f_sim/f_s)) == 0:
            pll_p[i] = pll_p[i-1] + pll_v[i-1]/f_s
            dp = float(e_p[i] - floor(pll_p[i]))
            pll_p[i] += (kp * dp)/f_s;
            pll_v[i] = pll_v[i-1] + (ki * dp)/f_s;
        else:
            pll_p[i] = pll_p[i-1]
            pll_v[i] = pll_v[i-1]

    plt.plot(t, v_true);
    plt.plot(t, p_true);
    plt.plot(t, e_p);
    #plt.plot(t, e_v);

    # plt.plot(t, pll_p);
    plt.plot(t, pll_v);

    plt.show()

def sim_vel_pll(enc_pos, dt):
    # use a PLL to estimate velocity
    length = len(enc_pos)

    pll_p = length*[0.0]
    pll_v = length*[0.0]

    kp = 60
    ki = kp*kp/4

    for i in range(1, length):
        pll_p[i] = pll_p[i-1] + (pll_v[i-1]*dt)

        dp = float(enc_pos[i] - floor(pll_p[i]))

        pll_p[i] += kp*dp*dt;
        pll_v[i] = pll_v[i-1] + ki*dp*dt;

    return (pll_p, pll_v)

def real_encoder():
    f = 500.0
    dt = 1/f

    fname = "encoder_samples"

    # get samples
    with open(fname) as file:
        enc = file.readlines()
        enc = [int(x.strip()) for x in enc]

    length = len(enc)

    t = [x*dt for x in range(length)]
    vel_simple = [0.0]
    vel_pll_int = [0.0]

    # ideal
    vel_ideal = [0.0]*500
    vel_ideal = vel_ideal + ((length-500)*[62.5])
    pos_ideal = [0.0]
    for i in range(1, length):
        pos_ideal.append(pos_ideal[i-1] + vel_ideal[i]*dt)

    # simple dx/dt calculation
    for i in range(1, length):
        vel_simple.append((enc[i] - enc[i-1])*f)

    # use a PLL to estimate velocity
    (pll_p, pll_v) = sim_vel_pll(enc, dt)

    # dx/dt calculation using pll position estimate
    for i in range(1, length):
        vel_pll_int.append((pll_p[i] - pll_p[i-1])*f)


    # now get the file that uses PID control for velocity
    fname = "encoder_samples_pid"

    with open(fname) as file:
        enc_pid = file.readlines()
        enc_pid = [int(x.strip()) for x in enc_pid]

    (pll_p_pid, pll_v_pid) = sim_vel_pll(enc_pid, dt)

    # plt.xlabel('t [s]')
    # plt.plot(t, pos_ideal, label='pos ideal')
    # #plt.plot(t, enc, label='pos encoder')
    # #plt.plot(t, pll_p, label='pos pll')
    # plt.plot(t, enc_pid, label='pos w/ pid')
    # plt.plot(t, vel_ideal, label='v ideal')

    # plt.plot(t, pll_p_pid, label='pos pll w/ pid')
    # #plt.plot(t, pll_v, label='v pll')
    # plt.plot(t, pll_v_pid, label='v pll w/ pid')

    # plt.legend(loc='upper right')

    # plt.figure()

    fname = "encoder_samples_pid2"
    f = 250.0;
    dt = 1/f;

    with open(fname) as file:
        enc_pid = file.readlines()
        enc_pid = [float(x.strip()) for x in enc_pid]

    length = len(enc_pid)

    t = [x*dt for x in range(length)]
    v_raw = [0]
    for i in range(1, length):
        v_raw.append((enc_pid[i] - enc_pid[i-1])/dt)

    (pll_p_pid, pll_v_pid) = sim_vel_pll(enc_pid, dt)
    #plt.plot(t, enc_pid, label='pos_raw')
    plt.plot(t, v_raw, label='vel_raw')
    plt.plot(t, pll_v_pid, label='v pll w/ pid')
    plt.legend(loc='upper right')

    # plt.figure();

    # ideal_v = 1000
    # v_vals = pll_v_pid[1000:]
    # t_new = t[1000:]
    # error = [x - ideal_v for x in v_vals]
    # mean_error = mean(error)
    # std_error = stdev(error)
    # print mean_error, std_error

    # plt.plot(t_new, error, label='velocity error')
    # plt.plot(t_new, len(t_new)*[mean_error], label='mean error')
    # plt.legend(loc='upper right')

    plt.show()

def main():
    plt.style.use('gadfly')

    real_encoder()
    #sim_encoder()

if __name__ == "__main__":
    main()
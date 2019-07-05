import matplotlib.pyplot as plt
from math import *

def main():
    plt.style.use('gadfly')

    n_steps = 3600

    time = range(n_steps)
    u = []
    v = []
    w = []

    # v = 0
    for t in time[:n_steps/3]:
        v_normal = cos(2*pi*(t+n_steps/3)/n_steps)
        v.append(0)

        u.append(cos(2*pi*t/n_steps) - v_normal)
        w.append(cos(2*pi*(t+2*n_steps/3)/n_steps) - v_normal)

    # u = 0
    for t in time[n_steps/3:2*n_steps/3]:
        u_normal = cos(2*pi*t/n_steps)
        u.append(0)

        v.append(cos(2*pi*(t+n_steps/3)/n_steps) - u_normal)
        w.append(cos(2*pi*(t+2*n_steps/3)/n_steps) - u_normal)

    # w = 0
    for t in time[2*n_steps/3:n_steps]:
        w_normal = cos(2*pi*(t+2*n_steps/3)/n_steps)
        w.append(0)

        u.append(cos(2*pi*t/n_steps)-w_normal)
        v.append(cos(2*pi*(t+n_steps/3)/n_steps)-w_normal)

    u_new = [int(65535*f/max(u)) for f in u]
    v_new = [int(65535*f/max(v)) for f in v]
    w_new = [int(65535*f/max(w)) for f in w]

    print(u_new[:n_steps/3])

    plt.plot(time, u_new, label='U')
    plt.plot(time, v_new, label='V')
    plt.plot(time, w_new, label='W')
    plt.legend(loc='upper right')

    plt.figure()
    plt.plot(time[:n_steps/3], u_new[:n_steps/3], label='U')

    plt.show()

if __name__ == "__main__":
    main()
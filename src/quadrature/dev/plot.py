import matplotlib.pyplot as plt

def get_input(fname):
    t = []
    v = []
    with open(fname) as f:
        for l in f.readlines():
            dat = l.strip().split(',')
            t.append(float(dat[0])/80000000.0)
            v.append(float(dat[1]))

    return (t, v);


def main():

    plt.style.use('gadfly')

    (t, v) = get_input('velocity_samples')
    plt.plot(t, v, label='vel')
    plt.show()

if __name__ == "__main__":
    main()
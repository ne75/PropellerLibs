import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import time
import struct
import threading
import sys

CMD_CLEAR = 1
CMD_RESET = 2
CMD_WRITE = 3
CMD_ADD = 4

port = '/dev/cu.usbserial-A601RZ26'

'''
Helper Classes

'''

class SerialUIMessage(object):
    """docstring for SerialUIMessage"""

    MESSAGE_FORMAT = '<2B8i' # a message is little endian, with 2 unsigned bytes to start and then 8 signed integers for values
    m_struct = struct.Struct(MESSAGE_FORMAT)

    def __init__(self, raw_message):
        self.raw = raw_message
        self.l = len(raw_message)
        zeros_needed = SerialUIMessage.m_struct.size - self.l             # figure out how many zeros are needed to fill the struct
        padded_m = raw_message + zeros_needed*b'\0'                       # pad the message with zeros to make it a fixed length for python structs
        message = SerialUIMessage.m_struct.unpack(padded_m)

        self.crc = message[0]
        self.op = message[1]
        self.vals = [x for x in message[2:]]

    def validate(self):
        crc = self._calcCRC8(self.raw[1:], self.l-1)
        if (self.crc == crc):
            return True
        else:
            print('got a bad message! CRC was {} but should have been {}'.format(crc, self.crc))
            print('data is: op = ' + str(self.op) + ', values = ' + str(self.vals))
            return False

    def _calcCRC8(self, s, l):

        crc = 0
        CRC7_POLY = 0x91

        for i in range(l):
            crc ^= s[i]
            for j in range(8):
                if (crc & 1):
                    crc ^= CRC7_POLY
                crc >>= 1

        return crc

class PlotterVariable(object):
    """docstring for PlotterVariable"""
    def __init__(self, name, i):
        self.name = name
        self.id = i
        self.t = []
        self.x = []

    def add_point(self, t, x):
        self.t.append(t)
        self.x.append(x)

'''
All the serial interface functions

'''

def init_serial():
    s = serial.Serial(port, 115200, timeout=1)
    s.reset_input_buffer()

    return s

def read(ser):
    # read out whatever is being written, whether it's a message or just a log print.
    # log prints must start with # and end with new line
    # this function will use the ser object's timeout. timeout shouldn't be 0, or else data might be skipped.

    b = ser.read()

    if b == b'#':
        m_raw = ser.read_until(b'\n')                           # log messages must end in \n
        m = m_raw.decode('ascii')                               # decode the log message as ascii (can't be anything else)
        log_message = ('LOG:' + m).strip()                      # format the message
        print(log_message)
    elif b == b'$':
        l_b = ser.read()                                        # read the next byte which tells the length
        l = int.from_bytes(l_b, 'little')                       # decode it to an integer l
        m = ser.read(l)                                         # read out l bytes into a message m
        msg = SerialUIMessage(m)
        if msg.validate():
            return msg
        else:
            return None

    return None

def process_add_buf(m_buf):

    var_str = ''
    buf_full = False

    for m in m_buf:
        if 0 in m.vals[2:]:
            end = m.vals.index(0)
            buf_full = True
        else:
            end = 8

        var_str = var_str + ''.join(chr(i) for i in m.vals[2:end])

    if buf_full:
        return var_str
    else:
        return ''

'''
All the plotter functions

'''

def plotter_add_variable(var):
    var_dict[var.id] = var

def plotter_add_point(var, t, x):
    var.add_point(t, x/65536.0)

def clear_plot(fig):
    for k, v, in var_dict.items():
        v.t = []
        v.x = []

def reset_plot(fig):
    clear_plot(fig)
    global var_dict
    var_dict = {}

def update_plot(ax1):
    ax1.clear()

    for k,v in var_dict.items():
        ax1.plot(v.t, v.x, '-o', markersize=3, markeredgewidth=0.5, markerfacecolor='None', label=v.name)

    if var_dict:
        ax1.legend(loc='upper left')

def reader(ser):
    add_msg_buf = []
    global ax1
    global stop_thread
    start_time = time.time()

    while(not stop_thread):
        m = read(ser)
        if m:
            if m.op == CMD_CLEAR:
                clear_plot(ax1)
            elif m.op == CMD_RESET:
                reset_plot(ax1)
            elif m.op == CMD_WRITE:
                ts = time.time() - start_time
                if m.vals[0] in var_dict:
                    plotter_add_point(var_dict[m.vals[0]], ts, m.vals[1])
                # ignore this message if we don't have that variable

            elif m.op == CMD_ADD:
                add_msg_buf.append(m)
                r = process_add_buf(add_msg_buf)
                if r:
                    plotter_add_variable(PlotterVariable(r, m.vals[1]))
                    add_msg_buf = []


def main():
    global fig
    global ax1
    global var_dict
    global stop_thread

    ser = init_serial()
    plt.style.use('gadfly')

    fig = plt.figure()
    ax1 = fig.add_subplot(1,1,1)
    var_dict = {}

    stop_thread = False
    x = threading.Thread(target=reader, args=(ser,))
    x.start()

    def animate(i):
        update_plot(ax1)

    ani = animation.FuncAnimation(fig, animate, interval=10)
    plt.show()

    stop_thread = True
    x.join()
    ser.close()


if __name__ == "__main__":
    main()
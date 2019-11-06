#include "SerialPlot.h"
#include <math.h>
#include <propeller.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SerialPlot::SerialPlot() {

}


SerialPlot::SerialPlot(FILE *ser_dev) : ser(ser_dev) {
    ser_dev->_flag &= ~_IOCOOKED;
    setvbuf(ser_dev, NULL, _IONBF, 0);

    num_vars = 0;
    msg_buf_ready = false;
}

void SerialPlot::init(uint32_t f) {
    this->f = f;
    cogstart(SerialPlot::runner, (void*)this, stack, sizeof(stack));
}

void SerialPlot::add(f16_t *v, const char *name) {

    if (num_vars < N_VARS) {
        /*
        1. get number of messages that need to be sent
        2. get id.
        3. schedule the message for the runner to send before the next set of variables.
        */
        while(msg_buf_ready); // wait for the buffer to be emptied (runner will set this false once it's emptied it)
        variables[num_vars] = v;

        uint8_t num_bytes = strlen(name);
        uint8_t num_messages = 1 + strlen(name)/6; // we can send 6 bytes at a time. Not going to try to make them into the 4 byte ints yet

        if (num_messages > 8) {
            return; // name is too long!
        }

        uint32_t id = (uint32_t)v;

        uint8_t bytes_remaining = num_bytes;
        for (int i = 0; i < num_messages; i++) {
            SerialUI::msg_t msg = {
                0,              // no confirmation
                8,              // we'll always send 8 values, padded with zeros if we are out of real bytes
                0,              // will be calculated
                0,              // will be calculated
                CMD_ADD,        // this is the add command
                i, id           // message number and id
            };

            for (int j = 0; j < 6; j++) {
                if (bytes_remaining) {
                    msg.vals[2+j] = name[6*i + j];
                } else {
                    msg.vals[2+j] = 0;
                }
                bytes_remaining--;
            }

            msg_buf[i] = msg;
        }

        buf_size = num_messages;
        msg_buf_ready = true;
        while(msg_buf_ready);
        num_vars++;

    }
}

void SerialPlot::clear() {
    SerialUI::msg_t msg = {
        0,              // no confirmation
        0,              // no values
        0,              // will be calculated
        0,              // will be calculated
        CMD_CLEAR
    };

    while(msg_buf_ready);
    msg_buf[0] = msg;
    buf_size = 1;
    msg_buf_ready = true;
}

void SerialPlot::reset() {
    SerialUI::msg_t msg = {
        0,              // no confirmation
        0,              // no values
        0,              // will be calculated
        0,              // will be calculated
        CMD_RESET
    };

    while(msg_buf_ready);
    msg_buf[0] = msg;
    buf_size = 1;
    msg_buf_ready = true;
}

void SerialPlot::runner(void *par) {

    SerialPlot *plt = (SerialPlot*)par;
    uint32_t period = CLKFREQ/plt->f;

    volatile uint32_t t = CNT;
    while(1) {
        t = CNT;
        if (plt->msg_buf_ready) {
            // write the buffered messages
            for (int i = 0; i < plt->buf_size; i++) {
                plt->ser.writeMessage(&plt->msg_buf[i]);
            }

            // reset the values so buffer can be used again
            plt->buf_size = 0;
            plt->msg_buf_ready = false;
        }

        // write the messages for existing variables
        for (int i = 0; i < plt->num_vars; i++) {
            f16_t *v = plt->variables[i];
            uint32_t id = (uint32_t)v;
            SerialUI::msg_t msg = {
                0,              // no confirmation
                2,              // we'll always send 2 values, the variable id and value
                0,              // will be calculated
                0,              // will be calculated
                CMD_WRITE,        // this is the add command
                id, v->x
            };
            plt->ser.writeMessage(&msg);
        }

        waitcnt(period + t);
    }
}
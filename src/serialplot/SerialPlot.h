#ifndef _SERIALPLOT_H
#define _SERIALPLOT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef LIB_COMPILE
    #include "serialui/serialui.h"
    #include "fmath/fmath.h"
#else
    #include "proplibs/serialui.h"
    #include "proplibs/fmath.h"
#endif


/**
 * This class utilizes the SerialUI class to create a plotting interface to a matching python program running mat plot lib.
 * Pointers to the desired values are added to a list, read and plotted at a set frequency. Requires a cog.
 *
 * Implementation details:
 * Each variable is added with a named string and an ID. ID will be the pointer of the variable. Since we can only do 8 ints at a time,
 * the name string gets broken up and sent in multiple messages. the python side remembers these names and uses them in a plot legend.
 *
 * There are four commands:
 *
 * WRITE
 * Values: [int: id | f16_t: v]
 * Write a value to the plotter: ID is a unique value for each variable.
 * V is the value to plot.
 *
 * CLEAR
 * Values: none
 * clear the plot, but leave the variable names/pointers intact.
 *
 * RESET
 * Values: none
 * clear the plot and delete all variables/pointers
 *
 * ADD
 * Values: [int: count | int: id | 6 bytes: chars ]
 * add a variable to the logger.
 * count: the message count in this request, since multiple messages need to be sent for the name
 * id: id of the variable. will probably just use the pointer of the variable
 * chars: string for the variable name. Will expect the string to be \0 terminated, that's how the plotter will when not to expect another message.
 *
 */

#define SERIALPLOT_STACK_SIZE   100
#define N_VARS                  32

class SerialPlot {

private:

    enum {
        CMD_NOP = 0,
        CMD_CLEAR,
        CMD_RESET,
        CMD_WRITE,
        CMD_ADD
    };

    unsigned stack[SERIALPLOT_STACK_SIZE + 1];

    SerialUI ser;
    f16_t *variables[N_VARS];

    volatile bool msg_buf_ready;
    volatile uint8_t buf_size;

    volatile uint8_t num_vars;

    volatile uint32_t f;

    static void runner(void *par);

public:

    SerialUI::msg_t msg_buf[8]; // this means max length for a name is 48 bytes.

    SerialPlot();

    /*
     *
     */
    SerialPlot(FILE *ser_dev);

    /*
     * start the runner cog to begin printing out variables.
     */
    void init(uint32_t f);

    /*
     * add a value to plot.
     */
    void add(f16_t *v, const char *name);

    /*
     * clear the plot, but keep variable names saved
     */
    void clear();

    /*
     * clear the plot and reset the variables
     */
    void reset();

};

#endif
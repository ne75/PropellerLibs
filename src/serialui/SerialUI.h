#ifndef _SERIALUI_H
#define _SERIALUI_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

/**
 * This class provides an interface to read/write messages between a master and a slave using UART
 * A sublcass should define a usable object that can then define message types, etc. this class
 * only handles reading and parsing the raw message format. [it isn't necessary to subclass, but makes it more readable to do so]
 * it is up to the user to coordinate responses.
 *
 * === message structure (not ASCII) ===
 *
 * $<byte: length><byte crc><byte: op code><int: value 1>...<int: value n>
 *
 * length: number of bytes after $ and this byte to the end of the message. the sender may want to introduce a small pause after this byte to allow
 *  the reciever to set up
 * op code: defined in this header, it's either the id of a value (or set of values) to read/write
 *  or it will perform an operation with given parameters. code itself is 7 bits,
 *  top bit is read (0) or write (1) if the op code specifies a register
 * values are 4 byte integer numbers, their meaning is defined by the op code.
 *  if float values are needed, use fix16 format (float*65536)
 * crc is crc8 of op code onward
 *
 * currently, message buffer is 36 bytes, so message can contain up to 8 values and 4 bytes for header/footer
 *
 * === confirmations ===
 *
 * To allow for a reliable use case, a confirmation message can be sent back to the upon reading. The confirmation message is simply
 * $^ to indicate that the message was received succesfully or $~ if there was an error (such as CRC non lining up). It is up to the
 * user to decide how to handle errors in transmission, this class will just report them.
 *
 * === usage ===
 *
 * A system using this class/library should define a common header that all devices import and use.
 * It defines every message op code and associated data (expected n_vals, etc). It's up to each system to build the messages, this library will
 * only handle actual data transmission.
 *
 */

#define MSG_MAX_VALS        10
#define MSG_HEADER_SIZE     4
#define MSG_BUFFER_SIZE     ((4*MSG_MAX_VALS) + MSG_HEADER_SIZE)

class SerialUI {

    FILE *ser_r;
    FILE *ser_w;

    bool setup_valid;

    /*
     * computes byte sized CRC code for the given string.
     * taken from: https://www.pololu.com/docs/0J44/6.7.6
     */
    uint8_t calcCRC8(uint8_t *s, uint8_t len);

public:

    enum {
        SUI_ERR_GENERIC = 0,            // generic error
        SUI_ERR_NO_MSG,                 // no message found when reading
        SUI_ERR_BAD_DATA,               // CRC in message doesn't match data
        SUI_ERR_BAD_SETUP,
        SUI_OK                          // success.
    };

    struct msg_t {
        // message metadata. this order is 1) ideal for memory alignment and 2) puts things in order for how it's sent out.
        uint32_t confirm;   // if confirm_time is non-zero, this function will wait that amount time to see a
                            // message confirmation come back before returning false. units are microseconds.
                            // Otherwise, it will send the message and return true immediately.
                            // when reading, if confirm is non-zero, then a confirmation message will be written
                            // This will enventually include checking the CRC. Currently not implemented
        uint8_t n_vals;     //the number of values in the message buffer to send.

        // message data. In theory, a memcpy from the recieve buffer to &op should copy all the message contents.
        uint8_t len;
        uint8_t crc;
        uint8_t op;
        uint32_t vals[MSG_MAX_VALS];

    };

    uint8_t msg_buf[MSG_BUFFER_SIZE];

    /*
     * default constructor
     */
    SerialUI();

    /*
     * ser_r is the device to read from and ser_w is the device to write to. Can be the same descripter if opened correctly.
     * Can be UART or another driver (this class is designed for UART,
     * but could be used with any communication protocal that has a driver written)
     *
     */
    SerialUI(FILE *ser_r, FILE *ser_w);

    /*
     * read a message from the serial device into msg.
     * returns true if a message was succesfully read, false if not (no data available or there was an error in the transmission)
     * if confirm is true, then a confirmation message will be written.
     *
     * If reading from ser is blocking, then this will block.
     *
     */
    uint8_t readMessage(msg_t *msg);

    /*
     * write a message, msg, to the serial device. returns true if all good, false if not.
     *
     * if writing to ser is blocking, then this will block
     *
     * in msg, op and vals must be filled out, length and crc will be overwritten by this function.
     *
     */
    uint8_t writeMessage(msg_t *msg);

    /*
     * prints the message out to stdout, useful for debugging
     */
    static void print(msg_t *msg);

};

#endif
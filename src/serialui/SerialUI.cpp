#include "SerialUI.h"

#include <string.h>

#define CHAR_START      '$'
#define CHAR_ACK        '^'
#define CHAR_NACK       '~'

#define CRC7_POLY       0x91

SerialUI::SerialUI() {
    setup_valid = false;
}

SerialUI::SerialUI(FILE *ser) {
    this->ser = ser;
    if (this->ser == NULL) return;

    setup_valid = true; // currently, no way to check for a bad setup (other than ser is null), but future proofing the code for now.
}

uint8_t SerialUI::readMessage(msg_t *msg) {
    if (setup_valid = false) return SUI_ERR_BAD_SETUP;

    uint8_t c, crc = 0;

    c = fgetc(ser);

    if (c != CHAR_START) {
        return SUI_ERR_NO_MSG;
    }

    msg->len = fgetc(ser);
    fread(&(msg->crc), sizeof(uint8_t), msg->len, ser);

    crc = calcCRC8(&(msg->op), msg->len-1);
    msg->n_vals = (msg->len-2)/4;

    if (crc != msg->crc)
        return SUI_ERR_BAD_DATA; // crc didn't match, there was a recieve error.

    return SUI_OK;
}

uint8_t SerialUI::writeMessage(msg_t *msg) {
    if (setup_valid = false) return SUI_ERR_BAD_SETUP;


    if (msg->n_vals > MSG_MAX_VALS) msg->n_vals = MSG_MAX_VALS; // limit to MSG_MAX_VALS values.
    msg->len = MSG_HEADER_SIZE-2 + (4*msg->n_vals); // compute length. First 2 bytes don't count towards length, the rest of the header does.
    msg->crc = 0;

    msg_buf[0] = '$';
    msg_buf[1] = msg->len;
    memcpy(&(msg_buf[3]), &(msg->op), msg->len-1); // copy all the data into the buffer

    msg->crc = calcCRC8(&(msg_buf[3]), msg->len-1); // compute CRC, not counting the CRC byte
    msg_buf[2] = msg->crc; // set the CRC value in the buffer.

    fwrite(msg_buf, sizeof(uint8_t), msg->len + 2, ser);

    return SUI_OK;
}

uint8_t SerialUI::calcCRC8(uint8_t *s, uint8_t len) {
    unsigned char i, j, crc = 0;

    for (i = 0; i < len; i++) {
        crc ^= s[i];
        for (j = 0; j < 8; j++) {
            if (crc & 1)
                crc ^= CRC7_POLY;
            crc >>= 1;
        }
    }

    return crc;
}

void SerialUI::print(msg_t *msg) {
    printf("message:\n length = %d\n op = %d\n crc = %d\n n_vals = %d\n values = %d, %d, %d, %d, %d, %d, %d, %d\n",
                msg->len,
                msg->op,
                msg->crc,
                msg->n_vals,
                msg->vals[0], msg->vals[1], msg->vals[2], msg->vals[3], msg->vals[4], msg->vals[5], msg->vals[6], msg->vals[7]);
}
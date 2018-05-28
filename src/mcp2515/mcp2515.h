//
// automatically generated by spin2cpp v1.05 on Sun Dec 18 23:15:39 2016
// \\vmware-host\Shared Folders\Documents\spin2cpp\spin2cpp.exe jm_mcp2515.spin
//

/**
 * MCP2515 CAN Interface Driver
 *
 * Ported from OBEX object using spin2cpp. This means this code isn't the most optimized and is a bit messy,
 * but it works well.
 *
 */

#ifndef jm_mcp2515_Class_Defined__
#define jm_mcp2515_Class_Defined__

#include <stdint.h>

class mcp2515 {
public:

    /**
     * CAN frame struct
     *
     * - id: id of the message
     * - buf: 8-byte buffer storing the payload
     * - len: length of the payload
     */
    struct can_frame {
        uint16_t id;
        uint8_t buf[8];
        uint8_t len;
    };

    /**
     * Create new MCP2515 driver using given SPI pins
     */
    mcp2515(uint8_t p_cs, uint8_t p_sck, uint8_t p_mosi, uint8_t p_miso);

    /**
     * Initialize the CAN interface with the given baud rate and mode
     *
     * - baud: baud rate in KHz. Supported baudrates are:
     *      10, 20, 50, 100, 125, 250, 500, 800, 1000
     * - mode:
     *      mode_config: configuration mode (not typically used)
     *      mode_listen: listen-only mode. No data will be transmitted
     *      mode_loopback: loop-back moode. All sent data will automatically go into the RX buffer
     *                      instead of being transmitted on the bus
     *      mode_sleep: sleep mode. Data will not be sent or read
     *      mode_normal: normal operation (typically used)
     */
    void init(uint16_t baud, uint8_t mode);

    /**
     * Set up the RX0 buffer filters. An incoming message ID is first masked by mask, then ANDed wih
     * the filters. If this generates a valid message ID, the message is put in the buffer and can
     * be read. Messages go to this buffer first, and then to the RX1 buffer if this one is full.
     * 2 filters are allowed on RX0.
     *
     * Example 1: set_rx0_filters(0x7ff, 0x100, 0x200)
     * - This will only buffer messages with the ID 0x100 or 0x200
     *
     * Example 2: set_rx0_filters(0x00f, 0x002, 0x00a)
     * - This will only buffer messages with IDs that end with a 0x2 or a 0xa.
     *
     * TODO: verify these examples
     */
    void set_rx0_filters(int16_t mask, int16_t f0, int16_t f1);

    /**
     * Set up the RX1 buffer filters. Same idea as RX0 buffer, but 4 filters are allowed.
     */
    void set_rx1_filters(int16_t mask, int16_t f2, int16_t f3, int16_t f4, int16_t f5);

    /**
     * Transmit a frame on the bus
     */
    bool tx(can_frame f);

    /**
     * Read a frame from the buffer. If nothing is available, an empty frame is returned
     *
     * - b: buffer number
     *      -1: first non-empty buffer
     *      0: RX0
     *      1: RX1
     */
    can_frame rx(int8_t b);

    uint32_t lock;
    uint32_t mode;

    static const int Cmd_reset = 192;
    static const int Cmd_read = 3;
    static const int Cmd_rd_rxbuf = 144;
    static const int Cmd_write = 2;
    static const int Cmd_ld_txbuf = 64;
    static const int Cmd_rts = 128;
    static const int Cmd_rts_txb0 = 129;
    static const int Cmd_rts_txb1 = 130;
    static const int Cmd_rts_txb2 = 132;
    static const int Cmd_rd_status = 160;
    static const int Cmd_rx_status = 176;
    static const int Cmd_bit_mod = 5;
    static const int mode_config = 128;
    static const int mode_listen = 96;
    static const int mode_loopback = 64;
    static const int mode_sleep = 32;
    static const int mode_normal = 0;
    static const int mode_mask = 224;
    static const int Frame_none = 0;
    static const int Frame_data = 1;
    static const int Frame_remote = 2;
    static const int Remote_req = 64;
    static const int Pri_low = 0;
    static const int Pri_medlow = 1;
    static const int Pri_medhi = 2;
    static const int Pri_high = 3;
    static const int Pullup = 0;
    static const int Driven = 1;
    static const int Bfpctrl = 12;
    static const int Txrtsctrl = 13;
    static const int Canstat = 14;
    static const int Canctrl = 15;
    static const int Tec = 28;
    static const int Rec = 29;
    static const int Cnf3 = 40;
    static const int Cnf2 = 41;
    static const int Cnf1 = 42;
    static const int Caninte = 43;
    static const int Canintf = 44;
    static const int Eflg = 45;
    static const int Txb0ctrl = 48;
    static const int Txb1ctrl = 64;
    static const int Txb2ctrl = 80;
    static const int Rxb0ctrl = 96;
    static const int Rxb1ctrl = 112;
    static const int Rxf0sidh = 0;
    static const int Rxf0sidl = 1;
    static const int Rxf0eid8 = 2;
    static const int Rxf0eid0 = 3;
    static const int Rxf1sidh = 4;
    static const int Rxf1sidl = 5;
    static const int Rxf1eid8 = 6;
    static const int Rxf1eid0 = 7;
    static const int Rxf2sidh = 8;
    static const int Rxf2sidl = 9;
    static const int Rxf2eid8 = 10;
    static const int Rxf2eid0 = 11;
    static const int Rxf3sidh = 16;
    static const int Rxf3sidl = 17;
    static const int Rxf3eid8 = 18;
    static const int Rxf3eid0 = 19;
    static const int Rxf4sidh = 20;
    static const int Rxf4sidl = 21;
    static const int Rxf4eid8 = 22;
    static const int Rxf4eid0 = 23;
    static const int Rxf5sidh = 24;
    static const int Rxf5sidl = 25;
    static const int Rxf5eid8 = 26;
    static const int Rxf5eid0 = 27;
    static const int Canstat1 = 30;
    static const int Canctrl1 = 31;
    static const int Rxm0sidh = 32;
    static const int Rxm0sidl = 33;
    static const int Rxm0eid8 = 34;
    static const int Rxm0eid0 = 35;
    static const int Rxm1sidh = 36;
    static const int Rxm1sidl = 37;
    static const int Rxm1eid8 = 38;
    static const int Rxm1eid0 = 39;
    static const int Canstat2 = 46;
    static const int Canctrl2 = 47;
    static const int Txb0sidh = 49;
    static const int Txb0sidl = 50;
    static const int Txb0eid8 = 51;
    static const int Txb0eid0 = 52;
    static const int Txb0dlc = 53;
    static const int Txb0d0 = 54;
    static const int Txb0d1 = 55;
    static const int Txb0d2 = 56;
    static const int Txb0d3 = 57;
    static const int Txb0d4 = 58;
    static const int Txb0d5 = 59;
    static const int Txb0d6 = 60;
    static const int Txb0d7 = 61;
    static const int Canstat3 = 62;
    static const int Canctrl3 = 63;
    static const int Txb1sidh = 65;
    static const int Txb1sidl = 66;
    static const int Txb1eid8 = 67;
    static const int Txb1eid0 = 68;
    static const int Txb1dlc = 69;
    static const int Txb1d0 = 70;
    static const int Txb1d1 = 71;
    static const int Txb1d2 = 72;
    static const int Txb1d3 = 73;
    static const int Txb1d4 = 74;
    static const int Txb1d5 = 75;
    static const int Txb1d6 = 76;
    static const int Txb1d7 = 77;
    static const int Canstat4 = 78;
    static const int Canctrl4 = 79;
    static const int Txb2sidh = 81;
    static const int Txb2sidl = 82;
    static const int Txb2eid8 = 83;
    static const int Txb2eid0 = 84;
    static const int Txb2dlc = 85;
    static const int Txb2d0 = 86;
    static const int Txb2d1 = 87;
    static const int Txb2d2 = 88;
    static const int Txb2d3 = 89;
    static const int Txb2d4 = 90;
    static const int Txb2d5 = 91;
    static const int Txb2d6 = 92;
    static const int Txb2d7 = 93;
    static const int Canstat5 = 94;
    static const int Canctrl5 = 95;
    static const int Rxb0sidh = 97;
    static const int Rxb0sidl = 98;
    static const int Rxb0eid8 = 99;
    static const int Rxb0eid0 = 100;
    static const int Rxb0dlc = 101;
    static const int Rxb0d0 = 102;
    static const int Rxb0d1 = 103;
    static const int Rxb0d2 = 104;
    static const int Rxb0d3 = 105;
    static const int Rxb0d4 = 106;
    static const int Rxb0d5 = 107;
    static const int Rxb0d6 = 108;
    static const int Rxb0d7 = 109;
    static const int Canstat6 = 110;
    static const int Canctrl6 = 111;
    static const int Rxb1sidh = 113;
    static const int Rxb1sidl = 114;
    static const int Rxb1eid8 = 115;
    static const int Rxb1eid0 = 116;
    static const int Rxb1dlc = 117;
    static const int Rxb1d0 = 118;
    static const int Rxb1d1 = 119;
    static const int Rxb1d2 = 120;
    static const int Rxb1d3 = 121;
    static const int Rxb1d4 = 122;
    static const int Rxb1d5 = 123;
    static const int Rxb1d6 = 124;
    static const int Rxb1d7 = 125;
    static const int Canstat7 = 126;
    static const int Canctrl7 = 127;

private:

    uint8_t cs, mosi, miso, sck;

    int32_t	Start(int32_t Cspin, int32_t Sckpin, int32_t Mosipin, int32_t Misopin, int32_t Lock);
    int32_t	Stop(void);
    int32_t	Set_mode(int32_t Mode);
    int32_t	Get_mode(void);
    int32_t	Set_baud(int32_t Kbps);
    int32_t	Tx_buf_ready(int32_t Buf);
    int32_t	Tx_stdframe(int32_t Buf, int32_t Stdid, int32_t Dlc, int32_t Pntr, int32_t Priority);
    int32_t	Set_rxb0_filters(int32_t Mask0, int32_t Filter0, int32_t Filter1);
    int32_t	Set_rxb1_filters(int32_t Mask1, int32_t Filter2, int32_t Filter3, int32_t Filter4, int32_t Filter5);
    int32_t	Rx_stdframe(int32_t Buf, int32_t Msgpntr, int32_t Dlcpntr, int32_t Datapntr);
    int32_t	Get_rx_stdid(int32_t Buf);
    int32_t	Get_rx_dlc(int32_t Buf);
    int32_t	Clear_rx_intf(int32_t Buf);
    int32_t	Reset(void);
    int32_t	Read(int32_t Addr);
    int32_t	Read_block(int32_t Addr, int32_t Pntr, int32_t N);
    int32_t	Read_rxbuf(int32_t Bufsel);
    int32_t	Write(int32_t Addr, int32_t Value);
    int32_t	Write_block(int32_t Addr, int32_t Pntr, int32_t N);
    int32_t	Load_txbuf(int32_t Datain, int32_t Bufsel);
    int32_t	Rts(int32_t Buf);
    int32_t	Rd_status(void);
    int32_t	Rx_status(void);
    int32_t	Bit_modify(int32_t Addr, int32_t Mask, int32_t Datain);
    int32_t	Cs;
    int32_t	Sck;
    int32_t	Mosi;
    int32_t	Miso;
    int32_t	Lockid;
    int32_t	Inuse;
    int32_t	Csmode;
    int32_t	Lock_buss(int32_t State);
    int32_t	Chip_select(int32_t State);
    int32_t	Spi_readwrite(int32_t Datain);
};

#endif

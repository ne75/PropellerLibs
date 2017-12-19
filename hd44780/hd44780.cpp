/* 

This is a port of the LCD driver by Simon Ampleman, foudnd on the Propeller OBEX

┌──────────────────────┐
│ Parallel LCD Driver  │
├──────────────────────┴───────────────────────┐
│  Width      : 16 Characters                  │
│  Height     :  2 Lines                       │
│  Interface  :  8 Bit                         │
│  Controller :  HD44780-based                 │
├──────────────────────────────────────────────┤
│  By      : Simon Ampleman                    │
│            sa@infodev.ca                     │
│  Modified: Nikita Ermoshkin                  │
│            ne75@cornell.edu                  │
│  Date    : 2006-11-18                        │
│  Version : 1.0                               │
└──────────────────────────────────────────────┘

Hardware used : SSC2F16DLNW-E                    

Schematics
                         P8X32A
                       ┌────┬────┐ 
                       ┤0      31├              0V    5V    0V   P16   P17   P18   P15   P14
                       ┤1      30├              │     │     │     │     │     │     │     │
                       ┤2      29├              │VSS  │VDD  │VO   │R/S  │R/W  │E    │DB0  │DB1                              
                       ┤3      28├              ┴1    ┴2    ┴3    ┴4    ┴5    ┴6    ┴7    ┴8
                       ┤4      27├            ┌────────────────────────────────────────────────┐
                       ┤5      26├            │ 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16│  LCD 16X2
                       ┤6      25├            │ 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16│  HD44780-BASED
                       ┤7      24├            └────────────────────────────────────────────────┘  SSC2F16DLNW-E
                       ┤VSS   VDD├              ┬9    ┬10   ┬11   ┬12   ┬13   ┬14   ┬15   ┬16
                       ┤BOEn   XO├              │DB2  │DB3  │DB4  │DB5  │DB6  │DB7  │A(+) │K(-)             
                       ┤RESn   XI├              │     │     │     │     │     │     │     │
                       ┤VDD   VSS├             P13   P12   P11   P10   P9    P8     5V    0V
                   DB0 ┤8      23├ 
                   DB1 ┤9      22├ 
                   DB2 ┤10     21├ 
                   DB3 ┤11     20├
                   DB4 ┤12     19├ 
                   DB5 ┤13     18├ E
                   DB6 ┤14     17├ RW
                   DB7 ┤15     16├ R/S
                       └─────────┘ 


PIN ASSIGNMENT
   VSS  - POWER SUPPLY (GND)
   VCC  - POWER SUPPLY (+5V)
   VO   - CONTRAST ADJUST (0-5V)
   R/S  - FLAG TO RECEIVE INSTRUCTION OR DATA
            0 - INSTRUCTION
            1 - DATA
   R/W  - INPUT OR OUTPUT MODE
            0 - WRITE TO LCD MODULE
            1 - READ FROM LCD MODULE
   E    - ENABLE SIGNAL 
   DB0  - DATA BUS LINE 0 (LSB)
   DB1  - DATA BUS LINE 1 
   DB2  - DATA BUS LINE 2 
   DB3  - DATA BUS LINE 3 
   DB4  - DATA BUS LINE 4 
   DB5  - DATA BUS LINE 5 
   DB6  - DATA BUS LINE 6 
   DB7  - DATA BUS LINE 7 (MSB)
   A(+) - BACKLIGHT 5V
   K(-) - BACKLIGHT GND

INSTRUCTION SET
   ┌──────────────────────┬───┬───┬─────┬───┬───┬───┬───┬───┬───┬───┬───┬─────┬─────────────────────────────────────────────────────────────────────┐
   │  INSTRUCTION         │R/S│R/W│     │DB7│DB6│DB5│DB4│DB3│DB2│DB1│DB0│     │ Description                                                         │
   ├──────────────────────┼───┼───┼─────┼───┼───┼───┼───┼───┼───┼───┼───┼─────┼─────────────────────────────────────────────────────────────────────┤
   │ CLEAR DISPLAY        │ 0 │ 0 │     │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │     │ Clears display and returns cursor to the home position (address 0). │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ CURSOR HOME          │ 0 │ 0 │     │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ * │     │ Returns cursor to home position (address 0). Also returns display   │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │ being shifted to the original position.                             │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ ENTRY MODE SET       │ 0 │ 0 │     │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │I/D│ S │     │ Sets cursor move direction (I/D), specifies to shift the display(S) │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │ These operations are performed during data read/write.              │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ DISPLAY ON/OFF       │ 0 │ 0 │     │ 0 │ 0 │ 0 │ 0 │ 1 │ D │ C │ B │     │ Sets On/Off of all display (D), cursor On/Off (C) and blink of      │
   │ CONTROL              │   │   │     │   │   │   │   │   │   │   │   │     │ cursor position character (B).                                      │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ CURSOR/DISPLAY       │ 0 │ 0 │     │ 0 │ 0 │ 0 │ 1 │S/C│R/L│ * │ * │     │ Sets cursor-move or display-shift (S/C), shift direction (R/L).     │
   │ SHIFT                │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ FUNCTION SET         │ 0 │ 0 │     │ 0 │ 0 │ 1 │ DL│ N │ F │ * │ * │     │ Sets interface data length (DL), number of display line (N) and     │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │ character font(F).                                                  │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ SET CGRAM ADDRESS    │ 0 │ 0 │     │ 0 │ 1 │      CGRAM ADDRESS    │     │ Sets the CGRAM address. CGRAM data is sent and received after       │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │ this setting.                                                       │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ SET DDRAM ADDRESS    │ 0 │ 0 │     │ 1 │       DDRAM ADDRESS       │     │ Sets the DDRAM address. DDRAM data is sent and received after       │                                                             
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │ this setting.                                                       │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ READ BUSY FLAG AND   │ 0 │ 1 │     │ BF│    CGRAM/DDRAM ADDRESS    │     │ Reads Busy-flag (BF) indicating internal operation is being         │
   │ ADDRESS COUNTER      │   │   │     │   │   │   │   │   │   │   │   │     │ performed and reads CGRAM or DDRAM address counter contents.        │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ WRITE TO CGRAM OR    │ 1 │ 0 │     │         WRITE DATA            │     │ Writes data to CGRAM or DDRAM.                                      │
   │ DDRAM                │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │ READ FROM CGRAM OR   │ 1 │ 1 │     │          READ DATA            │     │ Reads data from CGRAM or DDRAM.                                     │
   │ DDRAM                │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   │                      │   │   │     │   │   │   │   │   │   │   │   │     │                                                                     │
   └──────────────────────┴───┴───┴─────┴───┴───┴───┴───┴───┴───┴───┴───┴─────┴─────────────────────────────────────────────────────────────────────┘
   Remarks :
            * = 0 OR 1
        DDRAM = Display Data Ram
                Corresponds to cursor position                  
        CGRAM = Character Generator Ram        

   ┌──────────┬──────────────────────────────────────────────────────────────────────┐
   │ BIT NAME │                          SETTING STATUS                              │                                                              
   ├──────────┼─────────────────────────────────┬────────────────────────────────────┤
   │  I/D     │ 0 = Decrement cursor position   │ 1 = Increment cursor position      │
   │  S       │ 0 = No display shift            │ 1 = Display shift                  │
   │  D       │ 0 = Display off                 │ 1 = Display on                     │
   │  C       │ 0 = Cursor off                  │ 1 = Cursor on                      │
   │  B       │ 0 = Cursor blink off            │ 1 = Cursor blink on                │
   │  S/C     │ 0 = Move cursor                 │ 1 = Shift display                  │
   │  R/L     │ 0 = Shift left                  │ 1 = Shift right                    │
   │  DL      │ 0 = 4-bit interface             │ 1 = 8-bit interface                │
   │  N       │ 0 = 1/8 or 1/11 Duty (1 line)   │ 1 = 1/16 Duty (2 lines)            │
   │  F       │ 0 = 5x7 dots                    │ 1 = 5x10 dots                      │
   │  BF      │ 0 = Can accept instruction      │ 1 = Internal operation in progress │
   └──────────┴─────────────────────────────────┴────────────────────────────────────┘

   DDRAM ADDRESS USAGE FOR A 1-LINE DISPLAY
   
    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39   <- CHARACTER POSITION
   ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
   │00│01│02│03│04│05│06│07│08│09│0A│0B│0C│0D│0E│0F│10│11│12│13│14│15│16│17│18│19│1A│1B│1C│1D│1E│1F│20│21│22│23│24│25│26│27│  <- ROW0 DDRAM ADDRESS
   └──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘

   DDRAM ADDRESS USAGE FOR A 2-LINE DISPLAY

    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39   <- CHARACTER POSITION
   ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
   │00│01│02│03│04│05│06│07│08│09│0A│0B│0C│0D│0E│0F│10│11│12│13│14│15│16│17│18│19│1A│1B│1C│1D│1E│1F│20│21│22│23│24│25│26│27│  <- ROW0 DDRAM ADDRESS
   │40│41│42│43│44│45│46│47│48│49│4A│4B│4C│4D│4E│4F│50│51│52│53│54│55│56│57│58│59│5A│5B│5C│5D│5E│5F│60│61│62│63│64│65│66│67│  <- ROW1 DDRAM ADDRESS
   └──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘

   DDRAM ADDRESS USAGE FOR A 4-LINE DISPLAY

    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19   <- CHARACTER POSITION
   ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
   │00│01│02│03│04│05│06│07│08│09│0A│0B│0C│0D│0E│0F│10│11│12│13│  <- ROW0 DDRAM ADDRESS
   │40│41│42│43│44│45│46│47│48│49│4A│4B│4C│4D│4E│4F│50│51│52│53│  <- ROW1 DDRAM ADDRESS
   │14│15│16│17│18│19│1A│1B│1C│1D│1E│1F│20│21│22│23│24│25│26│27│  <- ROW2 DDRAM ADDRESS
   │54│55│56│57│58│59│5A│5B│5C│5D│5E│5F│60│61│62│63│64│65│66│67│  <- ROW3 DDRAM ADDRESS
   └──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘
  
 */

#include "hd44780.h"

#include <propeller.h>

#include <string.h>

static inline int32_t rotl(uint32_t a, uint32_t b) { return (a<<b) | (a>>(32-b)); }
static inline int32_t rotr(uint32_t a, uint32_t b) { return (a>>b) | (a<<(32-b)); }

HD44780::HD44780(uint8_t p_rs, uint8_t p_rw, uint8_t p_e, uint8_t p_db0) {
    this->p_rs = p_rs;
    this->p_rw = p_rw;
    this->p_e = p_e;
    this->p_db0 = p_db0;
    this->p_db7 = p_db0+7;
}

void HD44780::init(void) {
    DIRA |= (0xff << p_db0);
    DIRA |= (1 << p_rs);
    DIRA |= (1 << p_rw);
    DIRA |= (1 << p_e);

    waitcnt(15*CLKFREQ/1000 + CNT);

    // Output low on all pins
    OUTA &= (~(0xff << p_db0));
    OUTA &= (~(1 << p_rs));
    OUTA &= (~(1 << p_rw));
    OUTA &= (~(1 << p_e));
    // Set to DL=8 bits, N=2 lines, F=5x7 fonts
    _inst8(0b00111000);
    clear();
    // Display on, Cursor off, Blink off                                             
    _inst8(0b00001100);
    // Increment Cursor + No-Display Shift                                             
    _inst8(0b00000110);
}

void HD44780::_busy(void) {
    int32_t is_busy;
    DIRA &= (~(0xff << p_db0));
    OUTA |= (1 << p_rw);
    OUTA &= (~(1 << p_rs));

    do {
        OUTA |= (1 << p_e);
        is_busy = (INA >> p_db7) & 0x1;
        OUTA &= (~(1 << p_e));
    } while (is_busy == 1);
    
    DIRA |= (0xff << p_db0);
}

void HD44780::_inst8(uint8_t lcd_data) {
    _busy();
    OUTA &= (~(1 << p_rw));
    OUTA &= (~(1 << p_rs));
    OUTA |= (1 << p_e);

    OUTA = (OUTA & ~(0xff << p_db0)) | (lcd_data) << p_db0;

    OUTA &= (~(1 << p_e));
}

void HD44780::_char(uint8_t lcd_data) {
    _busy();
    OUTA &= (~(1 << p_rw));
    OUTA |= (1 << p_rs);
    OUTA |= (1 << p_e);

    OUTA = (OUTA & ~(0xff << p_db0)) | (((lcd_data) & 0xff) << p_db0);

    OUTA &= (~(1 << p_e));
}

void HD44780::clear(void) {
    // Clear display, Return Cursor Home
    _inst8(1);
}

void HD44780::move(int32_t x, int32_t y) {
    int32_t adr;
    // X : Horizontal Position : 1 to 16
    // Y : Line Number         : 1 or 2
    adr = (y - 1) * 64;
    adr = adr + ((x - 1) + 128);
    _inst8(adr);
}

void HD44780::str(const char *string_ptr) {
    while (strlen(string_ptr)) _char(*string_ptr++);
}

int32_t HD44780::dec(int32_t value) {
    int32_t temp;
    int32_t result = 0;

    if (value < 0) {
        value = -value;
        _char('-');
    }

    temp = 1000000000;
    for(int i = 0; i < 10; i++) {
        if (value >= temp) {
            _char((value / temp) + '0');
            value = value % temp;
            result = -1;
        } else {
            if ((result) || (temp == 1)) {
                _char('0');
            }
        }
        temp = temp / 10;
    }

    return result;
}

void HD44780::hex(int32_t value, int32_t digits) {

    static const char *chars = "0123456789ABCDEF";

    value = value << ((8 - digits) << 2);

    for (int i = 0; i < digits; i++) {
        value = rotl(value, 4); // rotate left
        _char(chars[value & 0xf]);
    }
}

void HD44780::bin(int32_t value, int32_t digits) {

    value = value << (32-digits);

    for (int i = 0; i < digits; i++) {
        value = rotl(value, 1);
        _char((value & 0x1) + '0');
    }
}

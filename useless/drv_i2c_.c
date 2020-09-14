#include "drv_i2c_.h"

#define i2cDelay1 i2cDelay
/*		PA0	SCL
 * 		PA1	SDA
 *
 * */
static void SCL_H(void);
static void SCL_L(void);
static void SDA_H(void);
static void SDA_L(void);
static void SDADirOut(void);
static void SDADirIn(void);
static unsigned char SDAIn(void);

static void i2cPortInit(void);
static void i2cDelay(void);
static void i2cStart(void);
static void i2cStop(void);
static void i2cAck(void);
static void i2cNoAck(void);
static unsigned char i2cWaitAck(void);
static void i2cWriteByte(unsigned char byte);
static unsigned char i2cReadByte(void);

void delay_ms(unsigned int xms) {
    int i, j;
    for (i = 0; i < 2000; i++)
        for (j = 0; j < xms; j++);
}

void i2cInit() {
    i2cPortInit();
}

static void i2cPortInit(void) {
    SDA_L();
    SCL_H();
    SCL_H();
}

static void SCL_H(void) {
    IIC_SCL = 1;
}

static void SCL_L(void) {
    IIC_SCL = 0;
}

static void SDA_H(void) {
    IIC_SDA = 1;
}

static void SDA_L(void) {
    IIC_SDA = 0;
}

static void SDADirOut(void) {
    DIR_SDA = 0;
}

static void SDADirIn(void) {
    DIR_SDA = 1;
}

static unsigned char SDAIn(void) {
    if (READ_SDA) {
        return 1;
    } else {
        return 0;
    }
}

/**********************************************************/

static void i2cDelayMs(unsigned short x) {
    unsigned short i;
    unsigned char j, k, l;

    for (i = 0; i < x; i++)
        for (j = 0; j < 100; j++)
            for (k = 0; k < 25; k++)
                for (l = 0; l < 20; l++)
                    ;
}

/*******************************************************************************
 * Function Name  : i2cDelay
 * Description    : Simulation IIC Timing series delay
 * Input          : None
 * Output         : None
 * Return         : None
 ****************************************************************************** */
static void i2cDelay(void) {

    unsigned char i = 5;
    while (i) {
        i--;
    }
}

static void i2cStart(void) {
    SDADirOut();
    SDA_H();
    i2cDelay();
    SCL_H();
    i2cDelay();
    SDA_L();
    i2cDelay();
}

static void i2cStop(void) {
    SDADirOut();

    SCL_L();
    i2cDelay();
    SDA_L();
    i2cDelay();
    SCL_H();
    i2cDelay();
    SDA_H();
    i2cDelay();
}

static void i2cAck(void) {
    SDADirOut();
    SCL_L();
    i2cDelay();
    SDA_L();
    i2cDelay();
    SCL_H();
    i2cDelay();
    SCL_L();
    i2cDelay();
}

static void i2cNoAck(void) {
    SDADirOut();
    SCL_L();
    i2cDelay();
    SDA_H();
    i2cDelay();
    SCL_H();
    i2cDelay();
    SCL_L();
    i2cDelay();
}

static unsigned char i2cWaitAck(void) {
    unsigned short Out_Time = 255;

    SCL_L();
    i2cDelay();
    SDA_H();
    SDADirIn();
    i2cDelay();
    SCL_H();
    i2cDelay();
    while (SDAIn()) {
        if (--Out_Time < 1) {
            SCL_L();
            return 1;
        }
    }
    SCL_L();
    i2cDelay();
    return 0;
}

static void i2cWriteByte(unsigned char byte) {
    unsigned char i;
    SDADirOut();

    for (i = 0; i < 8; i++) {
        SCL_L();
        i2cDelay();
        if (byte & 0x80)
            SDA_H();
        else
            SDA_L();
        byte <<= 1;
        i2cDelay();
        SCL_H();
        i2cDelay();
    }
    SCL_L();
    i2cDelay();
}

static unsigned char i2cReadByte(void) {
    unsigned char i, receive_byte = 0;
    SDADirIn();

    for (i = 0; i < 8; i++) {
        receive_byte <<= 1;
        SCL_L();
        i2cDelay();
        SCL_H();
        i2cDelay();
        if (SDAIn())
            receive_byte |= 0x01;
    }
    SCL_L();
    i2cDelay();
    return receive_byte;
}

/*****************************************************************************************/

unsigned char i2cWrite(unsigned char dev_addr, unsigned char reg_addr, unsigned char data) {
    i2cStart();

    i2cWriteByte(dev_addr);
    i2cWaitAck();

    i2cWriteByte(reg_addr);
    i2cWaitAck();

    i2cWriteByte(data);
    i2cWaitAck();

    i2cStop();

    return 0;
}

unsigned char i2cRead(unsigned char dev_addr, unsigned char reg_addr, unsigned char *buf_ptr, unsigned char length) {
    unsigned char i;

    i2cStart();
    i2cWriteByte(dev_addr);
    i2cWaitAck();
    i2cWriteByte(reg_addr);
    i2cWaitAck();
    i2cStart();
    i2cWriteByte(dev_addr | 0x01);
    i2cWaitAck();
    for (i = 0; i < (length - 1); i++) {
        *buf_ptr = i2cReadByte();
        i2cAck();
        buf_ptr++;
    }
    *buf_ptr = i2cReadByte();
    i2cNoAck();
    i2cStop();

    return 0;
}
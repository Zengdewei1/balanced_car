#ifndef _DRV_I2C_H_
#define _DRV_I2C_H_

#include <xc.h>
#include <pic16f18854.h>

#define IIC_SCL    LATCbits.LATC3
#define IIC_SDA    LATCbits.LATC4
#define READ_SDA   PORTCbits.RC4
#define DIR_SDA    TRISCbits.TRISC4

extern void i2cInit(void);
extern unsigned char i2cWrite(unsigned char dev_addr, unsigned char reg_addr, unsigned char data);
extern unsigned char i2cRead(unsigned char dev_addr, unsigned char reg_addr, unsigned char *buf_ptr, unsigned char length);

#endif /* IIC_H_ */

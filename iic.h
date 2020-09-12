#ifndef IIC_H
#define	IIC_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define I2C_READ	0x01		/* read bit used with address */
#define I2C_WRITE	0x00		/* write bit used with address */
    
void iic_init(void);
void iic_ack(uint8_t ack);
void iic_write_byte(uint8_t data);
uint8_t iic_read_byte(uint8_t ack);
void iic_start(void);
void iic_stop(void);
void iic_wait(void);
void iic_wait_send(void);

// info extract from
// http://www.hobbytronics.co.uk/hi-tech-c-i2c-master

// i2c_Restart - Re-Start I2C communication
void i2c_Restart(void);

// i2c_Address - Sends Slave Address and Read/Write mode
// mode is either I2C_WRITE or I2C_READ
void i2c_Address(unsigned char address, unsigned char mode);


void LDByteReadI2C(unsigned char i2cSlaveAddress, unsigned char Address, unsigned char *Data, unsigned char Length);



#ifdef	__cplusplus
}
#endif

#endif	/* IIC_H */


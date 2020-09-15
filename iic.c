#include <pic16f18854.h>
#include <stdio.h>
#include "iic.h"

//#define IIC_TIMEOUT 300000      // approximately 1 sec
#define IIC_TIMEOUT 300000      // approximately 1 sec

// The following events will cause the SSP Interrupt Flag 
// bit, SSPxIF, to be set (SSP interrupt, if enabled):
// - Start condition detected
// - Stop condition detectedaddress
// - Data transfer byte transmitted/received
// - Acknowledge transmitted/received
// - Repeated Start generated

int _iic_start(void);
int _iic_stop(void);
int _iic_wait(void);
int _iic_wait_ack(void);
int _iic_wait_buf(void);
int _iic_ack(uint8_t ack);
int _iic_send_and_get_ack(uint8_t data, int *nack);
int _iic_receive_and_send_ack(uint8_t *p_data, uint8_t nack);

void init_iic(void) {
    // I2C programming guide
    // https://electronics.stackexchange.com/questions/417806/pic16f18877-using-i2c-to-read-sensor-lsm9ds0-value
    // set the rc3(scl2) and rc4(sda2) as input for iic master mode
    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    WPUCbits.WPUC3 = 1;
    WPUCbits.WPUC4 = 1;
    // pps
    SSP2CLKPPS = 0x13; // rc3
    SSP2DATPPS = 0x14; // rc4
    RC3PPS = 0x16; // scl2
    RC4PPS = 0x17; // sda2

    // set the ssp working mode as iic master mode
    SSP2CON1bits.SSPM = 0b1000; // i2c baud rate clock = Fosc/(4*(SSP2ADD+1))
    // set baud rate: 100 KHz
    SSP2ADD = 19; // baud rate
    // optionally set the sda hold time to at least 300ns after the falling edge of scl
    SSP2CON3bits.SDAHT = 1; // set SDA hold time to at least 300ns
    // trigger up the ssp1
    SSP2CON1bits.SSPEN = 1;
}

int _iic_ack(uint8_t ack) { // 0 - acknowledged 1 - not acknowledged
    // load the ack data
    SSP2CON2bits.ACKDT = (ack & 0x01);
    // start ack
    SSP2CON2bits.ACKEN = 1;
    return _iic_wait();
}

int _iic_send_and_get_ack(uint8_t data, int *nack) {
    // send address
    SSP2BUF = data;
    if (_iic_wait() == 0) return 0;
    *nack = SSP2CON2bits.ACKSTAT;
    if (*nack == 1) {
        _iic_stop();
        return 0;
    }
    return 1;
}

int _iic_receive_and_send_ack(uint8_t *p_data, uint8_t nack) {
    SSP2CON2bits.RCEN = 1;
    if (_iic_wait_buf() == 0) return 0;
    PIR3bits.SSP2IF = 0;
    *p_data = SSP2BUF;
    // ack
    if (_iic_ack(nack) == 0) return 0;
    return 1;
}

int iic_write_byte(uint8_t addr, uint8_t data, int *n_ack) {
    *n_ack = 1;
    // start condition
    if (_iic_start() != 1) return 2;
    // send address
    if (_iic_send_and_get_ack((addr << 1) | 0, n_ack) != 1) return 3;
    // send data
    if (_iic_send_and_get_ack(data, n_ack) != 1) return 4;
    // stop condition
    if (_iic_stop() != 1) return 5;
    return 1;
}

int mpu_write_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len, int *n_ack) {
    // start
    if (_iic_start() != 1) return 2;
    // send address
    if (_iic_send_and_get_ack((addr << 1) | 0, n_ack) != 1) return 3;
    // send reg address
    if (_iic_send_and_get_ack(reg, n_ack) != 1) return 4;
    // send buffer
    for (int i = 0; i < len; i++) {
        if (_iic_send_and_get_ack(buf[i], n_ack) != 1) return 5 + i;
    }
    // stop
    if (_iic_stop() != 1) return -1;
    return 1;
}

int mpu_write_byte(uint8_t addr, uint8_t reg, uint8_t data, int *n_ack) {
    // start
    if (_iic_start() != 1) return 2;
    // send address
    if (_iic_send_and_get_ack((addr << 1) | 0, n_ack) != 1) return 3;
    // send reg addr
    if (_iic_send_and_get_ack(reg, n_ack) != 1) return 4;
    // send data
    if (_iic_send_and_get_ack(data, n_ack) != 1) return 5;
    // stop
    if (_iic_stop() != 1) return 6;
    return 1;
}

int iic_read_byte(uint8_t addr, uint8_t *p_data) {
    int n_ack = 1;
    // start condition
    if (_iic_start() != 1) return 2;
    // send address and r/w
    if (_iic_send_and_get_ack((addr << 1) | 1, &n_ack) != 1) return 3;
    // receive
    if (_iic_receive_and_send_ack(p_data, 0) != 1) return 4;
    // stop
    if (_iic_stop() != 1) return 5;
    return 1;
}

int mpu_read_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len) {
    int n_ack = 1;
    // start condition
    if (_iic_start() != 1) return 2;
    // send address
    if (_iic_send_and_get_ack((addr << 1) | 0, &n_ack) != 1) return 3;
    // send reg addr
    if (_iic_send_and_get_ack(reg, &n_ack) != 1) return 4;
    // restart
    if (_iic_start() != 1) return 5;
    // send address
    if (_iic_send_and_get_ack((addr << 1) | 1, &n_ack) != 1) return 6;
    // receive the data
    while (len) {
        if (len == 1) {
            _iic_receive_and_send_ack(buf, 1);
        } else {
            _iic_receive_and_send_ack(buf, 0);
        }
        len--;
        buf++;
    }
    // stop condition
    if (_iic_stop() != 1) return 7;
    return 1;
}

int mpu_read_byte(uint8_t addr, uint8_t reg, uint8_t *p_data) {
    int n_ack = 1;
    // start condition
    if (_iic_start() != 1) return 2;
    // send address
    if (_iic_send_and_get_ack((addr << 1) | 0, &n_ack) != 1) return 3;
    // send reg addr
    if (_iic_send_and_get_ack(reg, &n_ack) != 1) return 4;
    // restart
    if (_iic_start() != 1) return 5;
    // send address
    if (_iic_send_and_get_ack((addr << 1) | 1, &n_ack) != 1) return 6;
    // read data
    if (_iic_receive_and_send_ack(p_data, 1) != 1) return 7;
    // stop condition
    if (_iic_stop() != 1) return 7;
    return 1;
}

int _iic_start(void) {
    // start condition enable
    SSP2CON2bits.SEN = 1;
    if (_iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_start timed out!\n");
#endif
        return 0;
    }
    return 1;
}

int _iic_stop(void) {
    // stop condition enable
    SSP2CON2bits.PEN = 1;
    if (_iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_stop timed out!\n");
#endif
        return 0;
    }
    return 1;
}

int _iic_wait(void) {
    uint32_t counter = 0;
    while (PIR3bits.SSP2IF == 0) {
        counter++;
        if (counter >= IIC_TIMEOUT) {
            PIR3bits.SSP2IF = 0;
#ifdef DEBUG
            printf("iic_wait timed out!\n");
#endif
            return 0; // wait failed
        }
    }
    PIR3bits.SSP2IF = 0;
    return 1;
}

int _iic_wait_ack(void) {
    uint32_t counter = 0;
    while (SSP2CON2bits.ACKSTAT == 1) {
        counter++;
        if (counter >= IIC_TIMEOUT) {
#ifdef DEBUG
            printf("iic_wait_ack timed out!\n");
#endif
            return 0;
        }
    }
    return 1;
}

int _iic_wait_buf(void) {
    uint32_t counter = 0;
    while (SSP2STATbits.BF == 1) {
        counter++;
        if (counter >= IIC_TIMEOUT) {
#ifdef DEBUG
            printf("iic_wait_buf timed out!\n");
#endif
            return 0;
        }
    }
    return 1;
}


//
//void iic_slave_handler(void) {
//    PIR3bits.SSP2IF = 0;
//    if (SSP2STATbits.S) {
////        SSP2CON3bits.SCIE = 0;
//        if (iic_slave_wait() == 0) {
//            printf("iic_slave_handler\n");
//            SSP2CON3bits.SCIE = 1;
//            return;
//        }
////        SSP2CON3bits.SCIE = 1;
//    }
//}
//
//int iic_slave_wait(void) {
//    uint32_t counter = 0;
//    while (PIR3bits.SSP2IF == 0) {
//        counter++;
//        if (counter >= IIC_TIMEOUT) {
//            PIR3bits.SSP2IF = 0;
//            printf("iic_slave_wait timed out!\n");
//            return 0; // wait failed
//        }
//    }
//    PIR3bits.SSP2IF = 0;
//    return 1;
//}
//
//int iic_slave_wait_buf(void) {
//    uint32_t counter = 0;
//    while (SSP2STATbits.BF == 1) {
//        counter++;
//        if (counter >= IIC_TIMEOUT) {
//            printf("iic_slave_wait_buf timed out!\n");
//            return 0;
//        }
//    }
//    return 1;
//}
//
//void init_iic_slave(void) {
//    // rc2 (sda2)  rc1 (scl2)
//    TRISCbits.TRISC1 = 1;
//    TRISCbits.TRISC2 = 1;
//    SSP2CLKPPS = 0x11;
//    SSP2DATPPS = 0x12;
//    RC1PPS = 0x16;
//    RC2PPS = 0x17;
//
//    // set the ssp working mode as iic slave mode
//    SSP2CON1bits.SSPM = 0b0110;
//    // interrupts on start and stop
//    SSP2CON3bits.PCIE = 1;
//    SSP2CON3bits.SCIE = 1;
//    // address
//    SSP2ADD = 100 << 1;
//    SSP2MSK = 0xff;
//    // automatically holds the clk after the address and data
//    SSP2CON3bits.AHEN = 1;
//    SSP2CON3bits.DHEN = 1;
//    // start up the peripheral
//    SSP2CON1bits.SSPEN = 1;
//
//}
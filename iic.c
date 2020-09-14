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

void init_iic(void) {
    // I2C programming guide
    // https://electronics.stackexchange.com/questions/417806/pic16f18877-using-i2c-to-read-sensor-lsm9ds0-value
    // set the rc3(scl) and rc4(sda) as input for iic master mode
    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    WPUCbits.WPUC3 = 1;
    WPUCbits.WPUC4 = 1;
    // pps
    SSP1CLKPPS = 0x13; // rc3
    SSP1DATPPS = 0x14; // rc4
    RC3PPS = 0x14; // scl
    RC4PPS = 0x15; // sda

    // set the ssp working mode as iic master mode
    SSP1CON1bits.SSPM = 0b1000; // i2c baud rate clock = Fosc/(4*(SSP1ADD+1))
    // set baud rate: 100 KHz
    SSP1ADD = 19; // baud rate
    // optionally set the sda hold time to at least 300ns after the falling edge of scl
    SSP1CON3bits.SDAHT = 1; // set SDA hold time to at least 300ns
    // disable the slew rate control
    //    SSP1STATbits.SMP = 1;
    // enable input logic so that thresholds are compliant with SMBus
    //    SSP1STATbits.CKE = 1;
    // trigger up the ssp1
    SSP1CON1bits.SSPEN = 1;
}

int iic_ack(uint8_t ack) { // 0 - acknowledged 1 - not acknowledged
    // load the ack data
    SSP1CON2bits.ACKDT = (ack & 0x01);
    // start ack
    SSP1CON2bits.ACKEN = 1;
    return iic_wait();
}

int iic_write_byte(uint8_t addr, uint8_t data, int *n_ack) {
    *n_ack = 1;
    // start condition
    if (iic_start() == 0) {
#ifdef DEBUG
        printf("iic_write_byte start failed!\n");
#endif
        return 0;
    }

    // send address
    SSP1BUF = (addr << 1);
    if (iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_write_byte send address failed!\n");
#endif
        return 0;
    }
    *n_ack = SSP1CON2bits.ACKSTAT;
    if (*n_ack == 1) {  // TODO
#ifdef DEBUG
//        printf("iic_write_byte send address not recognized!\n");
#endif
    }

    // send data
    SSP1BUF = data;
    if (iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_write_byte send data failed!\n");
#endif
        return 0;
    }
    *n_ack = SSP1CON2bits.ACKSTAT;
    if (*n_ack == 1) {  // TODO 
#ifdef DEBUG
//        printf("iic_write_byte send data not recognized!\n");
#endif
    }

    // stop condition
    if (iic_stop() == 0) {
#ifdef DEBUG
        printf("iic_write_byte stop failed!\n");
#endif
        return 0;
    }

    return 1;
}

int iic_read_byte(uint8_t addr, uint8_t *p_data) { // TODO to be altered
    // start condition
    if (iic_start() == 0) {
#ifdef DEBUG
        printf("iic_read_byte start failed!\n");
#endif
        return 0;
    }

    // send address and r/w
    SSP1BUF = (addr << 1) + 1;
    if (iic_wait_ack() == 0 || iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_read_byte send address failed!\n");
#endif
        return 0;
    }

    // receive
    SSP1CON2bits.RCEN = 1;
    if (iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_read_byte receive failed!\n");
#endif
        return 0;
    }
    *p_data = SSP1BUF;
    if (iic_wait_buf() == 0) {
#ifdef DEBUG
        printf("iic_read_byte clear buf failed!\n");
#endif
        return 0;
    }

    // ack
    if (iic_ack(0) == 0) {
#ifdef DEBUG
        printf("iic_read_byte ack timed out!\n");
#endif
        return 0;
    }

    // stop
    if (iic_stop() == 0) {
#ifdef DEBUG
        printf("iic_read_byte stop failed!\n");
#endif
        return 0;
    }

    return 1;
}

int iic_start(void) {
    // start condition enable
    SSP1CON2bits.SEN = 1;
    if (iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_start timed out!\n");
#endif
        return 0;
    }
    return 1;
}

int iic_stop(void) {
    // stop condition enable
    SSP1CON2bits.PEN = 1;
    if (iic_wait() == 0) {
#ifdef DEBUG
        printf("iic_stop timed out!\n");
#endif
        return 0;
    }
    return 1;
}

int iic_wait(void) {
    uint32_t counter = 0;
    while (PIR3bits.SSP1IF == 0) {
        counter++;
        if (counter >= IIC_TIMEOUT) {
            PIR3bits.SSP1IF = 0;
#ifdef DEBUG
            printf("iic_wait timed out!\n");
#endif
            return 0; // wait failed
        }
    }
    PIR3bits.SSP1IF = 0;
    return 1;
}

int iic_wait_ack(void) {
    uint32_t counter = 0;
    while (SSP1CON2bits.ACKSTAT == 1) {
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

int iic_wait_buf(void) {
    uint32_t counter = 0;
    while (SSP1STATbits.BF == 1) {
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
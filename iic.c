#include <pic16f18854.h>
#include "iic.h"

void iic_init(void) { // TODO determine if SSP1 correct
    // I2C programming guide
    // https://electronics.stackexchange.com/questions/417806/pic16f18877-using-i2c-to-read-sensor-lsm9ds0-value
    // set the rc3(scl) and rc4(sda) as input for iic master mode
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    // set the ssp working mode as iic master mode
    SSP1CON1bits.SSPM = 0b1000; // i2c baud rate clock = Fosc/(4*(SSP1ADD+1))
    // TODO set baud rate
    SSP1ADD = 100; // baud rate: (SSP1ADD+1)*4/Fosc
    // enable interrupt on stop bit
    //    SSP1CON3bits.PCIE = 0;
    // optionally set the sda hold time to at least 300ns after the falling edge of scl
    SSP1CON3bits.SDAHT = 1; // set SDA hold time to at least 300ns
    // disable the slew rate control
    SSP1STATbits.SMP = 1;
    // enable input logic so that thresholds are compliant with SMBus
    SSP1STATbits.CKE = 1;
    // trigger up the ssp1
    SSP1CON1bits.SSPEN = 1;
}

void iic_ack(uint8_t ack) {
    // load the ack data
    SSP1CON2bits.ACKDT = (ack & 0x01);
    // start ack
    SSP1CON2bits.ACKEN = 1;
    // wait the data
    while (PIR3bits.SSP1IF == 0);
    // clear the interrupt status
    PIR3bits.SSP1IF = 0;
}

void iic_write_byte(uint8_t data) {
    // send the data
    SSP1BUF = data;
    // wait for completion
    while (PIR3bits.SSP1IF == 0);
    // clear the bit
    PIR3bits.SSP1IF = 0;
}

uint8_t iic_read_byte(uint8_t ack) {
    uint8_t data;
    // enable receive
    SSP1CON2bits.RCEN = 1;
    // wait for completion
    while (PIR3bits.SSP1IF == 0);
    // clear the bit
    PIR3bits.SSP1IF = 0;
    // read the data
    data = SSP1BUF;
    // clear the BUFFER FULL status
    SSP1STATbits.BF = 0;
    // send ack
    iic_ack(ack);
    return data;
}

void iic_start(void) {
    // start condition enable
    SSP1CON2bits.SEN = 1;
    // wait for transmission to complete
    while (PIR3bits.SSP1IF == 0);
    // clear the status
    PIR3bits.SSP1IF = 0;
}

void iic_stop(void) {
    // stop condition enable
    SSP1CON2bits.PEN = 1;
    // wait for the transmission to stop
    while (PIR3bits.SSP1IF == 0);
    // clear the interrupt status
    PIR3bits.SSP1IF = 0;
}
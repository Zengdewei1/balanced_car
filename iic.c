#include <pic16f18854.h>
#include <stdio.h>
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
    iic_wait();
}

void iic_write_byte(uint8_t data) {
    // send the data
    SSP1BUF = data;
    iic_wait();
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

void iic_wait(void) {
    // wait for transmission to complete
    while (PIR3bits.SSP1IF == 0);
    // clear the status
    // PIR3bits.SSP1IF = 0;
}

void iic_wait_send(void) {
    while ((SSP1STAT & 0x08) || (SSP1CON2 & 0X01));
}

void iic_start(void) {
    printf("start\r\n");
    // start condition enable
    SSP1CON2bits.SEN = 1;
    iic_wait_send();
}

void iic_stop(void) {
    // stop condition enable
    SSP1CON2bits.PEN = 1;
    iic_wait();
}

// i2c_Restart - Re-Start I2C communication
void i2c_Restart(void){
    iic_wait();
    SSP1CON2bits.RSEN=1;
}

// i2c_Address - Sends Slave Address and Read/Write mode
// mode is either I2C_WRITE or I2C_READ
void i2c_Address(unsigned char address, unsigned char mode)
{
    printf("sending\r\n");
    unsigned char l_address;

    l_address=address<<1;
    l_address+=mode;
    iic_write_byte(l_address);
}



void LDByteReadI2C(unsigned char i2cSlaveAddress, unsigned char Address, unsigned char *Data, unsigned char Length)
{
    unsigned char read_byte;
    iic_start();                        // send Start
    i2c_Address(i2cSlaveAddress, I2C_WRITE);  // Send slave address - write operation
    iic_write_byte(Address);	                // Set register for servo 0
    i2c_Restart();                      // Restart
    i2c_Address(i2cSlaveAddress, I2C_READ);   // Send slave address - read operation	

    while(Length-- > 1)
    *(Data++) = iic_read_byte(1);            // Read one byte
    *(Data++) = iic_read_byte(0);
    iic_stop();
}
/*
 * File:   main.c
 * Author: zengdewei123
 *
 * Created on 2020?8?25?, ??2:57
 */


#include <xc.h>
#include <pic16f18854.h>
#include "bt.h"

void init_args(void);
void initAccelerometer(void);
void initGyroscope(void);
void init_i2c(void);

void main(void) {
    init_args();
    init_i2c(); 
    unsigned int i = 0;
    while (1){
        i++;
        if (i % 100 == 0){
            BT_broadcast();
        }
    }
    
    return;
}

void init_args(void) {
    // GPIO??
    ANSELA = 0;         
    ANSELB = 0;
    TRISA = 0B00011111;
    TRISB=0B11110000;
    TRISC = 0;
    // ??GPIO??
    LATA = 0B00100000;
    LATB = 0b11111111;
    return;
}

void initAccelerometer(void) {
    return;
}

void initGyroscope(void) {
    return;
}

void init_i2c(void) {  
    // I2C programming guide
    // https://electronics.stackexchange.com/questions/417806/pic16f18877-using-i2c-to-read-sensor-lsm9ds0-value
    // set the rc3(scl) and rc4(sda) as input for iic master mode
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    // set the ssp working mode as iic master mode
    SSP2CON1bits.SSPM = 0b1000;     // i2c baud rate clock = Fosc/(4*(SSP2ADD+1))
    // TODO set baud rate
    SSP2ADD = 100;                  // baud rate: (ADD+1)*4/Fosc
    // enable interrupt on stop bit
//    SSP2CON3bits.PCIE = 0;
    // optionally set the sda hold time to at least 300ns after the falling edge of scl
    SSP2CON3bits.SDAHT = 1;         // set SDA hold time to at least 300ns
    // disable the slew rate control
    SSP2STATbits.SMP = 0b1;
    // enable input logic so that thresholds are compliant with SMBus
    SSP2STATbits.CKE = 0b1;         
    // trigger up the ssp2
    SSP2CON1bits.SSPEN = 0b1;
}

void initBluetoothUART(void) {
    // Set UART I/O pins
    TRISC7 = 1; // RX as input
    TRISC6 = 0; // TX as output

    // Set baud rate of UART
    // At 4MHz system clock
    BRGH = 1; // for high baud_rate
    BRG16 = 0;
    SPBRG = 25; // 9600 Baud rate

    TXSTAbits_t.SYNC = 0;     // Asynchronous mode
    SPEN  = 1;    // Enable serial port pins

    // Enable Tx and Rx of UART
    TXEN = 1; // Tx
    CREN = 1; // Rx

    //Set 8-bit reception and transmission
    RX9 = 0;
    TX9 = 0;

    // Enable Rx and Tx interrupts 
    // RCIE?USART ???????
    RCIE = 1;
    //TXIE = 1;

    // TXIF?USART ???????
    TXIF = 0;
}
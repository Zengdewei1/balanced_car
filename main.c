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
    
}

void initGyroscope(void) {
    
}

void init_i2c(void) {  
    // ??I2C????
    // https://electronics.stackexchange.com/questions/417806/pic16f18877-using-i2c-to-read-sensor-lsm9ds0-value
    // SSP2???????1
    SSP2CON1.SSPEN = 0b1;       // ??SSP??
    SSP2CON1.SSPM = 0b1000;     // i2c??? clock = Fosc/(4*(SSP2ADD+1))
    // SSP2???????2
    // SSP3???????3
    SSP2CON3.PCIE = 0;          // ??stop bit??
    SSP2CON3.SDAHT = 1;         // SDA???????300ns
    // SSP2???????
    SSP2STAT.SMP = 0b1;         // ??????
    SSP2STAT.CKE = 0b1;         // ??input logic
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

    SYNC = 0; // Asynchronous mode
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
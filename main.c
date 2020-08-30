/*
 * File:   main.c
 * Author: zengdewei123
 *
 * Created on 2020?8?25?, ??2:57
 */


#include <xc.h>
#include <pic16f18854.h>
#include "bt.h"

void init_args();

void main(void) {
    // init_args();
    unsigned int i = 0;
    while (1){
        i++;
        if (i % 100 == 0){
            BT_broadcast();
        }
    }
    
    return;
}

void init_args() 
{
    ANSELA = 0;
    ANSELB = 0;
    TRISA = 0B00011111;
    TRISB=0B11110000;
    TRISC = 0;
    LATA = 0B00100000;
    LATB = 0b11111111;
    return;
}

void initBluetoothUART() {
    // Set UART I/O pins
    TRISC7 = 1; // RX as input
    TRISC6 = 0; // TX as output

    // Set baud rate of UART
    // At 4MHz system clock
    BRGH = 1; // for high baud_rate
    BRG16 = 0;
    SPBRG = 25; // 9600 Baud rate

//    SYNC = 0; // Asynchronous mode
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
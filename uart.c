#include <pic16f18854.h>
#include <stdio.h>
#include "uart.h"

void recv_handler(void) {
    PIE3bits.RCIE = 0; // disable the RX interrupt
    is_recvd = 1;
    // handle overrun error
    if (RC1STAbits.OERR == 1) {
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
    }
    do { // if frame error, keep reading RC1REG
        recvd_char = RC1REG;
    } while (RC1STAbits.FERR == 1);
    PIE3bits.RCIE = 1; // re-enable the RX interrupt
}

void init_eusart(void) {
    // init port
    // RC6->TX
    TRISCbits.TRISC6 = 0;
    ANSELCbits.ANSC6 = 0;
    // RC5->RX
    TRISCbits.TRISC5 = 1;
    ANSELCbits.ANSC5 = 0;

    // init baudrate
    BAUD1CONbits.BRG16 = 0;
    SP1BRGH = 1;
    SP1BRGL = 51;

    //set sync master clock from BRC
    TX1STAbits.CSRC = 1; // ignored in async
    TX1STAbits.SYNC = 0;
    TX1STAbits.TX9 = 0;
    RC1STAbits.RX9 = 0;

    // defaults to asynchronous receiving mode
    RC1STAbits.SPEN = 1;
    RC1STAbits.CREN = 1; // continuous 
    RC1STAbits.SREN = 1; // single 
    TX1STAbits.TXEN = 0;
}

void send_char(char data) {
    enable_out();
    TX1REG = data;
    while (TRMT != 1);
    disable_out();
}

void enable_out() {
    //enable eusart
    TX1STAbits.TXEN = 1;
    RC1STAbits.SREN = 0;
    RC1STAbits.CREN = 0;
}

void disable_out() {
    //enable eusart
    TX1STAbits.TXEN = 0;
    RC1STAbits.SREN = 1;
    RC1STAbits.CREN = 1;
}

void putch(uint8_t txData) { // used to implement the `printf` function
    send_char(txData);
}
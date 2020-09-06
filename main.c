/*
 * File:   main.c
 * Author: zengdewei123
 *
 * Created on 2020?8?25?, ??2:57
 */

#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)

#include <xc.h>

#include <pic16f18854.h>
#include "bt.h"
#include "iic.h"

void motor1_run();
void init_oc();
void set_eusart();
void test_eusart_send();
void set_pps();
void init_port();
void set_interrupt();
unsigned int recv_data();
void init_accelerometer(void);
void init_gyroscope(void);

void interrupt irs_routine() {
   //  PERIPHERAL INTERRUPT STATUS REGISTER 0
   if (PIR0bits.INTF == 1) {
       PIE0bits.INTE = 0;
//        disable_out();
        recv_data();
//        enable_out();
//        out_reset();
       PIR0bits.INTF = 0;
       PIE0bits.INTE = 1;
   }
}

unsigned int recv_data(){
   int i = 0, k = 0;
   unsigned int t = 0;
    // CREN enables the receiver circuitry of the EUSART
    if(RC1STAbits.OERR == 1) {
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
    }
    if(PIR3bits.RCIF == 1){
        while(PIR3bits.RCIF != 1);
        t = RC1REG;
    }
   while(PORTAbits.RA0 == 1);
   return t;
}

void main(void) {

    init_oc();
    init_port();
    set_interrupt();
    set_pps();
    set_eusart();
        iic_init();
    unsigned int i = 0;
    while (1) {
        i++;
        if (i % 10000 == 0){
            test_eusart_send();
//            BT_broadcast();
        }
//        uint8_t get = BT_get_char();
//        if (get == 'w'){
//            motor1_run();
//        }     
    }  
    return;
}

//void init_args() 
//{
//    ANSELA = 0;
//    ANSELB = 0;
//    TRISA = 0B00011111;
//    TRISB=0B11110000;
//    TRISC = 0;
//    LATA = 0B00000000;
//    LATB = 0b11111111;
//    return;
//}

void init_port() {
    //uart
    
    //TX -> RC5
    TRISCbits.TRISC5 = 0;
    ANSELCbits.ANSC5 = 0;
    //RC3
    TRISCbits.TRISC3 = 0;
    ANSELCbits.ANSC3 = 0;
    //RC7
    TRISCbits.TRISC7 = 1;
    ANSELCbits.ANSC7 = 0;
    
    // motor1
    // RA5 out
    TRISAbits.TRISA5 = 0;
    // RA7 out
    TRISAbits.TRISA7 = 0;
    
    //motor2
    // RC1 out
    TRISCbits.TRISC1 = 0;
    // RC2 out
    TRISCbits.TRISC2 = 0;
}

void init_oc() {
    // enable the EXTOSC with 4x PLL, 4 MHz
    OSCCON1bits.NOSC = 0b000;
    OSCCON1bits.NDIV = 0b0000;
    // Reserved
    OSCFRQbits.HFFRQ = 0b0111;
}
void init_accelerometer(void) {
    return;
}

void init_gyroscope(void) {
    return;
}


void set_interrupt() {
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE0bits.INTE = 1;

    INTCONbits.INTEDG = 1;
    INTPPS = 0x00;

    // T0CON0bits.T0EN = 1;
    // T0CON0bits.T016BIT = 0;
    // T0CON0bits.T0OUTPS = 0b0000;
    // T0CON1bits.T0CS = 0b010;
    // T0CON1bits.T0ASYNC = 0;
    // T0CON1bits.T0CKPS = 0b1100;
    // TMR0H = 0x7C;
}

void set_eusart() {
    //set BRC default
    // Fosc / 4
    BAUD1CONbits.BRG16 = 1;
    SP1BRGL = 0xCF;
    SP1BRGH = 0x00;

    //set sync master clock from BRC
    TX1STAbits.CSRC = 1;
    TX1STAbits.SYNC = 1;

    // enable EUSART
    RC1STAbits.SPEN = 1;
    
    // 9 bit send
    TX1STAbits.TX9 = 0;
    RC1STAbits.RX9 = 0;
    
    // for send
    RC1STAbits.CREN = 0;
    RC1STAbits.SREN = 0;
}

void set_pps() {
    // ref https://www.youtube.com/watch?v=tf2SfSm6fQg
    //EURSART IN
    // RC6 -> EUSART:RX1
    RXPPSbits.RXPPS = 0X16;
    // RC6PPS = 0x15;

    // OUTPUT SOURCE SELECTION REGISTER
    //EURSART OUT: TX -> RC5
    //RC0PPSbits.RC5PPS = 0X16;
    RC5PPS = 0x16;

    // RXPPS = 0X0C;RB4->RX1
    // RA0PPS = 0X14;TX->RA0
    
    // ANSELBbits.ANSB5 = 0;
    // TRISBbits.TRISB5 = 0;
}

void test_eusart_send(){
    uint8_t send_b = 'a';
    TX1REG = send_b;
    TX1STAbits.TXEN = 1;
    // TX9D 9th bit
    TX1STAbits.TXEN = 0;
    if (LATAbits.LATA5 == 1){
        LATAbits.LATA5 = 0;
    }
    else if (LATAbits.LATA5 == 0) {
        LATAbits.LATA5 = 1;
    }
}

void motor1_run(){
    LATAbits.LATA5 = 1;
}
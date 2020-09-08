#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)

#include <xc.h>
#include <pic16f18854.h>


void init_oc();
void set_pps();
void set_interrupt();
void init_port();
void set_eusart();
void send_data();
void recv_data();
void enable_out();
void disable_out();
void out_reset();
void delay(int delay_time);

unsigned int data = 0;

void __interrupt() irs_routine() {
   //  PERIPHERAL INTERRUPT STATUS REGISTER 0
   if (PIR0bits.INTF == 1) {
        PIE0bits.INTE = 0;
        disable_out();
        recv_data();
        enable_out();
        out_reset();
       PIR0bits.INTF = 0;
       PIE0bits.INTE = 1;
   }
}

void send_data(){
    enable_out();
    out_reset();
    do{
        while (PIR3bits.TXIF != 1);
        TX1REG = 0b10101100;
    }while(0);
    disable_out();
}

void recv_data(){
    if(RC1STAbits.OERR == 1) {
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
    }
    if(PIR3bits.RCIF == 1){
        while(PIR3bits.RCIF != 1);
        data = RC1REG;
    }
}

void main(void) {
    init_oc();
    init_port();
    set_interrupt();
    set_pps();
    set_eusart();
    while (1)
    {
        send_data();
        if (data != 0){
            LATAbits.LATA5 = 1;
        }
    }
    
    return;
}


void init_port() {
    //uart
    
    //RX -> RC6
    TRISCbits.TRISC6 = 0;
    ANSELCbits.ANSC6 = 0;
    
    // motor1
    // RA5 out
    TRISAbits.TRISA5 = 0;
    // RA7 out
    TRISAbits.TRISA7 = 0;
    
    //motor2
    // RC1 out
    TRISAbits.TRISA6 = 0;
    // RC2 out
    TRISCbits.TRISC1 = 0;

    //motor1
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
}

void init_oc() {
    // HFINTOSC
    OSCCON1bits.NOSC = 0b000;
    // 32 MHz
    OSCFRQbits.HFFRQ = 0b0111;
    // 1x divider
    OSCCON1bits.NDIV = 0b0000;
}

void set_interrupt() {
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE0bits.INTE = 1;

    INTCONbits.INTEDG = 1;
    INTPPS = 0x00;
}

void set_eusart() {
    BAUD1CONbits.BRG16 = 1;
    SP1BRGL = 0xCF;
    SP1BRGH = 0x00;

    //set sync master clock from BRC
    TX1STAbits.CSRC = 1;
    TX1STAbits.SYNC = 1;

    TX1STAbits.TX9 = 0;
    RC1STAbits.RX9 = 0;

    RC1STAbits.SPEN = 1;
    RC1STAbits.CREN = 0;
}

void set_pps() {
    // ref https://www.youtube.com/watch?v=tf2SfSm6fQg
    //EURSART IN
    // RC6 -> EUSART:RX1
    // RXPPSbits.RXPPS = 0X;
    // RC6PPS = 0x15;

    // OUTPUT SOURCE SELECTION REGISTER
    //EURSART OUT: TX -> RC5
    //RC0PPSbits.RC5PPS = 0X16;
    // TXPPSbits.TXPPS = 0X15;

    //EURSART OUT: DT->RC5, TC/CK->RC6
    RC5PPS = 0x11;
    RC6PPS = 0x10;
}


void enable_out() {
    //ensble eusart
    TX1STAbits.TXEN = 1;
    TX1STAbits.SYNC = 1;
    RC1STAbits.CREN = 0;
}

void delay(int delay_time) {
    int i = delay_time;
    while (i > 0) {
        --i;
    }
}

void disable_out() {
    delay(100);
    //ensble eusart
    TX1STAbits.TXEN = 0;
    TX1STAbits.SYNC = 0;
    RC1STAbits.CREN = 1;
}

void out_reset() {
    delay(88); // i = 88 time = 50.3125us; i = 100 time = 57.0625us
}

void turn_off_all(int num) {
    int i = num * 3;
    while (i > 0) {
        while (PIR3bits.TXIF != 1);
        TX1REG = 0x00;
        --i;
    }
}
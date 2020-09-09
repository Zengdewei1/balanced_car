#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit (CLKOUT function is enabled; FOSC/4 clock appears at OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (FSCM timer enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config WRT = OFF        // UserNVM self-write protection bits (Write protection off)
#pragma config SCANE = available// Scanner Enable bit (Scanner module is available for use)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (Program Memory code protection disabled)
#pragma config CPD = OFF        // DataNVM code protection bit (Data EEPROM code protection disabled)

#include <xc.h>
#include <pic16f18854.h>


void init_oc();
void set_pps();
void set_interrupt();
void init_port();
void set_eusart();
void send_data(char data);
void recv_data();
void enable_out();
void disable_out();
void out_reset();
void delay(int delay_time);

unsigned int data = 0;
char a = 'g';

void __interrupt() irs_routine() {
   //  PERIPHERAL INTERRUPT STATUS REGISTER 0
   if (0){// (PIR0bits.INTF == 1) {
        PIE0bits.INTE = 0;
        disable_out();
        recv_data();
        enable_out();
        out_reset();
       PIR0bits.INTF = 0;
       PIE0bits.INTE = 1;
   }
}

void send_data(char data){
   // enable_out();
   // out_reset();
    do{
        while (TRMT == 0);
        TX1REG = data;
        // while (PIR3bits.TXIF != 1);
        
    }while(0);
    //disable_out();
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
    //TX1STAbits.TXEN=0;
    while (1)
    {
       // send_data('a');
        
        //TX1STAbits.TXEN=0;
        while (PIR3bits.TXIF != 1);
        TX1REG = 'a';
        //TX1STAbits.TXEN=1;
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
    OSCFRQbits.HFFRQ = 0b0110;
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
    BAUD1CONbits.BRG16 = 0;
    SP1BRGL = 0x44;
    //SP1BRGH = 0x03;

    //set sync master clock from BRC
    TX1STAbits.CSRC = 1;
    TX1STAbits.SYNC = 1;

    TX1STAbits.TX9 = 0;
    RC1STAbits.RX9 = 0;

    RC1STAbits.SPEN = 1;
    RC1STAbits.CREN = 0;
    RC1STAbits.SREN = 0;
    TX1STAbits.TXEN = 1;

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
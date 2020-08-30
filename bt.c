#include "bt.h"
#include <pic16f18854.h>

//Function to load the Bluetooth Rx. buffer with one char.//
void BT_load_char(uint8_t byte) {
    TXREG = byte;
    while(!TXIF);  
    while(!TRMT);
}

//Function to Load Bluetooth Rx. buffer with string//
void BT_load_data_array(uint8_t data[], uint8_t size) {
    for (uint8_t i = 0; i < size; i++) {
        BT_load_char(data[i]);
    }
}

void BT_broadcast() {
    BT_load_char(13);
}

uint8_t BT_get_char() {
    // check for over run error 
    if(OERR) {
        CREN = 0;
        CREN = 1; //Reset CREN
    }
    
    //if the user has sent a char return the char (ASCII value)
    if(RCIF == 1) {
        uint8_t data;
        while(!RCIF);
        data = RCREG; 
        return data;
    } else {
        return 0;
    }
}

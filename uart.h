/* 
 * File:   uart.h
 * Author: leegenux
 *
 * Created on September 11, 2020, 8:15 PM
 */

#ifndef UART_H
#define	UART_H
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    char recvd_char;
    int is_recvd; // you should manually clear it

    inline void recv_handler(void);
    void init_eusart(void);
    void send_char(char data);
    void enable_out(void);
    void disable_out(void);
    void putch(uint8_t txData);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */


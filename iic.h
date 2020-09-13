#ifndef IIC_H
#define	IIC_H

#include <stdint.h>

#define DEBUG

#ifdef	__cplusplus
extern "C" {
#endif

    void init_iic(void);
    int iic_ack(uint8_t ack);
    int iic_write_byte(uint8_t addr, uint8_t data, int *n_ack);
    int iic_read_byte(uint8_t addr, uint8_t *p_data);
    int iic_start(void);
    int iic_stop(void);
    int iic_wait(void);
    int iic_wait_ack(void);
    int iic_wait_buf(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IIC_H */


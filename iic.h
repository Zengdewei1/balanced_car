#ifndef IIC_H
#define	IIC_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    void iic_init(void);
    void iic_ack(uint8_t ack);
    void iic_write_byte(uint8_t data);
    uint8_t iic_read_byte(uint8_t ack);
    void iic_start(void);
    void iic_stop(void);


#ifdef	__cplusplus
}
#endif

#endif	/* IIC_H */


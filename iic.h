#ifndef IIC_H
#define	IIC_H

#include <stdint.h>

#define DEBUG

#ifdef	__cplusplus
extern "C" {
#endif

    void init_iic(void);
    int iic_write_byte(uint8_t addr, uint8_t data, int *n_ack);
    int iic_read_byte(uint8_t addr, uint8_t *p_data);
    int mpu_read_byte(uint8_t addr, uint8_t reg, uint8_t *p_data);
    int mpu_write_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len, int *n_ack);
    int mpu_write_byte(uint8_t addr, uint8_t reg, uint8_t data, int *n_ack);
    int mpu_read_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);
    int mpu_write_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len, int *n_ack);


#ifdef	__cplusplus
}
#endif

#endif	/* IIC_H */


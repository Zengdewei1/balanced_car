/* 
 * File:   mpu6050.h
 * Author: leegenux
 *
 * Created on September 13, 2020, 7:23 PM
 */

#ifndef MPU6050_H
#define	MPU6050_H

#include <stdint.h>
#define MPU6050_DEFAULT_ADDRESS 0x68

#ifdef	__cplusplus
extern "C" {
#endif

    int read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t length, uint8_t *data);


#ifdef	__cplusplus
}
#endif

#endif	/* MPU6050_H */


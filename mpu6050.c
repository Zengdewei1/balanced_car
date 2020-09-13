#include "mpu6050.h"
#include "iic.h"

// when AD0 is the 0th bit of the Address

int read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t length, uint8_t *data) {
    int nack;
    if (iic_write_byte(dev_addr, reg_addr, &nack) == 0) {
        printf("read_reg send reg_addr failed\n");
        return 0;
    }
    for (int i = 0; i < length; i++) {
        if (iic_read_byte(dev_addr, data + i) == 0) {
            printf("read_reg recv value failed\n");
            return 0;
        }
    }
    return 1;
}
#ifndef BT_H
#define	BT_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    void BT_load_char(uint8_t byte);
    //Function to Load Bluetooth Rx. buffer with string//
    void BT_load_data_array(uint8_t data[], uint8_t size);
    void BT_broadcast();
    uint8_t BT_get_char();

#ifdef	__cplusplus
}
#endif

#endif	/* BT_H */


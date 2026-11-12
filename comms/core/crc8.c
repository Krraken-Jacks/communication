#include <stdint.h>

//crc8
uint8_t crc8(uint8_t* data, uint8_t length) {
    uint8_t crc = 0;

    while (length--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
        }
    }

    return crc;
}

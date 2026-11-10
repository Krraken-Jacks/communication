#include <libpynq.h>
#include <stdint.h>
#include "laserSend.h"
#include "crc8.h"

//send message
void send_message_laser(LaserMessage* msg) {
    //calculate crc
    msg->checksum = crc8(&msg->id, 9);

    //send bytes in required order
    uart_send(UART0, msg->start_byte);
    uart_send(UART0, msg->id);

    for (uint8_t i = 0; i < 8; i++) {
        uart_send(UART0, msg->payload[i]);
    }

    uart_send(UART0, msg->checksum);
}
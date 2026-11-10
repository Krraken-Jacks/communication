#include <libpynq.h>
#include <stdint.h>
#include "motorSend.h"
#include "crc8.h"

//send message
void send_message_motor(MotorMessage* msg) {
  //calculate crc
  msg->checksum = crc8(&msg->id, 9);

  //send bytes in required order
  uart_send(UART0, msg->start_byte);
  uart_send(UART0, msg->id);

  //send x (MSB first)
  uint8_t *byte_ptr = (uint8_t *)&msg->x;
  for (int i = 0; i < 4; i++) {
    uart_send(UART0, *(byte_ptr + i)); 
  }

  //send y (MSB first)
  byte_ptr = (uint8_t *)&msg->y;
  for (int i = 0; i < 4; i++) {
    uart_send(UART0, *(byte_ptr + i)); 
  }

  uart_send(UART0, msg->checksum);
}
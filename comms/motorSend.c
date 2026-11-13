#include <libpynq.h>
#include <stdint.h>
#include "motorSend.h"
#include "crc8.h"

//send message
void send_message_motor(float _x, float _y) {
  static MotorMessage msg = {
      .start_byte = START_BYTE,
      .id = MOTOR_MODULE_ID
  };

  msg.x = _x;
  msg.y = _y;
  
  //calculate crc
  msg.checksum = crc8(&msg.id, 9);

  //send bytes in required order
  uart_send(UART0, msg.start_byte);
  uart_send(UART0, msg.id);

  //send x (LSB first)
  uint8_t *byte_ptr = (uint8_t *)&msg.x;
  for (uint8_t i = 0; i < 4; i++) {
    uart_send(UART0, *(byte_ptr + i)); 
  }

  //send y (LSB first)
  byte_ptr = (uint8_t *)&msg.y;
  for (uint8_t i = 0; i < 4; i++) {
    uart_send(UART0, *(byte_ptr + i)); 
  }

  uart_send(UART0, msg.checksum);
}

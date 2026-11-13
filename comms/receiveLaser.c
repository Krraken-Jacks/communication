#include <libpynq.h>
#include "MessageFormat.h"
#include "crc8.h"
#include "receiveLaser.h"

#define BUFF_CAP 50

void clear_buffer(uint8_t buffer[BUFF_CAP], uint8_t *buff_size, uint8_t keep_from_index) {
  if (keep_from_index >= (*buff_size)) {
    // outabounds!
    (*buff_size) = 0;
    return;
  }

  for (uint8_t i = keep_from_index; i < (*buff_size); i++) {
    uint8_t new_index = i - keep_from_index;

    buffer[new_index] = buffer[i];
  }

  (*buff_size) -= keep_from_index;
}

// returns: 0 - nothing meaningful; 1 - meaningful message!
uint8_t interpret_msg(LaserMessage *defaultM, uint8_t buffer[BUFF_CAP], /* uint8_t *buff_size, */ uint8_t start) {
  uint8_t i = start;

  // is the first byte ok?
  if (buffer[i] != defaultM->start_byte) {
    // nah..
    return 0;
  }

  // first byte is ok thus lets check the id
  // output->start_byte = buffer[i];
  i++;

  if (buffer[i] != defaultM->id) {
    return 0;
  }

  // ok, it's for me
  // output->id = buffer[i];
  i++;

  uint8_t j;
  for (j = 0; j < 8; j++) {
    defaultM->payload[j] = buffer[i + j];
  }
  // Now: j=8

  i += j; // this will be the index of the last byte - check sum

  defaultM->checksum = buffer[i];


  // OK, lets check the message integrity... (we skip START_BYTE!)
  if (crc8(buffer + start + 1, sizeof(LaserMessage) - 1) != 0) {
    // Payload with the checksum should give 0 when this crc8 is performed.
    // But we got here if it is not... Bad message...
    return 0;
  }


  // all the pathways led to success ;)
  return 1;
}


// Returns 0 when no new message was received (or we have it only partially); returns 1 when you have new data.
// Robust: non-blocking when no uart data and keeps the buffer
// NOTE: FUCKS UP YOUR Message output (it is garbage unless 1 is returned)
uint8_t check_for_msg(LaserMessage *defaultM, uint8_t buffer[BUFF_CAP], uint8_t *buff_size) {
  while (uart_has_data(UART0)) {
    if ((*buff_size) >= BUFF_CAP) { // safety net, discard oldest byte
      clear_buffer(buffer, buff_size, 1);
    }

    buffer[(*buff_size)++] = uart_recv(UART0);


    // discard leading junk: nothing before a start byte can be useful
    uint8_t skip = 0;
    while (skip < (*buff_size) && buffer[skip] != START_BYTE) {
      skip++;
    }
    if (skip > 0) {
      clear_buffer(buffer, buff_size, skip);
    }


    if ((*buff_size) < sizeof(LaserMessage)) {
      // not much to interpret... Let's skip...
      continue;
    }


    if (interpret_msg(defaultM, buffer, 0) == 1) {
      // got a sensible message
      // lets clear up the buffer till the end of the good message
      uint8_t keep_from = sizeof(LaserMessage);

      clear_buffer(buffer, buff_size, keep_from);

      return 1;
    }


    // so we tried to interpret, but nothing good.
    // Bad CRC, other ID, or a false start byte...
    clear_buffer(buffer, buff_size, 1);
  }

  // no good message was received
  return 0;
}




// ONLY THIS FUNCTION GETS EXPOSED TO THE USER; if the function returns 1, payload will contain the received payload (as a c string, thus it is 9 chars with an \0 at the end!)
uint8_t check_laser_msg(char payload[8], uint8_t buffer[], uint8_t *buff_size) {
  static LaserMessage msg = {
    .start_byte = START_BYTE,
    .id = LASER_MODULE_ID
  };

  uint8_t r = check_for_msg(&msg, buffer, buff_size);

  if (r == 1) {
    for (int i = 0; i < 8; i++) {
      *(payload + i) = msg.payload[i];
    }
  }

  return r;
}



// int main(void) {
//   pynq_init();

//   comms_init();


//   Message temp;
//   uint8_t buffer[BUFF_CAP];
//   uint8_t size = 0;

//   while (1) {
//     if (check_for_msg(&temp, buffer, &size) == 1) {
//       printf("Hey! A message received:\n");
//       printf("start_byte: %d (duh..)\n", temp.start_byte);
//       printf("id: %c (duh..)\n", temp.id);

//       printf("payload:\n");
//       for (int i = 0; i < 8; i++) {
//         printf("%c, ", temp.payload[i]);
//       }
//       printf("\n checksum: %d", temp.check_sum);
//     }
//   }

//   pynq_destroy();
//   return EXIT_SUCCESS;
// }

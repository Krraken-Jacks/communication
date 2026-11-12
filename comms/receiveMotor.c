#include <libpynq.h>

#define START_BYTE 0b10000000
#define MY_ID 'M'
#define BUFF_CAP 50

struct __attribute__((packed)) message {
  uint8_t start_byte;
  char id;
  float x, y;
  uint8_t check_sum;
};

typedef struct message Message;

_Static_assert(sizeof(Message) == 11, "Hey! The parsing might be bad!");

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

uint8_t crc8(uint8_t data[], uint8_t length) {
  uint8_t crc = 0;

  while (length--) {
    crc ^= *data++;
    for (uint8_t i = 0; i < 8; i++) {
      crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
    }
  }

  return crc;
}

// returns: 0 - nothing meaningful; 1 - meaningful message!
uint8_t interpret_msg(Message *output, uint8_t buffer[BUFF_CAP], /* uint8_t *buff_size, */ uint8_t start) {
  uint8_t i = start;

  // is the first byte ok?
  if (buffer[i] != START_BYTE) {
    // nah..
    return 0;
  }

  // first byte is ok thus lets check the id
  output->start_byte = buffer[i];
  i++;
  // printf("START BYTE!\n");

  if (buffer[i] != MY_ID) {
    // printf("not my id\n");
    return 0;
  }

  // ok, it's for me
  output->id = buffer[i];
  // printf("MY ID!\n");
  i++;

  uint8_t j;
  uint8_t *adress_first_x_byte = (uint8_t*)(&(output->x));
  for (j = 0; j < 4; j++) {
    (*(adress_first_x_byte + 3 - j)) = buffer[i + j];
  }
  
  uint8_t *adress_first_y_byte = (uint8_t*)(&(output->y));
  for (j = 4; j < 8; j++) {
    (*(adress_first_y_byte + 7 - j)) = buffer[i + j];
  }
  // Now: j=8

  i += j; // this will be the index of the last byte - check sum

  output->check_sum = buffer[i];
  // printf("PAYLOAD COLLECTED!\n");


  // OK, lets check the message integrity... (we skip START_BYTE!)
  if (crc8(buffer + start + 1, sizeof(Message) - 1) != 0) {
    // Payload with the checksum should give 0 when this crc8 is performed.
    // But we got here if it is not... Bad message...

    
    // printf("BAD CRC!\n");
    return 0;
  }


  // all the pathways led to success ;)
  return 1;
}


// Returns 0 when no new message was received (or we have it only partially); returns 1 when you have new data.
// Robust: non-blocking when no uart data and keeps the buffer
// NOTE: FUCKS UP YOUR Message output (it is garbage unless 1 is returned)
uint8_t check_for_msg(Message *output, uint8_t buffer[BUFF_CAP], uint8_t *buff_size) {
  while (uart_has_data(UART0)) {
    if ((*buff_size) >= BUFF_CAP) { // safety net, discard oldest byte
      clear_buffer(buffer, buff_size, 1);
    }

    buffer[(*buff_size)++] = uart_recv(UART0);
    // printf("========\n");
    // printf("%d\n", buffer[(*buff_size) - 1]);
    // printf("========\n");


    // discard leading junk: nothing before a start byte can be useful
    uint8_t skip = 0;
    while (skip < (*buff_size) && buffer[skip] != START_BYTE) {
      skip++;
    }
    if (skip > 0) {
      clear_buffer(buffer, buff_size, skip);
      // printf("discarded %d bytes\n", skip);
    }


    if ((*buff_size) < sizeof(Message)) {
      // not much to interpret... Let's skip...
      continue;
    }


    if (interpret_msg(output, buffer, 0) == 1) {
      // got a sensible message
      // lets clear up the buffer till the end of the good message
      uint8_t keep_from = sizeof(Message);

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

void comms_init() {
  switchbox_set_pin(IO_AR0, SWB_UART0_RX);
  switchbox_set_pin(IO_AR1, SWB_UART0_TX);

  uart_init(UART0);
  uart_reset_fifos(UART0);
}



int main(void) {
  pynq_init();

  comms_init();


  Message temp;
  uint8_t buffer[BUFF_CAP];
  uint8_t size = 0;

  printf("Waiting...:\n");

  while (1) {
    if (check_for_msg(&temp, buffer, &size) == 1) {
      printf("Hey! A message received:\n");
      printf("start_byte: %d (duh..)\n", temp.start_byte);
      printf("id: %c (duh..)\n", temp.id);

      printf("payload:\n");
      printf("x: %.2f\n", temp.x);
      printf("y: %.2f\n", temp.y);
      printf("\nchecksum: %d\n\n", temp.check_sum);
    }
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}

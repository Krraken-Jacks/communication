#include <libpynq.h>
#include "comms_init.h"

//AR0 - RX, AR1 - TX!!!
void comms_init() {
  switchbox_set_pin(IO_AR0, SWB_UART0_RX);
  switchbox_set_pin(IO_AR1, SWB_UART0_TX);

  uart_init(UART0);
  uart_reset_fifos(UART0);
}

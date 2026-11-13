### Hi, dear friends
we made you a "library" you can copy over into your applications folder and then communicate between PYNQs.
(Made for Laser, Motor and Encoding submodules...)

### Usage:
(if you get a Clock skew error, look at the bottom)

#### 1. Instalation:
1. copy the comms folder into your application folder (libPYNQ.../applications/SocialSubmodule/) via scp;
2. copy also the Makefile from the repository to the same application folder. (This is important for compiling.)

#### 2. Example usage:

main.c
``` main.c
#include <libpynq.h>

#include "comms_init.h" // You always need to include this; Adds: void comms_init();
//By default: AR0 - RX, AR1 - TX!!!

// From these guys include only what you need
#include "receiveLaser.h" // Adds: uint8_t check_laser_msg(char payload[8], uint8_t buffer[], uint8_t *buff_size);
#include "receiveMotor.h" // Adds: uint8_t check_motor_msg(float *x, float *y, uint8_t buffer[], uint8_t *buff_size);
#include "laserSend.h" // Adds: void send_message_laser(char _payload[8]);
#include "motorSend.h" // Adds: void send_message_motor(float _x, float _y);
// .....

int main(void) {
  pynq_init();

  // 1. Before beginning communications do this!
  comms_init();

  // 2. If you will be receiving, create a buffer; using default settings replace BUFF_CAP with 50
  uint8_t buffer[BUFF_CAP];
  uint8_t size = 0;


  while (1) {
    // 3. Laser Message

    // 3.1 Send
    char payloadForSending[8] = "ABCDEFGH"; // NOTICE: NO \0 at the end; this is just an array of chars, not a c string

    send_message_laser(payloadForSending); // sending a payload 😊


    // 3.2 Receive
    char payloadForReceiving[8]; // NOTICE: NO \0 at the end; this is just an array of chars, not a c string

    if (check_laser_msg(payload, buffer, buff_size) == 1) {
      // Received a payload; you can use it somewhere.
      // Let's print it for example.

      // because it is not a C string, you cannot print it via %s
      for (int i = 0; i < 8; i++) {
        printf("%c", payloadForReceiving[i]);
      }
    }



    // 4. Motor Message

    // 4.1 Send
    float x = 50.10;
    float y = 150.22;

    send_message_motor(x, y);


    // 4.2 Receive
    float xr, yr;
    
    if (check_motor_msg(&xr, &yr, buffer, buff_size) == 1) {
      // Received a payload; you can use it somewhere.
      // Let's print it for example.

      printf("x: %.2f, y: %.2f", xr, yr);
    }


    // ...
    // Do whatever
    // ...
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}

```


#### 3. Compilation
Compile via
```
?
```

#### 4. Clock skew error
Make edit date of files to be "now"
```
find . -type f -exec touch {} +
```
Then build:
```
make clean && make
```

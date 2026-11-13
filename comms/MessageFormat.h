#ifndef MESSAGE_FORMAT_H
#define MESSAGE_FORMAT_H

#define START_BYTE 0x80 // (0b10000000)
#define LASER_MODULE_ID 'L'
#define MOTOR_MODULE_ID 'M'

#include <stdint.h>


#pragma pack(push, 1)

//message (11 bytes long)
struct laser_message {
  uint8_t start_byte;
  uint8_t id;
  uint8_t payload[8];
  uint8_t checksum;
};

#pragma pack(pop)

typedef struct laser_message LaserMessage;
_Static_assert(sizeof(LaserMessage) == 11, "Ayo! Shi' ain't packed...");





#pragma pack(push, 1)

//message (11 bytes long)
struct motor_message {
  uint8_t start_byte;
  uint8_t id;
  float x;
  float y;
  uint8_t checksum;
};

#pragma pack(pop)

typedef struct motor_message MotorMessage;
_Static_assert(sizeof(MotorMessage) == 11, "Ayo! Shi' ain't packed...");


#endif // MESSAGE_FORMAT_H

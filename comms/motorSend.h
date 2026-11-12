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

void send_message_motor(MotorMessage* msg);

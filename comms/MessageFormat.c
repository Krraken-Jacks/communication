#define START_BYTE 0x80 // (0b10000000)

const LaserMessage LASER_MESSAGE_DEFAULT = { .start_byte = START_BYTE, .id = 'L' };
const MotorMessage MOTOR_MESSAGE_DEFAULT = { .start_byte = START_BYTE, .id = 'M' };

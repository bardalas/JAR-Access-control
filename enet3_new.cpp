#include "enet3_new.h"

// Define constants for communication
#define COMM_MESSAGE_START 0x3C
#define COMM_MESSAGE_END 0x3E
#define COMM_PADBYTE 0x10
#define ENET3_SLAVE_ID 0x30
#define START_POSITION_IN_ARRAY 3  // Use this if you need to refer to the start position in the data array

void ENet3_Init() {
  // Initialize any necessary settings here if needed
}

unsigned char SendByte(unsigned char byte) {
  // Implement sending a byte via Serial or your preferred method
  Serial.write(byte);
  return 1;  // Return 1 for success (implement failure checks if needed)
}

unsigned char SendByteSafe(unsigned char byte) {
  // This could implement additional checks or blocking sends if needed
  return SendByte(byte);
}

unsigned char ENet3_SSend(unsigned char *data, unsigned short dataLen) {
  signed short pos;
  unsigned char cs = 0;  // Checksum
  unsigned char t;

  // Check if data length is valid
  if ((dataLen == 0))
    return 0;

  // Start sending the message
  for (pos = -3; pos < (signed)(dataLen + 2); ++pos) {
    if (pos == -3) {
      t = SendByteSafe(COMM_MESSAGE_START);
    } else if (pos == -2) {
      t = SendByte(ENET3_SLAVE_ID >> 8);  // Most significant byte of slave ID
      cs += ENET3_SLAVE_ID >> 8;          // Update checksum
    } else if (pos == -1) {
      t = SendByte(ENET3_SLAVE_ID & 0xFF);  // Least significant byte of slave ID
      cs += ENET3_SLAVE_ID & 0xFF;          // Update checksum
    } else if (pos == dataLen) {
      t = SendByte(cs);  // Send the checksum
    } else if (pos == (dataLen + 1)) {
      t = SendByteSafe(COMM_MESSAGE_END);
    } else {
      t = SendByte(data[pos]);  // Send data byte
      cs += data[pos];          // Update checksum
    }

    if (!t)
      break;  // Exit if sending failed
  }

  return (t) ? 1 : 0;  // Return success if all bytes were sent
}




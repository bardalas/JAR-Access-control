#ifndef ENET3_NEW_H
#define ENET3_NEW_H

#include <Arduino.h>

// Function prototypes
void ENet3_Init();
unsigned char ENet3_SSend(unsigned char *data, unsigned short dataLen);

// Function to send a byte safely
unsigned char SendByteSafe(unsigned char byte);

// Function to send a byte
unsigned char SendByte(unsigned char byte);

#endif // ENET3_NEW_H

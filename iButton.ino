#include <OneWire.h>

/*
1 - 01 FD 03 F7 01 00 00 17
2 - 01 3B CA 34 01 00 00 5A
3 - 01 49 AC A0 01 00 00 CE
4 - 08 B9 85 75 00 00 00 50
*/

byte _MasterTag[8] = { 0x01, 0xFD, 0x03, 0xF7, 0x01, 0x00, 0x00, 0x17 };

OneWire ds(8);  // on pin 8 (a 4.7K resistor is necessary)
byte i;
byte addr[8];


boolean isMasterTag() {
  if (memcmp(addr, _MasterTag, sizeof(addr)) == 0)
    return true;
  else
    return false;
}


boolean getUID() {

  addr[0] = 0x10;  // cleacr the buffer... 0x10 is just a random number

  if (!ds.search(addr)) {
    Serial.print(".");
    ds.reset_search();
    delay(250);
    return false;

  } else {
    Serial.println("Found something on the bus:");
    for (int i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.println("");
    return true;
  }
  return false;
}
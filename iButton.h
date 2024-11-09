#include <OneWire.h>

/*
1 - 01 FD 03 F7 01 00 00 17
2 - 01 3B CA 34 01 00 00 5A
3 - 01 49 AC A0 01 00 00 CE
4 - 08 B9 85 75 00 00 00 50
*/

byte _MasterTag[8] = { 0x01, 0xFD, 0x03, 0xF7, 0x01, 0x00, 0x00, 0x17 }; //Master
byte _DriverTag[8] = { 0x01, 0x3B, 0xCA, 0x34, 0x01, 0x00, 0x00, 0x5A }; //Driver

OneWire ds_l(DALLAS_READER_R);  // on pin 2 (a 4.7K resistor is necessary)
OneWire ds_r(DALLAS_READER_L);  // on pin 2 (a 4.7K resistor is necessary)
byte i;
byte addr[8];


boolean isMasterTag() {

  Serial.print("_addr:");
    for (int i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }Serial.println(" ");

  Serial.print("_MasterTag:");
    for (int i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(_MasterTag[i], HEX);
    }Serial.println(" ");

  if (memcmp(addr, _MasterTag, sizeof(addr)) == 0)
    return true;
  else
    return false;
}


boolean isDriverTag() {

  Serial.print("_addr:");
    for (int i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }Serial.println(" ");

  Serial.print("_DriverTag:");
    for (int i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(_DriverTag[i], HEX);
    }Serial.println(" ");

  if (memcmp(addr, _DriverTag, sizeof(addr)) == 0)
    return true;
  else
    return false;
}


boolean getUID_L() {

  addr[0] = 0x10;  // cleacr the buffer... 0x10 is just a random number

  if (!ds_l.search(addr)) {
    Serial.print(".");
    ds_l.reset_search();
    delay(250);
    return false;

  } else {
  //  Serial.println("Found something on the bus:");
    for (int i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.println("");
    return true;
  }
  return false;
}

boolean getUID_R() {

  addr[0] = 0x10;  // cleacr the buffer... 0x10 is just a random number

  if (!ds_r.search(addr)) {
    Serial.print(".");
    ds_r.reset_search();
    delay(250);
    return false;

  } else {
  //  Serial.println("Found something on the bus:");
    for (int i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.println("");
    return true;
  }
  return false;
}
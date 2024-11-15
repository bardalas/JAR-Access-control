//****************************************************************************************************
//****************************************************************************************************
//*****************************************   INCs   *************************************************
//****************************************************************************************************
//****************************************************************************************************

#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "enet3_new.h"
#include "vars.h"
#include "iButton.h"
#include "EEPROM_writeAnything.h"



//****************************************************************************************************
//****************************************************************************************************
//****************************************************************************************************
//****************************************************************************************************



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //reportToEnet(MASTER);
  //  char x[10]  = {'testing'};
  // sendToEnet(x);

  // loadAll();
  initSyS();

  Serial.println(VERSION);

  delay(2000);
  enterDeepSleep();
}

void loop() {

  if (!(millis() - wakeUpTime > MAX_ALOWED_UNLOCKED) && isBoxClosed()) {
    //Woke up ==> scan tag
    while (getUID_L() || getUID_R()) {

      // wokeUp = false;
      //A new device is found - put info on LCD
      //  Serial.println("getUID=true");

      //should we lock or should we open ?
      if (isMasterTag()) {
        openBox("Master");  //blue
        report = MASTER;
        //  reportToEnet(MASTER);
        //   wakeUpTime = millis() + MAX_ALOWED_UNLOCKED;
      }

      else if (isDriverTag()) {  //red
        openBox("Driver");
        report = DRIVER;
        //reportToEnet(DRIVER);
        //   wakeUpTime = millis() + MAX_ALOWED_UNLOCKED;
      }

      else {
        lcd.clear();
        lcd.print(" Access Denied!");
        //lock box ?
        digitalWrite(BlueLED, LOW);
        digitalWrite(GreenLED, LOW);
        tone(Buzzer, 2000);
        //   servo.write(10);
        for (int i = 0; i < 2; i++) {
          digitalWrite(RedLED, HIGH);
          delay(250);
          digitalWrite(RedLED, LOW);
          delay(250);
        }
        noTone(Buzzer);
      }
    }
  } else {

    if (isBoxClosed()) {
      delay(2000);
      enterDeepSleep();
      return;
    }

    //check if time passed and box is still closed
    // if (millis() - wakeUpTime > MAX_ALOWED_UNLOCKED) {
    //    Serial.println("time passed and box is still closed");

    if (isBoxClosed()) {
      delay(2000);
      enterDeepSleep();

    } else {
      //Box is open ==> Do we need to report ?
      if (report != NO_USER) {

        Serial.println("Reporting to enet3");
        reportToEnet(report);
        report = NO_USER;
      }


      //The box is open
      if (millis() - openBoxTime > MAX_ALOWED_OPEN) {  //should we set an alarm ?
        Serial.println("Alarm");
        for (int i = 0; i < 30; i++) {
          tone(Buzzer, 2000);
          delay(20);
          noTone(Buzzer);
          delay(20);
        }
      } else {

        //No alarm - the box is just open
        //     Serial.println("Box is open - No alarem (yet)");
      }
      //the box is open.. wait until get close
    }
    // }
  }
}





void wakeUp() {
  // Serial.println("Wokeup");
  // Execution resumes here after waking up
  // sleep_disable();     // Disable sleep mode
  // power_all_enable();  // Re-enable all modules
  wokeUp = true;
  detachInterrupt(digitalPinToInterrupt(DALLAS_READER_L));
  detachInterrupt(digitalPinToInterrupt(DALLAS_READER_R));
}

void enterDeepSleep() {

  Serial.println("going to sleep");


  // Disable ADC and other unused modules
  servo.write(10);
  delay(500);
  //  Serial.println("Going to sleep");
  // sendToEnet("Going to sleep");
  lcd.clear();
  lcd.print(" Going to sleep");

  lcd.clear();
  lcd.noBacklight();
  lcd.noDisplay();

  digitalWrite(BlueLED, LOW);
  digitalWrite(RedLED, LOW);
  digitalWrite(GreenLED, LOW);

  reportToEnet(NO_USER);
  delay(2000);

  attachInterrupt(digitalPinToInterrupt(DALLAS_READER_L), wakeUp, LOW);
  attachInterrupt(digitalPinToInterrupt(DALLAS_READER_R), wakeUp, LOW);
  sleep_enable();



  power_all_disable();
  sleep_cpu();

  sleep_disable();     // Disable sleep mode
  power_all_enable();  // Re-enable all modules

  // delay(500);

  //  Serial.println("wokeUp=true");
  wakeUpTime = millis();  // Record start time
  Serial.println("Wake up");
  wokeUp = true;
  // sendToEnet("Wake up");

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  lcd.print("Ready");
  loop();

  // delay(1000);
}

boolean isBoxLocked() {
  if (analogRead(LOCK_DET) > 5)  //5 is in order to be above noise level
    return false;
  return true;
}

void openBox(String s) {

  wokeUp = false;
  Serial.println("Unlocking Box");
  lcd.clear();
  // lcd.setCursor(2, 0);
  //  lcd.print("Permission");
  lcd.setCursor(0, 1);
  lcd.print(" Access Granted! - ");
  Serial.println(s);
  lcd.print(s);
  digitalWrite(GreenLED, HIGH);
  digitalWrite(BlueLED, LOW);
  digitalWrite(RedLED, LOW);
  delay(50);
  servo.write(150);
  for (int i = 0; i < 2; i++) {
    tone(Buzzer, 2000);
    delay(250);
    noTone(Buzzer);
    delay(250);
  }
}

void closeBox() {
}


boolean isBoxClosed() {
  if (analogRead(CLOSE_DET) > 5)  //5 is in order to be above noise level
    return false;
  return true;
}

//1,IDIDIDIDIDID,2,0,3,0
//2,IDIDIDIDIDID,2,1,3,1
//USER,ID,LID,STAT,LOCK,STAT
void reportToEnet(int user) {


  temp_msg.msg_len = 0;  // reset msg.length to zero.
  temp_msg.message[temp_msg.msg_len++] = 0x83;
  temp_msg.message[temp_msg.msg_len++] = ENET3_Client_ID;
  temp_msg.message[temp_msg.msg_len++] = 0;



  if (user != NULL) {

    //Send user data
    Serial.println("Bos is opened:");

    if (user == MASTER) {
      temp_msg.message[temp_msg.msg_len++] = '1';
      Serial.println("Master:");
    }
    if (user == DRIVER) {
      temp_msg.message[temp_msg.msg_len++] = '2';
      Serial.println("Driver:");
    }

    temp_msg.message[temp_msg.msg_len++] = ',';

    //byte _MasterTag[8] = { 0x01, 0xFD, 0x03, 0xF7, 0x01, 0x00, 0x00, 0x17 }; //Master

    char asciiArray[17];  // Each byte will take two hex characters + null-terminator
    for (int i = 0; i < 8; ++i) {
      // Convert each byte to a two-character hexadecimal representation
      sprintf(asciiArray + i * 2, "%02X", ((user == MASTER) ? _MasterTag[i] : _DriverTag[i]));
    }
    for (uint8_t a = 0; a < 16; a++)
      temp_msg.message[temp_msg.msg_len++] = asciiArray[a];

    temp_msg.message[temp_msg.msg_len++] = ',';
  }




  //send box status
  temp_msg.message[temp_msg.msg_len++] = '2';
  temp_msg.message[temp_msg.msg_len++] = ',';
  isBoxClosed() ? temp_msg.message[temp_msg.msg_len++] = '1' : temp_msg.message[temp_msg.msg_len++] = '0';
  temp_msg.message[temp_msg.msg_len++] = ',';
  temp_msg.message[temp_msg.msg_len++] = '3';
  temp_msg.message[temp_msg.msg_len++] = ',';
  isBoxLocked() ? temp_msg.message[temp_msg.msg_len++] = '1' : temp_msg.message[temp_msg.msg_len++] = '0';


  ENet3_SSend(temp_msg.message, temp_msg.msg_len);

  openBoxTime = millis();
}
/*
unsigned char ENet3_SSend1(unsigned char *data, unsigned short dataLen){

  Serial.print

}*/

void initSyS() {
  pinMode(GreenLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  pinMode(DALLAS_READER_L, INPUT_PULLUP);
  pinMode(DALLAS_READER_R, INPUT_PULLUP);

  servo.attach(SERVO_PIN);


  //Init System
  lcd.begin();
  lcd.backlight();
  lcd.clear();


  //Write Welcome screen
  lcd.print(NAME);
  lcd.setCursor(0, 1);
  lcd.print(VERSION);

  servo.write(10);
  noTone(Buzzer);
  digitalWrite(BlueLED, HIGH);
  digitalWrite(RedLED, LOW);
  digitalWrite(GreenLED, LOW);
}

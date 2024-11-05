

//****************************************************************************************************
//****************************************************************************************************
//*****************************************   INCs   *************************************************
//****************************************************************************************************
//****************************************************************************************************

#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <avr/sleep.h>
#include <avr/power.h>



//****************************************************************************************************
//****************************************************************************************************
//*****************************************   DEFs   *************************************************
//****************************************************************************************************
//****************************************************************************************************

#define NAME "JAR Access control"
#define VERSION "Ver 1.0"

#define BlueLED 12
#define GreenLED 3
#define RedLED 4
#define Buzzer 5
#define WAKEUP_PIN 2
#define SERVO_PIN 6

#define LCD_ADDR 0x27
#define LCD_backlighPin 16

#define SLEEP_TIMEOUT 1 * (10 * 1000)  //minutes

#define SEND_DATA_BUFFER_SIZE 1024
#define ENET3_Client_ID 13

typedef struct
{
  uint8_t message[SEND_DATA_BUFFER_SIZE];
  uint16_t msg_len;
} tMessage;

//****************************************************************************************************
//****************************************************************************************************
//*****************************************   VARS   *************************************************
//****************************************************************************************************
//****************************************************************************************************

Servo servo;
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_backlighPin, 2);


unsigned long startTime;
unsigned long endTime;
unsigned long loopTime;

//****************************************************************************************************
//****************************************************************************************************
//****************************************************************************************************
//****************************************************************************************************



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(GreenLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  pinMode(WAKEUP_PIN, INPUT_PULLUP);

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

  Serial.println("Welcome");

  delay(2000);
  enterDeepSleep();

}

void loop() {

  // lcd.on();


  // Your loop code here

  endTime = millis();              // Record end time
  loopTime = endTime - startTime;  // Calculate loop duration

   if (loopTime >= SLEEP_TIMEOUT) enterDeepSleep();



  while (getUID()) {


    //A new device is found - put info on LCD

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Permission");
    lcd.setCursor(0, 1);


    if (isMasterTag()) {
      lcd.print(" Access Granted!");
      digitalWrite(GreenLED, HIGH);
      digitalWrite(BlueLED, LOW);
      digitalWrite(RedLED, LOW);
      servo.write(100);
      delay(50);
      for (int i = 0; i < 2; i++) {
        tone(Buzzer, 2000);
        delay(250);
        noTone(Buzzer);
        delay(250);
      }
    } else {
      lcd.print(" Access Denied!");
      digitalWrite(BlueLED, LOW);
      digitalWrite(GreenLED, LOW);
      tone(Buzzer, 2000);
      servo.write(10);
      for (int i = 0; i < 2; i++) {
        digitalWrite(RedLED, HIGH);
        delay(250);
        digitalWrite(RedLED, LOW);
        delay(250);
      }
      noTone(Buzzer);
    }

    delay(2000);

    lcd.clear();
    lcd.print(" Access Control ");
    lcd.setCursor(0, 1);
    lcd.print("Scan Your Card>>");
  }
}

void wakeUp() {
  // Execution resumes here after waking up
 // sleep_disable();     // Disable sleep mode
 // power_all_enable();  // Re-enable all modules

  detachInterrupt(digitalPinToInterrupt(WAKEUP_PIN));


}

void enterDeepSleep() {
  // Disable ADC and other unused modules
  Serial.println("Going to sleep");
  lcd.clear();
  lcd.print(" Going to sleep");
  
  lcd.clear();
  lcd.noBacklight();
  lcd.noDisplay();
  
  digitalWrite(BlueLED, LOW);
  digitalWrite(RedLED, LOW);
  digitalWrite(GreenLED, LOW);

  attachInterrupt(digitalPinToInterrupt(WAKEUP_PIN), wakeUp, LOW);
  sleep_enable();

  power_all_disable();
  sleep_cpu();

  sleep_disable();     // Disable sleep mode
  power_all_enable();  // Re-enable all modules

 // delay(500);
  startTime = millis();  // Record start time
  Serial.println("Wake up");
  
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  lcd.print("Ready");
  loop();

 // delay(1000);

}

void sendToInit(String s){

  temp_msg.msg_len = 0;  // reset msg.length to zero.
  temp_msg.message[temp_msg.msg_len++] = 0x83;
  temp_msg.message[temp_msg.msg_len++] = ENET3_Client_ID;
  temp_msg.message[temp_msg.msg_len++] = 0;

  for (uint8_t j=0; j>=s.length; j++)
    temp_msg.message[temp_msg.msg_len++] = s[j];


  //    Add_Num_To_Send_Data_Buffer(1, true);
  //    Add_Num_To_Send_Data_Buffer(2, true);
  //    Add_Num_To_Send_Data_Buffer(3, false);

  // Send the message without waiting for an ack
  ENet3_SSend(temp_msg.message, temp_msg.msg_len);
}
//****************************************************************************************************
//****************************************************************************************************
//*****************************************   DEFs   *************************************************
//****************************************************************************************************
//****************************************************************************************************

#define NAME "JAR Access control"
#define VERSION "Ver 1.3"

#define BlueLED 12
#define GreenLED 3
#define RedLED 4
#define Buzzer 5
#define WAKEUP_PIN 2

#define SERVO_PIN 6
#define DALLAS_READER_R 2
#define DALLAS_READER_L 3

#define LOCK_DET A0
#define CLOSE_DET A1

#define  NO_USER 0
#define  MASTER  1
#define  DRIVER  2

#define LCD_ADDR 0x27
#define LCD_backlighPin 16

#define MAX_ALOWED_OPEN       1 * (20 * 1000)  //minutes
#define MAX_ALOWED_UNLOCKED   1 * (20 * 1000)

#define SEND_DATA_BUFFER_SIZE 200
#define ENET3_Client_ID 13

typedef struct
{
  uint8_t message[SEND_DATA_BUFFER_SIZE];
  uint16_t msg_len;
} tMessage;

tMessage temp_msg;

//****************************************************************************************************
//****************************************************************************************************
//*****************************************   VARS   *************************************************
//****************************************************************************************************
//****************************************************************************************************

Servo servo;
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_backlighPin, 2);


unsigned long wakeUpTime;
unsigned long openBoxTime;
unsigned long loopTime;
boolean wokeUp = false;
int report = NO_USER;

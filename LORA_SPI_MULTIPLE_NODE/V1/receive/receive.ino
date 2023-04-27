#include <SPI.h>  // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
//hw_timer_t* timer = NULL;

//#define ss 5
//#define rst 16
//#define dio0 17
#define ss 5
#define rst 21
#define dio0 22

#define D2 26
#define D3 27
#define D4 14
#define buzzer 15
//#define D2 4
//#define D3 0
//#define D4 2
//#define buzzer 25

#define Den2(x) digitalWrite(D2, x)
#define Den3(x) digitalWrite(D3, x)
#define Den4(x) digitalWrite(D4, x)
#define buzz(x) digitalWrite(buzzer, x)
unsigned long
previousMillis = 0,
previoussecs = 0,
currentsecs = 0,
currentMillis = 0;

int interval = 1;  // updated every 1 second
int Secs = 0;
int pack;
byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;
String SenderNode = "";
String outgoing;    // outgoing message
byte msgCount = 0;  // count of outgoing messages
String incoming = "";
String message = "";
String
h1s,
t1s,
fT1s,
h2s,
t2s,
fT2s,
checkDevice;
float
h1,
t1,
fT1,
h2,
t2,
fT2;
bool
ttClearLcd = 0,
ttBuzzer = 0,
dataErr = 0,
tt = 0,
ttConnectLora = 0;
int
clr = 0,
coi = 0,
countSend = 0,
checkDevices;

byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};
//void IRAM_ATTR onTimer() {
//  if (ttConnectLora == 1) {
//
//  }
//}
void setup() {
  Serial.begin(115200);  // initialize serial
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, degree);
  while (!Serial);
  //  timer = timerBegin(0, 80, true);
  //  timerAttachInterrupt(timer, &onTimer, true);
  //  timerAlarmWrite(timer, 10000, true);    //1000000 -> 1s
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    //ttConnectLora = 0;
    while (1)
      ;
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Master Node");
  //ttConnectLora = 1;
}

void loop() {
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    countSend++;
    clr++;
    coi++;
    message = "10";
    sendMessage(message, MasterNode, Node1);

    if (clr == 2) {
      ttClearLcd = 1;
      clr = 0;
    }
    if (coi >= 2 && t1 >= 36) {
      coi = 0;
      ttBuzzer = !ttBuzzer;
    }
    if (t1 < 36) {
      ttBuzzer = 0;
    }
    previoussecs = currentsecs;
  }
  //Serial.println(LoRa.parsePacket());
  onReceive(LoRa.parsePacket());
}

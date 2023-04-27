#include <SPI.h>  // include libraries
#include <LoRa.h>
#include "DHT.h"

hw_timer_t* timer = NULL;

#define ss 5
#define rst 21
#define dio0 22

#define D2 26
#define D3 27
#define D4 14

#define Den2(x) digitalWrite(D2, x)
#define Den3(x) digitalWrite(D3, x)
#define Den4(x) digitalWrite(D4, x)

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long
previousMillis = 0,
previoussecs = 0,
currentsecs = 0,
currentMillis = 0;
int
interval = 1,  // updated every 1 second
Secs = 0,
ttConnect = 0;

String
outgoing,  // outgoing message
Mymessage = "";

byte
msgCount = 0,  // count of outgoing messages
MasterNode = 0xFF,
Node1 = 0xBB;
float
humidity = 0.0,
temperature = 0.0,
ftemperature = 0.0;
volatile bool
tt = 0,
errorDHT = 0,
ttSend = 0,
ttConnectLora = 0;
void IRAM_ATTR onTimer() {
  if (ttConnectLora == 1) {

  }
}
void setup() {
  Den4(0);
  Serial.begin(115200);  // initialize serial
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 80000, true);    //1000000 -> 1s
  dht.begin();
  while (!Serial)
    ;
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    ttConnectLora = 0;
    while (1)
      ;
  }
  LoRa.setSyncWord(0xF3);
  ttConnectLora = 1;
  Serial.println("LoRa Node1");
}
void loop() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  ftemperature = dht.readTemperature(true);

  onReceive(LoRa.parsePacket());
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;  // if there's no packet, return
  // read packet header bytes:
  int recipient = LoRa.read();        // recipient address
  byte sender = LoRa.read();          // sender address
  byte incomingMsgId = LoRa.read();   // incoming msg ID
  byte incomingLength = LoRa.read();  // incoming msg length
  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  if (incomingLength != incoming.length()) {  // check length for error
    // Serial.println("error: message length does not match length");
    ;
    return;  // skip rest of function
  }
  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;  // skip rest of function
  }
  Serial.println(incoming);
  int Val = incoming.toInt();
  if (Val == 10) {
    Mymessage = Mymessage + humidity + ',' + temperature + ',' + ftemperature;
    sendMessage(Mymessage, MasterNode, Node1);  //message , address master , address node
    delay(100);
    Serial.println(Mymessage);
    Mymessage = "";
  }
}
void sendMessage(String outgoing, byte MasterNode, byte Node1) {
  LoRa.beginPacket();             // start packet
  LoRa.write(MasterNode);         // add destination address
  LoRa.write(Node1);              // add sender address
  LoRa.write(msgCount);           // add message ID
  LoRa.write(outgoing.length());  // add payload length
  LoRa.print(outgoing);           // add payload
  LoRa.endPacket();               // finish packet and send it
  msgCount++;                     // increment message ID
}

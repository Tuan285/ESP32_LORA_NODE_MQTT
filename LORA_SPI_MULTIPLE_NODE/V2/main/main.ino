#include <SPI.h>  // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <WiFi.h>
extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>


#define WIFI_SSID "NGOC XUAN"
#define WIFI_PASSWORD "01021967"

// #define WIFI_SSID "NGAITIN"
// #define WIFI_PASSWORD "123123123"

#define MQTT_SERVER "test.mosquitto.org"
#define MQTT_PORT 1883

#define MQTT_NODE1_TOPIC_T "MQTT_ESP32_LORA/NODE1/TEMPERATURE"
#define MQTT_NODE1_TOPIC_H "MQTT_ESP32_LORA/NODE1/HUMIDITY"

#define MQTT_NODE2_TOPIC_T "MQTT_ESP32_LORA/NODE2/TEMPERATURE"
#define MQTT_NODE2_TOPIC_H "MQTT_ESP32_LORA/NODE2/HUMIDITY"

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


#define ss 5
#define rst 16
#define dio0 17

// #define ss 5
// #define rst 21
// #define dio0 22

#define D2 4
#define D3 0
#define D4 2
#define buzzer 25

// #define D2 26
// #define D3 27
// #define D4 14
// #define buzzer 15

#define Den2(x) digitalWrite(D2, x)
#define Den3(x) digitalWrite(D3, x)
#define Den4(x) digitalWrite(D4, x)
#define buzz(x) digitalWrite(buzzer, x)
unsigned long
  previousMillis = 0,
  previoussecs = 0,
  currentsecs = 0,
  currentMillis = 0;


byte
  MasterNode = 0xFF,
  Node1 = 0xBB,
  Node2 = 0xCC,
  msgCount = 0;  // count of outgoing messages

String
  h1s,
  t1s,
  fT1s,
  h2s,
  t2s,
  fT2s,
  SenderNode1 = "",
  SenderNode2 = "",
  outgoing,
  incoming = "",
  message = "",
  formattedDate,
  dayStamp = "04-03-23",
  timeStamp = "16:10";
float h1, s_h1,
  t1, s_t1,
  fT1, s_fT1,
  h2, s_h2,
  t2, s_t2,
  fT2, s_fT2;
bool
  ttClearLcd = 0,
  ttBuzzer = 0,
  tt = 0,
  ttSend_node_1 = 0,
  ttSend_node_2 = 0,
  ttSend_node_1_MQTT = 0,
  ttSend_node_2_MQTT = 0,
  ttConnect_Node1 = 0,
  ttConnect_Node2 = 0,
  ttConnectLora = 0;
int
  clr = 0,
  coi = 0,
  countSend = 0,
  checkDevices,
  Secs = 0,
  dataErr = 0,
  checkDevice = 0,
  interval = 1000;

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
void setup() {
  Serial.begin(115200);  // initialize serial
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(buzzer, OUTPUT);
  //---------- LCD -----------------
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, degree);
  // TimerHandle_t xTimerCreate(const char* const pcTimerName,
  //                            TickType_t xTimerPeriodInTicks,
  //                            UBaseType_t uxAutoReload,
  //                            void* pvTimerID,
  //                            TimerCallbackFunction_t pxCallbackFunction);
  mqttReconnectTimer = xTimerCreate("mqttTimer",                                                // name description
                                    pdMS_TO_TICKS(2000),                                        // time callback funtion
                                    pdFALSE,                                                    // auto reload
                                    (void*)0,                                                   // timer ID
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));  // funtion callback
  wifiReconnectTimer = xTimerCreate("wifiTimer",
                                    pdMS_TO_TICKS(2000),
                                    pdFALSE,
                                    (void*)0,
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
  connectToWifi();

  while (!Serial)
    ;
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    //ttConnectLora = 0;
    while (1)
      ;
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Master Node");
  lcd.clear();
  //ttConnectLora = 1;
}
void loop() {
  //-------------- Send Message--------------
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;
    countSend++;
    clr++;
    coi++;

    if (countSend >= 1 && countSend < 5) {
      message = "10";
      sendMessage(message, MasterNode, Node1);
      ttSend_node_1 = 1;
      ttSend_node_2 = 0;
      ttSend_node_1_MQTT = 0;
      ttSend_node_2_MQTT = 0;
      ttConnect_Node1 = 1;
    }
    if (countSend >= 5 && countSend < 10) {
      message = "20";
      sendMessage(message, MasterNode, Node1);
      ttSend_node_1 = 0;
      ttSend_node_2 = 1;
      ttSend_node_2_MQTT = 0;
      ttSend_node_1_MQTT = 0;
      ttConnect_Node2 = 1;
    }
    if (countSend == 5) {
      // Publish an MQTT message on topic MQTT_ESP32_LORA/NODE1/TEMPERATURE
      uint16_t packetIdPub1 = mqttClient.publish(MQTT_NODE1_TOPIC_T, 1, true, String(t1).c_str());
      //Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_NODE1_TOPIC_T, packetIdPub1);
      //Serial.printf("Message: %.2f \n", t1);

      // Publish an MQTT message on topic MQTT_ESP32_LORA/NODE1/HUMIDITY
      uint16_t packetIdPub2 = mqttClient.publish(MQTT_NODE1_TOPIC_H, 1, true, String(h1).c_str());
      //Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_NODE1_TOPIC_H, packetIdPub2);
      //Serial.printf("Message: %.2f \n", h1);
      ttSend_node_1_MQTT = 1;
    }
    if (countSend == 10) {
      //Publish an MQTT message on topic MQTT_ESP32_LORA/NODE2/TEMPERATURE
      uint16_t packetIdPub3 = mqttClient.publish(MQTT_NODE2_TOPIC_T, 1, true, String(t2).c_str());
      //Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_NODE2_TOPIC_T, packetIdPub3);
      //Serial.printf("Message: %.2f \n", t2);

      // Publish an MQTT message on topic MQTT_ESP32_LORA/NODE2/HUMIDITY
      uint16_t packetIdPub4 = mqttClient.publish(MQTT_NODE2_TOPIC_H, 1, true, String(h2).c_str());
      //Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_NODE2_TOPIC_H, packetIdPub4);
      //Serial.printf("Message: %.2f \n", h2);
      ttSend_node_2_MQTT = 1;
    }

    if (countSend > 10) {
      ttSend_node_1 = 0;
      ttSend_node_2 = 0;
      countSend = 0;
      ttSend_node_1_MQTT = 0;
      ttSend_node_2_MQTT = 0;
    }
    // if (clr == 2) {
    //   ttClearLcd = 1;
    //   clr = 0;
    // }
    if (coi >= 2 && t1 >= 36) {
      coi = 0;
      ttBuzzer = !ttBuzzer;
    }
    if (t1 < 36) {
      ttBuzzer = 0;
    }
  }
  onReceive(LoRa.parsePacket());
  if (ttConnect_Node1 == 1) {
    if (SenderNode1 != "Node1" || SenderNode1 == "") {
      checkDevice = 1;
      SenderNode1 = "";
    }
  }
  if (ttConnect_Node2 == 1) {
    if (SenderNode2 != "Node2" || SenderNode2 == "") {
      checkDevice = 2;
      SenderNode2 = "";
    }
  }
  if (SenderNode1 == "" && SenderNode2 == "") {
    checkDevice = 3;
  } else {
    checkDevice = 0;
  }
  print_data_to_lcd();
  //Serial.println(LoRa.parsePacket());
  Den3(ttSend_node_1);
  Den4(ttSend_node_2);
}

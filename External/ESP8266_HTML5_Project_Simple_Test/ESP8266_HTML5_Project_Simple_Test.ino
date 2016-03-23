#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <EEPROM.h>

//const char* ssid     = "NAT.WRTNODE";
//const char* password = "devicenetwork";
const char* ssid     = "Slow Net";
const char* password = "0842216218";

#define APPID   "Chuannatcha"
#define KEY     "pqy4O0n8320qlNR"
#define SECRET  "y0nqtVNxXVEiCZXyVJ0QY8IXo"
#define ALIAS   "ESP8266_HTML5"

#define LEDPin 2

WiFiClient client;
AuthClient *authclient;

MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();
  if(strState[0] == '1')
  {
    digitalWrite(LEDPin, LOW);
    Serial.println("ON");
  }
  else
  {
    digitalWrite(LEDPin, HIGH);
    Serial.println("OFF");
  }
/*
  String stateStr = String(strState).substring(0, msglen);
  if (stateStr == "OPEN") {
    digitalWrite(relayPin, LOW);
    microgear.chat("controllerplug", "OPEN");
    Serial.println("Accepted 'ON'");
  } else if (stateStr == "CLOSE") {
    digitalWrite(relayPin, HIGH);
    microgear.chat("controllerplug", "CLOSE");
    Serial.println("Accepted 'OFF'");
  }
*/
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("command");
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting...");

  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);

  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  if (WiFi.begin(ssid, password)) {

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //uncomment the line below if you want to reset token -->
    microgear.resetToken();
    microgear.init(KEY, SECRET, ALIAS);
    microgear.connect(APPID);
  }
}


void loop() {
  if (microgear.connected()) {
    microgear.loop();
    //Serial.println("connect...");
  } else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
}

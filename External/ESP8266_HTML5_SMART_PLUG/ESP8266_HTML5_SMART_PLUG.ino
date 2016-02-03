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
#define KEY     "l4Pw1hd7iLEdJgD"
#define SECRET  "U4iI9Y6vMX2jG8byiqppP77umskulu"
#define SCOPE   "ESP8266_NO1"

WiFiClient client;
AuthClient *authclient;

int relayPin = 15;

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
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("pieplug");
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting...");

  pinMode(relayPin, OUTPUT);

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
    microgear.init(KEY, SECRET, SCOPE);
    microgear.connect(APPID);
  }
}


void loop() {
  if (microgear.connected()) {
    microgear.loop();
    Serial.println("connect...");
  } else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(1000);
}

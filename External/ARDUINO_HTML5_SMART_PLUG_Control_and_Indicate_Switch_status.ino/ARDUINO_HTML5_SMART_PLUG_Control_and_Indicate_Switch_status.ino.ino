#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
//#include <EEPROM.h>

#define APPID   "Chuannatcha"
#define KEY     "l4Pw1hd7iLEdJgD"
#define SECRET  "U4iI9Y6vMX2jG8byiqppP77umskulu"
#define ALIAS   "anything"
#define SCOPE   "Arduino_Ethernet"

EthernetClient client;
AuthClient *authclient;

int relayPin = 2;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xBD, 0xAC };
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
  } else if (stateStr == "CLOSE") {
    digitalWrite(relayPin, HIGH);
    microgear.chat("controllerplug", "CLOSE");
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

    if (Ethernet.begin(mac)) {
      Serial.println(Ethernet.localIP());
      microgear.resetToken();
      microgear.init(KEY,SECRET,ALIAS);
      microgear.connect(APPID);
      Serial.println("Initial Success");
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

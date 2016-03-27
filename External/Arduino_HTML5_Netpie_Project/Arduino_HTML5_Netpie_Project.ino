#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <MicroGear.h>
#include <ArduinoJson.h>

#define APPID   "Chuannatcha"
#define KEY     "h0RCXwe863KbaTG"
#define SECRET  "KRmdMV1WBagOTn27USkn0dcDo"
#define ALIAS   "Arduino_Main"
#define SCOPE       ""

#define intervalTime 2500
#define StatusPin 2

char A2Wmsg[50],W2Amsg[50];
unsigned long timer = 0, lastMsg = 0;
unsigned char num =0;
boolean button, st = false;
byte mac[] = { 0x00, 0xCD, 0x12, 0xEC, 0x2D, 0x48 };

EthernetClient client;
AuthClient *authclient;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* inmsg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  
  char* text;
  text = (char*) malloc(msglen + 1);
  memcpy(text, inmsg, msglen);
  text[msglen] = '\0';
  strcpy(W2Amsg, text);

  Serial.println(W2Amsg);
  Serial.println(msglen);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("command");
}

void Generate_message()
{
  StaticJsonBuffer<200> TxBuffer;
  JsonObject& Tx = TxBuffer.createObject();
  Tx["Mode"] = "Saving";
  Tx["A1"] = st;
  //Tx["millis"] = millis();
  strcpy(A2Wmsg, "");
  Tx.printTo(A2Wmsg, sizeof(A2Wmsg));
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  pinMode(StatusPin, INPUT_PULLUP);

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);

  if (Ethernet.begin(mac)) {
    Serial.println(Ethernet.localIP());
    microgear.resetToken();
    microgear.init(KEY, SECRET, ALIAS);
    microgear.connect(APPID);
  }
}

void loop() {

  button = digitalRead(StatusPin);
  if(button == false)
  {
    while(!digitalRead(StatusPin))
      delay(10);
    Generate_message();
    Serial.print("Publish message: ");
    Serial.println(A2Wmsg);
    microgear.chat("Status", A2Wmsg);
    st = !st;
  }
  
  if (microgear.connected()) {
    microgear.loop();
  }
  else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  delay(100);
}

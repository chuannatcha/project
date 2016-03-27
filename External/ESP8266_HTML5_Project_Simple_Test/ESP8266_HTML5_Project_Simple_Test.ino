#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
//#include <EEPROM.h>


//const char* ssid     = "ESPERT-002";
//const char* password = "espertap";
const char* ssid     = "Slow Net";
const char* password = "0842216218";

#define APPID   "Chuannatcha"
#define KEY     "h0RCXwe863KbaTG"
#define SECRET  "KRmdMV1WBagOTn27USkn0dcDo"
#define ALIAS   "ESP8266_HTML5"

#define intervalTime 2500
#define LEDPin 2
#define StatusPin 0

unsigned long lastMsg = 0;
unsigned char num =0;
char A2Wmsg[50],W2Amsg[50];
boolean button, st = false;

WiFiClient client;
AuthClient *authclient;

MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* payload, unsigned int length) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  
  char* text;
  text = (char*) malloc(length + 1);
  memcpy(text, payload, length);
  text[length] = '\0';
  strcpy(W2Amsg, text);

  Serial.println(W2Amsg);
  Serial.println(length);
  
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

void Generate_message()
{
  StaticJsonBuffer<200> TxBuffer;
  JsonObject& Tx = TxBuffer.createObject();
  Tx["Mode"] = "Saving";
  Tx["A1"] = st;
  //Tx["signal_level"] = WiFi.RSSI();
  //Tx["free_heap"] = ESP.getFreeHeap();
  //Tx["millis"] = millis();
  strcpy(A2Wmsg, "");
  Tx.printTo(A2Wmsg, sizeof(A2Wmsg));
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting...");

  pinMode(LEDPin, OUTPUT);
  pinMode(StatusPin, INPUT_PULLUP);
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
  /*
  long now = millis();
  if (now - lastMsg > intervalTime) {
    lastMsg = now;

    Generate_message();
    Serial.print("Publish message: ");
    Serial.println(A2Wmsg);
    microgear.chat("Status", A2Wmsg);

    digitalWrite(LEDPin, LOW);
    delayMicroseconds(200);
    digitalWrite(LEDPin, HIGH);

    num++;
  }
  */
}

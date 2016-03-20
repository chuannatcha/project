#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define RemoteRelayPinL1  12
#define SavingRelayPinL1  16
#define RemoteRelayPinL2  12
#define SavingRelayPinL2  16
#define RemoteRelayPinL3  12
#define SavingRelayPinL3  16
#define LEDPin          2
#define intervalTime    1000

// Update these with values suitable for your network.
const char* ssid = "OpenWrt";
const char* password = "0842216218";
IPAddress ip_server(192, 168, 100, 1);

const char* ClientID = "ESP8266_Light";
const char* inTopic = "Light";
const char* outTopic = "Status";

unsigned long lastMsg = 0;
char C2Mmsg[100], M2Cmsg[100];

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] -> ");

  char* text;
  text = (char*) malloc(length + 1);
  memcpy(text, payload, length);
  text[length] = '\0';

  strcpy(M2Cmsg, "");
  strcpy(M2Cmsg, text);

  Serial.println(M2Cmsg);

  DynamicJsonBuffer RxBuffer;
  JsonObject& Rx = RxBuffer.parseObject(M2Cmsg);
  if (!Rx.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  const char* Rname = Rx["Rname"];
  char mode = Rx["mode"];
  boolean Rlogic = Rx["Rlogic"];
  Serial.println(Rname);
  Serial.println(mode);
  Serial.println(Rlogic);

  if (!strcmp(Rname, "L1"))
  {
    if (mode == 'R')
    {
      digitalWrite(RemoteRelayPinL1, !Rlogic);
      Serial.println("Remote Pin L1 has changed");
    }
    else if (mode == 'S')
    {
      digitalWrite(SavingRelayPinL1, !Rlogic);
      Serial.println("Saving Pin L1 has changed");
    }
  }
  else if (!strcmp(Rname, "L2"))
  {
    if (mode == 'R')
    {
      digitalWrite(RemoteRelayPinL2, !Rlogic);
      Serial.println("Remote Pin L2 has changed");
    }
    else if (mode == 'S')
    {
      digitalWrite(SavingRelayPinL2, !Rlogic);
      Serial.println("Saving Pin L2 has changed");
    }
  }
  else if (!strcmp(Rname, "L3"))
  {
    if (mode == 'R')
    {
      digitalWrite(RemoteRelayPinL3, !Rlogic);
      Serial.println("Remote Pin L3 has changed");
    }
    else if (mode == 'S')
    {
      digitalWrite(SavingRelayPinL3, !Rlogic);
      Serial.println("Saving Pin L3 has changed");
    }
  }
  free(text);
  //if ((char)payload[0] == '1') {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ClientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Light Connected");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void Generate_message()
{
  //boolean st = digitalRead(StatusPin);
  //snprintf (msg, 75, "M1#%d", st);

  //StaticJsonBuffer<200> jsonBuffer;
  DynamicJsonBuffer TxBuffer;
  JsonObject& Tx = TxBuffer.createObject();
  Tx["Tname"] = "A1";
  Tx["Tlogic"] = 0;//digitalRead(StatusPin);
  Tx["signal_level"] = WiFi.RSSI();
  Tx["free_heap"] = ESP.getFreeHeap();
  Tx["millis"] = millis();
  strcpy(C2Mmsg, "");
  Tx.printTo(C2Mmsg, sizeof(C2Mmsg));
}

void setup() {
  pinMode(RemoteRelayPinL1, OUTPUT);
  pinMode(SavingRelayPinL1, OUTPUT);
  pinMode(RemoteRelayPinL2, OUTPUT);
  pinMode(SavingRelayPinL2, OUTPUT);
  pinMode(RemoteRelayPinL3, OUTPUT);
  pinMode(SavingRelayPinL3, OUTPUT);
  pinMode(LEDPin, OUTPUT);
  digitalWrite(RemoteRelayPinL1, HIGH);
  digitalWrite(SavingRelayPinL1, HIGH);
  digitalWrite(RemoteRelayPinL2, HIGH);
  digitalWrite(SavingRelayPinL2, HIGH);
  digitalWrite(RemoteRelayPinL3, HIGH);
  digitalWrite(SavingRelayPinL3, HIGH);
  digitalWrite(LEDPin, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(ip_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > intervalTime) {
    lastMsg = now;
    digitalWrite(LEDPin, LOW);
    delayMicroseconds(200);
    digitalWrite(LEDPin, HIGH);
  }
}

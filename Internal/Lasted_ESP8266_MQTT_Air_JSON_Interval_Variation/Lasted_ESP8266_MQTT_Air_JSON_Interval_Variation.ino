#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define RemoteRelayPin  12
#define SavingRelayPin  16
#define StatusPin       5
#define LEDPin          2

// Update these with values suitable for your network.
const char* ssid = "LASCS";
const char* password = "0842216218";
IPAddress ip_server(192, 168, 100, 1);

const char* ClientID = "ESP8266_Air1";
const char* inTopic = "Air1";
const char* outTopic = "Status";

unsigned long lastMsg = 0;
unsigned int intervalTime = 1000;
char C2Mmsg[200], M2Cmsg[200];
boolean st,RR1,SR1;

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

  strcpy(M2Cmsg,"");
  strcpy(M2Cmsg, text);

  Serial.println(M2Cmsg);
  
  StaticJsonBuffer<200> RxBuffer;
  JsonObject& Rx = RxBuffer.parseObject(M2Cmsg);
  if (!Rx.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  const char* Rname = Rx["Rname"];
  char mode = Rx["mode"];
  boolean Rlogic = Rx["Rlogic"];
  unsigned int Time = Rx["Time"];
  Serial.println(Rname);
  Serial.println(mode);
  Serial.println(Rlogic);
  Serial.println(Time);

  if(!strcmp(Rname,"A1"))
  {
    if(mode == 'R')
    {
      digitalWrite(RemoteRelayPin,!Rlogic);
      Serial.println("Remote Pin has changed");
    }
    else if(mode == 'S')
    {
      digitalWrite(SavingRelayPin,!Rlogic);
      Serial.println("Saving Pin has changed");
    }
  }

  if(Time != 0)
  {
    intervalTime = Time;
    Serial.println("Time Changed");
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
      client.publish(outTopic, "Air1 Connected");
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

  st = !digitalRead(StatusPin);
  RR1 = !digitalRead(RemoteRelayPin);
  SR1 = !digitalRead(SavingRelayPin);
  
  StaticJsonBuffer<200> TxBuffer;
  JsonObject& Tx = TxBuffer.createObject();
  Tx["Tname"] = "A1";
  Tx["Tlogic"] = st;
  Tx["RR1"] = RR1;
  Tx["SR1"] = SR1;
  Tx["signal_level"] = WiFi.RSSI();
  Tx["free_heap"] = ESP.getFreeHeap();
  Tx["millis"] = millis();
  strcpy(C2Mmsg, "");
  Tx.printTo(C2Mmsg, sizeof(C2Mmsg));
}

void setup() {
  pinMode(RemoteRelayPin, OUTPUT);
  pinMode(SavingRelayPin, OUTPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(StatusPin, INPUT);
  digitalWrite(RemoteRelayPin, HIGH);
  digitalWrite(SavingRelayPin, HIGH);
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

    Generate_message();
    Serial.print("Publish message: ");
    Serial.println(C2Mmsg);
    client.publish(outTopic, C2Mmsg);

    digitalWrite(LEDPin, LOW);
    delayMicroseconds(200);
    digitalWrite(LEDPin, HIGH);

  }
}

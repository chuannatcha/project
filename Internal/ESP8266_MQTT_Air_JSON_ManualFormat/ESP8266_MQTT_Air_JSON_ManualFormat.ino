#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define RemoteRelayPin  12
#define SavingRelayPin  16
#define StatusPin       5
#define LEDPin          2
#define intervalTime    1000

// Update these with values suitable for your network.
const char* ssid = "OpenWrt";
const char* password = "0842216218";
IPAddress ip_server(192, 168, 100, 1);

const char* ClientID = "ESP8266_Air1";
const char* inTopic = "Air1";
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
  Serial.print("] : ");

  char* text;
  text = (char*) malloc(length + 1);
  memcpy(text, payload, length);
  text[length] = '\0';

  strcpy(M2Cmsg,"");
  strcpy(M2Cmsg, text);

  Serial.println(M2Cmsg);

  if(M2Cmsg[0] == 'A' && M2Cmsg[1] == '1')
  {
    if(M2Cmsg[3] == 'R')
    {
      digitalWrite(RemoteRelayPin,M2Cmsg[5]-48);
      Serial.println("Remote Pin has changed");
    }
    else if(M2Cmsg[3] == 'S')
    {
      digitalWrite(SavingRelayPin,M2Cmsg[5]-48);
      Serial.println("Saving Pin has changed");
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

  //StaticJsonBuffer<200> jsonBuffer;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["name"] = "A1";
  root["status"] = digitalRead(StatusPin);
  root["signal_level"] = WiFi.RSSI();
  root["free_heap"] = ESP.getFreeHeap();
  root["millis"] = millis();
  strcpy(C2Mmsg, "");
  root.printTo(C2Mmsg, sizeof(C2Mmsg));
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

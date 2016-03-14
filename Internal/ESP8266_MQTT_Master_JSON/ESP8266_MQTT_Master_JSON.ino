#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LEDPin 2
#define A2 0
#define A1 1
#define L1 2
#define L2 3
#define L3 4
#define M1 5
#define M2 6
#define M3 7

// Update these with values suitable for your network.
const char* ssid = "OpenWrt";
const char* password = "0842216218";
IPAddress ip_server(192, 168, 100, 1);

const char* ClientID = "ESP8266_Master";
const char* inTopic = "Status";
const char* outTopic = "outTopic";
const char* Air1 = "Air1";
const char* Light = "Light";
const char* Motion1 = "Motion1";

char in_msg[100];
boolean RemoteState = 0, SavingState = 0;
boolean Status[] = {false, false, false, false, false, false, false, false};
//Type            A2    A1    L1    L2    L3    M1    M2    M3
//Index           0     1     2     3     4     5     6     7

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
  strcpy(in_msg, text);
  Serial.println(in_msg);
  Parse_message();

  for (int x = 0; x < 8; x++)
    Serial.print(Status[x]);
  Serial.println("");

  free(text);
  Serial.println(ESP.getFreeHeap());

  //Serial.println((char)payload[length-1]);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ClientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "hello world");
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

void Parse_message()
{
  DynamicJsonBuffer  jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(in_msg);
  const char* name = root["name"];
  long status = root["status"];
  Serial.println(name);
  Serial.println(status);

  if (!strcmp(name,"A1"))
    Status[A1] = status;
  else if (!strcmp(name,"L1"))
    Status[L1] = status;
  else if (!strcmp(name,"L2"))
    Status[L2] = status;
  else if (!strcmp(name,"L3"))
    Status[L3] = status;
  else if (!strcmp(name,"M1"))
    Status[M1] = status;
  else if (!strcmp(name,"M2"))
    Status[M2] = status;
  else if (!strcmp(name,"M3"))
    Status[M3] = status;
}

void setup() {
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(ip_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
    reconnect();
  client.loop();

}

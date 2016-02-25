#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define StatusPin       5
#define LEDPin          2

// Update these with values suitable for your network.
const char* ssid = "OpenWrt";
const char* password = "0842216218";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress ip_server(192, 168, 100, 1);

const char* ClientID = "ESP8266-Motion4";
const char* inTopic = "Motion4";
const char* outTopic = "Status";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

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
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266_Client1")) {
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

void setup() {
  pinMode(LEDPin, OUTPUT);
  pinMode(StatusPin, INPUT);
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
  if (now - lastMsg > 1000) {
    lastMsg = now;
    boolean st = digitalRead(StatusPin);
    snprintf (msg, 75, "M1#%d", st);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(outTopic, msg);
    digitalWrite(LEDPin, LOW);
    delay(10);
    digitalWrite(LEDPin, HIGH);
  }
}

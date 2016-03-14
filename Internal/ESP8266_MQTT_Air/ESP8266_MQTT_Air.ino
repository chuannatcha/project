#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define RemoteRelayPin  12
#define SavingRelayPin  16
#define StatusPin       5
#define LEDPin          2

// Update these with values suitable for your network.
const char* ssid = "OpenWrt";
const char* password = "0842216218";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress ip_server(192, 168, 100, 1);

<<<<<<< HEAD
const char* ClientName = "Motion_1";
const char* inTopic = "Mot1_Command";
=======
const char* ClientID = "ESP-Air1";
const char* inTopic = "Air1_Command";
>>>>>>> project/master
const char* outTopic = "Status";
const unsigned int IntervalTime = 2500;

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
    digitalWrite(RemoteRelayPin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("Remote Relay --> ON");
  }
  else if ((char)payload[0] == '2') {
    digitalWrite(RemoteRelayPin, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("Remote Relay --> OFF");
  }
  else if ((char)payload[0] == '3') {
    digitalWrite(SavingRelayPin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("Saving Relay --> ON");
  }
  else if ((char)payload[0] == '4'){
    digitalWrite(SavingRelayPin, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("Saving Relay --> OFF");
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
<<<<<<< HEAD
    if (client.connect(ClientName)) {
=======
    if (client.connect(ClientID)) {
>>>>>>> project/master
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
  if (now - lastMsg > IntervalTime) {
    lastMsg = now;
    boolean st = digitalRead(StatusPin);
    snprintf (msg, 75, "M1#%d", st);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(outTopic, msg);
    digitalWrite(LEDPin, LOW);
    delay(1);
    digitalWrite(LEDPin, HIGH);
  }
}

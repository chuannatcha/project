#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define ShowLED 2
#define OnBtn 5
#define OffBtn 0

// Update these with values suitable for your network.
const char* ssid = "OpenWrt";
const char* password = "0842216218";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress ip_server(192,168,100,1);

const char* inTopic = "Status";
const char* Air1 = "Air1_Command";
const char* Air2 = "Air2_Command";

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
  /*
  char* text;
  text = (char*) malloc(length + 1);
  memcpy(text, payload, length);
  text[length] = '\0';
  Serial.println(text);
  */
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  Serial.println((char)payload[length-1]);
  if ((char)payload[length-1] == '1') {
    digitalWrite(ShowLED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(ShowLED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266_Host")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(outTopic, "hello world");
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
  pinMode(ShowLED, OUTPUT);
  pinMode(OnBtn, INPUT_PULLUP);
  pinMode(OffBtn, INPUT_PULLUP);
  Serial.begin(115200);
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

  if(!digitalRead(OnBtn))
  {
    client.publish(Air1, "1");
    Serial.println("Send On Command");
    while(!digitalRead(OnBtn))
      delay(100);
  }
  else if(!digitalRead(OffBtn))
  {
    client.publish(Air1, "0");
    Serial.println("Send Off Command");
    while(!digitalRead(OffBtn))
      delay(100);
  }
}

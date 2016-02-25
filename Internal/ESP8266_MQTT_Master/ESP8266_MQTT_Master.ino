#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LEDPin 2
#define OnBtn 5
#define OffBtn 0
#define LED1 16
#define LED2 14
#define LED3 12
#define LED4 13

// Update these with values suitable for your network.
const char* ssid = "OpenWrt";
const char* password = "0842216218";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress ip_server(192, 168, 100, 1);

const char* inTopic = "Status";
const char* Air1 = "Air1_Command";
const char* Air2 = "Air2_Command";

boolean RemoteState = 0, SavingState = 0;

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
  Serial.print(" millis : ");
  Serial.println(millis());
  // Switch on the LED if an 1 was received as first character
  //Serial.println((char)payload[length-1]);

  if ((char)payload[length-3] == '1' && (char)payload[length-1] == '1')
    digitalWrite(LED1, LOW);
  else if ((char)payload[length-3] == '1' && (char)payload[length-1] == '0')
    digitalWrite(LED1, HIGH);

  else if ((char)payload[length-3] == '2' && (char)payload[length-1] == '1')
    digitalWrite(LED2, LOW);
  else if ((char)payload[length-3] == '2' && (char)payload[length-1] == '0')
    digitalWrite(LED2, HIGH);

  else if ((char)payload[length-3] == '3' && (char)payload[length-1] == '1')
    digitalWrite(LED3, LOW);
  else if ((char)payload[length-3] == '3' && (char)payload[length-1] == '0')
    digitalWrite(LED3, HIGH);

  else if ((char)payload[length-3] == '4' && (char)payload[length-1] == '1')
    digitalWrite(LED4, LOW);
  else if ((char)payload[length-3] == '4' && (char)payload[length-1] == '0')
    digitalWrite(LED4, HIGH);
    
  //if ((char)payload[length - 1] == '1') {
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
  pinMode(LEDPin, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(OnBtn, INPUT_PULLUP);
  pinMode(OffBtn, INPUT_PULLUP);
  digitalWrite(LEDPin, HIGH);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
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

  if (!digitalRead(OnBtn))
  {
    while (!digitalRead(OnBtn))
      delay(100);
    if (RemoteState == 0) {
      client.publish(Air1, "1");
      Serial.println("Send Remote ON");
    }
    else {
      client.publish(Air1, "2");
      Serial.println("Send Remote OFF");
    }
    RemoteState = !RemoteState;
  }
  else if (!digitalRead(OffBtn))
  {
    while (!digitalRead(OffBtn))
      delay(100);
    if (SavingState == 0) {
      client.publish(Air1, "3");
      Serial.println("Send Saving ON");
    }
    else {
      client.publish(Air1, "4");
      Serial.println("Send Saving OFF");
    }
    SavingState = !SavingState;
  }
}

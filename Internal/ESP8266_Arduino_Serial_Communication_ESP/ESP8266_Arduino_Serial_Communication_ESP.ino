#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED 2
#define data_length 7

String incomingString = "", check = "", dataR = "";         // a string to hold incoming data
boolean incomingComplete = false;  // whether the string is complete
char data[20], leng;
int chksum_msg, chksum_result;

const char* ssid = "OpenWrt";
const char* password = "0842216218";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress ip_server(192, 168, 100, 1);

const char* ClientID = "ESP-Master";
const char* inTopic = "Status";
const char* outTopic = "Command";

boolean RemoteState = 0, SavingState = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
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

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    incomingString += inChar;
    digitalWrite(LED,LOW);
    delay(1);
    digitalWrite(LED,HIGH);
    if (inChar == '\n') {
      incomingComplete = true;
    }
  }
}

void CheckSum()
{
  //
}

void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(ip_server, 1883);
  client.setCallback(callback);
  incomingString.reserve(20);
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();
  
  if (incomingComplete) {
    Serial.print("Incoming : ");
    Serial.print(incomingString);

    dataR = incomingString.substring(0,data_length);
    //const char* Msg = incomingString.c_str();
    dataR.toCharArray(data, 20);

    leng = strlen(incomingString.c_str());
    check = incomingString.substring(leng-5,leng-2);
    chksum_msg = check.toInt();

    chksum_result = 0;
    for(int x=0; x < strlen(data) ; x++) //-6
    {
      chksum_result ^= data[x];
    }
    Serial.print("Check Sum Data   : ");
    Serial.println(chksum_msg);
    Serial.print("Check Sum Result : ");
    Serial.println(chksum_result);
    
    Serial.println("");
    
    //strcpy(checksum, incomingString.substring(strlen(Msg)-2,strlen(Msg)-1));
    //snprintf (msg, 50, "%s", incomingString.c_str());
    //strcpy(msg, incomingString.c_str());
    //client.publish(outTopic,incomingString.c_str());
    
    digitalWrite(LED,LOW);
    delay(1);
    digitalWrite(LED,HIGH);
    // clear the string:
    incomingString = "";
    check = "";
    dataR = "";
    incomingComplete = false;
  }
  serialEvent();
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */

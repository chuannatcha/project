#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define intervalTime 2500
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

String A2Minput = "";
boolean A2Mcomplete = false;
char C2Mmsg[100], M2Cmsg[100], M2Amsg[20], A2Mmsg[20];

char M2Ctype[2] = {'A', '1'};
char M2Cmode = 'R';
boolean M2Clogic = 0;

unsigned char M2Achecksum, A2Mchecksum;
unsigned long lastMsg = 0;
boolean Tstatus[] = {false, false, false, false, false, false, false, false};
      //Type            A2     A1     L1     L2     L3     M1     M2     M3
      //Index            0      1      2      3      4      5      6      7

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] -> ");

  char* text;
  text = (char*) malloc(length + 1);
  memcpy(text, payload, length);
  text[length] = '\0';
  strcpy(C2Mmsg, text);
  //Serial.println(C2Mmsg);
  Parse_message();
/*
  for (int x = 0; x < 8; x++)
    Serial.print(Tstatus[x]);
  Serial.println("");
*/
  free(text);
  //Serial.println(ESP.getFreeHeap());

  //Serial.println((char)payload[length-1]);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ClientID)) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "hello world");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void Parse_message()
{
  DynamicJsonBuffer RxBuffer;
  JsonObject& Rx = RxBuffer.parseObject(C2Mmsg);
  if (!Rx.success()) {
    //Serial.println("parseObject() failed");
    return;
  }
  const char* Tname = Rx["Tname"];
  boolean Tlogic = Rx["Tlogic"];
  //Serial.println(Tname);
  //Serial.println(Tlogic);

  if (!strcmp(Tname, "A1"))
    Tstatus[A1] = Tlogic;
  else if (!strcmp(Tname, "L1"))
    Tstatus[L1] = Tlogic;
  else if (!strcmp(Tname, "L2"))
    Tstatus[L2] = Tlogic;
  else if (!strcmp(Tname, "L3"))
    Tstatus[L3] = Tlogic;
  else if (!strcmp(Tname, "M1"))
    Tstatus[M1] = Tlogic;
  else if (!strcmp(Tname, "M2"))
    Tstatus[M2] = Tlogic;
  else if (!strcmp(Tname, "M3"))
    Tstatus[M3] = Tlogic;
}

void Send_M2C()
{
  DynamicJsonBuffer TxBuffer;
  JsonObject& Tx = TxBuffer.createObject();
  Tx["Rname"] = M2Ctype;
  Tx["mode"] = M2Cmode;
  Tx["Rlogic"] = M2Clogic;
  Tx["FreeMem"] = ESP.getFreeHeap();
  strcpy(M2Cmsg,"");
  Tx.printTo(M2Cmsg, sizeof(M2Cmsg));
  
  if (M2Ctype[0] == 'A')
    client.publish(Air1, M2Cmsg);
  else if (M2Ctype[0] == 'L')
    client.publish(Light, M2Cmsg);
  //Serial.println(M2Cmsg);
  //Serial.println(strlen(M2Cmsg));
}

void Send_M2A()
{
  strcpy(M2Amsg,"");
  M2Achecksum = 0;
  for (char x = 0; x < 8; x++)
  {
    M2Amsg[x] = Tstatus[x] + 48;
    M2Achecksum += Tstatus[x];
  }
  M2Amsg[8] = '\0';
  snprintf(M2Amsg, 20, "%s#%01d", M2Amsg, M2Achecksum);
  Serial.println(M2Amsg);
}

void serialEvent() {
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    A2Minput += inChar;
    if (inChar == '\n')
    {
      A2Mcomplete = true;
    }
  }
  if (A2Mcomplete)
  {
    Parse_Serial();
    A2Minput = "";
    A2Mcomplete = false;
  }
}

void Parse_Serial()
{
  unsigned char ChksumPosition = A2Minput.indexOf('#');
  String Chksum = A2Minput.substring(ChksumPosition + 1, ChksumPosition + 4);
  unsigned char checksum = Chksum.toInt();

  A2Minput = A2Minput.substring(0, ChksumPosition);
  A2Minput.toCharArray(A2Mmsg, 20);

  A2Mchecksum = 0;
  for (char x = 0; x < ChksumPosition; x++)
  {
    A2Mchecksum ^= A2Mmsg[x];
  }
  //Serial.println(A2Mmsg);
  //Serial.println(checksum);
  //Serial.println(A2Mchecksum);

  if (checksum == A2Mchecksum)
  {
    //Serial.println("Checksum OK");
    M2Ctype[0] = A2Mmsg[0];
    M2Ctype[1] = A2Mmsg[1];
    M2Cmode = A2Mmsg[3];
    M2Clogic = A2Mmsg[5]-48;
    Send_M2C();
  }
}

void setup() {
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);
  Serial.begin(9600);
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

  serialEvent();
  
  long now = millis();
  if (now - lastMsg > intervalTime) {
    lastMsg = now;

    Send_M2A();
    
    digitalWrite(LEDPin, LOW);
    delayMicroseconds(200);
    digitalWrite(LEDPin, HIGH);
  }

}

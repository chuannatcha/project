#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#define LED_Pin 2

const char* ssid = "Linksys";
const char* password = "0842216218";
const char* Main_IP = "192.168.2.200";
IPAddress Local_IP(192,168,2,203);
IPAddress Subnet(255,255,255,0);
IPAddress Gateway(192,168,2,1);

unsigned int localPort = 8888; // the destination port

char message[2];
char data[2];
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged"; // a string to send back

WiFiUDP UDP;

void get_UDP();
void compare_data();

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting...");
  WiFi.mode(WIFI_STA);
  WiFi.config(Local_IP, Gateway, Subnet);
  if(WiFi.begin(ssid, password)) 
  {
    while(WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  while(!UDP.begin(localPort)) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connection successful");
  pinMode(LED_Pin,OUTPUT);
}

void loop() 
{/*
  if(Serial.available() > 0)
  {
    if(WiFi.status() == WL_CONNECTED)
    { 
      if(Serial.read() == 'n')
      {
        message[1] = 'n';
        message[0] = '1';
        UDP.beginPacket(Main_IP,localPort);
      }
      else if(Serial.read() == 'f')
      {
        message[1] = 'f';
        message[0] = '0';
        UDP.beginPacket(Main_IP,localPort);
      }
      else goto skip_send;
      //UDP.send(message, ip , port);
      UDP.write(message,sizeof(message));
      UDP.endPacket();
      Serial.println("Sent");
    }
  }
  skip_send:*/
  get_UDP();
  compare_data();
  delay(100);
}

void get_UDP()
{
      int packetSize = UDP.parsePacket();
      if(packetSize)
      {
        // read the packet into packetBufffer
        UDP.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Data: ");
        data[0] = packetBuffer[0];
        data[1] = packetBuffer[1];
        //int value = packetBuffer[0]*10 + packetBuffer[1];
        //Serial.println(value);
        Serial.print(data[1]);
        Serial.println(data[0]);
        
        // send a reply, to the IP address and port that sent us the packet we received
        //UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        //UDP.write(ReplyBuffer);
        //UDP.endPacket();
      }
}

void compare_data()
{
  if(data[1] == 'b')
  {
  if(data[0] == '1')
    digitalWrite(LED_Pin,HIGH);
  else if(data[0] == '0')
    digitalWrite(LED_Pin,LOW);
  }
}


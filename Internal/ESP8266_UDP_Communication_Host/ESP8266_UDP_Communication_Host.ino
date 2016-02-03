#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const char* ssid = "NAT.WRTNODE";
const char* password = "devicenetwork";
//const char* ssid = "Linksys";
//const char* password = "0842216218";

//IPAddress Air1_IP(192,168,0,203);
//IPAddress Air2_IP(192,168,0,250);
const char* Air1_IP = "192.168.2.201";
const char* Air2_IP = "192.168.2.203";
IPAddress Local_IP(192,168,2,200);
IPAddress Subnet(255,255,255,0);
IPAddress Gateway(192,168,2,1);

const uint16_t localPort = 8888; // the destination port

char message[2];
char data[2];
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged"; // a string to send back

WiFiUDP UDP;

void get_UDP();
//void compare_data();

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
}

void loop() 
{
    if(WiFi.status() == WL_CONNECTED)
    { 
      if(Serial.available() > 0)
      { char char_serial = Serial.read();
        if(char_serial == 'a')
        {
          message[1] = 'a';
          message[0] = '1';
          UDP.beginPacket(Air1_IP,localPort);
          Serial.print("a1 ");
        }
        else if(char_serial == 'b')
        {
          message[1] = 'a';
          message[0] = '0';
          UDP.beginPacket(Air1_IP,localPort);
          Serial.print("a0 ");
        }
        else if(char_serial == 'c')
        {
          message[1] = 'b';
          message[0] = '1';
          UDP.beginPacket(Air2_IP,localPort);
          Serial.print("b1 ");
        }
        else if(char_serial == 'd')
        {
          message[1] = 'b';
          message[0] = '0';
          UDP.beginPacket(Air2_IP,localPort);
          Serial.print("b0 ");
        }
        else goto skip_send;
      UDP.write(message,sizeof(message));
      UDP.endPacket();
      Serial.println("Sent");
      }
    }
  skip_send:
  get_UDP();
  //compare_data();
  delay(1000);
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
        
        IPAddress remoteIp = UDP.remoteIP();
        Serial.print("Remote IP :");
        Serial.print(remoteIp);
        Serial.print(":");
        Serial.println(UDP.remotePort());
        // send a reply, to the IP address and port that sent us the packet we received
        //UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        //UDP.write(ReplyBuffer);
        //UDP.endPacket();
      }
}
/*
void compare_data()
{

}
*/

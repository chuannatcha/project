#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const char* ssid = "Slow Net";
const char* password = "0842216218";

const char* ip = "192.168.0.201"; // the remote IP address
unsigned int localPort = 8888; // the destination port

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged"; // a string to send back

WiFiUDP UDP;

void get_UDP();

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting...");
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
      byte message[2] = {3,4};
      //UDP.send(message, ip , port);
      UDP.beginPacket(ip,localPort);
      UDP.write(message,sizeof(message));
      UDP.endPacket();
      Serial.println("Sent");
  }
  get_UDP();
  delay(1000);
  Serial.println("Continue Running");
}

void get_UDP()
{
      int packetSize = UDP.parsePacket();
      if(packetSize)
      {
        // read the packet into packetBufffer
        UDP.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Data: ");
        int value = packetBuffer[0]*10 + packetBuffer[1];
        Serial.println(value);

        // send a reply, to the IP address and port that sent us the packet we received
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        UDP.write(ReplyBuffer);
        UDP.endPacket();
      }
}

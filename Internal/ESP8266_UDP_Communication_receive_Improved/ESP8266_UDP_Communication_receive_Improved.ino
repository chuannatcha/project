#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const char* ssid = "Slow Net";
const char* password = "0842216218";

//const char* ip = "192.168.2.106"; // the remote IP address
unsigned int localPort = 8888; // the destination port

WiFiUDP UDP;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged"; // a string to send back

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

void loop() {
    if(WiFi.status() == WL_CONNECTED)
    {
      // if there’s data available, read a packet
      int packetSize = UDP.parsePacket();
      if(packetSize)
      {
        Serial.println("");
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = UDP.remoteIP();
        for (int i =0; i < 4; i++){
          Serial.print(remote[i], DEC);
          if (i < 3){
            Serial.print(".");
          }
        }
        Serial.print(", port ");
        Serial.println(UDP.remotePort());

        // read the packet into packetBufffer
        UDP.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Contents:");
        int value = packetBuffer[0]*256 + packetBuffer[1];
        Serial.println(value);

        // send a reply, to the IP address and port that sent us the packet we received
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        UDP.write(ReplyBuffer);
        UDP.endPacket();

        // turn LED on or off depending on value recieved
        //digitalWrite(5,value);
      }
    }
    delay(10);
}

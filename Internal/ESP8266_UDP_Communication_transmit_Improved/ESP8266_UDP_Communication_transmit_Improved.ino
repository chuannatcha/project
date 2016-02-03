#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const char* ssid = "Slow Net";
const char* password = "0842216218";

const char* ip = "192.168.0.201"; // the remote IP address
unsigned int localPort = 8888; // the destination port

WiFiUDP UDP;

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
      char message[2];
      message[1] = 'a';
      message[0] = 'b';
      //UDP.send(message, ip , port);
      UDP.beginPacket(ip,localPort);
      UDP.write(message,sizeof(message));
      UDP.endPacket();
      Serial.println("Sent");
  }
  delay(1000);
  Serial.println("Continue Delay");
}

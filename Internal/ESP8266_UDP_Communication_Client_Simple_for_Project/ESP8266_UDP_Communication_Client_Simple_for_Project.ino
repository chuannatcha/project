#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#define Detector 16
#define Para_Relay 2
#define Seri_Relay 5

const char* ssid = "Linksys";
const char* password = "0842216218";
const char* Main_IP = "192.168.2.200";

IPAddress Local_IP(192,168,2,201);
IPAddress Subnet(255,255,255,0);
IPAddress Gateway(192,168,2,1);

unsigned int localPort = 8888; // the destination port

unsigned long last_millis = 0;
char data_out[2];
char data_in[2];
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
//char ReplyBuffer[] = "acknowledged"; // a string to send back

WiFiUDP UDP;

void get_UDP();
void compare_data_in();

void setup() 
{
  pinMode(Detector,INPUT);
  pinMode(Para_Relay,OUTPUT);
  pinMode(Seri_Relay,OUTPUT);
  digitalWrite(Para_Relay,HIGH);
  digitalWrite(Seri_Relay,LOW);
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
  Serial.println("UDP Connection Established");
  data_out[1] = 'z';
}

void loop() 
{
  if(WiFi.status() == WL_CONNECTED)
  { 
    if(abs(millis()-last_millis) > 2500)
    { 
      last_millis = millis();
      if(digitalRead(Detector))
        data_out[0] = '0';
      else
        data_out[0] = '1';
      UDP.beginPacket(Main_IP,localPort);
      UDP.write(data_out,sizeof(data_out));
      UDP.endPacket();
      Serial.println("Sent");
     }
  }  
  get_UDP();
  delay(10);
}

void get_UDP()
{
      int packetSize = UDP.parsePacket();
      if(packetSize)
      {
        // read the packet into packetBufffer
        UDP.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
        Serial.println("data_in: ");
        data_in[0] = packetBuffer[0];
        data_in[1] = packetBuffer[1];
        //int value = packetBuffer[0]*10 + packetBuffer[1];
        //Serial.println(value);
        Serial.print(data_in[1]);
        Serial.println(data_in[0]);
        
        // send a reply, to the IP address and port that sent us the packet we received
        //UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        //UDP.write(ReplyBuffer);
        //UDP.endPacket();

        compare_data_in();
      }
}

void compare_data_in()
{
  if(data_in[1] == 'a')
  {
    if(data_in[0] == '1')
    {
      digitalWrite(Seri_Relay,LOW);
      digitalWrite(Para_Relay,LOW);
    }
    else if(data_in[0] == '0')
    {
      digitalWrite(Seri_Relay,HIGH);
      digitalWrite(Para_Relay,HIGH);
    }
    else if(data_in[0] == 'n')
      digitalWrite(Seri_Relay,HIGH);
    else if(data_in[0] == 'f')
      digitalWrite(Seri_Relay,LOW);
  }
}


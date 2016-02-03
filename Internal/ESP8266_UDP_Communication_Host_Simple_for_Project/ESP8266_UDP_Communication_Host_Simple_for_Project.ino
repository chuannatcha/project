#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#define switch_on 5
#define switch_off 0
#define LED 2

//const char* ssid = "NAT.WRTNODE";
//const char* password = "devicenetwork";
const char* ssid = "Linksys";
const char* password = "0842216218";

//IPAddress Air1_IP(192,168,0,203);
//IPAddress Air2_IP(192,168,0,250);
const char* Air1_IP = "192.168.2.201";
const char* Air2_IP = "192.168.2.203";

IPAddress Local_IP(192,168,2,200);
IPAddress Subnet(255,255,255,0);
IPAddress Gateway(192,168,2,1);

const uint16_t localPort = 8888; // the destination port

char data_out[2];
char data_in[2];
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
//char ReplyBuffer[] = "acknowledged"; // a string to send back

WiFiUDP UDP;

void get_UDP();
void compare_data();
unsigned char chk_button();

void setup() 
{ 
  pinMode(switch_on,INPUT_PULLUP);
  pinMode(switch_off,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
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
}

void loop() 
{
    if(WiFi.status() == WL_CONNECTED)
    { 
      unsigned char button = chk_button();
      if(button != 0)
      {
        if(button == 1)
        {
          data_out[0] = '1';
          Serial.print("On ");
        }
        else if(button == 2)
        {
          data_out[0] = '0';
          Serial.print("Off ");
        }
        data_out[1] = 'a';
        UDP.beginPacket(Air1_IP,localPort);
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
        Serial.println("Data: ");
        data_in[0] = packetBuffer[0];
        data_in[1] = packetBuffer[1];
        //int value = packetBuffer[0]*10 + packetBuffer[1];
        //Serial.println(value);
        Serial.print(data_in[1]);
        Serial.println(data_in[0]);
        
        IPAddress remoteIp = UDP.remoteIP();
        Serial.print("Remote IP :");
        Serial.print(remoteIp);
        Serial.print(":");
        Serial.println(UDP.remotePort());
        // send a reply, to the IP address and port that sent us the packet we received
        //UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        //UDP.write(ReplyBuffer);
        //UDP.endPacket();

        compare_data();
      }
}

void compare_data()
{
  if(data_in[1] == 'z')
  {
    if(data_in[0] == '1')
      digitalWrite(LED,LOW);
    else if(data_in[0] == '0')
      digitalWrite(LED,HIGH);
  }
}

unsigned char chk_button()
{
  if(!digitalRead(switch_on))
  {
    while(!digitalRead(switch_on))
      delay(10);
    return 1;
  }
  else if(!digitalRead(switch_off))
  {
    while(!digitalRead(switch_off))
      delay(10);
    return 2;
  }
  return 0;
}


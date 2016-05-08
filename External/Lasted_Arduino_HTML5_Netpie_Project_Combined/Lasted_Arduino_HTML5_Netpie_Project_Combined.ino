#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
//#include <EEPROM.h>
#include <MicroGear.h>
#include <ArduinoJson.h>

#define APPID   "RMUTLLASCS"
#define KEY     "S2ezVjJCmVVdPrq"
#define SECRET  "7AoqXKrZeV8ptz1GPKA4oAtGZ"
#define ALIAS   "Arduino_Main"
#define SCOPE   ""

#define intervalTime 2500
#define SavingMoniPin 42
#define SavingActPin 44
#define SecurityMoniPin 46
#define SecurityActPin 48
#define A2 0
#define A1 1
#define L1 2
#define L2 3
#define L3 4
#define M1 5
#define M2 6
#define M3 7

const unsigned long MaxEnSavingTime = 60000;
const unsigned long MaxDisSavingTime = 10000;
const unsigned long MaxEnSecurityTime = 20000;
const unsigned long MaxDisSecurityTime = 10000;
const unsigned long SecurityModeStartTime = 20000;
const char* TelNumber = "AT+CMGS=\"0842216218\"\r";
const char TimesCommand = 2;
const unsigned int EachCommandRelay = 250;
const char SwitchModePin = 22;
byte mac[] = { 0x00, 0xCD, 0x12, 0xEC, 0x2D, 0x48 };

const char* ClientID = "Arduino_Mega_Main";
char* inTopic = "WebCommand";
char* outTopic = "WebStatus";

String M2Ainput = "";

char A2Wmsg[100], W2Amsg[50], M2Amsg[20], A2Mmsg[20];
unsigned char M2Achecksum, A2Mchecksum;
unsigned long timer = 0, lastMsg = 0;
boolean M2Acomplete = false, SavingModeRunning = false, SecurityModeRunning = false, SecurityFirstTimeEvent = false, DeviceMode = false; /* 0:Saving 1:Security */
unsigned long EnSavingBeginCount = 0, DisSavingBeginCount = 0, EnSecurityBeginCount = 0, DisSecurityBeginCount = 0, SecurityFirstTimeCount = 0;

boolean SavingProcessing[] = {0, 1, 0, 0, 0, 0, 0, 0};
boolean RemoteProcessing[] = {1, 0, 0, 0, 0, 0, 1, 0};
//Index                       0  1  2  3  4  5  6  7
boolean RemoteRelay[] = {false, false, false, false, false};
boolean SavingRelay[] = {false, false, false, false, false};
boolean Tstatus[] =     {false, false, false, false, false, false, false, false};
//Type                   A2    A1    L1    L2    L3    M1    M2    M3
//Index                  0     1     2     3     4     5     6     7

char A2Mtype[2] = {'A', '1'};
char A2Mmode = 'R';
boolean A2Mlogic = 1;

boolean button, st = false;

EthernetClient client;
AuthClient *authclient;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* inmsg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");

  char* text;
  text = (char*) malloc(msglen + 1);
  memcpy(text, inmsg, msglen);
  text[msglen] = '\0';
  strcpy(W2Amsg, text);

  Serial.println(W2Amsg);

  StaticJsonBuffer<200> RxBuffer;
  JsonObject& Rx = RxBuffer.parseObject(W2Amsg);
  if (!Rx.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  const char* Rname = Rx["name"];
  boolean Rlogic = Rx["logic"];
  //Serial.println(name);
  //Serial.println(logic);

  A2Mtype[0] = Rname[0];
  A2Mtype[1] = Rname[1];
  if (!DeviceMode)
  {
    unsigned char DeviceNum;
    boolean SavingCommand, RemoteCommand;
    if (!strcmp(Rname, "A1"))
      DeviceNum = A1;
    else if (!strcmp(Rname, "L1"))
      DeviceNum = L1;
    else if (!strcmp(Rname, "L2"))
      DeviceNum = L2;
    else if (!strcmp(Rname, "L3"))
      DeviceNum = L3;
    //Serial.println(DeviceNum);
    unsigned char TypeIndex = (SavingRelay[DeviceNum] * 4) + (RemoteRelay[DeviceNum] * 2) + Tstatus[DeviceNum];
    SavingCommand = SavingProcessing[TypeIndex];
    RemoteCommand = RemoteProcessing[TypeIndex];
    //Serial.println(TypeIndex);
    if (RemoteCommand == RemoteRelay[DeviceNum])
    {
      A2Mmode = 'S';
      A2Mlogic = SavingCommand;
    }
    else
    {
      A2Mmode = 'R';
      A2Mlogic = RemoteCommand;
    }
    Send_A2M();
  }
  else
  {
    A2Mmode = 'R';
    A2Mlogic = Rlogic;
    Send_A2M();
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen)
{
  Serial.println("Connected to NETPIE...");
  microgear.setName(inTopic);
}

void Generate_message()
{
  StaticJsonBuffer<200> TxBuffer;
  JsonObject& Tx = TxBuffer.createObject();
  Tx["Mode"] = (DeviceMode) ? 1 : 0;
  if (!DeviceMode)
    Tx["ModeSt"] = (SavingModeRunning) ? 1 : 0;
  else
    Tx["ModeSt"] = (SecurityModeRunning) ? 1 : 0;
  Tx["A1"] = (Tstatus[A1]) ? 1 : 0;
  Tx["L1"] = (Tstatus[L1]) ? 1 : 0;
  Tx["L2"] = (Tstatus[L2]) ? 1 : 0;
  Tx["L3"] = (Tstatus[L3]) ? 1 : 0;
  Tx["M1"] = (Tstatus[M1]) ? 1 : 0;
  Tx["M2"] = (Tstatus[M2]) ? 1 : 0;
  Tx["M3"] = (Tstatus[M3]) ? 1 : 0;
  strcpy(A2Wmsg, "");
  Tx.printTo(A2Wmsg, sizeof(A2Wmsg));
}

void serialEvent() {
  while (Serial1.available())
  {
    char inChar = (char)Serial1.read();
    M2Ainput += inChar;
    if (inChar == '\n')
    {
      M2Acomplete = true;
    }
  }
  if (M2Acomplete)
  {
    Parse_Serial();
    M2Ainput = "";
    M2Acomplete = false;
  }
}

void Parse_Serial()
{
  unsigned char ChksumPosition = M2Ainput.indexOf('#');
  String Chksum = M2Ainput.substring(ChksumPosition + 1, ChksumPosition + 3);
  unsigned char checksum = Chksum.toInt();

  M2Ainput = M2Ainput.substring(0, ChksumPosition);
  M2Ainput.toCharArray(M2Amsg, 20);

  M2Achecksum = 0;
  for (char x = 0; x < ChksumPosition; x++)
  {
    if (x != 8)
    {
      M2Achecksum += M2Amsg[x] - 48;
    }
  }
  Serial.print("M2A Status : ");
  Serial.println(M2Amsg);
  //Serial.println(checksum);
  //Serial.print("Checksum : ");
  //Serial.println(M2Achecksum);

  //When all status arrived then ...
  if (checksum == M2Achecksum)
  {
    Serial.println("Status Checksum OK");
    for (char y = 0; y < 8; y++)
    {
      Tstatus[y] = M2Amsg[y] - 48;
      //Serial.print(Tstatus[y], DEC);
    }
    for (char z = 9; z < ChksumPosition; z++)
    {
      unsigned char valueRelay = M2Amsg[z] - 48;
      SavingRelay[z - 9] = bitRead(valueRelay, 1);
      RemoteRelay[z - 9] = bitRead(valueRelay, 0);

      //Serial.print(SavingRelay[z-9], DEC);
      //Serial.print(RemoteRelay[z-9], DEC);
    }
    //Serial.println("");
    //Serial.println("Checksum OK");

    // When message from ESP-Master arrived then Send to Web
    Generate_message();
    Serial.print("Message published to web");
    //Serial.println(A2Wmsg);
    microgear.chat(outTopic, A2Wmsg);
  }
}

void Send_A2M()
{
  strcpy(A2Mmsg, "");
  A2Mchecksum = 0;
  snprintf(A2Mmsg, 20, "%c%c-%c-%d", A2Mtype[0], A2Mtype[1], A2Mmode, A2Mlogic);
  for (char x = 0; x < strlen(A2Mmsg); x++)
  {
    A2Mchecksum ^= A2Mmsg[x];
  }
  A2Mmsg[strlen(A2Mmsg)] = '\0';
  snprintf(A2Mmsg, 20, "%s#%03d", A2Mmsg, A2Mchecksum);
  for (char x = 1; x <= TimesCommand; x++)
  {
    Serial1.println(A2Mmsg);
    delay(EachCommandRelay);
  }

  //Monitor Arduino to ESP-Master Serial
  Serial.print("A2M Command : ");
  Serial.println(A2Mmsg);
}

void Enable_Saving()
{
  if (Tstatus[M1] || Tstatus[M2] || Tstatus[M3])
  {
    EnSavingBeginCount = millis();
  }
  else
  {
    if ((millis() - EnSavingBeginCount) > MaxEnSavingTime)
    {
      //Saving Enable
      for (char x = 1; x <= TimesCommand; x++)
      {
        Serial1.println("SA-S-1#112");
        delay(EachCommandRelay);
      }
      SavingModeRunning = true;
      Serial.println("Saving Condition Running");
      digitalWrite(SavingMoniPin, LOW);
      digitalWrite(SavingActPin, HIGH);
      digitalWrite(SecurityMoniPin, LOW);
      digitalWrite(SecurityActPin, LOW);
    }
  }
}

void Disable_Saving()
{
  if (Tstatus[M1] || Tstatus[M2] || Tstatus[M3])
  {
    if ((millis() - DisSavingBeginCount) > MaxDisSavingTime)
    {
      //Saving Disable
      for (char x = 1; x <= TimesCommand; x++)
      {
        Serial1.println("SA-S-0#113");
        delay(EachCommandRelay);
      }
      SavingModeRunning = false;
      Serial.println("Saving Condition Cancel");
      digitalWrite(SavingMoniPin, HIGH);
      digitalWrite(SavingActPin, LOW);
      digitalWrite(SecurityMoniPin, LOW);
      digitalWrite(SecurityActPin, LOW);
    }
  }
  else
  {
    DisSavingBeginCount = millis();
  }
}

void Enable_Security()
{
  if (Tstatus[M1] || Tstatus[M2] || Tstatus[M3])
  {
    if ((millis() - EnSecurityBeginCount) > MaxEnSecurityTime)
    {
      //Security Enable ** Found thief **
      Serial2.print("AT+CMGF=1\r");
      delay(1000);
      Serial2.print(TelNumber);
      delay(1000);
      Serial2.print("Found Thief in the RMUTL ELEC Computer Room\r");   //The text for the message
      delay(1000);
      Serial2.write(26);  //Equivalent to sending Ctrl+Z

      for (char x = 1; x <= TimesCommand; x++)
      {
        Serial1.println("LA-R-1#110");
        delay(EachCommandRelay);
      }
      SecurityModeRunning = true;
      Serial.println("Found thief and System has been sent SMS");
      digitalWrite(SavingMoniPin, LOW);
      digitalWrite(SavingActPin, LOW);
      digitalWrite(SecurityMoniPin, LOW);
      digitalWrite(SecurityActPin, HIGH);
    }
  }
  else
  {
    EnSecurityBeginCount = millis();
  }
}

void Disable_Security()
{
  if (Tstatus[M1] || Tstatus[M2] || Tstatus[M3])
  {
    DisSecurityBeginCount = millis();
  }
  else
  {
    if ((millis() - DisSecurityBeginCount) > MaxDisSecurityTime)
    {
      //Security Disable ** Thief went away **

      Serial2.print("AT+CMGF=1\r");
      delay(1000);
      Serial2.print(TelNumber);
      delay(1000);
      Serial2.print("Thief has left the RMUTL Computer Room\r");   //The text for the message
      delay(1000);
      Serial2.write(26);  //Equivalent to sending Ctrl+Z

      for (char x = 1; x <= TimesCommand; x++)
      {
        Serial1.println("LA-R-0#111");
        delay(EachCommandRelay);
      }
      SecurityModeRunning = false;
      Serial.println("Thief went away");
      digitalWrite(SavingMoniPin, LOW);
      digitalWrite(SavingActPin, LOW);
      digitalWrite(SecurityMoniPin, HIGH);
      digitalWrite(SecurityActPin, LOW);
    }
  }
}

void SwitchMode()
{
  if (!DeviceMode)
  {
    //Change to Security Mode
    Serial.println("Device was on Security Mode");
    for (char x = 1; x <= TimesCommand; x++)
    {
      Serial1.println("RA-R-0#113");//Cancel All Remote Command that operated
      delay(EachCommandRelay);
    }
    for (char x = 1; x <= TimesCommand; x++)
    {
      Serial1.println("SA-S-0#113");//Cancel All Remote Command that operated
      delay(EachCommandRelay);
    }
    DeviceMode = true;
    SecurityFirstTimeCount = millis();
    digitalWrite(SavingMoniPin, LOW);
    digitalWrite(SavingActPin, LOW);
    digitalWrite(SecurityMoniPin, HIGH);
    digitalWrite(SecurityActPin, LOW);
  }
  else
  {
    //Change to Saving Mode
    Serial.println("Device was on Saving Mode");
    for (char x = 1; x <= TimesCommand; x++)
    {
      Serial1.println("RA-R-0#113");//Cancel All Remote Command that operated
      delay(EachCommandRelay);
    }
    DeviceMode = false;
    SecurityFirstTimeEvent = false;
    EnSavingBeginCount = millis();
    digitalWrite(SavingMoniPin, HIGH);
    digitalWrite(SavingActPin, LOW);
    digitalWrite(SecurityMoniPin, LOW);
    digitalWrite(SecurityActPin, LOW);
  }
}

void setup()
{
  M2Ainput.reserve(20);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(2400);
  Serial.println("Starting...");
  pinMode(SwitchModePin, INPUT);
  pinMode(SavingMoniPin, OUTPUT);
  pinMode(SavingActPin, OUTPUT);
  pinMode(SecurityMoniPin, OUTPUT);
  pinMode(SecurityActPin, OUTPUT);
  digitalWrite(SavingMoniPin, HIGH);
  digitalWrite(SavingActPin, LOW);
  digitalWrite(SecurityMoniPin, LOW);
  digitalWrite(SecurityActPin, LOW);
  Serial.println("Waiting for 10 second");
  delay(10000);

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);

  if (Ethernet.begin(mac)) {
    Serial.println(Ethernet.localIP());
    microgear.resetToken();
    microgear.init(KEY, SECRET, ALIAS);
    microgear.connect(APPID);
  }
  EnSavingBeginCount = millis();
}

void loop()
{
  //Serial.println("Entered loop code");

  serialEvent();

  if (!digitalRead(SwitchModePin))
  {
    delay(250);
    if (!digitalRead(SwitchModePin))
    {
      while (!digitalRead(SwitchModePin))
        delay(250);
      SwitchMode();
    }
  }

  if (!DeviceMode)
  {
    if (!SavingModeRunning)
      Enable_Saving();
    else
      Disable_Saving();
  }
  else
  {
    if (SecurityFirstTimeEvent)
    {
      if (!SecurityModeRunning)
        Enable_Security();
      else
        Disable_Security();
    }
    else
    {
      if ((millis() - SecurityFirstTimeCount) > SecurityModeStartTime)
      {
        SecurityFirstTimeEvent = true;
        EnSecurityBeginCount = millis();
        Serial.println("First Time of Security End");
      }
    }
  }
  /*
    long now = millis();
    if (now - lastMsg > 2500) {
      lastMsg = now;

      Generate_message();
      Serial.print("Publish message: ");
      Serial.println(A2Wmsg);
      microgear.chat(outTopic, A2Wmsg);

    }
  */

  if (microgear.connected())
  {
    microgear.loop();
  }
  else
  {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);

    if (timer >= 5000)
    {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;

  }
}

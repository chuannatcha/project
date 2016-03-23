#define A2 0
#define A1 1
#define L1 2
#define L2 3
#define L3 4
#define M1 5
#define M2 6
#define M3 7

const unsigned long MaxEnSavingTime = 60000;
const unsigned long MaxDisSavingTime = 5000;
const unsigned long MaxEnSecurityTime = 30000;
const unsigned long MaxDisSecurityTime = 60000;
const unsigned long SecurityModeStartTime = 60000;

const char SwitchModePin = 2;

String M2Ainput = "";

char M2Amsg[20], A2Mmsg[20];
char A2Mtype[2] = {'A', '1'};
char A2Mmode = 'S';
boolean A2Mlogic = 1;

unsigned char M2Achecksum, A2Mchecksum;
boolean M2Acomplete = false, SavingModeRunning = false, SecurityModeRunning = false, SecurityFirstTimeEvent = false, DeviceMode = false; /* 0:Saving 1:Security */
unsigned long EnSavingBeginCount = 0, DisSavingBeginCount = 0, EnSecurityBeginCount = 0, DisSecurityBeginCount = 0, SecurityFirstTimeCount = 0;
boolean Tstatus[] = {false, false, false, false, false, false, false, false};
//Type            A2    A1    L1    L2    L3    M1    M2    M3
//Index           0     1     2     3     4     5     6     7

void serialEvent() {
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
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
  Serial.println(A2Mmsg);
}

void Parse_Serial()
{
  unsigned char ChksumPosition = M2Ainput.indexOf('#');
  String Chksum = M2Ainput.substring(ChksumPosition + 1, ChksumPosition + 2);
  unsigned char checksum = Chksum.toInt();

  M2Ainput = M2Ainput.substring(0, ChksumPosition);
  M2Ainput.toCharArray(M2Amsg, 20);

  M2Achecksum = 0;
  for (char x = 0; x < ChksumPosition; x++)
  {
    M2Achecksum += M2Amsg[x] - 48;
  }
  //Serial.println(M2Amsg);
  //Serial.println(checksum);
  //Serial.println(M2Achecksum);

  //When all status arrived then ...
  if (checksum == M2Achecksum)
  {
    for (char y = 0; y < ChksumPosition; y++)
    {
      Tstatus[y] = M2Amsg[y] - 48;
      Serial.print(Tstatus[y], DEC);
    }
    Serial.println("");
    Serial.println("Checksum OK");
  }
}

void Enable_Saving()
{
  if (Tstatus[M1] || Tstatus[M2] || Tstatus[M3])
  {
    EnSavingBeginCount = millis();
  }
  else
  {
    if ((millis() - EnSavingBeginCount) > MaxDisSavingTime)
    {
      //Saving Enable
      Serial.println("SA-S-1#112");
      SavingModeRunning = true;
    }
  }
}

void Disable_Saving()
{
  if (Tstatus[M1] || Tstatus[M2] || Tstatus[M3])
  {
    if ((millis() - DisSavingBeginCount) > MaxEnSavingTime)
    {
      //Saving Disable
      Serial.println("SA-S-0#113");
      SavingModeRunning = false;
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
      //Security Enable ** Found theif **
      Serial.println("LA-R-1#110");
      SecurityModeRunning = true;
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
      Serial.println("LA-R-0#111");
      SecurityModeRunning = false;
    }
  }
}

void SwitchMode()
{
  if (!DeviceMode)
  {
    //Change to Security Mode
    Serial.println("RA-R-0#113");//Cancel All Remote Command that operated
    attachInterrupt(digitalPinToInterrupt(SwitchModePin), SwitchMode, FALLING);
    DeviceMode = true;
    SecurityFirstTimeCount = millis();
  }
  else
  {
    //Change to Saving Mode
    attachInterrupt(digitalPinToInterrupt(SwitchModePin), SwitchMode, RISING);
    DeviceMode = false;
    SecurityFirstTimeEvent = false;
  }
}

void setup() {
  Serial.begin(9600);
  M2Ainput.reserve(20);
  //attachInterrupt(digitalPinToInterrupt(SwitchModePin), SwitchMode, RISING);
  Serial.println("Ready to Receive");
}

void loop() {
  /*
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
        SecurityFirstTimeEvent = true;
    }
  }
  */
  Send_A2M();
  delay(2000);

}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/


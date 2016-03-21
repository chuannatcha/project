String Msg = "";         // a string to hold incoming data
int num = 1000, checksum;
char data[20];

String M2Ainput = "";
char M2Amsg[20], A2Mmsg[100];
unsigned char M2Achecksum, A2Mchecksum;
boolean M2Acomplete = false;
boolean Tstatus[] = {false, false, false, false, false, false, false, false};
//Type            A2    A1    L1    L2    L3    M1    M2    M3
//Index           0     1     2     3     4     5     6     7

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
  Serial.println(M2Amsg);
  Serial.println(checksum);
  Serial.println(M2Achecksum);

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

void setup() {
  Serial.begin(9600);
  M2Ainput.reserve(20);
  Msg.reserve(20);
  Serial.println("Ready to Receive");
}

void loop() {
  /*
    if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
    }
  */
  /*
    snprintf (data, 20, "M1-%d#", num);
    checksum = 0;
    for(char x=0; x < strlen(data)-1 ; x++)
    checksum ^= data[x];
    if(checksum < 100 )
    Msg = String(data) + '0' + String(checksum);
    else
    Msg = String(data) + String(checksum);
    Serial.println(Msg);
    Msg = "";
    if(num == 9900)
    num = 1000;
    num += 100;
    delay(2000);
  */
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    M2Ainput += inChar;
    Serial.println("Receiving String");
    if (inChar == '\n')
    {
      M2Acomplete = true;
      Serial.println("Receive Completed");
    }
  }
  if (M2Acomplete)
  {
    Parse_Serial();
    M2Ainput = "";
    M2Acomplete = false;
  }
}



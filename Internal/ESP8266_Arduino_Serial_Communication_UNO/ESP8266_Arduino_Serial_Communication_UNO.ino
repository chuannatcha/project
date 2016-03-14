String inputString = "", Msg = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int num=1000, checksum;
char data[20];

void setup() {
  Serial.begin(9600);
  inputString.reserve(20);
  Msg.reserve(20);
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
  snprintf (data, 20, "M1#%d-", num);
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
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}



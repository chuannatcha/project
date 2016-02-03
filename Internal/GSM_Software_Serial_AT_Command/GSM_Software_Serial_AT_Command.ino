#include  <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);
void setup()
{
  mySerial.begin(19200);               // the GPRS baud rate   
  Serial.begin(19200);                 // the GPRS baud rate   
  delay(10000);
  Serial.println("Calling");
  mySerial.write("ATD0842216218;\r"); 
}
void loop()
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());  
}

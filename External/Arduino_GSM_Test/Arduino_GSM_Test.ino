//To change pins for Software Serial, use the two lines in GSM.cpp.
#include "SIM900.h"
//#include <SoftwareSerial.h>
#include "sms.h"
#include "call.h"
#include "GSM.h"

//CallGSM call;
//SMSGSM sms;

char msg[20];
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  inputString.reserve(100);
  Serial.begin(2400);
  Serial1.begin(2400);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  /*
    if (gsm.begin(2400))
      Serial.println("\nstatus=READY");
    else
      Serial.println("\nstatus=IDLE");
  */
}

void loop()
{
  if (Serial1.available())
    Serial.write(Serial1.read());
  
  if (stringComplete) {
    Serial.println(inputString);

    char command[20];
    unsigned char len = inputString.length();
    inputString.toCharArray(command, len);

    if (!strcmp(command, "call"))
    {
      Serial.println("Calling");
      Serial1.print("ATD0842216218;\r");
      //call.Call("0842216218");
      //gsm.SimpleWriteln("ATD0842216218;");
    }
    else if (!strcmp(command, "stop"))
    {
      Serial.println("Stop Call");
      Serial1.print("ATH\r");
      //call.HangUp();
      //gsm.SimpleWriteln("ATH");
    }
    else if (!strcmp(command, "sms"))
    {
      //sms.SendSMS("0842216218", msg);
      Serial1.print("AT+CMGF=1\r");
      delay(1000);
      Serial1.print("AT+CMGS=\"0842216218\"\r");
      delay(1000);
      Serial1.print("SIM900 and Arduino say Hi!\r");   //The text for the message
      delay(1000);
      Serial1.write(26);  //Equivalent to sending Ctrl+Z 
    }
    else
    {
      Serial1.print(command);
      Serial1.print("\r");
      //strcpy(msg, command);
      //gsm.SimpleWriteln(command);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
/*
    GPRS.print("AT+CMGF=1\r"); //mandando SMS em modo texto
    Serial.println("AT+CMGF=1\r"); //mandando SMS em modo texto
    delay(1000);
    GPRS.print("AT+CMGS=\"+554988063979\"\r"); // numero que vamos mandar o SMS
    Serial.println("AT+CMGS=\"+554988063979\"\r"); // numero que vamos mandar o SMS
*/

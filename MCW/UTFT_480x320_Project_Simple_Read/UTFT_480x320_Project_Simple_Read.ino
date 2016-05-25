// UTFT_Demo_480x320
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution
// of 480x320 pixels.
//
// This program requires the UTFT library.
//

#include <UTFT.h>
// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t UbuntuBold[];
// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Arduino Uno / 2009:
// -------------------
// Standard Arduino Uno/2009 shield            : <display model>,A5,A4,A3,A2
// DisplayModule Arduino Uno TFT shield        : <display model>,A5,A4,A3,A2
//
// Arduino Mega:
// -------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Mega      : <display model>,38,39,40,41
//
// Remember to change the model parameter to suit your display module!
#define PtPin 0
#define Slope 31

const unsigned int corner_line1 = 14;
const unsigned int corner_line2 = 111;
const unsigned int corner_line3 = 208;
const unsigned int corner_line4 = 305;
const unsigned int x_text1 = 10;
const unsigned int x_text2 = 250;
const unsigned int x_dbm = 185;
const unsigned int y_line1 = 47;
const unsigned int y_line2 = 144;
const unsigned int y_line3 = 241;

double Pt, Pr, Po, VSWR, S11, S12, ReCof;
float temp;

UTFT myGLCD(CTE32HR, 38, 39, 40, 41);

void setup()
{
  Serial.begin(9600);
  analogReference(INTERNAL1V1);
  //randomSeed(analogRead(0));
  myGLCD.InitLCD();

  draw_layout();
  draw_text();

  Pr = Po = 0;
}

void loop()
{
  temp = analogRead(PtPin);
  Pt = (temp/1023.0)*1.1*1000;
  print_value();
  delay(1000);
}

void print_value()
{
  if (VSWR > 99.99)
    VSWR = 99.99;
  myGLCD.setFont(UbuntuBold);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  /*
    myGLCD.print("      ", x_text1 + 40, y_line1);
    myGLCD.print("      ", x_text1 + 40, y_line2);
    myGLCD.print("      ", x_text1 + 40, y_line3);
    myGLCD.print("      ", x_text2 + 40, y_line1);
    myGLCD.print("      ", x_text2 + 60, y_line2);
    myGLCD.print("      ", x_text2 + 40, y_line3);
  */

  myGLCD.printNumF(Pt, 2, x_text1 + 40, y_line1);
  myGLCD.printNumF(Pt*Slope, 2, x_text1 + 40, y_line2);
  myGLCD.printNumF(10 * log10(Pt*Slope), 2, x_text1 + 40, y_line3);

  myGLCD.printNumF(temp,  1, x_text2 + 40, y_line1);
  //myGLCD.printNumF(VSWR, 2, x_text2 + 60, y_line2);
  //myGLCD.printNumF(S12,  2, x_text2 + 40, y_line3);
}

void calculate()
{
  ReCof = Pr / Pt;
  S11 = 10 * log10(ReCof);
  VSWR = (1 + ReCof) / (1 - ReCof);
  S12 = 10 * log10(Po / Pt);

  Serial.println(ReCof);
  Serial.println(S11);
  Serial.println(VSWR);
  Serial.println(S12);
}

void draw_text()
{
  myGLCD.setFont(BigFont);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("Pt:", x_text1, corner_line1 + 10);
  myGLCD.print("Pt:", x_text1, corner_line2 + 10);
  myGLCD.print("Pt:", x_text1, corner_line3 + 10);

  myGLCD.print("ADC:", x_text2, corner_line1+10);
  //myGLCD.print("VSWR:", x_text2, corner_line2+10);
  //myGLCD.print("S21:", x_text2, corner_line3+10);

  myGLCD.print("mV", x_dbm, corner_line2 - 25);
  myGLCD.print("mW", x_dbm, corner_line3 - 25);
  myGLCD.print("dBm", x_dbm, corner_line4 - 25);
  myGLCD.print("1.1", x_dbm+255, corner_line2-25);
  //myGLCD.print("dB", x_dbm+255, corner_line4-25);
}

void draw_layout()
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 479, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 306, 479, 319);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);

  myGLCD.setFont(SmallFont);
  myGLCD.print("5.8 GHz Digital Power Meter", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255, 255, 0);
  myGLCD.print("Copyright 2016 Electronic Engineering#2 All rights reserved", CENTER, 307);

  myGLCD.setColor(  0,   0, 255);
  myGLCD.drawRect(  0,  14, 479, 305);
  myGLCD.drawLine(  0, 111, 479, 111);
  myGLCD.drawLine(  0, 208, 479, 208);
  myGLCD.drawLine(240,  14, 240, 305);
}


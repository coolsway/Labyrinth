#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Arduino.h" // because why not

extern "C" {
  #include "delay.h"
  #include "FillPat.h"
  #include "I2CEEPROM.h"
  #include "LaunchPad.h"
  #include "OrbitBoosterPackDefs.h"
  #include "OrbitOled.h"
  #include "OrbitOledChar.h"
  #include "OrbitOledGrph.h"
  #include <stdlib.h>
  #include <math.h>
}

void oledDraw(char* bmp, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void oledPrintText(char* text, uint8_t x, uint8_t y);
void oledReset();
void deviceInit();
void initializeI2CAccelerometer();
char I2CGenTransmit(char* pbData, int cSize, bool fRW, char bAddr);
bool I2CGenIsNotIdle();

#endif


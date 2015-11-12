extern "C" {
  #include <delay.h>
  #include <FillPat.h>
  #include <I2CEEPROM.h>
  #include <LaunchPad.h>
  #include <OrbitBoosterPackDefs.h>
  #include <OrbitOled.h>
  #include <OrbitOledChar.h>
  #include <OrbitOledGrph.h>
  #include "functions.h"
}

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

extern int xchOledMax; 
extern int ychOledMax; 
const unsigned int bitmapHeight =7;  // The height of the bitmap we're about to create
const unsigned int bitmapWidth = 10;   // The width of the bitmap we're about to create
unsigned int x = 0;
unsigned int y = 0;
bool	fClearOled;

int	xcoBallStart 	= 64;
int	ycoBallStart	= 16;

int	xcoExhstStart	= 39;
int	ycoExhstStart	= 11;

int	cBallWidth 	= 6;
int	cBallHeight 	= 4;

int	fExhstSwt	= 0;

char	rgBMPBall[] = {
  {1, 0, 0, 1, 0, 1, 1, 1, 1, 1},
  {1, 0, 1, 0, 0, 1, 0, 0, 0, 0},
  {1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 0, 1, 1, 1, 1, 1}
};

void DeviceInit();
void OrbitSetOled();

void Labyrinth();

void BallRight(int xcoUpdate, int ycoUpdate);
void BallLeft(int xcoUpdate, int ycoUpdate);
void BallStop(int xcoUpdate, int ycoUpdate, bool fDir);

char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr);
bool I2CGenIsNotIdle();

void setup()
{
  DeviceInit();
}
char* formattedBitmap(char* input, unsigned int width, unsigned int height)
{
  
  unsigned int h = ceil(height / 8.0);
  char *output = (char*)calloc(h * width, sizeof(char));
  char b, temp;
  for (unsigned int hbyte = 0; hbyte < h; ++hbyte) {
    for (unsigned int i = 0; i < width; ++i) {
      b = 0;
      for (unsigned int j = 0; j < ((height - hbyte * 8)/8 ? 8 : (height%8)); ++j) {
        temp = input[(8*hbyte+j)*width+i];
        if (temp) b |= 1 << j;
      }

      output[hbyte*width+i]|=b;
    }
  }
  return output;
}
char* bitmap = formattedBitmap((char*)bmp, bitmapWidth, bitmapHeight);

void loop()
{
  Labyrinth();
  oledDraw(bitmap, x, y, bitmapWidth, bitmapHeight);
  OrbitOledUpdate();
  oledReset();
}

void DeviceInit()
{
  SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_4);

  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOA );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );

  GPIOPadConfigSet(SWTPort, SWT1 | SWT2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(BTN1Port, BTN1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPadConfigSet(BTN2Port, BTN2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(LED1Port, LED1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED2Port, LED2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED3Port, LED3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED4Port, LED4, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  GPIOPinTypeGPIOInput(SWTPort, SWT1 | SWT2);

  GPIOPinTypeGPIOInput(BTN1Port, BTN1);
  GPIOPinTypeGPIOInput(BTN2Port, BTN2);

  GPIOPinTypeGPIOOutput(LED1Port, LED1);
  GPIOPinTypeGPIOOutput(LED2Port, LED2);
  GPIOPinTypeGPIOOutput(LED3Port, LED3);
  GPIOPinTypeGPIOOutput(LED4Port, LED4);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

  GPIOPinTypeADC(AINPort, AIN);

  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCSequenceEnable(ADC0_BASE, 0);

  OrbitOledInit();

  fClearOled = true;

}

void Labyrinth() {

  short	dataX;

  char 	chPwrCtlReg = 0x2D;
  char 	chX0Addr = 0x32;

  char 	rgchReadAccl[] = {
    0, 0, 0            };
  char 	rgchWriteAccl[] = {
    0, 0            };

  int	xcoBallCur = xcoBallStart;
  int 	ycoBallCur = ycoBallStart;

  int		xDirThreshPos = 50;
  int		xDirThreshNeg = -50;

  bool fDir = true;

  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    rgchWriteAccl[0] = chPwrCtlReg;
    rgchWriteAccl[1] = 1 << 3;		
    I2CGenTransmit(rgchWriteAccl, 1, WRITE, ACCLADDR);

  }

  OrbitOledMoveTo(xcoBallStart, ycoBallStart);
  OrbitOledPutBmp(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledUpdate();

  while(1) {

    rgchReadAccl[0] = chX0Addr;
    I2CGenTransmit(rgchReadAccl, 2, READ, ACCLADDR);

    dataX = (rgchReadAccl[2] << 8) | rgchReadAccl[1];

    if(dataX < 0 && dataX < xDirThreshNeg) {
      fDir = true;

      if(xcoBallCur >= (ccolOledMax - 32)) {
        xcoBallCur = 0;

        OrbitOledClear();
      }

      else {
        xcoBallCur++;
      }

      BallRight(xcoBallCur, ycoBallCur);
    }

    else if(dataX > 0 && dataX > xDirThreshPos) {
      fDir = false;

      if(xcoBallCur <= 0) {
        xcoBallCur = ccolOledMax - 32;

        OrbitOledClear();
      }

      else {
        xcoBallCur--;
      }

      BallLeft(xcoBallCur, ycoBallCur);
    }

    else {
      BallStop(xcoBallCur, ycoBallCur, fDir);
    }
  }
}

void BallRight(int xcoUpdate, int ycoUpdate) {
  OrbitOledClear();
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmp(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledMoveTo(xcoUpdate, ycoUpdate);

  OrbitOledUpdate();
}

void BallLeft(int xcoUpdate, int ycoUpdate) {
  OrbitOledClear();
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmpFlipped(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledMoveTo(xcoUpdate + cBallWidth, ycoUpdate);

  OrbitOledUpdate();
}

void BallStop(int xcoUpdate, int ycoUpdate, bool fDir) {
  if(fDir) {
    OrbitOledMoveTo(xcoUpdate - cBallWidth, ycoUpdate);
    OrbitOledSetFillPattern(OrbitOledGetStdPattern(0));
    OrbitOledFillRect(xcoUpdate - cBallWidth, ycoUpdate);
  }
  else {
    OrbitOledMoveTo(xcoUpdate + cBallWidth, ycoUpdate);
    OrbitOledSetFillPattern(OrbitOledGetStdPattern(0));
    OrbitOledFillRect(xcoUpdate + cBallWidth, ycoUpdate);
  }

  OrbitOledUpdate();
}

char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr) {

  int 		i;
  char * 		pbTemp;

  pbTemp = pbData;
  
  I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, WRITE);
  I2CMasterDataPut(I2C0_BASE, *pbTemp);

  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

  DelayMs(1);

  while(I2CGenIsNotIdle());

  pbTemp++;

  if(fRW == READ) {

    I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, READ);

    while(I2CMasterBusy(I2C0_BASE));

    for(i = 0; i < cSize; i++) {

      if(cSize == i + 1 && cSize == 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else if(cSize == i + 1 && cSize > 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else if(i == 0) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));

        while(I2CGenIsNotIdle());
      }
      else {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));

        while(I2CGenIsNotIdle());
      }

      while(I2CMasterBusy(I2C0_BASE));

      *pbTemp = (char)I2CMasterDataGet(I2C0_BASE);

      pbTemp++;

    }

  }
  else if(fRW == WRITE) {

    for(i = 0; i < cSize; i++) {
      
      I2CMasterDataPut(I2C0_BASE, *pbTemp);

      while(I2CMasterBusy(I2C0_BASE));

      if(i == cSize - 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));

        while(I2CGenIsNotIdle());
      }

      pbTemp++;
    }

  }

  return 0x00;

}

bool I2CGenIsNotIdle() {

  return !I2CMasterBusBusy(I2C0_BASE);

}

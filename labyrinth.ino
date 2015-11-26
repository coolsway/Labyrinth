extern "C" {
  #include <delay.h>
  #include <FillPat.h>
  #include <I2CEEPROM.h>
  #include <LaunchPad.h>
  #include <OrbitBoosterPackDefs.h>
  #include <OrbitOled.h>
  #include <OrbitOledChar.h>
  #include <OrbitOledGrph.h>
}

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
//#define GREEN_LED GPIO_PIN_3

extern int xchOledMax; 
extern int ychOledMax; 

bool	fClearOled;

int	xcoBallStart 	= 4;
int	ycoBallStart	= 4;

int	cBallWidth 	= 4;
int	cBallHeight 	= 4;

char	rgBMPBall[] = {
  0xFF, 0xFF,
  0xFF, 0xFF
};

int   maze[][32] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void DeviceInit();
void OrbitSetOled();

void Labyrinth();

void BallRight(int xcoUpdate, int ycoUpdate);
void BallLeft(int xcoUpdate, int ycoUpdate);
void BallStop(int xcoUpdate, int ycoUpdate, bool fDir);

char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr);
bool I2CGenIsNotIdle();

// Random Maze Generation

struct cellsData {
  int **cells;
  int length;
};

void clearCell(int maze[8][32], int rowNum, int colNum) {
  maze[rowNum][colNum] = 0;
}

int checkAdjacency(int maze[8][32], int numRows, int numCols, int rowNum, int colNum) {

  int checks = 0;

  if (rowNum > 0 && maze[rowNum-1][colNum] == 0) checks++;
  if (rowNum < numRows-1 && maze[rowNum+1][colNum] == 0) checks++;
  if (colNum > 0 && maze[rowNum][colNum-1] == 0) checks++;
  if (colNum < numCols-1 && maze[rowNum][colNum+1] == 0) checks++;

  if (checks < 2) return 1;

  return 0;
}

void addToCells(struct cellsData *cellsData1, int *index) {

  int size = cellsData1->length + 1;

  cellsData1->cells[size - 1][0] = index[0];
  cellsData1->cells[size - 1][1] = index[1];
  cellsData1->length++;

}

void addSurroundingCells(int maze[8][32], int numRows, int numCols, struct cellsData *data, int rowNum, int colNum) {

  int *upIndex = (int *)calloc(2, sizeof(int));
  int *downIndex = (int *)calloc(2, sizeof(int));
  int *leftIndex = (int *)calloc(2, sizeof(int));
  int *rightIndex = (int *)calloc(2, sizeof(int));

  // UP
  if (rowNum > 1) {
    upIndex[0] = rowNum-1;
    upIndex[1] = colNum;
    addToCells(data, upIndex);
  }

  // DOWN
  if (rowNum < numRows-2) {
    downIndex[0] = rowNum+1;
    downIndex[1] = colNum;
    addToCells(data, downIndex);
  }

  // LEFT
  if (colNum > 1) {
    leftIndex[0] = rowNum;
    leftIndex[1] = colNum-1;
    addToCells(data, leftIndex);
  }

  // RIGHT
  if (colNum < numCols-2) {
    rightIndex[0] = rowNum;
    rightIndex[1] = colNum+1;
    addToCells(data, rightIndex);
  }

  free(upIndex);
  free(downIndex);
  free(leftIndex);
  free(rightIndex);
}

void generateMaze(int maze[8][32]) {

  struct cellsData *data = (struct cellsData*) malloc(sizeof(struct cellsData));
  data->cells = (int**) malloc(250*sizeof(int));
  data->length = 0;

  for (int i = 0; i < 250; i++) {
    data->cells[i] = (int*)calloc(2,sizeof(int));
    data->cells[i][0] = 999;
    data->cells[i][1] = 999;
  }

  int numRows = 8;
  int numCols = 32;

  // Choose starting cell
  int startRow = random(1,numRows);
  int startCol = random(1,numCols);

  clearCell(maze,startRow,startCol);
  addSurroundingCells(maze, numRows, numCols, data, startRow, startCol);

  int *temp;
  
  while (data->length > 0) {
          
    int randomIndex = random(0,data->length);
    int *randomCell = (int *) calloc(2, sizeof(int));
    randomCell[0] =  data->cells[randomIndex][0];
    randomCell[1] =  data->cells[randomIndex][1];
		
    if (checkAdjacency(maze, numRows, numCols, randomCell[0], randomCell[1]) == 1) {
      clearCell(maze, randomCell[0], randomCell[1]);
      addSurroundingCells(maze, numRows, numCols, data, randomCell[0], randomCell[1]);
    }

    data->cells[randomIndex][0] = data->cells[data->length - 1][0];
    data->cells[randomIndex][1] = data->cells[data->length - 1][1];

    data->cells[data->length - 1][0] = 999;
    data->cells[data->length - 1][1] = 999;
    data->length--;

    temp = randomCell;
    free(randomCell);
  }

  //printf("FINAL RANDOM SELECTION: %d %d\n\n", temp[0], temp[1]);

  free(data->cells);
  free(data);
  
  // Top Left Start
  
  maze[1][1] = 0;
  maze[1][2] = 0;
  maze[2][2] = 0;
  maze[2][1] = 0;
  
  // Bottom Right End
  
  maze[6][30] = 0;
  maze[5][30] = 0;
  maze[6][29] = 0;
  maze[5][29] = 0;

  Labyrinth();
}

void setup()
{
  DeviceInit();
  pinMode(GREEN_LED, OUTPUT);
  //digitalWrite(GREEN_LED, HIGH);
}

void loop()
{
  randomSeed(analogRead(A1));
  generateMaze(maze);
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

int *ballLocation(int x, int y){
  int *coord = (int*)malloc(2*sizeof(int));
  coord[0] = x/4;
  coord[1] = y/4;
  return coord;
}

char *createBMP (int width, int height, int **data){
  
  char *bmp = (char*)calloc(width*(height/8),sizeof(char));
  
  for (int k = 0; k < (height/8); k++){
  
    for (int i = 0; i < width; i++){
      int val = 0;
      int multiplier = 1;
      
      for (int j = 0; j < 8; j++){
        if (data[j+k*8][i]) val += multiplier;
        multiplier *= 2;
      }
      
      bmp[i+k*width] = val;
    }
    
  }
  
  return bmp;
}

int** expandMaze(int maze[][32]){
  int **expandedMaze = (int**)malloc(32*sizeof(int));
  
  for (int i = 0; i < 8; i++){
    expandedMaze[i*4] = (int*)calloc(128,sizeof(int));
    expandedMaze[i*4+1] = (int*)calloc(128,sizeof(int));
    expandedMaze[i*4+2] = (int*)calloc(128,sizeof(int));
    expandedMaze[i*4+3] = (int*)calloc(128,sizeof(int));
    
    for (int j = 0; j < 32; j++){
      if (maze[i][j] == 1){
        expandedMaze[i*4][j*4] = 1;
        expandedMaze[i*4][j*4+1] = 1;
        expandedMaze[i*4][j*4+2] = 1;
        expandedMaze[i*4][j*4+3] = 1;
        
        expandedMaze[i*4+1][j*4] = 1;
        expandedMaze[i*4+1][j*4+1] = 1;
        expandedMaze[i*4+1][j*4+2] = 1;
        expandedMaze[i*4+1][j*4+3] = 1;
        
        expandedMaze[i*4+2][j*4] = 1;
        expandedMaze[i*4+2][j*4+1] = 1;
        expandedMaze[i*4+2][j*4+2] = 1;
        expandedMaze[i*4+2][j*4+3] = 1;
        
        expandedMaze[i*4+3][j*4] = 1;
        expandedMaze[i*4+3][j*4+1] = 1;
        expandedMaze[i*4+3][j*4+2] = 1;
        expandedMaze[i*4+3][j*4+3] = 1;
        
      }
    }
  }
  
  return expandedMaze;
}

int mazeWidth = 128;
int mazeHeight = 32;

char *finalMaze;

void Labyrinth() {

  short	dataX;
  short dataY;
  
  finalMaze = createBMP(128,32,expandMaze(maze));

  char 	chPwrCtlReg = 0x2D;
  char 	chX0Addr = 0x32;
  char  chY0Addr = 0x34;

  char 	rgchReadAccl[] = {0, 0, 0};
  char  rgchReadAcclY[] = {0, 0, 0};
  char 	rgchWriteAccl[] = {0, 0};

  int	xcoBallCur = xcoBallStart;
  int 	ycoBallCur = ycoBallStart;

  int	xDirThreshPos = 50;
  int	xDirThreshNeg = -50;
  
  int   yDirThreshPos = 110;
  int   yDirThreshNeg = -110;

  bool fDir = true;
  bool fDirY = true;

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
  
  OrbitOledMoveTo(0,0);
  OrbitOledPutBmp(mazeWidth, mazeHeight, finalMaze);

  OrbitOledMoveTo(xcoBallStart, ycoBallStart);
  OrbitOledPutBmp(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledUpdate();
  
  int upStopped = 0;
  int leftStopped = 0;

  while(1) {

    rgchReadAccl[0] = chX0Addr;
    rgchReadAcclY[0] = chY0Addr;
    
    I2CGenTransmit(rgchReadAccl, 2, READ, ACCLADDR);
    I2CGenTransmit(rgchReadAcclY, 2, READ, ACCLADDR);

    dataX = (rgchReadAccl[2] << 8) | rgchReadAccl[1];
    dataY = (rgchReadAcclY[2] << 8) | rgchReadAcclY[1];
    
    int *storage = ballLocation(xcoBallCur, ycoBallCur);
    
    // Winning Condition
    
    if (storage[0] == 30 && storage[1] == 6){
        for (int i = 0; i < 3; i++) {
          digitalWrite(GREEN_LED, HIGH);
          delay(300);
          digitalWrite(GREEN_LED, LOW);
          delay(300);
        }
      break;
    }

    if(dataX < 0 && dataX < xDirThreshNeg) {
      
      leftStopped = 0;
      
      int condition;
      
      if (upStopped == 1){
        condition = maze[storage[1]+1][storage[0]+1];
      }else{
        condition = maze[storage[1]][storage[0]+1];
      }
      
      if (condition==1){
        
      }else{
        fDir = true;
 
        xcoBallCur++;
  
        BallRight(xcoBallCur, ycoBallCur);
      }
    }

    else if(dataX > 0 && dataX > xDirThreshPos) {
      
      int condition;
      
      if (upStopped == 1){
        condition = maze[storage[1]+1][storage[0]];
      }else{
        condition = maze[storage[1]][storage[0]];
      }
      
      if (condition==1){
        leftStopped = 1;
      }else{
        fDir = false;
  
        xcoBallCur--;
  
        BallLeft(xcoBallCur, ycoBallCur);
      }
    }
    
    else if(dataY > 0 && dataY > yDirThreshPos) {
      upStopped = 0;
      
      int condition;
      
      if (leftStopped == 1){
        condition = maze[storage[1]+1][storage[0]+1];
      }else{
        condition = maze[storage[1]+1][storage[0]];
      }
      
      if (condition==1){
      }else{
        fDirY = true;
        
        ycoBallCur++;
        
        BallDown(xcoBallCur, ycoBallCur);
      }
    }
    
    else if(dataY < 0 && dataY < yDirThreshPos) {
      
      int condition;
      
      if (leftStopped == 1){
        condition = maze[storage[1]][storage[0]+1];
      }else{
        condition = maze[storage[1]][storage[0]];
      }
      
      if (condition==1){
        upStopped = 1;
      }else{
        fDirY = false;
       
        ycoBallCur--;
  
        BallUp(xcoBallCur, ycoBallCur);
      }
    }

    else {
      BallStop(xcoBallCur, ycoBallCur, fDir);
    }
    
    free(storage);
  }
  
  
}

void BallRight(int xcoUpdate, int ycoUpdate) {
  OrbitOledClear();
  
  OrbitOledMoveTo(0,0);
  OrbitOledPutBmp(mazeWidth, mazeHeight, finalMaze);
  
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmp(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledMoveTo(xcoUpdate, ycoUpdate);

  OrbitOledUpdate();
}

void BallLeft(int xcoUpdate, int ycoUpdate) {
  OrbitOledClear();
  
  OrbitOledMoveTo(0,0);
  OrbitOledPutBmp(mazeWidth, mazeHeight, finalMaze);
  
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmpFlipped(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledMoveTo(xcoUpdate + cBallWidth, ycoUpdate);

  OrbitOledUpdate();
}

void BallDown(int xcoUpdate, int ycoUpdate) {
  OrbitOledClear();
  
  OrbitOledMoveTo(0,0);
  OrbitOledPutBmp(mazeWidth, mazeHeight, finalMaze);
  
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmp(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledMoveTo(xcoUpdate, ycoUpdate);

  OrbitOledUpdate();
}

void BallUp(int xcoUpdate, int ycoUpdate) {
  OrbitOledClear();
  
  OrbitOledMoveTo(0,0);
  OrbitOledPutBmp(mazeWidth, mazeHeight, finalMaze);
  
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmpFlipped(cBallWidth, cBallHeight, rgBMPBall);

  OrbitOledMoveTo(xcoUpdate, ycoUpdate-cBallHeight);

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

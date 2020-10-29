#include "PSWR_T.h"

const unsigned long bandswitch[] = {160,80,40,20,15,10};

uint8_t f_band = 0;  // initital band
uint8_t c_band = 0;  // detect band change
uint8_t c_rxtx = 1;  // detect rx_tx change
uint8_t f_rxtx = 0;  // initial rx
uint8_t c_mode = 1;  // if need to renew display, set this flag to 1
uint8_t f_mode = 0; 
int     first_B = 1;

/*--------------------------------------------------------------------------
        Initialize 74HC595
---------------------------------------------------------------------------*/
//Pin connected to ST_CP pin 12 of 74HC595
int latchPin = RCLK;
//Pin connected to SH_CP pin 11 of 74HC595
int clockPin = SRCLK;
////Pin connected to DS of 74HC595
int dataPin = SER;
int c_rxswitch = -1;
volatile int lastSwitch = 0;


void ShiftTxRx() 
  {  
  int band;
  int tx_switch;
  
  pinMode(TXRX_SWITCH, INPUT);
  tx_switch = digitalRead(TXRX_SWITCH);
  if (tx_switch != c_rxswitch)
    {
    int currMillis = millis();
    if (currMillis - lastSwitch > 50)
      {
      c_rxswitch = tx_switch ;
      if (tx_switch == 0)
        f_rxtx = 1;
      else
        f_rxtx = 0;
      band = f_band;
      c_band = -1;
      set_band(band);
      lastSwitch = currMillis;
      }   
    }
}


void shiftOut(byte bpf, byte lpf) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low
  byte myDataOut;

//  Serial.println("Shift Out"); 


//internal function setup
  int i=0, ii=0;
  int pinState;
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

//clear everything out just in case to
//prepare shift register for bit shifting
  digitalWrite(dataPin, 0);
  digitalWrite(clockPin, 0);
  digitalWrite(latchPin, 0);

  // for each 74HC595
  for (ii=0; ii<3; ii++)
  {
    switch (ii)
    {
      case 0:
        myDataOut = lpf;
        break;
      case 1:
        if (f_rxtx)
          { // TX
          myDataOut = bpf | BP_TX;
          }
        else
          {
          myDataOut = bpf;   
          }
        break;
      case 2:
        if (f_rxtx)
          {
          myDataOut = SW_TX;
          }
        else
          {
          myDataOut = SW_RX;
          }
        break;
    }   
  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
    for (i=7; i>=0; i--)  {
      digitalWrite(clockPin, 0);
      //if the value passed to myDataOut and a bitmask result 
      // true then... so if we are at i=6 and our value is
      // %11010100 it would the code compares it to %01000000 
      // and proceeds to set pinState to 1.
      if ( myDataOut & (1<<i) ) {
        pinState= 1;
      }
      else {  
        pinState= 0;
      }

      //Sets the pin to HIGH or LOW depending on pinState
      digitalWrite(dataPin, pinState);
      //register shifts bits on upstroke of clock pin  
      digitalWrite(clockPin, 1);
      //zero the data pin after shift to prevent bleed through
      digitalWrite(dataPin, 0);
    }
  }
  //stop shifting
  digitalWrite(clockPin, 0);
  digitalWrite(latchPin, 1);
}

TextBox VirtLCDb;

void set_band(int &band)
{

  if (band > MAX_BAND)
    band = 0;
  if (band < 0)
    band = MAX_BAND;
  
  f_band = band;
  if (c_band != f_band)
  {
    c_band = f_band;
    switch (f_band)
    {
    case 0:
      f_mode = 0;
      shiftOut(BP_160M,LP_160M);
      break;
  
    case 1:
      f_mode = 0;
      shiftOut(BP_80M,LP_80M);
      break;
  
    case 2:
      f_mode = 0;
      shiftOut(BP_40M,LP_40M);
      break;
  
  case 3:
      f_mode = 1;
      shiftOut(BP_20M,LP_20M);
      break;
      
  case 4:
      f_mode = 1;
      shiftOut(BP_15M,LP_15M);
      break;

  case 5:
      f_mode = 1;
      shiftOut(BP_10M,LP_10M);
      break;
      }
  }
}
/*--------------------------------------------------------------------------
        Display the current band 160,80,40,20,15,10
---------------------------------------------------------------------------*/
void display_band()
{

  int16_t x1, y1; 
  uint16_t w, h;

  if (first_B == 1)
  {
    VirtLCDb.init(20,2,10,185, FONT_12, BAND_TEXT_COLOR, ILI9341_BLACK); 
    VirtLCDb.clear();
    first_B = 0;
  } 
 
     
  switch (f_band)
    {
    case 0:
      VirtLCDb.clear();
      VirtLCDb.setCursor(0,0);
      if (!f_rxtx)
        VirtLCDb.print(String("160M"));
      else
        VirtLCDb.print(String("160M TX"));
      VirtLCDb.transfer();
      break;
  
    case 1:
      VirtLCDb.clear();
      VirtLCDb.setCursor(0,0);
      if (!f_rxtx)
        VirtLCDb.print(String("80M"));
      else
        VirtLCDb.print(String("80M TX"));
      VirtLCDb.transfer();
      break;
  
    case 2:
      VirtLCDb.clear();
      VirtLCDb.setCursor(0,0);
      if (!f_rxtx)
        VirtLCDb.print("40M");
      else
        VirtLCDb.print("40M TX");
      VirtLCDb.transfer();
      break;
  
  case 3:
      VirtLCDb.clear();
      VirtLCDb.setCursor(0,0);
      if (!f_rxtx)
        VirtLCDb.print("20M");
      else  
        VirtLCDb.print("20M TX");
      VirtLCDb.transfer();
      break;
      
  case 4:
      VirtLCDb.clear();
      VirtLCDb.setCursor(0,0);
      if (!f_rxtx)
        VirtLCDb.print("15M");
      else
        VirtLCDb.print("15M TX");
      VirtLCDb.transfer();
      break;

  case 5:
      VirtLCDb.clear();
      VirtLCDb.setCursor(0,0);
      if (!f_rxtx)
        VirtLCDb.print("10M");
      else
        VirtLCDb.print("10M TX");
      VirtLCDb.transfer();
      break;
      }
} 

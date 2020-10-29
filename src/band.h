/*-------------------------------------------------------
   Bandswitching and RX/TX switching
--------------------------------------------------------*/
#ifndef _BAND_H_
#define _BAND_H_

#define SW_RX 0x00
#define SW_TX 0x20
#define BP_TX 0x20
#define MAX_BAND 5

// port (LPF)
// 1 -> 0x08
// 2 -> 0x10
// 3 -> 0x40
// 4 -> 0x04
// 5 -> 0x02
// 6 -> 0x01

// Band pass filter
#define BP_160M 0x08
#define BP_80M 0x02 //0x10
#define BP_40M 0x04
#define BP_20M 0x40
#define BP_15M 0x10 // 0x02
#define BP_10M 0x01

// Low pass filter
#define LP_160M 0x08
#define LP_80M 0x02 // 0x10
#define LP_40M 0x40
#define LP_20M 0x04
#define LP_15M 0x10 // 0x02
#define LP_10M 0x01

#define band_x 6 
#define band_y 195

#define BAND_BACKGROUND_COLOR ILI9341_BLACK
#define BAND_TEXT_COLOR tft.color565(255,128,0)
/*-------------------------------------------------------
   74HC595 Connection
--------------------------------------------------------*/

#define SER   13  // (pin 14)
#define SRCLK 12  // (pin 11)
#define RCLK  25  // (pin 12)

// RXTX digital pin
#define TXRX_SWITCH       39

extern TextBox VirtLCDb;  

void display_band();
void set_band(int &band);
void ShiftTxRx();

#endif

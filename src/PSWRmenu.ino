//*********************************************************************************
//**
//** Project.........: A menu driven Multi Display RF Power and SWR Meter
//**                   using a Tandem Match Coupler and 2x AD8307; or
//**                   diode detectors.
//**
//** Copyright (c) 2014  Loftur E. Jonasson  (tf3lj [at] arrl [dot] net)
//**
//** This program is free software: you can redistribute it and/or modify
//** it under the terms of the GNU General Public License as published by
//** the Free Software Foundation, either version 3 of the License, or
//** (at your option) any later version.
//**
//** This program is distributed in the hope that it will be useful,
//** but WITHOUT ANY WARRANTY; without even the implied warranty of
//** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//** GNU General Public License for more details.
//**
//** You should have received a copy of the GNU General Public License
//** along with this program.  If not, see <http://www.gnu.org/licenses/>.
//**
//** Platform........: Teensy 3.1 / 3.2 / 3.5 / 3.6 (http://www.pjrc.com)
//**
//** Initial version.: 0.50, 2013-09-29  Loftur Jonasson, TF3LJ / VE2LJX
//**                   (beta version)
//**
//*********************************************************************************

#define SSD1306         0 // Select 0 if this file is used with TFT version, 
                          // Select 1 if used with OLED version

// First Level Menu Items

#if SSD1306               // OLED display version 
#if BATTERY_POWER
const uint8_t level0_menu_size = 10;
#else
const uint8_t level0_menu_size = 9;
#endif
const char *level0_menu_items[] =
          {  "1 SWR Alarm",
             "2 SWR Alarm Power",
             "3 PEP Period",
             #if BATTERY_POWER
             "4 Display&Battery",
             #else
             "4 Display Setup",
             #endif
             "5 Scale Ranges",					
             "6 Calibrate",
             "7 Debug Display",
             "8 Reset-> Default",
             #if BATTERY_POWER
             "X Power Off",
             #endif
             "0 Exit"  };
#else                     // TFT Colour LCD version
#if BATTERY_POWER
const uint8_t level0_menu_size = 12;
#else
const uint8_t level0_menu_size = 11;
#endif
const char *level0_menu_items[] =
          {  "1 SWR Alarm",
             "2 SWR Alarm Power",
             "3 PEP Period",
             #if BATTERY_POWER
             "4 Display&Battery",
             #else
             "4 Display Setup",
             #endif
             "5 Scale Ranges",          
             "6 Modulation Scope",
             "7 Calibrate",
             "8 Debug Display",
             "9 Debug2 Display",
             "10 Reset-> Default",
             #if BATTERY_POWER             
             "X Power Off",
             #endif
             "0 Exit"  };
#endif


// Flags for SWR Alarm & Threshold menu
#define SWR_ALARM_THRESHOLD      1
#define	SWR_ALARM_PWR_THRESHOLD  2
// SWR Alarm Power Threshold menu Items
const uint8_t swr_alarm_pwr_thresh_menu_size = 5;
const char *swr_alarm_pwr_thresh_menu_items[] =
          {  "1   1mW",
             "2  10mW",
             "3  0.1W",
             "4    1W",
             "5   10W"  };

// Flag for PEP Sample Period select
#define PEP_MENU                 3
// PEP Sample Period select menu Items
const uint8_t pep_menu_size = 3;
const char *pep_menu_items[] =
          {  "1    1s",
             "2  2.5s",
             "3    5s"  };

// Flag for Encoder Resolution Change
//#define ENCODER_MENU	8

// Display and Battery Management
#define DISPLAY_BATT_MENU        5
#if BATTERY_POWER
const uint8_t display_bat_menu_size = 8;
const char *display_bat_menu_items[] =
          {  "Display Rotate",
             #if SSD1306
             "Display Dim Toggle",
             #else
             "Backlight Adjust",
             #endif
             "Screensaver Thresh",
             "Low Power Floor",
             "Power Off Time",
             "Shutoff Voltage",
             "Go Back",
             "Exit"     };
#else
const uint8_t display_bat_menu_size = 6;
const char *display_bat_menu_items[] =
          {  "Display Rotate",
             #if SSD1306
             "Display Dim Toggle",
             #else
             "Backlight Adjust",
             #endif
             "Screensaver Thresh",
             "Low Power Floor",
             "Go Back",
             "Exit"     };
#endif
#define DISPLAYROTATE_MENU     501
#define DISPLAYDIM_MENU        502
// Flag for Screensaver Threshold select menu
#define SCREENSAVERTHRESH_MENU 503
// Flag for Low Power Floor select menu
#define LOWPOWERFLOOR_MENU     504
#if AD8307_INSTALLED
// Screensaver Threshold select menu Items
const uint8_t screenthresh_menu_size = 6;
const char *screenthresh_menu_items[] =
          {  "1     Off",
             "2     1uW",
             "3    10uW",
             "4   100uW",
             "5     1mW",
             "6    10mW"   };
// Low Power Floor select menu Items
const uint8_t lowpowerfloor_menu_size = 6;
const char *lowpowerfloor_menu_items[] =
          {  "1     Off",
             "2     1uW",
             "3  0.01mW",
             "4   0.1mW",
             "5     1mW",
             "6  0.01 W"   };
#endif					
#define IDLE_POWEROFF_MENU     504
// Menu items for Power Off Time when Idle and running on battery power
const uint8_t idle_poweroff_menu_size = 6;
const char *idle_poweroff_menu_items[] =
          {  "1  10 minutes",
             "2  30 minutes",
             "3  60 minutes",
             "4 120 minutes",
             "5 240 minutes",
             "6       Never" };
#define SHUTOFFVOLTAGE_MENU    505

// Flag for Scale Range menu
#define SCALERANGE_MENU         6

// Flags for Scale Range Submenu functions
#define SCALE_SET0_MENU	600
#define SCALE_SET1_MENU	601
#define SCALE_SET2_MENU	602



// Flag for Modulation Scope menu
#define MODSCOPE_MENU          50
const uint8_t modscope_menu_size = 7;
    #if F_CPU >= 144000000         // CPU is overclocked at 144 MHz
const char *modscope_menu_items[] =
          {  "0  Go Back",
             "1   200 ms",         // divisor = 1
             "2   400 ms",         // 2
             "3   600 ms",         // 3
             "4   800 ms",         // 4
             "5    1.0 s",         // 5
             "6    2.0 s"  };      // 10             
#else                              // CPU is at normal speed
const char *modscope_menu_items[] =
          {  "0  Go Back",
             "1   300 ms",         // divisor = 1
             "2   600 ms",         // 2
             "3   900 ms",         // 3
             "4  1200 ms",         // 4
             "5    1.5 s",         // 5
             "6    3.0 s"  };      // 10             
#endif

// Flag for Calibrate menu
#define CAL_MENU               60
#if AD8307_INSTALLED
const uint8_t calibrate_menu_size = 4;
const char *calibrate_menu_items[] =
          {  "1 OneLevelCal(dBm)",
             "2  1st level (dBm)",
             "3  2nd level (dBm)",
             "0 Go Back" };


// Flags for Calibrate Submenu functions
#define CAL_SET0_MENU          61  // Single level calibration
#define CAL_SET1_MENU          62  // 1st level
#define CAL_SET2_MENU          63  // 2nd level
#endif

// Flags for Debug Screen
#define DEBUG_MENU           1000
#define DEBUG2_MENU          1001

// Flag for Factory Reset
#define FACTORY_MENU           90
// Factory Reset menu Items
const uint8_t factory_menu_size = 2;
const char *factory_menu_items[] =
          {  "1 Yes - Reset",
             "0 No  - Go back" };

// Shutdown if this is selected
#define POWEROFF_NOW         9999

uint8_t   menu_data = 0;           // Pass data to lower menu

int8_t    gain_selection;          // keep track of which GainPreset is currently selected

var_t     eeprom_R;                // Used to read and compare EEPROM R with current R

//
//--------------------------------------------------------------------
// Multipurpose Pushbutton 
//
// Returns 0 for no push, 1 for short push and 2 for long push
// (routine should be called once every 1 ms)
//--------------------------------------------------------------------
//
uint8_t multipurpose_pushbutton(void)
{
  static uint16_t pushcount;       // Measure push button time (max 65s)
  uint8_t         state;           // 1 for push or 0 for no-push
  static uint8_t  prevstate;       // Used for clumsy debounce
  uint8_t         retstate = 0;    // 1 for short push, 2 for long push

  if (digitalRead(EnactSW) == LOW) state = 1;    // Pin low = pushed
  else state = 0;

  //-------------------------------------------------------------------    
  // Is this the end of a successful push?
  // Was state stable for a minimum number of consecutive readings?
  if ((state != prevstate) && (pushcount >= ENACT_MIN))
  {
    if (prevstate == 1)                          // Menu/Enact switch
    { 
      if (pushcount >= ENACT_MAX)                // "Long Push"
      {
        retstate = 2;                            // Indicate long push
      }
      else retstate = 1;
    }
    pushcount = 0;                               // set counter to zero
    prevstate = 0;
  }
    
  //-------------------------------------------------------------------    
  // Debounce and measure time of push
  // If state is not stable, then reset to no-push
  else if (state != prevstate)                   // Change of state, seed prevstate and reset counter
  {
    pushcount = 0;
    prevstate = state;
  }
  else if (state == 0)                           // Push too short, reset everything
  {
    pushcount = 0;
    prevstate = 0;
  }
  else if ((state > 0) && (state == prevstate)) pushcount++; // Count upwards if state is stable

  //-------------------------------------------------------------------    
  // Kludge, enter Config mode immediately when long push condition is satisfied
  if ((state == 1) && (pushcount >= ENACT_MAX))
  {
    retstate = 2;   
  }
  return retstate;
}


//----------------------------------------------------------------------
// Display a Menu of choices, one line at a time
//
// **menu refers to a pointer array containing the Menu to be printed
//
// menu_size indicates how many pointers (menu items) there are in the array
//
// current_choice indicates which item is currently up for selection if
// pushbutton is pushed
//
// begin row, begin_col indicate the upper left hand corner of the one, three or 
// four lines to be printed
//
// lines indicates how many lines are displayed at any one time, 1, 3 or 4.
//----------------------------------------------------------------------
void lcd_scroll_Menu(char **menu, uint8_t menu_size,
uint8_t current_choice, uint8_t begin_row, uint8_t begin_col, uint8_t lines)
{
  uint8_t a, x;

  // Clear LCD from begin_col to end of line.
  VirtLCDw.setCursor(begin_col, begin_row);
  for (a = begin_col; a < 20; a++)
    VirtLCDw.print(" ");
    VirtLCDy.print(" ");
  if (lines > 1)
  {
    VirtLCDw.setCursor(begin_col, begin_row+1);
    for (a = begin_col; a < 20; a++)
      VirtLCDw.print(" ");
    VirtLCDy.print(" ");
  }
  if (lines > 2)
  {
    VirtLCDw.setCursor(begin_col, begin_row+2);
    for (a = begin_col; a < 20; a++)
      VirtLCDw.print(" ");
      VirtLCDy.print(" ");
  }
  // Using Menu list pointed to by **menu, preformat for print:
  // First line contains previous choice, secon line contains
  // current choice preceded with a '->', and third line contains
  // next choice
  if (current_choice == 0) x = menu_size - 1;
  else x = current_choice - 1;
  if (lines < 3)
  {
    // One or two lines
    VirtLCDy.setCursor(begin_col, begin_row);
    VirtLCDy.print("->");
    VirtLCDw.setCursor(begin_col + 2, begin_row);
    sprintf(lcd_buf,"%s", *(menu + current_choice));
    VirtLCDw.print(lcd_buf);

    if (lines == 2)
    {
      if (current_choice == menu_size - 1) x = 0;
      else x = current_choice + 1;
      VirtLCDw.setCursor(begin_col + 2, begin_row + 1);
      sprintf(lcd_buf,"%s", *(menu + x));
      VirtLCDw.print(lcd_buf);
    }   
  }

  else
  {
    // Three or four lines
    VirtLCDw.setCursor(begin_col + 2, begin_row);
    sprintf(lcd_buf,"%s", *(menu + x));
    VirtLCDw.print(lcd_buf);
    VirtLCDy.setCursor(begin_col, begin_row + 1);
    VirtLCDy.print("->");  
    VirtLCDw.setCursor(begin_col + 2, begin_row + 1);
    sprintf(lcd_buf,"%s", *(menu + current_choice));
    VirtLCDw.print(lcd_buf);
    if (current_choice == menu_size - 1) x = 0;
    else x = current_choice + 1;
    VirtLCDw.setCursor(begin_col + 2, begin_row + 2);
    sprintf(lcd_buf,"%s", *(menu + x));
    VirtLCDw.print(lcd_buf);

    // 4 line display.  Preformat and print the fourth line as well
    if (lines == 4)
    {
      if (current_choice == menu_size-1) x = 1;
      else if (current_choice == menu_size - 2 ) x = 0;
      else x = current_choice + 2;
      VirtLCDw.setCursor(begin_col, begin_row+3);
      for (a = begin_col; a < 20; a++)
      VirtLCDw.print(" ");
      VirtLCDy.print(" ");
      VirtLCDw.setCursor(begin_col + 2, begin_row + 3);
      sprintf(lcd_buf,"%s", *(menu + x));
      VirtLCDw.print(lcd_buf);
    } 
  }
}


//----------------------------------------------------------------------
// Menu functions begin:
//----------------------------------------------------------------------


//--------------------------------------------------------------------
// Debug Screen, exit on push
//--------------------------------------------------------------------
void debug_menu(void)
{
  // Reset data from Encoder
  Enc.clearCount();

  if (menu_level == DEBUG_MENU)
    lcd_display_debug();                // Display Config and measured input voltages etc...
  #if !SSD1306  
  else // DEBUG2_MENU
    lcd_display_debug2();               // Display All sorts of Meter Gauges etc...
  #endif
         
  // Exit on Button Push
  if (flag.short_push)
  {
    flag.short_push = false;            // Clear pushbutton status

    #if !SSD1306  
    lcd_display_debug_erase();          // Make sure to erase everything when we leave
    #endif
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDw.setCursor(0,1);				
    VirtLCDy.print("Exit from Debug");
    flag.menu_lcd_upd = false;          // Indicate that a LCD update is needed upon next entry into Config Mode
    Menu_exit_timer = 100;                // Timer to Exit and clear screen
    //flag.idle_refresh = true;           // Force screensaver reprint upon exit
    flag.config_mode = false;           // We're done, EXIT
    flag.power_detected = true;         // To wake up from sleep mode, if active - Indicate that Power was detected         
    flag.screensaver_on = false;
    power_timer = 0;                    // and seed the power timer (SLEEP_TIME)     
    menu_level = 0;                     // We are done with this menu level
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}


//--------------------------------------------------------------------
// SWR Alarm Threshold Set Menu
//--------------------------------------------------------------------
void swr_alarm_threshold_menu(void)
{
  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if ((Enc.read()/ENC_RESDIVIDE > 0) && (R.SWR_alarm_trig < 40) )  // SWR 4:1 is MAX value, and equals no alarm
    {
      R.SWR_alarm_trig++;
    }
    if ((Enc.read()/ENC_RESDIVIDE < 0) && (R.SWR_alarm_trig > 15) )  // SWR of 1.5:1 is MIN value for SWR alarm
    {
      R.SWR_alarm_trig--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;            // Keep track of LCD update requirements
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;            // We have serviced LCD

    VirtLCDw.clear();
    VirtLCDy.clear();

    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("SWR Alarm Threshold:");
    VirtLCDy.setCursor(0,2);
    VirtLCDy.print("Adjust->   ");
    VirtLCDw.setCursor(11,2);
    if (R.SWR_alarm_trig == 40)
    VirtLCDw.print("Off");		
    else
    {
      sprintf(lcd_buf,"%1u.%01u",R.SWR_alarm_trig/10, R.SWR_alarm_trig%10);
      VirtLCDw.print(lcd_buf);
    }					
    VirtLCDw.setCursor(0,4);
    VirtLCDw.print("Range is 1.5 to 3.9");
    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("4.0 = SWR Alarm Off");
  }	
  // Enact selection by saving in EEPROM
  if (flag.short_push)
  {
    flag.short_push = false;             // Clear pushbutton status

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(1,2);
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.SWR_alarm_trig != R.SWR_alarm_trig)  // New Value
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;               // Show on LCD for 1 second
    flag.idle_refresh = true;            // Force screensaver reprint upon exit
    flag.config_mode = true;             // Not done yet, back to Config Menu
    //flag.mode_change = true;             // Signal for display cleanup
    menu_level = 0;                      // We are done with this menu level
    flag.menu_lcd_upd = false;           // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}


//--------------------------------------------------------------------
// SWR Alarm Power Threshold Menu
//--------------------------------------------------------------------
void swr_alarm_power_threshold_menu(void)
{
  static int8_t	current_selection;

  // Get Current value
  // 1mW=>0, 10mW=>1, 100mW=>2, 1000mW=>3, 10000mW=>4
  // current_selection = log10(R.SWR_alarm_pwr_thresh); // This should have worked :(
  if      (R.SWR_alarm_pwr_thresh == 1) current_selection = 0;
  else if (R.SWR_alarm_pwr_thresh == 10) current_selection = 1;
  else if (R.SWR_alarm_pwr_thresh == 100) current_selection = 2;
  else if (R.SWR_alarm_pwr_thresh == 1000) current_selection = 3;
  else if (R.SWR_alarm_pwr_thresh == 10000) current_selection = 4;
	
  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();;

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;            // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = swr_alarm_pwr_thresh_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    // Update with currently selected value
    // R.SWR_alarm_pwr_thresh = pow(10,current_selection);  // Has roundoff error
    if      (current_selection == 0) R.SWR_alarm_pwr_thresh = 1;
    else if (current_selection == 1) R.SWR_alarm_pwr_thresh = 10;
    else if (current_selection == 2) R.SWR_alarm_pwr_thresh = 100;
    else if (current_selection == 3) R.SWR_alarm_pwr_thresh = 1000;
    else if (current_selection == 4) R.SWR_alarm_pwr_thresh = 10000;

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("SWR Alarm Power:");

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)swr_alarm_pwr_thresh_menu_items, menu_size, current_selection, 1, 7,3);
    VirtLCDy.setCursor(0,2);
    VirtLCDy.print("Select");

    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("Available thresholds");
    VirtLCDw.setCursor(0,6);
    VirtLCDw.print("1mW 10mW 0.1W 1W 10W");
  }
	
  // Enact selection
  if (flag.short_push)
  {
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,1);

    flag.short_push = false;             // Clear pushbutton status

    // Check if selected threshold is not same as previous
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.SWR_alarm_pwr_thresh != R.SWR_alarm_pwr_thresh)// New Value
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;               // Show on LCD for 1 seconds
    flag.idle_refresh = true;            // Force screensaver reprint upon exit
    flag.config_mode = true;             // Not done yet, bac to Config Menu
    //flag.mode_change = true;             // Signal for display cleanup
    menu_level = 0;                      // We are done with this menu level
    flag.menu_lcd_upd = false;           // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}


//--------------------------------------------------------------------
// Peak Envelope Power (PEP) period selection Menu
//--------------------------------------------------------------------
void pep_menu(void)
{
  static int8_t	current_selection;

  // Get Current value
  if (R.PEP_period == (5000000/SAMPLE_TIMER)/BUF_SHORT) current_selection = 2;          // 5 seconds
  else if (R.PEP_period == (2500000/SAMPLE_TIMER)/BUF_SHORT)    current_selection = 1;  // 2.5 seconds
  else current_selection = 0;         // Any other value, other than 1s, is invalid

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;					// We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = pep_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    if      (current_selection == 2) R.PEP_period = (5000000/SAMPLE_TIMER)/BUF_SHORT;
    else if (current_selection == 1) R.PEP_period = (2500000/SAMPLE_TIMER)/BUF_SHORT;
    else R.PEP_period = (1000000/SAMPLE_TIMER)/BUF_SHORT;			

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("PEP sampling period:");

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)pep_menu_items, menu_size, current_selection, 1, 7,3);
    VirtLCDy.setCursor(0,2);
    VirtLCDy.print("Select");

    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("Available periods");
    VirtLCDw.setCursor(0,6);
    VirtLCDw.print("1, 2.5 or 5 seconds");
  }

  // Enact selection
  if (flag.short_push)
  {
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,1);

    flag.short_push = false;                    // Clear pushbutton status

    // Check if selected threshold is not same as previous
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.PEP_period != R.PEP_period)
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;                      // Show on LCD for 1 second
    flag.idle_refresh = true;                   // Force screensaver reprint upon exit
    flag.config_mode = true;                    // Not done yet, bac to Config Menu
    //flag.mode_change = true;                    // Signal for display cleanup
    menu_level = 0;                             // We are done with this menu level
    flag.menu_lcd_upd = false;                  // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}


//--------------------------------------------------------------------
// Display Rotate.  Not really a Menu function
//--------------------------------------------------------------------
void displayrotate_menu()
{
  R.disp.rotate ^= 1;                     // XOR
  EEPROM_writeAnything(1,R);              // Write default settings into EEPROM
  #if SSD1306                             // OLED display version 
  oled.setRotation(2*R.disp.rotate);      // Assert the current value
  oled.clearDisplay();                    // Otherwise we may get a mess :)
  #else
  tft.setRotation(1+2*R.disp.rotate);     // Assert the current value
  tft.fillScreen(ILI9341_BLACK);          // Otherwise we may get a mess :)
  #endif

  VirtLCDw.clear();
  VirtLCDy.clear();
  VirtLCDy.setCursor(1,1);        
  VirtLCDy.print("Display Rotate");
  VirtLCDy.transfer();  
  #if TOUCHSCREEN_ENABLED
  setupTouchButtons();                     // Redraw touch buttons
  #endif
  Menu_exit_timer = 200;                   // Show on LCD for 2 seconds

  menu_level = DISPLAY_BATT_MENU;
  flag.menu_lcd_upd = false;              // Make ready for next time  
}
#if SSD1306 
//--------------------------------------------------------------------
// Screen Dim.  Not really a Menu function
//--------------------------------------------------------------------
void displaydim_menu(void)
{
  VirtLCDw.clear();
  VirtLCDy.clear();
  VirtLCDy.setCursor(1,1);        
  VirtLCDy.print("Display Dim Toggle");
  VirtLCDy.transfer();  
  Menu_exit_timer = 200;                  // Show on LCD for 2 seconds
  R.disp.dimmed ^= 1;                     // XOR
  EEPROM_writeAnything(1,R);              // Write default settings into EEPROM
  oled.dim(R.disp.dimmed);                // Assert the current value
  menu_level = DISPLAY_BATT_MENU;
  flag.menu_lcd_upd = false;              // Make ready for next time
}
#else
//--------------------------------------------------------------------
// TFT Backlight Adjust.
//--------------------------------------------------------------------
void displaydim_menu(void)
{
  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if ((Enc.read()/ENC_RESDIVIDE > 0) && (R.disp.tft_backlight < 10) )  // 10 is max value
    {
      R.disp.tft_backlight++;
    }
    if ((Enc.read()/ENC_RESDIVIDE < 0) && (R.disp.tft_backlight > 0) )  // 0 is min value
    {
      R.disp.tft_backlight--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;            // Keep track of LCD update requirements
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;            // We have serviced LCD

    VirtLCDw.clear();
    VirtLCDy.clear();

    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Display backlight:");
    VirtLCDy.setCursor(0,2);
    VirtLCDy.print("Adjust & Push->");
    sprintf(lcd_buf,"%2u",R.disp.tft_backlight);
    VirtLCDw.setCursor(16,2);
    VirtLCDw.print(lcd_buf);
        
    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("0 min, 10 max");
    //analogWrite(DisplayAwake, 5 + 25*R.disp.tft_backlight); // 5 to 255
  } 
  // Enact selection by saving in EEPROM
  if (flag.short_push)
  {
    flag.short_push = false;             // Clear pushbutton status

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(1,2);
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.disp.tft_backlight != R.disp.tft_backlight)  // New Value
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;               // Show on LCD for 1 second
    flag.idle_refresh = true;            // Force screensaver reprint upon exit
    flag.config_mode = true;             // Not done yet, back to Config Menu
    //flag.mode_change = true;             // Signal for display cleanup
    menu_level = DISPLAY_BATT_MENU;      // We are done with this menu level
    flag.menu_lcd_upd = false;           // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}
#endif

#if AD8307_INSTALLED
//--------------------------------------------------------------------
// Screensaver Threshold Sensitivity selection Menu
//--------------------------------------------------------------------
void screensaverthresh_menu(void)
{
  static int8_t	current_selection;
	
  // Get Current value
  // 0=0, 1uW=0.001=>1, 10uW=0.01=>2... 100uW=>3, 1mW=>4, 10mW=>5
  if (R.idle_disp_thresh == 0) current_selection = 0;
  //else current_selection = log10(R.idle_disp_thresh) + 4; // Roundoff error
  else if (R.idle_disp_thresh == 0.001) current_selection = 1;
  else if (R.idle_disp_thresh == 0.01)  current_selection = 2;
  else if (R.idle_disp_thresh == 0.1)   current_selection = 3;
  else if (R.idle_disp_thresh == 1)     current_selection = 4;
  else if (R.idle_disp_thresh == 10)    current_selection = 5;

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                   // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = screenthresh_menu_size;
    while(current_selection >= menu_size)
    current_selection -= menu_size;
    while(current_selection < 0)
    current_selection += menu_size;

    // Update with currently selected value
    // 0=0, 1uW=0.001=>1, 10uW=0.01=>2... 100uW=>3, 1mW=>4, 10mW=>5
    if (current_selection == 0) R.idle_disp_thresh = 0;
    //else R.idle_disp_thresh = pow(10,current_selection - 4); // Roundoff error
    else if (current_selection == 1) R.idle_disp_thresh = 0.001;
    else if (current_selection == 2) R.idle_disp_thresh = 0.01;
    else if (current_selection == 3) R.idle_disp_thresh = 0.1;
    else if (current_selection == 4) R.idle_disp_thresh = 1;
    else if (current_selection == 5) R.idle_disp_thresh = 10;
	
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Screensaver");
    VirtLCDy.setCursor(10,1);
    VirtLCDy.print("Threshold:");

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)screenthresh_menu_items, menu_size, current_selection, 2, 7,3);
    VirtLCDy.setCursor(0,3);
    VirtLCDy.print("Select");

    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("Available thresholds");
    VirtLCDw.setCursor(0,6);
    VirtLCDw.print("1uW-10mW,  10x steps");
  }

  // Enact selection
  if (flag.short_push)
  {
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,1);

    flag.short_push = false;                    // Clear pushbutton status

    // Check if selected threshold is not same as previous
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.idle_disp_thresh != R.idle_disp_thresh)// New Value
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;                      // Show on LCD for 1 second
    flag.idle_refresh = true;                   // Force screensaver reprint upon exit
    flag.config_mode = true;                    // Not done yet, bac to Config Menu
    //flag.mode_change = true;                  // Signal for display cleanup
    menu_level = DISPLAY_BATT_MENU;             // We are done with this menu level
    flag.menu_lcd_upd = false;                  // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}
#else
//--------------------------------------------------------------------
// Screensaver Threshold Sensitivity selection Menu
//--------------------------------------------------------------------
void screensaverthresh_menu(void)
{
  VirtLCDw.clear();
  VirtLCDy.clear();
  VirtLCDy.setCursor(1,1);        
  VirtLCDy.print("Nothing Here Yet!!");
  VirtLCDy.transfer();  
  Menu_exit_timer = 200;                  // Show on LCD for 2 seconds
  menu_level = DISPLAY_BATT_MENU;  
  flag.menu_lcd_upd = false;              // Make ready for next time
}
#endif

#if AD8307_INSTALLED
//--------------------------------------------------------------------
// Low Power Floor selection Menu
//--------------------------------------------------------------------
void lowpowerfloor_menu(void)
{
  static int8_t  current_selection;
  
  // Get current value
  // 0=No Floor, 1 = Floor at 1uW, 2 = Floor at 10uW, 3 = Floor at 1mW
  current_selection = R.low_power_floor;

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                   // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = lowpowerfloor_menu_size;
    while(current_selection >= menu_size)
    current_selection -= menu_size;
    while(current_selection < 0)
    current_selection += menu_size;

    // Update with currently selected value
    // 0=No Floor, 1 = Floor at 1uW, 2 = Floor at 10uW, 3 = Floor at 1mW
    R.low_power_floor = current_selection;
  
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Low Power Floor:");

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)lowpowerfloor_menu_items, menu_size, current_selection, 2, 7,3);
    VirtLCDy.setCursor(0,3);
    VirtLCDy.print("Select");

    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("Lowest non-zero");
    VirtLCDw.setCursor(1,6);
    VirtLCDw.print("indication on disp.");
  }

  // Enact selection
  if (flag.short_push)
  {
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,1);

    flag.short_push = false;                    // Clear pushbutton status

    // Check if selected threshold is not same as previous
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.low_power_floor != R.low_power_floor)// New Value
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");

      #if BATTERY_POWER
      // Update snooze-wakeup value according to new floor setting.
      if (R.low_power_floor >= FLOOR_ONE_mW)
        snooze_config.pinMode(WakeOnPwr, CMP, RISING, dBm_to_voltage(0.0));   // pin, mode, type, and threshold(v) set at 1mW
      else if (R.low_power_floor >= FLOOR_TEN_uW)
        snooze_config.pinMode(WakeOnPwr, CMP, RISING, dBm_to_voltage(-20.0)); // pin, mode, type, and threshold(v) set at 10uW
      else
        snooze_config.pinMode(WakeOnPwr, CMP, RISING, dBm_to_voltage(-30.0)); // pin, mode, type, and threshold(v) set at 1uW
      #endif
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;                      // Show on LCD for 1 second
    flag.idle_refresh = true;                   // Force screensaver reprint upon exit
    flag.config_mode = true;                    // Not done yet, bac to Config Menu
    //flag.mode_change = true;                  // Signal for display cleanup
    menu_level = DISPLAY_BATT_MENU;             // We are done with this menu level
    flag.menu_lcd_upd = false;                  // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}
#else
//--------------------------------------------------------------------
// Low Power Floor selection Menu
//--------------------------------------------------------------------
void lowpowerfloor_menu(void)
{
  VirtLCDw.clear();
  VirtLCDy.clear();
  VirtLCDy.setCursor(1,1);        
  VirtLCDy.print("Nothing Here Yet!!");
  VirtLCDy.transfer();  
  Menu_exit_timer = 200;                  // Show on LCD for 2 seconds
  menu_level = DISPLAY_BATT_MENU;  
  flag.menu_lcd_upd = false;              // Make ready for next time
}
#endif


#if BATTERY_POWER
//--------------------------------------------------------------------
// Time to Poweroff when idle
//--------------------------------------------------------------------
void idle_poweroff_menu(void)
{
  
  int8_t  current_selection = 0;

  // Get Current value
  while (OffTime[current_selection] < R.powoff_time) current_selection++;
  
  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;            // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = idle_poweroff_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    // Update with currently selected value
    R.powoff_time = OffTime[current_selection];

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Power Off Time when");
    VirtLCDy.setCursor(0,1);
    VirtLCDy.print("idle and on battery:");

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)idle_poweroff_menu_items, menu_size, current_selection, 3, 3, 4);
  }
  
  // Enact selection
  if (flag.short_push)
  {
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,1);

    flag.short_push = false;             // Clear pushbutton status

    // Check if selected threshold is not same as previous
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.powoff_time != R.powoff_time) // New Value
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;               // Show on LCD for 1 seconds
    flag.idle_refresh = true;            // Force screensaver reprint upon exit
    flag.config_mode = true;             // Not done yet, bac to Config Menu
    //flag.mode_change = true;           // Signal for display cleanup
    menu_level = DISPLAY_BATT_MENU;      // We are done with this menu level
    flag.menu_lcd_upd = false;           // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}

//--------------------------------------------------------------------
// Battery Shutoff Voltage Adjust Menu
//--------------------------------------------------------------------
void shutoffvoltage_menu(void)
{
  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if ((Enc.read()/ENC_RESDIVIDE > 0) && (R.loBatt < 40) )  // 4.0 Volts is max value
    {
      R.loBatt++;
    }
    if ((Enc.read()/ENC_RESDIVIDE < 0) && (R.loBatt > 33) )  // 3.3 Volts is min value
    {
      R.loBatt--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;            // Keep track of LCD update requirements
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;            // We have serviced LCD

    VirtLCDw.clear();
    VirtLCDy.clear();

    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Battery shutoff");
    VirtLCDy.setCursor(0,1);
    VirtLCDy.print("voltage threshold:");
    VirtLCDy.setCursor(0,3);
    VirtLCDy.print("Adjust & Push->");
    VirtLCDw.setCursor(16,3);
    sprintf(lcd_buf,"%1u.%01u",R.loBatt/10, R.loBatt%10);
    VirtLCDw.print(lcd_buf);
        
    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("3.3V min, 4.0V max");
  } 
  // Enact selection by saving in EEPROM
  if (flag.short_push)
  {
    flag.short_push = false;             // Clear pushbutton status

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(1,2);
    EEPROM_readAnything(1,eeprom_R);
    if (eeprom_R.loBatt != R.loBatt)  // New Value
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");

    Menu_exit_timer = 100;               // Show on LCD for 1 second
    flag.idle_refresh = true;            // Force screensaver reprint upon exit
    flag.config_mode = true;             // Not done yet, back to Config Menu
    //flag.mode_change = true;             // Signal for display cleanup
    menu_level = DISPLAY_BATT_MENU;      // We are done with this menu level
    flag.menu_lcd_upd = false;           // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}
#endif

//--------------------------------------------------------------------
// Screen and Battery Management
//--------------------------------------------------------------------
void display_batt_menu(void)
{
  static int8_t  current_selection;                      // Keep track of current LCD menu selection

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();
    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                            // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = display_bat_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    VirtLCDw.clear();
    VirtLCDy.clear();

    // Print the Rotary Encoder scroll Menu

    #if BATTERY_POWER
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Display and Battery");
    VirtLCDy.setCursor(0,1);
    VirtLCDy.print("management functions");
    #else
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Display");
    VirtLCDy.setCursor(0,1);
    VirtLCDy.print("management functions");
    #endif

    lcd_scroll_Menu((char**)display_bat_menu_items, menu_size, current_selection,3, 0,4);
  }

  // Enact selection
  if (flag.short_push)
  {
    flag.short_push = false;                             // Clear pushbutton status

    switch (current_selection)
    {
      case 0:
        menu_level = DISPLAYROTATE_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 1:
        menu_level = DISPLAYDIM_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 2:
        menu_level = SCREENSAVERTHRESH_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 3:
        menu_level = LOWPOWERFLOOR_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      #if !BATTERY_POWER
      case 4:
        menu_level = 0;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        VirtLCDw.clear();
        VirtLCDy.clear();
        #if BATTERY_POWER        
        VirtLCDy.setCursor(0,1);
        VirtLCDy.print("Done w. Display&Batt");
        #else
        VirtLCDy.setCursor(1,1);
        VirtLCDy.print("Done w. Display");
        #endif
        Menu_exit_timer = 100;                           // Show on LCD for 1 second
        flag.idle_refresh = true;                        // Force screensaver reprint upon exit
        flag.config_mode = true;                        // We're not done yet
        //flag.mode_change = true;                         // Signal for display cleanup
        menu_level = 0;                                  // We are done with this menu level
        flag.menu_lcd_upd = false;                       // Make ready for next time
        break;
      #endif   
      #if BATTERY_POWER
      case 4:
        menu_level = IDLE_POWEROFF_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 5:
        menu_level = SHUTOFFVOLTAGE_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 6:
        menu_level = 0;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        VirtLCDw.clear();
        VirtLCDy.clear();
        #if BATTERY_POWER        
        VirtLCDy.setCursor(0,1);
        VirtLCDy.print("Done w. Display&Batt");
        #else
        VirtLCDy.setCursor(1,1);
        VirtLCDy.print("Done w. Display");
        #endif
        Menu_exit_timer = 100;                           // Show on LCD for 1 second
        flag.idle_refresh = true;                        // Force screensaver reprint upon exit
        flag.config_mode = true;                        // We're not done yet
        //flag.mode_change = true;                         // Signal for display cleanup
        menu_level = 0;                                  // We are done with this menu level
        flag.menu_lcd_upd = false;                       // Make ready for next time
        break;
      #endif
      default:
        VirtLCDw.clear();
        VirtLCDy.clear();
        #if BATTERY_POWER        
        VirtLCDy.setCursor(0,1);
        VirtLCDy.print("Done w. Display&Batt");
        #else
        VirtLCDy.setCursor(1,1);
        VirtLCDy.print("Done w. Display");
        #endif
        Menu_exit_timer = 100;                           // Show on LCD for 1 second
        flag.idle_refresh = true;                        // Force screensaver reprint upon exit
        flag.config_mode = false;                        // We're done, exit
        flag.mode_change = true;    // Signal for display cleanup
        flag.mode_display = false;
        menu_level = 0;                                  // We are done with this menu level
        flag.menu_lcd_upd = false;                       // Make ready for next time
        break;
    }
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}



//--------------------------------------------------------------------
// Scale Range Setup Submenu functions
//--------------------------------------------------------------------
void scalerange_menu_level2(void)
{
  
  #if SSD1306                                   // OLED display version 
  static double	current_selection;              // Keep track of current LCD menu selection
  uint8_t scale_set;                            // Determine whether CAL_SET0, CAL_SET1 or CAL_SET2

  if (menu_level == SCALE_SET0_MENU) scale_set = 0;      // SCALE_SET0_MENU
  else if (menu_level == SCALE_SET1_MENU) scale_set = 1; // SCALE_SET1_MENU
  else scale_set = 2;                                    // SCALE_SET2_MENU

  // Get Current value
  current_selection = R.ScaleRange[scale_set];

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection += .1;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection -= .1;
    }
    // Reset data from Encoder
    Enc.clearCount();
    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                            // We are about to have serviced LCD

    // Keep Encoder Selection Within Scale Range Bounds
    int16_t max_range = R.ScaleRange[0] * 10.0;          // Highest permissible Scale Range for ranges 2 and 3,
    if (max_range > 99.9) max_range = 99.9;              // never larger than 10x range 1 and never larger than 99.9
    int16_t min_range = R.ScaleRange[0];                 // Lowest permissible Scale Range for ranges 2 and 3,
                                                         // never smaller than range 1
    if (scale_set==0)
    {
      // Set bounds for Range 1 adjustments
      if(current_selection > 99.9) current_selection = 99.9; // Range 1 can take any value between 1 and 99
      if(current_selection < 1.0) current_selection = 1.0;			
    }
    if (scale_set>0)
    {
      // Set bounds for Range 2 and 3 adjustments
      if(current_selection > max_range) current_selection = max_range;
      if(current_selection < min_range) current_selection = min_range;
    }
    // Store Current value in running storage
    R.ScaleRange[scale_set] = current_selection;
    
    //
    // Bounds dependencies check and adjust
    //
    // Ranges 2 and 3 cannot ever be larger than 9.9 times Range 1
    // Range 2 is equal to or larger than Range 1
    // Range 3 is equal to or larger than Range 2
    // If two Ranges are equal, then only two Scale Ranges in effect
    // If all three Ranges are equal, then only one Range is in effect
    // If Range 1 is being adjusted, Ranges 2 and 3 can be pushed up or down as a consequence
    // If Range 2 is being adjusted up, Range 3 can be pushed up
    // If Range 3 is being adjusted down, Range 2 can be pushed down
    if (R.ScaleRange[1] >= R.ScaleRange[0]*10) R.ScaleRange[1] = R.ScaleRange[0]*10 - 1;
    if (R.ScaleRange[2] >= R.ScaleRange[0]*10) R.ScaleRange[2] = R.ScaleRange[0]*10 - 1;
    // Ranges 2 and 3 cannot be smaller than Range 1			
    if (R.ScaleRange[1] < R.ScaleRange[0]) R.ScaleRange[1] = R.ScaleRange[0];
    if (R.ScaleRange[2] < R.ScaleRange[0]) R.ScaleRange[2] = R.ScaleRange[0];

    // Adjustment up of Range 2 can push Range 3 up
    if ((scale_set == 1) && (R.ScaleRange[1] > R.ScaleRange[2])) R.ScaleRange[2] = R.ScaleRange[1];
    // Adjustment down of Range 3 can push Range 2 down:
    if ((scale_set == 2) && (R.ScaleRange[2] < R.ScaleRange[1])) R.ScaleRange[1] = R.ScaleRange[2];

    VirtLCDw.clear();
    VirtLCDy.clear();

    // Populate the Display - including current values selected for scale ranges
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Adjust, Push to Set:");

    VirtLCDw.setCursor(4,3);
    sprintf(lcd_buf,"1st Range = %2.1f",R.ScaleRange[0]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(4,4);
    sprintf(lcd_buf,"2nd Range = %2.1f",R.ScaleRange[1]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(4,5);
    sprintf(lcd_buf,"3rd Range = %2.1f",R.ScaleRange[2]);
    VirtLCDw.print(lcd_buf);

    // Place "===>" in front of the "ScaleRange" currently being adjusted
    VirtLCDy.setCursor(0,scale_set+3);
    VirtLCDy.print("===>");
  }
  #else                                         // TFT version
  static int16_t  current_selection;            // Keep track of current LCD menu selection
  uint8_t scale_set;                            // Determine whether CAL_SET0, CAL_SET1 or CAL_SET2

  if (menu_level == SCALE_SET0_MENU) scale_set = 0;      // SCALE_SET0_MENU
  else if (menu_level == SCALE_SET1_MENU) scale_set = 1; // SCALE_SET1_MENU
  else scale_set = 2;                                    // SCALE_SET2_MENU

  // Get Current value
  current_selection = R.ScaleRange[scale_set];

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();
    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                            // We are about to have serviced LCD

    // Keep Encoder Selection Within Scale Range Bounds
    int16_t max_range = R.ScaleRange[0] * 10;            // Highest permissible Scale Range for ranges 2 and 3,
    if (max_range > 99) max_range = 99;                  // never larger than 10x range 1 and never larger than 99
    int16_t min_range = R.ScaleRange[0];                 // Lowest permissible Scale Range for ranges 2 and 3,
                                                         // never smaller than range 1
    if (scale_set==0)
    {
      // Set bounds for Range 1 adjustments
      if(current_selection > 99) current_selection = 99; // Range 1 can take any value between 1 and 99
      if(current_selection < 1) current_selection = 1;      
    }
    if (scale_set>0)
    {
      // Set bounds for Range 2 and 3 adjustments
      if(current_selection > max_range) current_selection = max_range;
      if(current_selection < min_range) current_selection = min_range;
    }
    // Store Current value in running storage
    R.ScaleRange[scale_set] = current_selection;

    //
    // Bounds dependencies check and adjust
    //
    // Ranges 2 and 3 cannot ever be larger than 9.9 times Range 1
    // Range 2 is equal to or larger than Range 1
    // Range 3 is equal to or larger than Range 2
    // If two Ranges are equal, then only two Scale Ranges in effect
    // If all three Ranges are equal, then only one Range is in effect
    // If Range 1 is being adjusted, Ranges 2 and 3 can be pushed up or down as a consequence
    // If Range 2 is being adjusted up, Range 3 can be pushed up
    // If Range 3 is being adjusted down, Range 2 can be pushed down
    if (R.ScaleRange[1] >= R.ScaleRange[0]*10) R.ScaleRange[1] = R.ScaleRange[0]*10 - 1;
    if (R.ScaleRange[2] >= R.ScaleRange[0]*10) R.ScaleRange[2] = R.ScaleRange[0]*10 - 1;
    // Ranges 2 and 3 cannot be smaller than Range 1      
    if (R.ScaleRange[1] < R.ScaleRange[0]) R.ScaleRange[1] = R.ScaleRange[0];
    if (R.ScaleRange[2] < R.ScaleRange[0]) R.ScaleRange[2] = R.ScaleRange[0];

    // Adjustment up of Range 2 can push Range 3 up
    if ((scale_set == 1) && (R.ScaleRange[1] > R.ScaleRange[2])) R.ScaleRange[2] = R.ScaleRange[1];
    // Adjustment down of Range 3 can push Range 2 down:
    if ((scale_set == 2) && (R.ScaleRange[2] < R.ScaleRange[1])) R.ScaleRange[1] = R.ScaleRange[2];

    VirtLCDw.clear();
    VirtLCDy.clear();

    // Populate the Display - including current values selected for scale ranges
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Adjust, Push to Set:");

    VirtLCDw.setCursor(6,3);
    sprintf(lcd_buf,"1st Range = %2u",R.ScaleRange[0]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(6,4);
    sprintf(lcd_buf,"2nd Range = %2u",R.ScaleRange[1]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(6,5);
    sprintf(lcd_buf,"3rd Range = %2u",R.ScaleRange[2]);
    VirtLCDw.print(lcd_buf);

    // Place "===>" in front of the "ScaleRange" currently being adjusted
    VirtLCDy.setCursor(0,scale_set+3);
    VirtLCDy.print("===>");
  }
  #endif

  // Enact selection by saving in EEPROM
  if (flag.short_push)
  {
    flag.short_push = false;                             // Clear pushbutton status
    VirtLCDy.clear();
    VirtLCDy.setCursor(1,1);

    // Save modified value
    // There are so many adjustable values that it is simplest just to assume
    // a value has always been modified.  Save all 3
    EEPROM_writeAnything(1,R);
    VirtLCDy.print("Value Stored");
    Menu_exit_timer = 100;                               // Show on LCD for 1 second
    flag.idle_refresh = true;                            // Force screensaver reprint upon exit
    flag.config_mode = true;                             // We're NOT done, just backing off
    menu_level = SCALERANGE_MENU;                        // We are done with this menu level
    flag.menu_lcd_upd = false;                           // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}



//--------------------------------------------------------------------
// Scale Range Menu functions
//--------------------------------------------------------------------
void scalerange_menu(void)
{
  static int8_t	current_selection;                       // Keep track of current LCD menu selection

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();
    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                            // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = 4;
    while(current_selection >= menu_size)
    current_selection -= menu_size;
    while(current_selection < 0)
    current_selection += menu_size;

    VirtLCDw.clear();
    VirtLCDy.clear();

    // Populate the Display - including current values selected for scale ranges
    VirtLCDy.setCursor(0,0);
    if (current_selection<3)
      VirtLCDy.print("Select Scale Range:");
    else
    {
      VirtLCDy.print("Left / Right or");
      VirtLCDy.setCursor(0,1);
      VirtLCDy.print("Select to exit");
    }

    #if SSD1306               // OLED display version 
    VirtLCDw.setCursor(4,3);
    sprintf(lcd_buf,"1st Range = %2.1f",R.ScaleRange[0]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(4,4);
    sprintf(lcd_buf,"2nd Range = %2.1f",R.ScaleRange[1]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(4,5);
    sprintf(lcd_buf,"3rd Range = %2.1f",R.ScaleRange[2]);
    VirtLCDw.print(lcd_buf);
    #else
    VirtLCDw.setCursor(6,3);
    sprintf(lcd_buf,"1st Range = %2u",R.ScaleRange[0]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(6,4);
    sprintf(lcd_buf,"2nd Range = %2u",R.ScaleRange[1]);
    VirtLCDw.print(lcd_buf);
    VirtLCDw.setCursor(6,5);
    sprintf(lcd_buf,"3rd Range = %2u",R.ScaleRange[2]);
    VirtLCDw.print(lcd_buf);
    #endif

    // Place "->" in front of the relevant "ScaleRange" to be selected with a push
    if (current_selection<3)
    {
      VirtLCDy.setCursor(2,current_selection+3);
      VirtLCDy.print("->");
    }
  }

  // Enact selection
  if (flag.short_push)
  {
    flag.short_push = false;                             // Clear pushbutton status

    switch (current_selection)
    {
      case 0:
        menu_level = SCALE_SET0_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 1:
        menu_level = SCALE_SET1_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 2:
        menu_level = SCALE_SET2_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      default:
        VirtLCDw.clear();
        VirtLCDy.clear();
        VirtLCDy.setCursor(0,1);
        VirtLCDy.print("Done w. Scale Ranges");
        Menu_exit_timer = 100;                           // Show on LCD for 1 second
        flag.idle_refresh = true;                        // Force screensaver reprint upon exit
        flag.config_mode = true;                         // Not done yet, bac to Config Menu
        //flag.mode_change = true;                         // Signal for display cleanup
        menu_level = 0;                                  // We are done with this menu level
        flag.menu_lcd_upd = false;                       // Make ready for next time
    }
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}


#if !SSD1306
//--------------------------------------------------------------------
// Modulation Scope scan period selection Menu
//--------------------------------------------------------------------
void modscope_menu(void)
{
  static int8_t  current_selection;

  // Get Current value
  if (R.modscopeDivisor > 5) current_selection = 6;  // Valid are 1 - 5 and 10
  else current_selection = R.modscopeDivisor;

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;         // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = modscope_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    if      (current_selection == 6) R.modscopeDivisor = 10;
    else R.modscopeDivisor = current_selection;

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Modulation Scope");
    VirtLCDy.setCursor(8,1);
    VirtLCDy.print("Scan Period:");
    VirtLCDy.setCursor(0,3);
    VirtLCDy.print("Select");

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)modscope_menu_items, menu_size, current_selection, 2, 7,3);

    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("Available Scan Rates");
    VirtLCDw.setCursor(0,6);
    #if F_CPU >= 144000000            // CPU is overclocked at 144 MHz
    VirtLCDw.print("0.2 to 2 seconds");
    #else                             // CPU is at normal speed
    VirtLCDw.print("0.3 to 3 seconds");
    #endif
  }

  // Enact selection
  if (flag.short_push)
  {
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,1);

    flag.short_push = false;                    // Clear pushbutton status

    // Check if selected threshold is not same as previous
    EEPROM_readAnything(1,eeprom_R);
    if ((current_selection > 0) && (eeprom_R.modscopeDivisor != R.modscopeDivisor))
    {
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
      ModScope.rate(R.modscopeDivisor);         // Update Modulation scope rate divisor
    }
    else                                        // Either 0 for Exit, or no change
    {
      R.modscopeDivisor = eeprom_R.modscopeDivisor;
      VirtLCDy.print("Nothing Changed");
    }

    Menu_exit_timer = 100;                      // Show on LCD for 1 second
    flag.idle_refresh = true;                   // Force screensaver reprint upon exit
    flag.config_mode = true;                    // Not done yet, bac to Config Menu
    //flag.mode_change = true;                    // Signal for display cleanup
    menu_level = 0;                             // We are done with this menu level
    flag.menu_lcd_upd = false;                  // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}
#endif

#if AD8307_INSTALLED
//--------------------------------------------------------------------
// Calibrate Submenu functions
//--------------------------------------------------------------------
void calibrate_menu_level2(void)
{
  static int16_t  current_selection;                     // Keep track of current LCD menu selection

  uint8_t cal_set;                                       // Determine whether CAL_SET0, CAL_SET1 or CAL_SET2

  if (menu_level == CAL_SET2_MENU) cal_set = 1;	         // CAL_SET2_MENU
  else cal_set = 0;                                      // CAL_SET0_MENU or CAL_SET1_MENU

  // These defines to aid readability of code
  #define CAL_BAD	0                                      // Input signal of insufficient quality for calibration
  #define CAL_FWD	1                                      // Good input signal detected, forward direction
  #define CAL_REV	2                                      // Good input signal detected, reverse direction (redundant)
  // Below variable can take one of the three above defined values, based on the
  // detected input "calibration" signal
  static uint8_t cal_sig_direction_quality;
  
  // Get Current value
  current_selection = R.cal_AD[cal_set].db10m;

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();
    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // Determine direction and level of calibration signal input
  // Check forward direction and sufficient level
  if ( ((adc_ref*(fwd - rev)/4096.0 * 1000/LOGAMP2_SLOPE) > CAL_INP_QUALITY)
      && (cal_sig_direction_quality != CAL_FWD))
  {
    cal_sig_direction_quality = CAL_FWD;
    flag.menu_lcd_upd = false;                           // Indicate that an LCD update is needed
  }
  // Check reverse direction and sufficient level
  else if ( ((adc_ref*(rev - fwd)/4096.0 * 1000/LOGAMP1_SLOPE) > CAL_INP_QUALITY)
           && (cal_sig_direction_quality != CAL_REV))
  {
    cal_sig_direction_quality = CAL_REV;
    flag.menu_lcd_upd = false;                           // Indicate that an LCD update is needed
  }
  // Check insufficient level
  else if (  (adc_ref*(ABS((fwd - rev))/4096.0 * 1000/LOGAMP2_SLOPE) <= CAL_INP_QUALITY)
           && (cal_sig_direction_quality != CAL_BAD))
  {
    cal_sig_direction_quality = CAL_BAD;
    flag.menu_lcd_upd = false;                           // Indicate that an LCD update is needed
  }
  
  // If LCD update is needed
  if(!flag.menu_lcd_upd)  
  {
    flag.menu_lcd_upd = true;                            // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    int16_t max_value = 530;                             // Highest permissible Calibration value in dBm * 10
    int16_t min_value = -100;                            // Lowest permissible Calibration value in dBm * 10
    if(current_selection > max_value) current_selection = max_value;
    if(current_selection < min_value) current_selection = min_value;

    // Store Current value in running storage
    R.cal_AD[cal_set].db10m = current_selection;

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);	

    if (menu_level == CAL_SET0_MENU)                     // equals cal_set == 0
    {
      VirtLCDy.print("Single Level Cal:");
    }
    else if (menu_level == CAL_SET1_MENU)                // equals cal_set == 1
    {
      VirtLCDy.print("First Cal SetPoint:");
    }
    else if (menu_level == CAL_SET2_MENU)
    {
      VirtLCDy.print("Second Cal SetPoint:");
    }

    VirtLCDy.setCursor(0,2);
    VirtLCDy.print("Adjust (dBm)->");
    VirtLCDw.setCursor(14,2);
    // Format and print current value
    int16_t val_sub = current_selection;
    int16_t val = val_sub / 10;
    val_sub = val_sub % 10;
    if (current_selection < 0)
    {
      val*=-1;
      val_sub*=-1;
      sprintf(lcd_buf," -%1u.%01u",val, val_sub);
    }
    else
    {
      sprintf(lcd_buf," %2u.%01u",val, val_sub);
    }
    VirtLCDw.print(lcd_buf);

    if (cal_sig_direction_quality == CAL_FWD)
    {
      VirtLCDw.setCursor(0,4);
      VirtLCDw.print(">Push to set<");
      VirtLCDy.setCursor(0,5);
      VirtLCDy.print("Signal detected");
    }
    else if (cal_sig_direction_quality == CAL_REV)
    {
      VirtLCDw.setCursor(0,4);
      VirtLCDw.print(">Push to set<");
      VirtLCDy.setCursor(0,5);
      VirtLCDy.print("Reverse detected");
    }
    else                                                 // cal_sig_direction_quality == CAL_BAD
    {
      VirtLCDw.setCursor(0,4);
      VirtLCDw.print(">Push to exit<");
      VirtLCDy.setCursor(0,5);
      VirtLCDy.print("Poor signal quality");
    }
  }

  // Enact selection by saving in EEPROM
  if (flag.short_push)
  {
    flag.short_push = false;                             // Clear pushbutton status
    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDw.transfer();
    VirtLCDy.transfer();
    VirtLCDy.setCursor(1,1);

    // Save modified value
    // If forward direction, then we calibrate for both, using the measured value for
    // in the forward direction only
    if (cal_sig_direction_quality == CAL_FWD)
    {
      if (menu_level == CAL_SET0_MENU)
      {
        R.cal_AD[0].Fwd = adc_ref * fwd/4096.0;
        R.cal_AD[0].Rev = R.cal_AD[0].Fwd;
        // Set second calibration point at 30 dB less
        R.cal_AD[1].db10m = R.cal_AD[0].db10m - 300;
        R.cal_AD[1].Fwd = R.cal_AD[0].Fwd - LOGAMP1_SLOPE * .001 * 30;
        R.cal_AD[1].Rev = R.cal_AD[0].Fwd - LOGAMP2_SLOPE * .001 * 30;
      }
      else
      {
        R.cal_AD[cal_set].Fwd = adc_ref * fwd/4096.0;
        R.cal_AD[cal_set].Rev = R.cal_AD[cal_set].Fwd;
      }
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    // If reverse, then we calibrate for reverse direction only
    else if (cal_sig_direction_quality == CAL_REV)
    {
      if (menu_level == CAL_SET0_MENU)
      {
        R.cal_AD[0].Rev = adc_ref * rev/4096.0;
        // Set second calibration point at 30 dB less
        R.cal_AD[1].Rev = R.cal_AD[0].Rev - LOGAMP2_SLOPE * .001 * 30;
      }
      else
      {
        R.cal_AD[cal_set].Rev = adc_ref * rev/4096.0;
      }
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else                                                 // cal_sig_direction_quality == CAL_BAD
    {
      VirtLCDy.print("Nothing changed");
    }

    Menu_exit_timer = 100;                               // Show on LCD for 1 second
    flag.idle_refresh = true;                            // Force screensaver reprint upon exit
    flag.config_mode = true;                             // We're NOT done, just backing off
    menu_level = CAL_MENU;                               // We are done with this menu level
    flag.menu_lcd_upd = false;                           // Make ready for next time
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}



//--------------------------------------------------------------------
// Calibrate Menu functions
//--------------------------------------------------------------------
void calibrate_menu(void)
{
  static int8_t	current_selection;                       // Keep track of current LCD menu selection

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();
    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                            // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = calibrate_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    VirtLCDw.clear();
    VirtLCDy.clear();

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)calibrate_menu_items, menu_size, current_selection,1, 0,3);

    switch (current_selection)
    {
      case 0:
        VirtLCDw.setCursor(0,5);
        VirtLCDw.print("Calibrate using one");
        VirtLCDw.setCursor(6,6);
        VirtLCDw.print("accurate level");
        break;
      case 1:
        VirtLCDw.setCursor(0,5);
        VirtLCDw.print("Set higher of two");
        VirtLCDw.setCursor(5,6);
        VirtLCDw.print("accurate levels");
        break;
      case 2:
        VirtLCDw.setCursor(0,5);
        VirtLCDw.print("Set lower of two");
        VirtLCDw.setCursor(5,6);
        VirtLCDw.print("accurate levels");
        break;
    }

    // Indicate Current value stored under the currently selected GainPreset
    // The "stored" value indication changes according to which GainPreset is currently selected.
    VirtLCDy.setCursor(0,0);				
    VirtLCDy.print("Calibrate");
    if (current_selection <= 2)
    {
      int16_t value=0;

      switch (current_selection)
      {
        case 0:
        case 1:
          value = R.cal_AD[0].db10m;
          break;
        case 2:
          value = R.cal_AD[1].db10m;
          break;
      }
      int16_t val_sub = value;
      int16_t val = val_sub / 10;
      val_sub = val_sub % 10;

      // Print value of currently indicated reference
      VirtLCDw.setCursor(16,0);
      if (value < 0)
      {
        val*=-1;
        val_sub*=-1;
        sprintf(lcd_buf,"-%1u.%01u",val, val_sub);
      }
      else
      {
        sprintf(lcd_buf,"%2u.%01u",val, val_sub);
      }
      VirtLCDw.print(lcd_buf);
    }
    else
    {
      VirtLCDw.setCursor(16,0);
      VirtLCDw.print(" --");
    }
  }

  // Enact selection
  if (flag.short_push)
  {
    flag.short_push = false;                             // Clear pushbutton status

    switch (current_selection)
    {
      case 0:
        menu_level = CAL_SET0_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 1:
        menu_level = CAL_SET1_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      case 2:
        menu_level = CAL_SET2_MENU;
        flag.menu_lcd_upd = false;                       // force LCD reprint
        break;
      default:
        VirtLCDw.clear();
        VirtLCDy.clear();
        VirtLCDy.setCursor(1,1);				
        VirtLCDy.print("Done w. Cal");
        Menu_exit_timer = 100;                           // Show on LCD for 1 second
        flag.idle_refresh = true;                        // Force screensaver reprint upon exit
        flag.config_mode = true;                         // We're NOT done, just backing off
        //flag.mode_change = true;                         // Signal for display cleanup
        menu_level = 0;                                  // We are done with this menu level
        flag.menu_lcd_upd = false;                       // Make ready for next time
        break;
    }
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}
#else
//--------------------------------------------------------------------
// Calibrate Menu functions
//--------------------------------------------------------------------
void calibrate_menu(void)
{
  static uint8_t   current_selection;
  char   range[5];

  // We want LCD update every time - to show Power measurement
  flag.menu_lcd_upd = false;
 
  // Calibration multiplier for diode detector type Power/SWR meter, 100 = 1.0
  current_selection = R.meter_cal;

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if ((Enc.read()/ENC_RESDIVIDE > 0) && (current_selection < 250) )
    {
      current_selection++;
    }
    if ((Enc.read()/ENC_RESDIVIDE < 0) && (current_selection > 10) )
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    R.meter_cal = current_selection;   
  }

  VirtLCDw.clear();
  VirtLCDy.clear();

  VirtLCDy.setCursor(0,0);
  VirtLCDy.print("Meter Calibrate:");
  VirtLCDy.setCursor(0,2);
  VirtLCDy.print("Adjust->   ");
  
  VirtLCDw.setCursor(11,2);
  sprintf(lcd_buf,"%1u.%02u",current_selection/100, current_selection%100);
  VirtLCDw.print(lcd_buf);
 					
  VirtLCDw.setCursor(0,6);
  VirtLCDw.print("Range is 0.10 - 2.50");
  
  //measure_power_and_swr();
  VirtLCDw.setCursor(0,4);
  VirtLCDw.print("MeasuredPower:");
  #if SSD1306                            // OLED display version 
  print_p_mw(power_mw, range);
  #else
  print_p_mw(power_mw);
  #endif
  VirtLCDy.setCursor(14,4);
  VirtLCDy.print(lcd_buf);
  VirtLCDy.print(range);
  	
  // Enact selection by saving in EEPROM
  if (flag.short_push)
  {
    flag.short_push = false;             // Clear pushbutton status

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(1,1);
    EEPROM_readAnything(1,R);
    if (R.meter_cal != current_selection)// New Value
    {
      R.meter_cal = current_selection;
      EEPROM_writeAnything(1,R);
      VirtLCDy.print("Value Stored");
    }
    else VirtLCDy.print("Nothing Changed");
    
    Menu_exit_timer = 100;               // Show on LCD for 1 second
    flag.idle_refresh = true;            // Force screensaver reprint upon exit
    flag.config_mode = true;             // Not done yet, bac to Config Menu
    //flag.mode_change = true;             // Signal for display cleanup
    menu_level = 0;                      // We are done with this menu level
  }
  
}
#endif


//--------------------------------------------------------------------
// Factory Reset with all default values
//--------------------------------------------------------------------
void factory_menu(void)
{
  static int8_t	current_selection;

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();

    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  // If LCD update is needed
  if (!flag.menu_lcd_upd)
  {
    flag.menu_lcd_upd = true;                            // We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = factory_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("All to Default?");

    // Print the Rotary Encoder scroll Menu
    lcd_scroll_Menu((char**)factory_menu_items, menu_size, current_selection, 2, 0,2);

    VirtLCDw.setCursor(0,5);
    VirtLCDw.print("Factory Reset Menu");
  }

  // Enact selection
  if (flag.short_push)
  {
    flag.short_push = false;                             // Clear pushbutton status

    switch (current_selection)
    {
      case 0:                                            // Factory Reset
        // Force an EEPROM update upon reboot by storing 0xfe in the first address
        EEPROM.write(0,0xfe);
        VirtLCDw.clear();
        VirtLCDy.clear();
        VirtLCDy.setCursor(0,0);				
        VirtLCDy.print("Factory Reset");
        VirtLCDy.setCursor(0,2);
        VirtLCDy.print("All default");
        Menu_exit_timer = 200;                           // Show on LCD for 2 seconds
        flag.idle_refresh = true;                        // Force screensaver reprint upon exit
        VirtLCDw.transfer();
        VirtLCDy.transfer();
        SOFT_RESET();
      default:
        VirtLCDw.clear();
        VirtLCDy.clear();
        VirtLCDy.setCursor(1,1);				
        VirtLCDy.print("Nothing Changed");
        Menu_exit_timer = 100;                           // Show on LCD for 1 second
        flag.idle_refresh = true;                        // Force screensaver reprint upon exit
        flag.config_mode = true;                         // Not done yet, bac to Config Menu
        //flag.mode_change = true;                         // Signal for display cleanup
        menu_level = 0;                                  // We are done with this menu level
        flag.menu_lcd_upd = false;                       // Make ready for next time
        break;
    }
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}


#if BATTERY_POWER
//
//--------------------------------------------------------------------
// Not really a menu function, immediate shutdown
//--------------------------------------------------------------------
//
void poweroff_now(void)
{
  VirtLCDw.clear();
  VirtLCDy.clear();
  VirtLCDy.setCursor(3,5);        
  VirtLCDy.print("Power Off");
  VirtLCDy.transfer();
  delay(2000);                                           // Show on LCD for 2 seconds
  digitalWrite(PowerOn, HIGH);                           // Turn off power
}
#endif

//
//--------------------------------------------------------------------
// Manage the first level of Menus
//--------------------------------------------------------------------
//
void menu_level0(void)
{
  static int8_t	current_selection;	// Keep track of current LCD menu selection

  // Selection modified by encoder.  We remember last selection, even if exit and re-entry
  if (Enc.read()/ENC_RESDIVIDE != 0)
  {
    if (Enc.read()/ENC_RESDIVIDE > 0)
    {
      current_selection++;
    }
    else if (Enc.read()/ENC_RESDIVIDE < 0)
    {
      current_selection--;
    }
    // Reset data from Encoder
    Enc.clearCount();
    // Indicate that an LCD update is needed
    flag.menu_lcd_upd = false;
  }

  if (!flag.menu_lcd_upd)		        // Need to update LCD
  {
    flag.menu_lcd_upd = true;				// We have serviced LCD

    // Keep Encoder Selection Within Bounds of the Menu Size
    uint8_t menu_size = level0_menu_size;
    while(current_selection >= menu_size)
      current_selection -= menu_size;
    while(current_selection < 0)
      current_selection += menu_size;

    VirtLCDw.clear();
    VirtLCDy.clear();
    VirtLCDy.setCursor(0,0);
    VirtLCDy.print("Config Menu:");

    // Print the Menu
    lcd_scroll_Menu((char**)level0_menu_items, menu_size, current_selection,2, 0,4);
  }

  if (flag.short_push)
  {
    flag.short_push = false;        // Clear pushbutton status
    flag.menu_lcd_upd = false;      //  force LCD reprint
    switch (current_selection)
    {
      //------------------------------------------------------------
      #if SSD1306                   // OLED display version 
      case 0:                       // SWR Alarm Threshold Set
        menu_level = SWR_ALARM_THRESHOLD;
        break;

      case 1:                       // SWR Alarm Power Threshold Set
        menu_level = SWR_ALARM_PWR_THRESHOLD;
        break;

      case 2:                       // PEP sampling period select
        menu_level = PEP_MENU;
        break;

      case 3:                       // Display and Battery
        menu_level = DISPLAY_BATT_MENU;
        break;
      
      case 4:                       // Scale Range Set
        menu_level = SCALERANGE_MENU;
        break;

      case 5:                       // Calibrate
        menu_level = CAL_MENU;
        break;

      case 6:                       // Display Debug stuff
        menu_level = DEBUG_MENU;
        break;

      case 7:                       // Factory Reset
        menu_level = FACTORY_MENU;
        break;

      #if BATTERY_POWER  
      case 8:                       // Shut Down
        menu_level = POWEROFF_NOW;
        break;
      #endif
      
      //------------------------------------------------------------    
      //------------------------------------------------------------
      #else                         // TFT Colour LCD version
      case 0:                       // SWR Alarm Threshold Set
        menu_level = SWR_ALARM_THRESHOLD;
        break;

      case 1:                       // SWR Alarm Power Threshold Set
        menu_level = SWR_ALARM_PWR_THRESHOLD;
        break;

      case 2:                       // PEP sampling period select
        menu_level = PEP_MENU;
        break;

      case 3:                       // Display and Battery
        menu_level = DISPLAY_BATT_MENU;
        break;
      
      case 4:                       // Scale Range Set
        menu_level = SCALERANGE_MENU;
        break;

      case 5:                       // Modulation Scope Scan Rate
        menu_level = MODSCOPE_MENU;
        break;

      case 6:                       // Calibrate
        menu_level = CAL_MENU;
        break;

      case 7:                       // Display Debug stuff
        menu_level = DEBUG_MENU;
        break;

      case 8:                       // Display more Debug stuff
        menu_level = DEBUG2_MENU;
        break;

      case 9:                       // Factory Reset
        menu_level = FACTORY_MENU;
        break;

      #if BATTERY_POWER  
      case 10:                      // Shut Down
        menu_level = POWEROFF_NOW;
        break;          
      #endif

      #endif
      //------------------------------------------------------------      
      default:
        // Exit
        VirtLCDw.clear();
        VirtLCDy.clear();
        VirtLCDy.setCursor(1,1);
        Menu_exit_timer = 100;      // Show on LCD for 1 second
        VirtLCDy.print("Exit from Menu");
        flag.config_mode = false;   // We're done
        flag.mode_change = true;    // Signal for display cleanup
        flag.mode_display = false;
        //flag.mode_display = true;
        flag.power_detected = true; // To wake up from sleep mode, if active - Indicate that Power was detected         
        flag.screensaver_on = false;
        power_timer = 0;            // and seed the power timer (SLEEP_TIME)  
        
    }
    VirtLCDw.transfer();
    VirtLCDy.transfer();
  }
}


//
//--------------------------------------------------------------------
// Scan the Configuraton Menu Status and delegate tasks accordingly
//--------------------------------------------------------------------
//
void ConfigMenu(void)
{
  // Select which menu level to manage
  if (menu_level == 0) menu_level0();

  else if (menu_level == SWR_ALARM_THRESHOLD) swr_alarm_threshold_menu();
  else if (menu_level == SWR_ALARM_PWR_THRESHOLD) swr_alarm_power_threshold_menu();

  else if (menu_level == PEP_MENU) pep_menu();

  else if (menu_level == DISPLAY_BATT_MENU)      display_batt_menu();
  else if (menu_level == DISPLAYROTATE_MENU)     displayrotate_menu();
  else if (menu_level == DISPLAYDIM_MENU)        displaydim_menu();
  else if (menu_level == SCREENSAVERTHRESH_MENU) screensaverthresh_menu();
  else if (menu_level == LOWPOWERFLOOR_MENU)     lowpowerfloor_menu();
  #if BATTERY_POWER
  else if (menu_level == IDLE_POWEROFF_MENU)     idle_poweroff_menu();
  else if (menu_level == SHUTOFFVOLTAGE_MENU)    shutoffvoltage_menu();
  #endif
  
  else if (menu_level == SCALERANGE_MENU) scalerange_menu();
  else if (menu_level == SCALE_SET0_MENU) scalerange_menu_level2();
  else if (menu_level == SCALE_SET1_MENU) scalerange_menu_level2();
  else if (menu_level == SCALE_SET2_MENU) scalerange_menu_level2();

  #if !SSD1306
  else if (menu_level == MODSCOPE_MENU) modscope_menu();
  #endif
  
  else if (menu_level == CAL_MENU) calibrate_menu();
  #if AD8307_INSTALLED
  else if (menu_level == CAL_SET0_MENU) calibrate_menu_level2();
  else if (menu_level == CAL_SET1_MENU) calibrate_menu_level2();
  else if (menu_level == CAL_SET2_MENU) calibrate_menu_level2();
  #endif

  else if (menu_level == DEBUG_MENU) debug_menu();
  #if !SSD1306
  else if (menu_level == DEBUG2_MENU) debug_menu();
  #endif
  
  else if (menu_level == FACTORY_MENU) factory_menu();

  #if BATTERY_POWER
  else if (menu_level == POWEROFF_NOW) poweroff_now();
  #endif

  #if BATTERY_POWER
  //--------------------------------------------------------------------
  // Menu timeout if no activity for 1 minute in battery mode
  static uint32_t menu_timeoutcount;
  static uint16_t old_menu_level = 65535;
  uint8_t  debug_mul;                  // Time multiplier if in Debug Mode
  if (!flag.extpwr || flag.extpwrdrop) // Check if we are running on Battery Power
  {   
    if (menu_level == old_menu_level)  // No change, count and do stuff
    {
      if (menu_level == DEBUG_MENU) debug_mul = 10; // Longer time in Debug Mode
      else debug_mul = 1;
      
      if (menu_timeoutcount >= debug_mul * 60*1000/POLL_TIMER) // 1 minute or longer if in Debug Mode
      {
        menu_level = 0;                // We are done with any menu level
        old_menu_level = 65535;
        menu_timeoutcount = 0;

        //flag.mode_change = true;     // Signal for display cleanup
        flag.mode_display = true;
        flag.menu_lcd_upd = false;          // Indicate that a LCD update is needed upon next entry into Config Mode
        //flag.idle_refresh = true;           // Force screensaver reprint upon exit
        flag.config_mode = false;           // We're done, EXIT
        flag.power_detected = true;         // To wake up from sleep mode, if active - Indicate that Power was detected         
        flag.screensaver_on = false;
        power_timer = 0;                    // and seed the power timer (SLEEP_TIME)     
        menu_level = 0;                     // We are done with this menu level

        VirtLCDw.clear();
        VirtLCDy.clear();
        VirtLCDy.setCursor(5,3);
        Menu_exit_timer = 200;        // Show on LCD for 1 second
        VirtLCDy.print("Timeout");
      }
      else menu_timeoutcount++;
    }
    else
    {
      menu_timeoutcount = 0;
      old_menu_level = menu_level;
    }
  }
  else menu_timeoutcount = 0;         // No timeout if running on USB power
  #endif
}

//*********************************************************************************
//**
//** Project.........: A menu driven Multi Display RF Power and SWR Meter
//**                   using a Tandem Match Coupler and 2x AD8307; or
//**                   diode detectors.
//**
//** Copyright (c) 2015  Loftur E. Jonasson  (tf3lj [at] arrl [dot] net)
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
//** Adapated for ESP32 and LDMOS + LPF management by PA0PHH 2020-10-10
//*********************************************************************************

#include "PSWR_T.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "network.h"

volatile adbuffer_t measure;// Two 256 byte circular buffers carrying the sampled adc-inputs
                            // from the interrupt function to the main loop.
                            
double      adc_ref;        // ADC reference (Teensy or external AD7991)
int16_t     fwd;            // AD input - 12 bit value, v-forward
int16_t     rev;            // AD input - 12 bit value, v-reverse
double      f_inst;         // Calculated Forward voltage
double      r_inst;         // Calculated Reverse voltage
#if AD8307_INSTALLED
double      ad8307_FdBm;    // Measured AD8307 forward voltage in dBm
double      ad8307_RdBm;    // Measured AD8307 reverse current in dBm
#endif
double      fwd_power_mw;   // Calculated forward power in mW
double      ref_power_mw;   // Calculated reflected power in mW
double      power_mw;       // Calculated power in mW
double      power_mw_pk;    // Calculated 100ms peak power in mW
double      power_mw_pep;   // Calculated PEP power in mW (1s, 2.5s or 5s)
double      power_mw_long;  // Calculated MAX power in mW, 30 sec or longer window
double      power_mw_avg;   // Calculated AVG power in mW, 100ms
double      power_mw_1savg; // Calculated AVG power in mW, 1 second
double      power_db;       // Calculated power in dBm
double      power_db_pk;    // Calculated 100ms peak power in dBm
double      power_db_pep;   // Calculated PEP power in dBm
double      power_db_long;  // Calculated MAX power in dBm, 30 sec or longer window
double      swr=1.0;        // SWR as an absolute value
double      swr_avg=1.0;    // SWR average over 10 ms (smoothed value)

uint16_t    menu_level = 0; // Used with PSWRmenu. Keep track of which menu we are in
char        lcd_buf[82];    // Used to process data to be passed to LCD and USB Serial

uint16_t    power_timer;    // Used to stay out of Screensaver
uint16_t    Menu_exit_timer;// Used for a timed display when returning from Menu
uint8_t     mode_display;   // Active display
flags       flag;           // Various op flags
touch_flags touch;          // Touchscreen flags

bool        Reverse;        // BOOL: True if reverse power is greater than forward power
bool        modScopeActive; // BOOL: Feed stuff to the Modulation Scope
volatile bool X_LedState;   // BOOL: Debug LED    

uint8_t     mode_band = 1;   // Band mode active
int         cur_band = 1;   // Band mode active

//-----------------------------------------------------------------------------------------
// Variables in ram/flash rom (default)
var_t  R  = {
              {
                {                       // Meter calibration if 2x AD8307
                  CAL1_NOR_VALUE,       // First calibrate point in 10 x dBm
                  CALFWD1_DEFAULT,      // First Calibrate point, Forward direction, Volts		 
                  CALREV1_DEFAULT       // First Calibrate point, Reverse direction, Volts   
                },
                {  
                  CAL2_NOR_VALUE,       // Second Calibrate point in 10 x dBm
                  CALFWD2_DEFAULT,      // Second Calibrate point, Forward direction, Volts   
                  CALREV2_DEFAULT       // Second Calibrate point, Reverse direction, Volts   
                }
              },                        // Second Calibrate point, Reverse direction, db*10 + 2 x AD values
                                        // Meter calibration if diode detectors
              (uint8_t) METER_CAL*100,  // Calibration fudge of diode detector style meter
              SWR_ALARM,                // Default SWR Alarm trigger, defined in PSWR_A.h
              SWR_THRESHOLD,            // Default SWR Alarm power threshold defined in PSWR_A.h
              0,                        // USB Continuous reporting off
              1,                        // USB Reporting type, 1=Instantaneous Power (raw format) and SWR to USB 
              PEP_PERIOD,               // PEP envelope sampling time in SAMPLE_TIME increments
              0,                        // Default AVG sampling time, 0 for short, 1 for 1 second
              {  
                SCALE_RANGE1,           // User definable Scale Ranges, up to 3 ranges per decade
                SCALE_RANGE2,           // e.g. ... 6W 12W 24W 60W 120W 240W ...
                SCALE_RANGE3            // If all values set as "2", then ... 2W 20W 200W ...
              },
              SLEEPMSG,                 // Shown when nothing else to display on LCD
                                        // Configurable by USB Serial input command: $sleepmsg="blabla"
              SLEEPTHRESHOLD,           // Minimum relevant power to exit Sleep Display (0.001=1uW),
                                        // valid values are 0, 0.001, 0.01, 0.1, 1 and 10
              FLOOR_NOISEFLOOR,         // No low power level threshold, lowest power shown is the effective noise floor
              DEFAULT_MODE,             // Set default Display Mode
              POWER_BARPK,              // Default initial shortcut Display Mode (anything other than MODSCOPE)
              MODSCOPE_DIVISOR,         // Modultion Scope scan rate divisor
                                        // total time of a scan = SAMPLE_TIMER * TFT_x_axis * Divisor
                                        // e.g. 1000us * 300 * 1 = 0.3 seconds for a full sweep
              {
                0,                      // 1 for Upside down, else 0
                7                       // PWM value for tft backlight. 10 max, 0 min
              }
            };
            

//-----------------------------------------------------------------------------------------
// Instanciate an Encoder Object for legacy reasons, always available - whether using it or not
ESP32Encoder   Enc;              // Only used if PUSHBUTTON_ENABLED

//-----------------------------------------------------------------------------------------
// Timers for various tasks:
//Metro     pollMetro = Metro(POLL_TIMER);// Power/SWR calc and TFT LCD printout timer
//Metro     pushMetro = Metro(1);         // 1 millisecond timer for Pushbutton management
//Metro     slowMetro = Metro(100);       // 100 millisecond timer for various tasks

class cpollMetro
{
private:
  int     pollTime = 0;
  int     iTime = 0;

public:  
  cpollMetro(int time)
  {
    iTime = iTime;
  }
  int     check()
          {
           if (pollTime == 0)
           {
           pollTime = millis(); 
           return 0;
           }
           else
           if ((pollTime - millis()) > iTime  )
              {
                pollTime = 0;
                return 1;
              }
           return 0;
         }
};

cpollMetro pollMetro(POLL_TIMER);
cpollMetro pushMetro(1);
cpollMetro slowMetro(100);

//-----------------------------------------------------------------------------------------
// Initialize the LCD
Adafruit_ILI9341 tft = Adafruit_ILI9341(tft_CS, tft_DC);

//-----------------------------------------------------------------------------------------
// Instanciate Power and SWR Meter gauges, virtual text lcd like handling for TFT LCD
// and a Modulation Scope widget
PowerMeter      Meter1;                 // A graphical Power Meter Gauge
PowerMeter      Meter2;                 // A second Power Meter Gauge, when needed
PowerMeter      Meter3;                 // A third Power Meter Gauge, when needed
PowerMeter      Meter4;                 // A fourth Power Meter Gauge, when needed
PowerMeter      Meter5;                 // A fifth Power Meter Gauge, when needed
VSWRmeter       SWR;                    // An SWR Meter Gauge
TextBox         VirtLCDw;               // A 20x10 Virtual Text LCD to TFT, white
TextBox         VirtLCDy;               // A 20x10 Virtual Text LCD to TFT, yellow
TextBox         VirtLCDlargeY;          // A Large 6x1 Virtual Text LCD to TFT, yellow
TextBox         VirtLCDlargeR;          // A Large 6x1 Virtual Text LCD to TFT, red
ModulationScope ModScope;


//
//-----------------------------------------------------------------------------------------
// Forward & Reverse Voltage Measure and collect Function ( Interrupt driven )
//-----------------------------------------------------------------------------------------
//
void powerSampler(void)
{
  #if INTR_LOOP_THRU_LED                    // Blink LED every time going through here 
  //digitalWrite(X_Led,X_LedState ^= 1);      // Set the LED
  #endif

  adc_poll_and_feed_circular();             // Read fwd and Rev AD, external or internal and feed circular buffer

  #if INTR_LOOP_THRU_LED                    // Blink LED every time going through here 
  //digitalWrite(X_Led,X_LedState ^= 1);      // Reset the LED
  #endif
}

//
//-----------------------------------------------------------------------------------------
// Top level task
// runs in an endless loop
//-----------------------------------------------------------------------------------------
//
void loop()
{

  ShiftTxRx();
  
  #if PUSHBUTTON_ENABLED
  static uint8_t  multi_button;             // State of Multipurpose Enact Switch
                                            // (Multipurpose pushbutton)
  static uint8_t  old_multi_button;         // Last state of Multipurpose Enact Switch
  #endif

  //-------------------------------------------------------------------------------
  // Here we do routines which are to be run through as often as possible
  //-------------------------------------------------------------------------------
  //#if FAST_LOOP_THRU_LED                    // Blink a LED every time
  //digitalWrite(X_Led,X_LedState ^= 1);      // Blink a led
  //#endif

  //pswr_sync_from_interrupt();               // Read and process circular buffers containing adc input,
                                            // calculate forward and reverse power, pep, pk and avg
  adc_poll_and_feed_circular();
  pswr_sync_from_interrupt();               // Read and process circular buffers containing adc input,
  //calc_SWR_and_power();                     // calculate forward and reverse power, pep, pk and avg
  
  //-------------------------------------------------------------------
  // Green LED if power is detected
  //if (power_mw > R.idle_disp_thresh) digitalWrite(G_Led, 1);
  //else digitalWrite(G_Led, 0);

  //-------------------------------------------------------------------
  // Check USB Serial port for incoming commands
  usb_read_serial();
  
  //-------------------------------------------------------------------------------
  // Here we do routines which are to be accessed once every POLL_TIMER milliseconds
  //-------------------------------------------------------------------------------
  // Poll timer.
  if (pollMetro.check())                    // check if the metro has passed its interval .
  {
    #if POLL_LOOP_THRU_LED                  // Blink LED every POLL_TIMER, when going through the main loop 
    //digitalWrite(X_Led,X_LedState ^= 1);    // Blink a led
    #endif
    
    //-------------------------------------------------------------------
    // Prepare various types of power for print to LCD and calculate SWR
    calc_SWR_and_power();
    
    //----------------------------------------------
    // Power Detected Flag and Timer
    // Flag is set whenever power above a minimum
    // relevant value is detected (see PSWR_A.h)
    #if AD8307_INSTALLED
    if ((power_mw > R.idle_disp_thresh) || (flag.mode_display))
    #else  
    if ((power_mw > MIN_PWR_FOR_METER)  || (flag.mode_display))
    #endif
    {
      power_timer = 0;
      flag.power_detected = true;      
    }
    else 
      if (power_timer < SLEEPTIME) 
        power_timer++;
      else 
        flag.power_detected = false;       // Activate screensaver if no power for a while
      
    #if TOUCHSCREEN_ENABLED
    //-------------------------------------------------------------------------------
    // Touch Screen Management
    //
    manage_Touchscreen();
    //-------------------------------------------------------------------------------
    // Meter Display Management and Entrance into Config Menu
    //
    // Long push in lower half of touch screen will enter Menu
    // Short tap of upper half of touch screen will cycle through the various meter displays
    if (!flag.config_mode) 
    {
      if (touch.longpush)                   // Check whether we have been asked for Config Menu Mode
      {
        if (touch.lower) 
        {
          touch.lower = false;
          flag.config_mode = true;          // Ask for Config Menu Mode
          flag.mode_change = true;          // Indicate that we need to set up for this mode
          flag.mode_display = false;
        }
      }
      else if (touch.pushed)
      {
        if (touch.upper)
        {
          touch.upper = false;
          if (touch.left)                   // Cycle through All Meter Displays
          {
            touch.left = false;
            mode_display++;                 // Cycle through meter displays            
            if (mode_display > MAX_MODE) mode_display = 1;
            flag.mode_newdefault = true;    // Indicate that patience timer for a potential new
                                            // default mode to be saved into EEPROM is running
          }
          else                              // Cycle between "default" display and Modulation Scope display
          {
            if (mode_display == MODULATIONSCOPE) mode_display = R.mode_default;
            else mode_display = MODULATIONSCOPE;
          }
          flag.mode_change = true;          // Force Mode Intro Display whenever Mode has been changed
          flag.mode_display = true;
        }
        else if (touch.lower)
        {
          //-------------------------------------------------------------------     
          // Short push, in lower 1/3 of screen to clear SWR Alarm, if set
          touch.lower = false;
          //digitalWrite(R_Led,false);        // Clear SWR Alarm LED
          flag.swr_alarm = false;           // Clear SWR Alarm Flag
        }
      }
      touch.longpush = false;               // Clear all push indications
      touch.pushed = false;
    }
    //-------------------------------------------------------------------------------
    // Configuration Menu Management
    else
    {
      if (touch.up)
      {
        Enc.write(-ENC_RESDIVIDE);          // Emulate encoder counterclockwise
        touch.up = false;
      }
      if (touch.down)
      {
        Enc.write(+ENC_RESDIVIDE);          // Emulate encoder counterclockwise
        touch.down = false;
      }
      if (touch.enact)                      // Enact pushed
      {
        flag.short_push = true;
        touch.enact = false;
      }
      if (touch.exit)                       // Exit from Menu has been requested
      {
        touch.exit = false;
        VirtLCDw.clear();
        VirtLCDy.clear();
        VirtLCDy.setCursor(1,1);
        Menu_exit_timer = 100;              // Show on LCD for 1 second
        VirtLCDy.print("Exit from Menu");
        flag.config_mode = false;           // We're done with Menu, EXIT
        flag.mode_change = true;            // Signal for display cleanup
        flag.menu_lcd_upd = false;          // Make ready for next time
        //flag.mode_display = false;
        if (menu_level == 1001)             // Clean up from Debug2 Menu
          lcd_display_debug_erase();
        menu_level = 0;                     // We're done with all Menu levels
      }
    }
    #endif
   
    //-------------------------------------------------------------------
    // Menu Exit announcement Timer
    if (Menu_exit_timer == 1) eraseDisplay();// Clear announcement before going live again
    if (Menu_exit_timer > 0)  Menu_exit_timer--;
    
    //-------------------------------------------------------------------
    // We are either entering or exiting Config Menu, need to clear up display
    // or
    // We just changed display modes, start/restart EEPROM save timer for new default display
    if ((flag.mode_change) && (!flag.mode_display))
    {
      flag.mode_change = false;

      if (flag.config_mode)                 // Entering Config Menu
      {
        eraseDisplay();
        #if TOUCHSCREEN_ENABLED
        setupTouchButtons();
        #endif
      }
      #if TOUCHSCREEN_ENABLED
      else if (touch.onscreen)              // Erase touch buttons if exiting Config Menu
      {
        eraseTouchButtons();
        //flag.mode_display = true;         // Force init of display
        //flag.mode_change = true;          // Introduce mode
      }
      #endif
    }    

    //-------------------------------------------------------------------
    // Various Menu (rotary encoder) selectable display/function modes
    //
    if (Menu_exit_timer == 0)
    {
      if (flag.config_mode)                     // Pushbutton Configuration Menu
      {
        ConfigMenu();
      }  
      else if (mode_display == POWER_BARPK)     // 100ms Peak Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("100ms Peak Power", "Pk: ", power_mw_pk);
      }
      else if (mode_display == POWER_BARAVG)    // 1s Average Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("100ms Average Power", "Avg:", power_mw_avg);
      }
      else if (mode_display == POWER_BARAVG1S)  // 1s Average Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("1s Average Power", "Av1:", power_mw_1savg);
      }
      else if (mode_display == POWER_BARINST)   // Instantaneous Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("Instantaneous Power", "    ", power_mw);
      }
      else if (mode_display == POWER_CLEAN_DBM) // Power Meter in dBm
      {
        lcd_display_clean_dBm();
      }
      else if (mode_display == POWER_MIXED)     // Fwd, Reflected and SWR
      {
        lcd_display_mixed();
      }
      else if (mode_display == MODULATIONSCOPE) // Modulation Scope
      {
        lcd_display_modscope();
      }
    }
    //----------------------------------------------
    // Send text to Display
    static bool alternate;
    alternate ^= 1;                         // Alternate between the Text outputs to spread load
    if (alternate)
    {
      VirtLCDw.transfer();
      VirtLCDy.transfer();
    }
    else
    {
      VirtLCDlargeY.transfer();
      VirtLCDlargeR.transfer();
    }
  } 
  
  //-------------------------------------------------------------------------------
  // Here we do timing related routines which are to be accessed once every millisecond
  // (mainly just puhbutton and rotary encoder stuff)
  //-------------------------------------------------------------------------------
  if (pushMetro.check())                    // check if the metro has passed its interval
  {
    
    #if PUSHBUTTON_ENABLED
    //-------------------------------------------------------------------------------
    // Encoder and Pushbutton  Management (if connected)
    //
    multi_button = multipurpose_pushbutton();
    if (old_multi_button != multi_button)   // A new state of the Multi Purpose Pushbutton
    {
      if (multi_button == 1)                // Short Push detected
      {
        flag.short_push = true;             // Used with Configuraton Menu functions
      }
      else if ((multi_button == 2) && (flag.config_mode != true)) // Long Push detected
      {
        flag.config_mode = true;            // Ask for Config Menu Mode
        flag.mode_change = true;            // Indicate that we need to set up for this mode
        flag.mode_display = false;
      }
    }
    old_multi_button = multi_button;
    //-------------------------------------------------------------------     
    // Do stuff when not in Config Menu
    if (!flag.config_mode)
    {
      if (flag.short_push)                  // Short push to clear SWR Alarm
      {
        if (mode_band)
          mode_band = false;
        else
          mode_band = true;

          
        flag.short_push = false;
        //digitalWrite(R_Led,false);          // Clear SWR Alarm LED
        flag.swr_alarm = false;             // Clear SWR Alarm Flag
      }
      
/*      if (Enc.read()/ENC_RESDIVIDE != 0)    // Encoder turned to cycle through modes
      {
        flag.mode_change = true;            // Force Mode Intro Display whenever Mode has been changed
        flag.mode_display = true;
        flag.mode_newdefault = true;        // Indicate that patience timer for a potential new
                                            // default mode to be saved into EEPROM is running
        // Mode switching travels only one click at a time, ignoring extra clicks
        if (Enc.read()/ENC_RESDIVIDE > 0)
        {
          mode_display++;
          if (mode_display > MAX_MODE) mode_display = 1;
          Enc.clearCount();                     // Reset data from Encoder
        }
        else if (Enc.read()/ENC_RESDIVIDE < 0)
        {
          mode_display--;
          if (mode_display == 0) mode_display = MAX_MODE;
          Enc.clearCount();                     // Reset data from Encoder
        }
*/
    if (Enc.getCount()/ENC_RESDIVIDE != 0)    // Encoder turned to cycle through modes
      {
                                            // default mode to be saved into EEPROM is running
        // Mode switching travels only one click at a time, ignoring extra clicks
        if (Enc.getCount()/ENC_RESDIVIDE > 0)
        {
        if (mode_band)
            {
              cur_band++;
              set_band(cur_band);  
            }
          else
            {
              flag.mode_display = true;
              flag.mode_newdefault = true;        // Indicate that patience timer for a potential new
              flag.mode_change = true;            // Force Mode Intro Display whenever Mode has been changed
              mode_display++;           
            }
          if (mode_display > MAX_MODE) mode_display = 1;
          Enc.setCount(0);                     // Reset data from Encoder
        }
        else if (Enc.getCount()/ENC_RESDIVIDE < 0)
        {
          if (mode_band)
            {
            cur_band--;
            set_band(cur_band);
            }
          else
            {
              flag.mode_display = true;
              flag.mode_newdefault = true;        // Indicate that patience timer for a potential new
              flag.mode_change = true;            // Force Mode Intro Display whenever Mode has been changed
              mode_display--;
            }   
          if (mode_display == 0) mode_display = MAX_MODE;
          Enc.setCount(0);                     // Reset data from Encoder
        }
      }
    }
    #endif
  }

  //-------------------------------------------------------------------------------
  // Here we do timing related routines which are to be accessed once every 1/10th of a second
  //-------------------------------------------------------------------------------
  if (slowMetro.check())                    // check if the metro has passed its interval .
  {
    #if SLOW_LOOP_THRU_LED                  // Blink LED every 100ms, when going through the main loop 
    //digitalWrite(X_Led,X_LedState ^= 1);    // Blink a led
    #endif
    
    usb_cont_report();                      // Report Power and SWR to USB, if in Continuous mode
    
    //----------------------------------------------
    // Patience timer for storing new Default Mode into EEPROM
    if (flag.mode_newdefault)
    {
      static uint16_t newdefaulttimer;

      if (newdefaulttimer++ == 100)         // Store current screen as default if stable for 10 seconds
      {
        flag.mode_newdefault = false;
        newdefaulttimer = 0;
        if (R.mode_display != mode_display)
        {
          R.mode_display = mode_display;
          // If new default mode is not modulation scope, then also store as default for the two-mode shortcut
          if (R.mode_display != MODULATIONSCOPE) R.mode_default = R.mode_display;
          EEPROM_writeAnything(1,R);
        }
      }
    }
  }
}


//
//-----------------------------------------------------------------------------------------
//      Setup Ports, timers, start the works and never return, unless reset
//                          by the watchdog timer
//                   then - do everything, all over again
//-----------------------------------------------------------------------------------------
//
void setup()
{
  uint8_t coldstart;
  
  // Enable LEDs and set to LOW = Off
/*  pinMode(R_Led, OUTPUT);                   // SWR Alarm
  digitalWrite(R_Led, LOW);
  pinMode(G_Led, OUTPUT);                   // Not used for now (previously used for USB traffic indication)
  digitalWrite(G_Led, LOW);
  pinMode(X_Led, OUTPUT);                   // Debug LED
  digitalWrite(X_Led, LOW);
  pinMode(DisplayAwake, OUTPUT);            // PWM modulated Power to Display Backlight
 */
  #if PUSHBUTTON_ENABLED
  pinMode(EnactSW, INPUT);                  // Menu/Enact pushbutton switch
  ESP32Encoder::useInternalWeakPullResistors=NONE;
  Enc.attachHalfQuad(EncI, EncQ);
  #endif
  set_band(cur_band);
  
  coldstart = EEPROM.read(0);               // Grab the coldstart byte indicator in EEPROM for
                                            // comparison with the COLDSTART_REFERENCE
  // Initialize all memories if first upload or if COLDSTART_REF has been modified
  // either through PSWR_A.h or through Menu functions
  if (coldstart != COLDSTART_REF)
  { 
    EEPROM.write(0,COLDSTART_REF);          // COLDSTART_REF in first byte indicates all initialized
    EEPROM_writeAnything(1,R);              // Write default settings into EEPROM
  }
  else                                      // EEPROM contains stored data, retrieve the data
  {
    EEPROM_readAnything(1,R);               // Read the stored data
  }

//  analogWrite(DisplayAwake, 5 + 25*R.disp.tft_backlight); // PWM modulation of TFT backlight (5 to 255)
  tft.begin();
  tft.setRotation(1+2*R.disp.rotate);       // Upside down or downside Up? :)
  wifiinit();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(ILI9341_WHITE);
//  tft.writeRect(40,0,240,240,(uint16_t*) Moon240x240); // Draw a pretty picture on screen

  adc_init();                               // Init Builtin ADCs
  
  Serial.begin(115200);                       // initialize USB virtual serial serial port
  delay(1000);

  #if WIRE_ENABLED
  // Start I2C on port SDA0/SCL0 (pins 18/19) - 400 kHz
  Wire.begin(I2C_MASTER,0x00,I2C_PINS_18_19,I2C_PULLUP_EXT,I2C_RATE_400); 
  uint8_t i2c_status = I2C_Init();          // Initialize I2C comms
  #endif
  
  VirtLCDw.init();                          // Instanciate one 20x10 text screen
  // Instanciate a second 20x10 text screen in yellow
  VirtLCDy.init(20,10,10,  1, FONT_12, ILI9341_YELLOW, ILI9341_BLACK); 
  // Instanciate two 6x1 large text screens, in Yellow and Red
  VirtLCDlargeY.init ( 6, 1,130,130, FONT_24, ILI9341_YELLOW, ILI9341_BLACK);  
  VirtLCDlargeR.init ( 6, 1,130,130, FONT_24, ILI9341_RED   , ILI9341_BLACK);  

  ModScope.init(5, 5, 310, 166, ILI9341_WHITE, ILI9341_GREEN, ILI9341_YELLOW);
  ModScope.rate(R.modscopeDivisor);         // Modulation scope rate divisor

  //------------------------------------------
  // LCD Print Version and I2C information (6 seconds in total during startup)
  // ToDo - rework to better fit 20x10 screen
  VirtLCDw.clear();
  VirtLCDy.clear();
  VirtLCDw.setCursor((20-strlen(STARTUPDISPLAY1))/2,0); // Center justify in line
  VirtLCDw.print(STARTUPDISPLAY1);
  VirtLCDy.setCursor((20-strlen(STARTUPDISPLAY2))/2,1); // Center justify in line
  VirtLCDy.print(STARTUPDISPLAY2);
  VirtLCDw.transfer();
  VirtLCDy.transfer();  
  delay(100);
  VirtLCDw.setCursor(0,2);
  VirtLCDw.print(STARTUPDISPLAY3);
  VirtLCDw.transfer();  
  delay(100);
  VirtLCDw.setCursor(20-strlen(STARTUPDISPLAY4),3);
  VirtLCDw.print(STARTUPDISPLAY4);
  VirtLCDw.transfer();  
  delay(300);

  VirtLCDy.setCursor(0,9);
  VirtLCDy.print(STARTUPDISPLAY5);
  sprintf(lcd_buf," V%s", VERSION);
  VirtLCDy.setCursor(20-strlen(lcd_buf),9);
  VirtLCDy.print(lcd_buf);
  VirtLCDy.transfer();  
  delay(500);
  #if WIRE_ENABLED                          // I2C scan report
  VirtLCDw.setCursor(0,5);
  if      (i2c_status==1) VirtLCDw.print("AD7991-0 detected   ");
  else if (i2c_status==2) VirtLCDw.print("AD7991-1 detected   ");
  else                    VirtLCDw.print("Using built-in A/D  ");
  VirtLCDw.transfer();  
  delay(1500);	
  #endif

  #if TOUCHSCREEN_ENABLED
  if (!setup_Touchscreen())                 // Initialize Touchscreen
  {
    VirtLCDy.setCursor(0,6);
    VirtLCDy.print("Couldn't start the");
    VirtLCDy.setCursor(0,7);
    VirtLCDy.print("touchscreen cntrller");
    VirtLCDy.transfer();  
    delay(2000);
  }
  #endif
 
  mode_display = R.mode_display;            // Active Display Mode
  flag.mode_change = true;                  // Force a Display of Mode Intro when starting up
  flag.mode_display = true;

  VirtLCDw.clear();                         // Prep for going live
  VirtLCDy.clear();
  VirtLCDlargeY.clear();
  VirtLCDlargeR.clear();
  tft.fillScreen(0x000000);                 // Erase picture
  CAT.begin (true);               // Initialize CAT control stuff

  xTaskCreatePinnedToCore(task0, "Task0", 4096, NULL, 1, NULL, 0);
}

void task0(void* arg)
{
    while (1)
     {
     network_loop();
     CheckCAT ();
     vTaskDelay(1);
     }
}

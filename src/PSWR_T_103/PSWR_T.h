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
#define                VERSION "1.03"
#define                DATE    "2020-06-21"
//**
//*********************************************************************************
#ifndef _PSWR_T_H_
#define _PSWR_T_H_

#include <arduino.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_ILI9341.h>

#include "Fonts/DroidSansMono8pt7b.h"
#include "Fonts/FreeMono8pt7b.h"
#include "Fonts/FreeMono12pt7b.h"
#include "Fonts/DroidSansMono24pt7b.h"
#include "Fonts/FreeMono48pt7b.h"


#include <EEPROM.h>
#include <ESP32Encoder.h>
#include "PSWRtft.h"
#include "_EEPROMAnything.h"
#include "FT891_CAT.h" 
#include "band.h"


#define   Arial_8 &DroidSansMono8pt7b
#define   FONT_8  DroidSansMono8pt7b
#define   FONT_12 FreeMono12pt7b
#define   FONT_24 DroidSansMono24pt7b
#define   FONT_48 FreeMono48pt7b


//
//-----------------------------------------------------------------------------
// Features Selection
//-----------------------------------------------------------------------------  
//

//-----------------------------------------------------------------------------
// Enable Encoder and Pushbutton
#define PUSHBUTTON_ENABLED        1 // 1 to enable, else 0.  Harmless if not used

//-----------------------------------------------------------------------------
// Enable Touch Screen
#define TOUCHSCREEN_ENABLED       0 // 1 to enable, else 0.
//-----------------------------------------------------------------------------
// FT6206 Capacitive Touch Controller or a Resistive XPT2046 Touch Controller
#define FT6206_INSTALLED          0 // 1 For Cap Touch Controller, else 0
//-----------------------------------------------------------------------------
// Inverted Y axis on Resistive XPT2046 Touch Controller
#define XPT2046_YINVERT           0 // 1 For inverted Y axis, else 0
// Inverted X axis on Resistive XPT2046 Touch Controller
#define XPT2046_XINVERT           0 // 1 For inverted X axis, else 0

//-----------------------------------------------------------------------------
// 2X AD8307 Log Amp Detectors, or Diode Detectors
#define AD8307_INSTALLED          1 // 0 for Diode Detectors, 1 for 2x AD8307

//-----------------------------------------------------------------------------  
// Poll for I2C enabled external AD7991 or no?
// (it is harmless to keep this on even if not used)
#define WIRE_ENABLED              0 // 1 to enable, 0 to disable

//
//-----------------------------------------------------------------------------
// Defaults for measurement rate, PEP, PK and AVG buffer sizes
#if F_CPU >= 144000000              // If CPU is overclocked at 144 MHz or higher, then 1500 samples per second
                                    // (Overclocking can be enabled by editing board.txt in Teensyduino)
#define SAMPLE_TIMER            667 // Interrupt timer, in microseconds
#define BUF_SHORT               150 // Buffer size for 100ms Peak
#define PEP_BUFFER               50 // PEP Buffer size in multiples of BUF_SHORT, can hold up to 5 second PEP
#define LONG_BUFFER              30 // LONG Buffer size in multiples of shortest PEP period (1s) - typically 30 seconds
#define AVG_BUF1S              1500 // AVG Buffer sample size (1000 is 1 second, if SAMPLE_TIMER = 1000)
#define AVG_BUFSHORT            150 // Short (100ms) AVG Buffer size
#define AVG_BUFSWR               15 // Very short buffer for SWR bargraph smoothing.  Time is POLL_TIMER*AVG_BUFSWR
#define MODSCOPE_DIVISOR          1 // Modultion Scope scan rate divisor
#else                               // 1000 samples per second at normal Teensy speed (96 MHz)
#define SAMPLE_TIMER           1000 // Interrupt timer, in microseconds
#define BUF_SHORT               100 // Buffer size for 100ms Peak
#define PEP_BUFFER               50 // PEP Buffer size in multiples of BUF_SHORT, can hold up to 5 second PEP
#define LONG_BUFFER              30 // LONG Buffer size in multiples of shortest PEP period (1s) - typically 30 seconds
#define AVG_BUF1S              1000 // AVG Buffer sample size (1000 is 1 second, if SAMPLE_TIMER = 1000)
#define AVG_BUFSHORT            100 // Short (100ms) AVG Buffer size
#define AVG_BUFSWR               10 // Very short buffer for SWR bargraph smoothing.  Time is POLL_TIMER*AVG_BUFSWR
#define MODSCOPE_DIVISOR          1 // Modultion Scope scan rate divisor
#endif

//-----------------------------------------------------------------------------
// Timer for Power/SWR calculations and LCD prints, milliseconds
#define POLL_TIMER               10 // Meter updates will be at half this rate
                                    // Text updates will be at quarter of this rate

//-----------------------------------------------------------------------------
// ADC Reference Voltage and Voltage Drop Definitions
#define ADCREF_BUILTIN            0 // 1 for builtin 1.2V reference, 0 for external 3.3V reference
//-----------------------------------------------------------------------------
// Automatic, no need to do anything with these (unless external voltage regulator is not exactly 3.3V)
#if ADCREF_BUILTIN
#define ADC_REF                1.20 // Internal Reference voltage for ADC
#else
#define ADC_REF                3.30 // External Reference voltage for ADC
#endif
//-----------------------------------------------------------------------------
// Resistors for Voltage Drop before AD input - used when internal 1.2V reference
// and/or when Diode Detector type coupler is used instead of a 2x AD8307 coupler.
#define RS                      0.0 // Series Resistor between measured voltage and ADC_IN (k ohm)
#define RP                     10.0 // Parallel Resistor, ADC_IN to ground for ADC_IN voltage drop (k ohm)
                                    // If no RS and RP, then set RS as 0 and RP as any non-zero value                                   
//-----------------------------------------------------------------------------
 // Equivalent Reference Voltage, derived from ADC_REF & voltage drop resistors
#define ADC_RES  ADC_REF*(RS+RP)/RP // Don't touch, automaticcally derived from the above
//-----------------------------------------------------------------------------
#define AD7991REF              2.60 // Reference voltage if using external AD7991

//-----------------------------------------------------------------------------
// DEFS for FWD and REF coupler, 2xAD8307 or Diode detectors
//-----------------------------------------------------------------------------
// DEFS for AD8307 Calibration (dBm *10)
// levels for the forward and reverse measurements - in order to allow a Calibration
//
// Note that the AD8307 generates 2uA per dB and applies an internal resistor of 12.5kohms
// 2uA * 12.5kohm gives the specified slope of 25mV/dB.  
// Unloaded Teensy 3.2 AD inputs appear to have an impedance of approx 200 kohms,
// hence the voltage slope is slightly affected:
// 12.5k||200k = 11.75k.  2uA * 11.75k gives a slope value of 23.5mV/dB.
// However if the meter has a voltage divider made out of RS=15kohm and RP=10kohm, 
// then the slope would need to be recalculated like below:
// 12.5k||25k = 8.333k.  2uA * 8.333k then gives a slope value of 16.67mV/dB 
// or if RS = 47kohm and RP = 33kohm (RS+RP = 80 kohm), then:
// 12.5k||80k = 10.81k.  2uA * 10.81k gives a slope value of 21.62mV/dB
//
#define LOGAMP1_SLOPE          23.5 // mV/dB.  Used with Single Level Calibration
#define LOGAMP2_SLOPE          23.5 // mV/dB.  Used with Single Level Calibration
                                    // Typical value for AD8307 is 25mV/dB if AD input is ~1Mohm or higher
                                    // Typical value for ADL5519 is 22mV/dB
//-----------------------------------------------------------------------------
#define CAL_INP_QUALITY        12.0 // (dB) Minimum difference between the Fwd and Rev
                                    // signals to be allowed to calibrate
//-----------------------------------------------------------------------------
// 20 to 1 Tandem Match coupler with Power and SWR Meter
// (default defines are for a 30 to 1 coupler)
#define TWENTYTOONE               0 // 1 to select the values for 20 to 1 coupler, else 0
//
#if TWENTYTOONE                     // Defs when using a 20 to 1 coupler
#define CAL1_NOR_VALUE          400 // 40 dBm, default dBm level1 for both AD8307
#define CAL2_NOR_VALUE          100 // 10 dBm, default dBm level2 for both AD8307
#define CALFWD1_DEFAULT       2.452 // Default raw Voltage level1 at 40 dBm
#define CALREV1_DEFAULT       2.452 // Default raw Voltage level1 at 40 dBm
#define CALFWD2_DEFAULT       1.702 // Default raw Voltage level2 at 10 dBm
#define CALREV2_DEFAULT       1.702 // Default raw Voltage level2 at 10 dBm
#else                               // Defs when using a 30 to 1 coupler
#define CAL1_NOR_VALUE          400 // 40 dBm, default dBm level1 for both AD8307
#define CAL2_NOR_VALUE          100 // 10 dBm, default dBm level2 for both AD8307
#define CALFWD1_DEFAULT       2.233 // Default raw Voltage level1 at  40 dBm
#define CALREV1_DEFAULT       2.233 // Default raw Voltage level1 at  40 dBm
#define CALFWD2_DEFAULT       1.528 // Default raw Voltage level2 at  10 dBm
#define CALREV2_DEFAULT       1.528 // Default raw Voltage level2 at  10 dBm
#endif                              // End coupler selection

//-----------------------------------------------------------------------------
// DEFS for Diode Detectors
// Typical Tandem Match parameters for a 200W bridge
//
// BRIDGE_COUPLING (attenuation) = N_transformer, e.g. 20 turns Tandem Match equals 20
#define BRIDGE_COUPLING       20.0  // Bridge Coupling
#define METER_CAL             1.00  // Calibration fudge factor (0 - 2.5)
#define D_VDROP               0.25  // Voltage Drop over Diode

//-----------------------------------------------------------------------------
// Defs for Power and SWR indication
//
#if AD8307_INSTALLED                // --------------Used with AD8307:
#if TWENTYTOONE                     // Defs when using a 20 to 1 coupler and AD7991
#define MIN_PWR_FOR_SWR_CALC   0.5  // Minimum Power in mW for SWR calculation and display
#else                               // Defs when using a 30 to 1 coupler and AD7991
#define MIN_PWR_FOR_SWR_CALC   1.0  // Minimum Power in mW for SWR calculation and display
#endif
#define MIN_PWR_FOR_SWR_SHOW  0.01  // Minimum Power in mW for SWR indication (use recent value)
//-----------------------------------------------------------------------------
#else                               // --------------Used if Diode detectors:
#define MIN_PWR_FOR_METER       30  // Minimum Power in mW for Power/SWR Meter indication on LCD
#define MIN_PWR_FOR_SWR_CALC    30  // Minimum Power in mW for SWR calculation and display
#define MIN_PWR_FOR_SWR_SHOW    30  // Minimum Power in mW for SWR indication (use recent value)
#endif

//-----------------------------------------------------------------------------
//Text for Startup Display... Play with and tailor to your own liking
#if AD8307_INSTALLED
#define STARTUPDISPLAY1         "LDMOS Management"
#define STARTUPDISPLAY2         "Power and SWR Meter"
#define STARTUPDISPLAY3         "Current and Temp"
#define STARTUPDISPLAY4         ""
#else
#define STARTUPDISPLAY1         ""
#define STARTUPDISPLAY2         ""
#define STARTUPDISPLAY3         ""
#define STARTUPDISPLAY4         ""
#endif
#define STARTUPDISPLAY5         "PA0PHH" // You may want to indicate your own callsign :)

//-----------------------------------------------------------------------------
// EEPROM settings Serial Number. Increment this number when firmware mods necessitate
// fresh "Factory Default Settings" to be forced into the EEPROM at first boot after
// an upgrade
#define COLDSTART_REF          0x07 // When started, the firmware examines this "Serial Number
                                    // and enforces factory reset if there is a mismatch.
                                    // Rolling this value is useful if the EEPROM structure has been modified

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Various Initial Default settings for Meter
// (many of these are configurable by user through Config Menu or USB commands)
//

//-----------------------------------------------------------------------------
// DEFS for SWR Alarm
#define SWR_ALARM                30 // 30=3.0, default SWR Alarm Trigger
//
#if AD8307_INSTALLED                // --------------Used with AD8307:
#define SWR_THRESHOLD            10 // Default SWR Alarm Power Threshold in mW
#else                               // A value fit for a Diode detector
#define SWR_THRESHOLD            50 // Default SWR Alarm Power Threshold in mW
#endif

//-----------------------------------------------------------------------------
// DEFS for User Definable Scale Ranges
#define SCALE_RANGE1             11 // User definable Scale Ranges, up to 3 ranges per decade                   
#define SCALE_RANGE2             22 // e.g. ... 11W 22W 55W 110W 220W 550W ...
#define SCALE_RANGE3             55 // If all values set as "2", then ... 2W 20W 200W ...

//-----------------------------------------------------------------------------
// PEP envelope sample size for 1, 2.5 or 5 second sample time
#define PEP_PERIOD (2500000/SAMPLE_TIMER)/BUF_SHORT // 2.5 seconds = Default


//-----------------------------------------------------------------------------
// DEFS for Default Screensaver Message
#define SLEEPMSG   "ZZzzzz zzz ..." // Shown when nothing else to display on LCD
                                    // Configurable by USB Serial input command:
                                    // $sleepmsg=blabla
#define SLEEPTIME              1000 // Time (xPOLL_TIMER) to enter screensaver mode and half of this time to renew
                                    //  position of screensaver message, in tenths of a second
#define SLEEPTHRESHOLD        0.001 // Only used if AD8307. Minimum relevant power to exit Sleep Display (0.001=1uW)

//-----------------------------------------------------------------------------
// Mode Intro Time (decides for how long mode intro is displayed when turning encoder
#define MODE_INTRO_TIME         250 // 100ths of a second (200 equals 2s)

//-----------------------------------------------------------------------------
// LED Blink
//
// None, or only one of the three below should be selected
#define INTR_LOOP_THRU_LED       0  // Blink the debug led on Microprocessor while sampling the AD within the interrupt function *OR*
#define FAST_LOOP_THRU_LED       0  // Blink the debug led on Microprocessor every time, when going through the mainloop *OR*
#define	POLL_LOOP_THRU_LED       1  // Blink the debug led on Microprocessor every 10ms, when going through the mainloop *OR*
#define SLOW_LOOP_THRU_LED       0  // Blink the debug led on Microprocessor every 100ms, when going through the mainloop

//-----------------------------------------------------------------------------
// Definitions for Rotary Encoder and Pushbutton - harmless if not used
#define  ENC_RESDIVIDE            4 // Encoder resolution reduction
#define  ENACT_MIN               20 // Minimum Menu/Enact push for "short push" (x 1 ms)
#define  ENACT_MAX             1000 // Minimum Menu/Enact push for Menu Mode (x 1 ms)


//-----------------------------------------------------------------------------  
// Assign pins to inputs and outputs (Arduino style)
//-----------------------------------------------------------------------------  
// Constraints:  SPI:     7 MOSI,       12 MISO,     14 CLK,   20 CS,   21 D/C
//               I2C:    18 SDA0,       19 SCL0
//               ADC:    A1 (15) ADC0,  A2 (16) ADC1
//               UART0:   0 RX,         1 TX
//               FrqInp: 13
//
// TFT LCD and Touch Screen
const int tft_DC            =  2;  // TFT LCD pins, using SPI2 - For optimized ILI9341_t3 library
const int tft_CS            =  5;
const int tft_MOSI          =  23;
const int tft_SCLK          =  18;
const int tft_MISO          =  27;
const int tft_RST           = 255;  // Not used
const int Touch_CS          =   8;  // CS signal for Resistive Touchscreen
//
const int DisplayAwake      =   6;  // PWM modulated output pin for TFT backlight control, if implemented
//
// UART0 pins:
//        RXD               =   0   // Take aside for potential later use
//        TXD               =   1
// I2C pins
//        SCL0              =  19   // Defined through: I2C_PINS_18_19
//        SDA0              =  18
// AD inputs for Forward and Reflected Power (SWR measurement)
const int Pfwd              =  32; //36 A1P
const int Pref              =  33; //39 A2P
// Encoder ports
const int EncI              =  14; // 27; //19;
const int EncQ              =  19; // 26; // 17;
const int EnactSW           =  26; // 14; //16;  // Menu/Enact Switch
// RX/TX
const int RxTx              =  4;

//
//const int frqCount        =  13;  // Take aside, use later for Frequency Counter input
//
const int R_Led             =  21;  // SWR Alarm
const int G_Led             =  22;  // Measurable Power Detected
const int X_Led             =  16;  // Debug
// Unassigned: 2 3 4 5


//
//-----------------------------------------------------------------------------
// Miscellaneous non-configurable software defines and variables
//-----------------------------------------------------------------------------
//

//-----------------------------------------------------------------------------
// Structures and Unions

typedef struct {
          int16_t  fwd[256];                  // Circular buffer of Forward measurement values
          int16_t  rev[256];                  // Circular buffer of Reverse measurement values
          uint8_t  incount;                   // Pointer to most recent input value of circular buffer
          uint8_t  outcount;                  // Pointer to most recent output value of circular buffer
               }  adbuffer_t;
               
typedef struct {
          int16_t  db10m;                     // Calibrate, value in dBm x 10
          double   Fwd;                       // corresponding AD8307 forward output value, V * 10000
          double   Rev;                       // corresponding AD8307 reverse output value, V * 10000
               }  cal_t;

typedef struct  {
          unsigned rotate              : 1;   // Upside Down or Not
//          unsigned dimmed              : 1;   // Screen Dimmed for lower power use
          uint8_t  tft_backlight;             // PWM control of brightness level, if implemented
                } disp_t;
                
typedef struct {
          cal_t    cal_AD[2];                 // 2 Calibration points for both AD8307, if AD8307 option
          uint8_t  meter_cal;                 // Calibration multiplier for diode detector option - 100=1.0
          uint8_t  SWR_alarm_trig;            // SWR level*10 for Alarm trigger
          uint16_t SWR_alarm_pwr_thresh;      // Minimum Power required to trigger SWR Alarm
          unsigned usb_report_cont     : 1;   // Continuous USB Serial Reporting of Power and SWR
          unsigned usb_report_type     : 4;   // Which type of Power reporting is active over USB?
                                              // serial/USB (selected/deselected w USB commands $pcont/$ppoll)
                     #define  REPORT_DATA      1    // Report Instantaneous Power (raw format) and SWR to USB
                     #define  REPORT_INST      2    // Report Instantaneous Power (formatted) and SWR to USB
                     #define  REPORT_PK        3    // Report Peak (100ms) Power and SWR to USB
                     #define  REPORT_PEP       4    // Report PEP (1s) Power and SWR to USB
                     #define  REPORT_AVG       5    // Report Average (1s) Power and SWR to USB
                     #define  REPORT_1SAVG     6    // Report Average (1s) Power and SWR to USB
                     #define  REPORT_INSTDB    7    // Report Instantaneous Power (formatted) and SWR to USB
                     #define  REPORT_PKDB      8    // Report Peak (100ms) Power and SWR to USB
                     #define  REPORT_PEPDB     9    // Report PEP (1s) Power and SWR to USB
                     #define  REPORT_AVGDB    10    // Report Average (1s) Power and SWR to USB
                     #define  REPORT_1SAVGDB  11    // Report Average (1s) Power and SWR to USB
                     #define  REPORT_LONG     12    // Report Power and SWR to USB, long Human Readable format                   
                     #define  REPORT_AD_DEBUG 13    // Report raw AD values
          uint16_t PEP_period;                // PEP envelope sampling time in SAMPLE_TIME increments
          uint16_t AVG_period;                // AVG sampling time in SAMPLE_TIME increments
          uint8_t  ScaleRange[3];             // User settable Scale ranges, up to 3 ranges per decade.
          char     idle_disp[21];             // Sleep Display (configurable by USB serial command)
          float    idle_disp_thresh;          // Minimum level in mW to exit Sleep Display	
          unsigned low_power_floor     : 3;   // Lowest power level shown on display
                     #define  FLOOR_NOISEFLOOR 0    // No low power level threshold, lowest power shown is the effective noise floor
                     #define  FLOOR_ONE_uW     1    // Any power levels below 1uW are displayed as "0 uW"
                     #define  FLOOR_TEN_uW     2    // Any power levels below 10uW are displayed as "0.00mW"
                     #define  FLOOR_100_uW     3    // Any power levels below 100uW are displayed as "0.0mW"
                     #define  FLOOR_ONE_mW     4    // Any power levels below 1mW are displayed as "0 mW"
                     #define  FLOOR_TEN_mW     5    // Any power levels below 10mW are displayed as "0.00W"
          unsigned mode_display        : 4;   // Indicates current Display mode where:
                     #define  POWER_BARPK      1
                     #define  POWER_BARAVG     2
                     #define  POWER_BARAVG1S   3
                     #define  POWER_BARINST    4
                     #define  POWER_CLEAN_DBM  5
                     #define  POWER_MIXED      6
                     #define  MODULATIONSCOPE  7
                     //#define  DEBUG          8
                     #define  DEFAULT_MODE     1
                     #define  MAX_MODE         7    // Same as highest
          unsigned mode_default        : 4;   // Default mode, excluding Modulation Scope

          uint8_t  modscopeDivisor;           // Modultion Scope scan rate divisor
                                              // total time of a scan = SAMPLE_TIMER * TFT_x_axis * Divisor
                                              // e.g. 1000us * 300 * 1 = 0.3 seconds for a full sweep
          disp_t   disp;                      // Runtime Settings for Display
                } var_t;

typedef struct {
          unsigned short_push          : 1;   // Short Push Button Action
          unsigned power_detected      : 1;   // Power measured
          unsigned mode_change         : 1;   // Display mode has changed, need to set up for this mode
          unsigned mode_display        : 1;   // Show on screen that Display mode has changed
          unsigned mode_newdefault     : 1;   // Display mode has changed, timer for new Default Mode is running
          unsigned swr_alarm           : 1;   // SWR Flag indicating SWR above threshold
          unsigned idle_refresh        : 1;   // Force Screensaver reprint
          unsigned screensaver_on      : 1;   // Indicates that Screensaver is running
          unsigned menu_lcd_upd        : 1;   // Refresh/Update LCD when in Menu Mode
          unsigned config_mode         : 1;   // Configuration Menu Mode is active
          unsigned picture             : 1;   // Picture on screen (to signal delete on clear if present)
                } push_flags;

typedef struct {                              // Touch screen push states
          unsigned onscreen            :  1;  // Keep track of wheter buttons are being displayed or not
          unsigned pushed              :  1;  // Indicate if touch screen has been pushed
          unsigned longpush            :  1;  // Indicatee if this is a long (>1s) push
          unsigned up                  :  1;  // Four Pushbuttons...
          unsigned down                :  1;
          unsigned enact               :  1;
          unsigned exit                :  1;
          unsigned upper               :  1;  // Indicate if upper 2/3rds of screen have been touched
          unsigned left                :  1;  // Indicate if left 1/3rd of screen has been touched
          unsigned lower               :  1;  // Indicate if lower 1/3rd of screen has been touched
                } touch_flags;

//-----------------------------------------------------------------------------
// Macros
#ifndef SQR
#define SQR(x) ((x)*(x))
#endif
#ifndef ABS
#define ABS(x) ((x>0)?(x):(-x))
#endif

//-----------------------------------------------------------------------------
// Soft Reset
#define RESTART_ADDR       0xE000ED0C
#define RESTART_VAL        0x5FA0004
#define SOFT_RESET()       ((*(volatile uint32_t *)RESTART_ADDR) = (RESTART_VAL))


extern Adafruit_ILI9341 tft;                        // To keep the .cpp stuff happy


#endif

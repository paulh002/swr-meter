//*********************************************************************************
//**
//** Project.........: A menu driven Multi Display RF Power and SWR Meter
//**                   using a Tandem Match Coupler and 2x AD8307; or
//**                   diode detectors.
//**
//** Copyright (C) 2016  Loftur E. Jonasson  (tf3lj [at] arrl [dot] net)
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
//** Initial version.: 0.00, 2016-04-15  Loftur Jonasson, TF3LJ / VE2LJX / VE2AO
//**
//*********************************************************************************

#if TOUCHSCREEN_ENABLED

#define BUTTON_X        40         // Determine size and shape of Touch Buttons
#define BUTTON_Y       210
#define BUTTON_W        74
#define BUTTON_H        55
#define BUTTON_SPACING_X 6
#define BUTTON_TEXTSIZE  4

//-----------------------------------------------------------------------------
// FT6206 Capacitive Touch Controller or a Resistive XPT2046 Touch Controller
#if FT6206_INSTALLED
// Adafruint_FT6206 library modified (c and h file),
// replacing #include <wire.h> with #include <i2c_t3.h>
#include <Adafruit_FT6206.h>
Adafruit_FT6206 ts = Adafruit_FT6206();
//
#else
#include <XPT2046_Touchscreen.h>
#if XPT2046_YINVERT 
// Calibration data for Adafruint 2.8" ILI9341 with homebrew XPT2046 addon
#define TS_MINX     140
#define TS_MINY     200
#define TS_MAXX    3670
#define TS_MAXY    3800
#define MINPRESSURE 100
//
#else
// Calibration data for eBay resistive 2.8" ILI9341 SPI with XPT2046
#define TS_MINX     200
#define TS_MINY     420
#define TS_MAXX    3650
#define TS_MAXY    3720
#define MINPRESSURE 300
#endif
//
XPT2046_Touchscreen ts(Touch_CS);
#endif

//-----------------------------------------------------------------------------
// Colour and Text indication inside four Pushbuttons
//const char *buttonlabels[] = {"Select", "Left", "Right", "Exit" };
const char *buttonlabels[] = {"\xc", "\x60", "\x61", "\xd" }; // "Awesome" Font symbols
uint16_t    buttoncolors[] = {ILI9341_DARKGREEN, ILI9341_BLUE,
                              ILI9341_BLUE,      ILI9341_ORANGE};
Adafruit_GFX_Button buttons[4];


//
//-----------------------------------------------------------------------------------------
//      Setup Touch Screen.  Return false if the setup fails
//-----------------------------------------------------------------------------------------
//
bool setup_Touchscreen(void)
{
  bool test;

  #if FT6206_INSTALLED
  test = ts.begin(40);
  #else
  test = ts.begin();  
  #endif
  return test;
}

//
//-----------------------------------------------------------------------------------------
//      Draw four Pushbuttons on screen, Up, Enact, Down and Exit.
//      Two states for each, unpushed and pushed.
//-----------------------------------------------------------------------------------------
//
void setupTouchButtons(void)
{
  //tft.setFontAdafruit();         // Default font
  //tft.setFont(DroidSansMono_14); // Nicer looking font
  tft.setFont(AwesomeF000_20);     // Awesome - Graphic Glyphs
  
  for (uint8_t a=0; a<4; a++) 
  {
    buttons[a].initButton(&tft, BUTTON_X+a*(BUTTON_W+BUTTON_SPACING_X), 
               BUTTON_Y,           // x, y, w, h, outline, fill, text
               BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[a], ILI9341_WHITE,
               buttonlabels[a], BUTTON_TEXTSIZE); 
    
    buttons[a].drawButton();
  }
  
  touch.onscreen = true;
}

//
//-----------------------------------------------------------------------------------------
//      Erase the four Pushbuttons
//-----------------------------------------------------------------------------------------
//
void eraseTouchButtons(void)       // A simple draw black box to erase
{
  tft.fillRect(0, BUTTON_Y-BUTTON_H/2,320,BUTTON_H,ILI9341_BLACK);
  touch.onscreen = false;
}


//
//-----------------------------------------------------------------------------------------
//      Manage Touch Screen
//      When in Meter mode, the screen is divided into upper two-thirds and lower one-third
//      as well as a left one-third and right two-thirds.
//      In addition, a short push and a long push can be identified within any of these areas.
//      When in Config Menu Mode, four push buttons are set up along the bottom of the screen.
//      Result of Function is presented in a number of bools:
//      touch.pushed, longpush, up, down, enact, exit, upper, lower.
//-----------------------------------------------------------------------------------------
//
void manage_Touchscreen(void)
{    
  unsigned long          currenttime;
  static   unsigned long startpush;            // Time when push starts
  static   bool          push;
  
  //tft.setFontAdafruit();                     // Default font
  //tft.setFont(DroidSansMono_14);             // Nicer looking font
  tft.setFont(AwesomeF000_20);                 // Awesome - Graphic Glyphs

  currenttime = millis();                      // Grab time
  
  TS_Point p = ts.getPoint();                  // Retrieve a point, if touched
  /*
  Serial.print("x: ");                         // Debug stuff to calibrate touch screen
  Serial.print(p.x);
  Serial.print(", y: ");
  Serial.print(p.y);
  Serial.print(", z: ");
  Serial.println(p.z);
  */
  #if FT6206_INSTALLED
  p.y = map(p.y, 0, 320, 320, 0);              // Flip it around to match the screen.
  #else
  #if XPT2046_YINVERT 
  p.y = map(p.y, TS_MAXY, TS_MINY, 0, tft.height());
  #else
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());    
  #endif
  #if XPT2046_XINVERT 
  p.x = map(p.x, TS_MAXX, TS_MINX, 0, tft.width());
  #else
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());    
  #endif
  #endif  
  
  // Rotate touch screen if tft has been rotated
  if (R.disp.rotate)
  {
    #if FT6206_INSTALLED                       // X and Y are swapped on Adafruit Cap Touch screen
    p.y = map(p.y, 0, 320, 320, 0);            // Flip it around
    p.x = map(p.x, 0, 240, 240, 0);
    #else
    p.x = map(p.x, 0, 320, 320, 0);            // Flip it around
    p.y = map(p.y, 0, 240, 240, 0);
    #endif
  }
  
  //-------------------------------------------------------------------
  // Manage Touch Screen when not in Configuration Menu
  if (!flag.config_mode)
  {
    #if FT6206_INSTALLED
    if (! ts.touched())                        // No touch
    #else
    if (p.z < MINPRESSURE)
    #endif
    {
      if (push)
      {
        push = false;
        if (currenttime - startpush >= 1000)   // Indicate Long push
          touch.longpush = true;
        else                                   // do stuff based on coordinates...
          touch.pushed = true;                 // Indicate normal push            
      }
      return;                                  // Exit
    }
  
    if (!push)                                 // Grab time when push starts
    {
      push = true;
      startpush = currenttime;
    }

    // Identify Region being Pushed
    #if FT6206_INSTALLED                       // X and Y are swapped on Adafruit Cap Touch screen
    if (p.x < 140) touch.upper = true;         // Upper 2/3 of screen
    #else
    if (p.y < 140) touch.upper = true;         // Upper 2/3 of screen
    #endif
    else touch.lower = true;                   // Lower 1/3 of screen
    
    #if FT6206_INSTALLED                       // X and Y are swapped on Adafruit Cap Touch screen
    if (p.y < 100) touch.left  = true;         // Left 1/3 of screen
    #else
    if (p.x < 100) touch.left = true;          // Left 1/3 of screen
    #endif
  }

  //-------------------------------------------------------------------
  // Manage Touch Screen while in Configuration Menu
  else
  {
    #if FT6206_INSTALLED
    if (ts.touched())
    #else
    if (p.z > MINPRESSURE)
    #endif
    {        
      if (!push)                               // Grab start time of push
      {
        push = true;
        startpush = currenttime;
      }

      for (uint8_t b=0; b<4; b++)              // Check wheter a button has been pressed
      {
        #if FT6206_INSTALLED                   // X and Y are swapped on Adafruit Cap Touch screen
        if (buttons[b].contains(p.y, p.x))     // Capture Individual Button Press
        #else
        if (buttons[b].contains(p.x, p.y))     // Capture Individual Button Press        
        #endif
        {
          buttons[b].press(true);              // Tell the button it is pressed
          if (buttons[b].justPressed()) 
            buttons[b].drawButton(true);       // Draw invert when pressed
        }
        else                                   // Capture traveling finger syndrome
        {
          buttons[b].press(false);             // Release button that is not pressed anymore
          if (buttons[b].justReleased()) 
            buttons[b].drawButton(false);
        }
      }
    }
    // Not pushed anymore or push duration exceeds 0.2s (repeat action on steady push)
    #if FT6206_INSTALLED
    if ((!ts.touched()) || ((currenttime-startpush) > 200))
    #else
    if ((p.z <  MINPRESSURE) || ((currenttime-startpush) > 200))
    #endif
    {
      push = false;
      // now we can ask the buttons if their state has changed
      for (uint8_t b=0; b<4; b++)
      {
        buttons[b].press(false);               // Tell the button it is NOT pressed
        if (buttons[b].justReleased())
        {
          buttons[b].drawButton(false);        // Draw button normal
          if      (b == 0) touch.enact = true; // Enact button has been pressed
          else if (b == 1) touch.up = true;    // Up Button has been pressed
          else if (b == 2) touch.down = true;  // Down Button has been pressed
          else             touch.exit = true;  // Exit Button has been pressed
        }
      }
    }
  }
}

#endif

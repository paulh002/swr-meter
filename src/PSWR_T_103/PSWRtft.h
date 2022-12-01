//*********************************************************************************
//**
//** TFT Power and SWR Meter Gauges; and
//** virtual TextLCD, using an ILI9341 based TFT LCD.
//** Virtual TextLCD object requires a monospace font.
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

#ifndef _PSWRtft_h_
#define _PSWRtft_h_

class PowerMeter
{
  public:
    //------------------------------------------------------------------------------
    // Init size, shape and colour of graph
    //           left x;    top y; length x; height y;   graph, low and high colour 
    void init (int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, int16_t);
    void init (int16_t, int16_t, int16_t, int16_t);
    //------------------------------------------------------------------------------
    // Draw, Redraw or Erase Meter Scale.  range provides optinal unit information, such as "mW", "W", "kW"
    // input argument is a number from 1.0 and up, divided into 10  or 11 segments (11 if (scale*10)%11=0 )
    void scale(double s, char *range);      // Scale Erases if required and then draws everything
    void scale(double s) { scale(s, nullptr); }// Blank if no scale range string
    void erase(void);                       // Erases everything
    //------------------------------------------------------------------------------
    // Draw Meter Graph
    // input arguments are low value, high value and full scale value
    void graph(double, double, double);

  private:
    void drawscale(double, int16_t, char*); // Draws the scale for the graph. Colour to draw/erase
    void printunits(int16_t, char*);        // Prints an units indication at the last scale tick
    void drawframe(int16_t);                // Draws the frame around the graph
    void erasegraph(void);                  // Erase the current graph contents
 
    int16_t x           = 5; 
    int16_t y           = 5;
    int16_t len         = 300;
    int16_t height      = 25;
    int16_t GaugeColour = ILI9341_WHITE;
    int16_t LowColour   = ILI9341_YELLOW;
    int16_t HighColour  = ILI9341_BLUE; 
    int16_t blnkColour  = ILI9341_BLACK;
    double  lastmax;                        // Max Level for full readout.  We have to redraw all if this changes
    int16_t lastlow, lasthigh;              // Prev levels for the two gauges, used to redraw only what is necessary
    double  current_scale = 0;
    char    current_range[10];
    bool    erased = true;                  // Keep track of whether graph needs to be redrawn
};


class VSWRmeter
{
  public:
    //------------------------------------------------------------------------------
    // Init size, shape and colour of VSWR graph
    //        left x; top y; length x; height y;  graph, low SWR,  mid SWR, high SWR colour 
    void init (int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, int16_t);
    void init (int16_t, int16_t, int16_t, int16_t);
    //------------------------------------------------------------------------------
    // Draw or Erase VSWR Meter Scale (no value)
    void scale(void);
    void erase(void);  // Erases everything
    //------------------------------------------------------------------------------
    // Draw VSWR Meter Graph
    // input arguments are mid SWR, high SWR and actual SWR input value
    void graph(double, double, double);

  private:
    void drawscale(int16_t);
    
    int16_t x = 5; 
    int16_t y = 5;
    int16_t len         = 300;
    int16_t height      = 25;
    int16_t GaugeColour = ILI9341_WHITE;
    int16_t lSWRcolour  = ILI9341_GREEN;
    int16_t mSWRcolour  = ILI9341_ORANGE;
    int16_t hSWRcolour  = ILI9341_RED;
    int16_t blnkColour  = ILI9341_BLACK;

    int16_t scalemid, scalehigh;        // Calculated log10 of mid and high values
    int16_t lastlow, lastmid, lasthigh; // Previous values, used to redraw only what is necessary
    double  midthresh, highthresh;      // Keep track of Mid and High thresholds
    bool    erased = true;              // Keep track of whether graph needs to be redrawn
};


#define SCOPE_BUFSIZE  480              // NOTE: Max number of iterations during one X_trace iteration

class ModulationScope
{
  public:
    //------------------------------------------------------------------------------
    // Init size, shape and colour of Modulation Scope
    //          left x;   top y; length x; height y; border, trace and peak colour 
    void init (int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, int16_t);
    void init (int16_t, int16_t, int16_t, int16_t);
    void init (void) { init(5, 5, 310, 150); }  // Default x, y coordinate settings

    //------------------------------------------------------------------------------
    // Draw scope if need be, and draw pending data onscreen
    void update(void);
    //------------------------------------------------------------------------------
    // Erase the scope widget
    void erase(void);
    //------------------------------------------------------------------------------
    // Update rate divisor
    void rate(int16_t);
    //------------------------------------------------------------------------------
    // Add one measurement to the x axis sweep (run from within interrupt function)
    // input arguments are level and fullscale
    void adddata(double, double);

  private:
    void draw(void);                // Draw scope if needed
    int16_t x;        //=   5; 
    int16_t y ;       //=   5;
    int16_t len;      //= 310;
    int16_t height;   //= 180;
    int16_t borderColour= ILI9341_WHITE;
    int16_t traceColour = ILI9341_GREEN;
    int16_t peakColour  = ILI9341_YELLOW;
    int16_t blnkColour  = ILI9341_BLACK;

    int16_t y_axis0;                // Start positions and y axis max deviation, derived from x,y, len, height
    int16_t x_axis0;
    int16_t y_max;

    int16_t newdata[SCOPE_BUFSIZE]; // New data for the scope - fed with data from a circular buffer fed by the interrupt funciton
    int16_t in_pos;                 // Two circular buffer pointers
    int16_t out_pos;                // this one is also current position on the x axis
    int16_t olddata[SCOPE_BUFSIZE]; // Current data on the scope - to add or subtract from...
    int16_t rateDivisor=1;          // Rate divisor other than 1 results in only every n_th request drawn
    int16_t rateCount;
    bool    erased = false;         // Keep track of whether scope needs to be erased
    bool    drawn  = false;         // Keep track of whether scope needs to be redrawn

};


#define TEXTBOXSIZE  200                // NOTE: Max size of character array (columns * rows) is Fixed at 200 characters

class TextBox
{
  public:
    //------------------------------------------------------------------------------
    // Init size, shape and colour of Text LCD.
    // variables are columns, rows; upper left corner x, y; Font_name, FG Colour, BG Colour
    void init (int16_t, int16_t, int16_t, int16_t, const GFXfont &, int16_t, int16_t);
    void init (void) ;  // Default settings
    //-----------------------------------------------------------------------------------------
    // Move characters to LCD from virtual LCD, only updates information that has changed
    void transfer(void);
    //-----------------------------------------------------------------------------------------
    // Print to a Virtual LCD - Max LCD_TEXTSIZE character long string representing a Column*Row LCD
    void write(char);                   // Write char
    void print(const char *);           // primitive Print command, no smart formatting
    void setCursor(int16_t, int16_t);   // SetCursor virtual LCD
    void clear(void);                   // Clear virtual LCD
    void print(String ch_in);
  
  private:
    int16_t xoffs = 10;                 // Graphics display offset coordinates
    int16_t yoffs =  1;
    int16_t Col   = 20;                 // Default Columns and Rows
    int16_t Row   = 10;
    int16_t fontXsize = 12;             // Default font size coordinates for an 18 point font
    int16_t fontYsize = 23;
    char    text_lcd[TEXTBOXSIZE+1];    // Character array representing what is visible on LCD
    char    virt_lcd[TEXTBOXSIZE+1];    // Character array representing what is pending for LCD
    int16_t virt_x, virt_y;             // x and y text display coordinates
    const   GFXfont *font;
    int16_t fontColour = ILI9341_WHITE;
    int16_t blnkColour = ILI9341_BLACK;
};

#endif

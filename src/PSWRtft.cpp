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

#include "PSWR_T.h"

extern var_t R;     // R.low_power_floor flag/variable used with Modulation Scope

//
//-----------------------------------------------------------------------------------------
//      Initialise Dual Power Meter, 
//      define left x and top y coordinates; 
//      length x; height y;
//      graph, low and high colour 
//-----------------------------------------------------------------------------------------
//
void PowerMeter::init(int16_t xcoord, int16_t ycoord, int16_t xlen, int16_t ylen, 
                      int16_t graphcolour, int16_t lowcolour, int16_t highcolour)
{
  x           = xcoord; 
  y           = ycoord;
  len         = xlen;
  height      = ylen;
  GaugeColour = graphcolour;
  LowColour   = lowcolour;
  HighColour  = highcolour; 
}
void PowerMeter::init(int16_t xcoord, int16_t ycoord, int16_t xlen, int16_t ylen)
{
  x           = xcoord; 
  y           = ycoord;
  len         = xlen;
  height      = ylen;
}

//
//-----------------------------------------------------------------------------------------
//      Draw, Redraw or Erase Power Meter Scale.
//      If requested action is same as last time, then nothing is done.
//      First input argument is a number from 1.0 and up, divided into 10  or 11 segments
//      (11 if (scale*10)%11=0 and if second input is used to indicate unit)
//      Second, optional input argument, is an unit indication for the measured level,
//      eg "uW", "mW", "W", "kW"
//-----------------------------------------------------------------------------------------
//
void PowerMeter::scale(double s, char *range)
{
  if (erased)                          // Draw complete Graph if requested and not already drawn
  {
    drawframe(GaugeColour);
    drawscale(s, GaugeColour, range);
    printunits(GaugeColour, range);
    current_scale = s;
    strcpy(current_range, range); 
    erased = false;
  }
  else 
  {
    if (current_scale != s)            // Replace scale if changed
    {
      drawscale(current_scale, blnkColour, current_range);
      drawscale(s, GaugeColour, range);
      current_scale = s;
    }
    if (strcmp(current_range, range))  // Replace units indication if changed
    {
      printunits(blnkColour, current_range);
      printunits(GaugeColour, range);
      strcpy(current_range, range); 
    }
  }
}
void PowerMeter::erase(void)
{
  if (!erased)
  {
    drawframe(blnkColour);
    drawscale(current_scale, blnkColour, current_range);
    printunits(blnkColour, current_range);
    erasegraph();
    erased = true;
  }
}
void PowerMeter::drawframe(int16_t colour) 
{
  tft.drawRect(x, y,len+2, height+2, colour);       // Draw the Graph Outline  
  tft.drawFastHLine(x, y + height+2, len+2, colour);// Draw the Scale line below
}
void PowerMeter::printunits(int16_t colour, char *range) 
{  
  tft.setFont(Arial_8); 
  tft.setTextColor(colour);
  tft.setCursor( x+len - 4*strlen(range), y+height+18);
  tft.print(range);                  
}
void PowerMeter::drawscale(double scale, int16_t colour, char *range) 
{  
  double  i, j, k;
  uint8_t subdecimal;
  int16_t offs = 0;
  double  divisor;                                  // Divisor of 10 or 11.
  
  // Certain scales are good for 11 segments, if range indication is used
  if (((int)(scale*10+.1)%11) == 0) divisor = 11;
  else divisor = 10;  
  
  tft.setFont(Arial_8); 
  tft.setTextColor(colour);
  tft.drawFastVLine( x+1, y+height+2, 5, colour);   // Draw the Scale
  tft.setCursor( x-1, y+height+18);                  // 1st value
  tft.print("0");     
  // Bars and numbers evenly spread over the full length
  for (i=len/divisor, j=scale/divisor; i<=len; i+=len/divisor, j+=scale/divisor)
  {
    tft.drawFastVLine( x+i, y+height+2, 5, colour); 
    if ((i>=len-1) && (range[0]!=0))                // Units at end of scale, if used            
    {
      ;    // If Units (e.g. "mW") used then do nothing here,
           // printunits(...) will take care of it
    }
    else
    {
      k=j; // Figure out offset for the value to be printed, based on number of digits
      offs = 3;
      while ((k+.1)>=10)
      {
        offs += 3;
        k /= 10;
      }
      // Decide whether we have subdecimals or not
      if ((modf(j, &k) > 0.05) && (modf(j, &k) < 0.95)) subdecimal = 1; 
      else subdecimal = 0;
      if (subdecimal) offs +=4;
      tft.setCursor( x+i - offs, y+height+18);
      tft.print(j, subdecimal);          
    }
  }
  for (i=len/(2*divisor); i<=len; i+=len/divisor)   // Draw additional 10 inbetween midpoints
  {
    tft.drawFastVLine( x+i, y+height+2, 3, colour); 
  }
}

//
//-----------------------------------------------------------------------------------------
//      Draw Power Meter Graph in an incremental/decremental manner
//      input arguments are low value, high value and full scale value
//      Note that high value shown is always same or higher than low value shown
//-----------------------------------------------------------------------------------------
//
void PowerMeter::graph(double lowlevel, double highlevel, double maxlevel)
{
  int16_t low, high;

  if (lowlevel > maxlevel) lowlevel = maxlevel;     // Prepare max bounds of input values
  if (highlevel > maxlevel) highlevel = maxlevel;
  if (lowlevel < 0)  lowlevel  = 0;
  if (highlevel < 0) highlevel = 0;

  low  = len * lowlevel / maxlevel;                 // Calculate length of lower level
  high = len * highlevel / maxlevel;                // Calculate length of higher level
  if (high < low) high = low;                       // Higher level is never lower than lower

  if (low > lastlow)                                // Draw an increasing low level
  {
    tft.fillRect(x+1+lastlow, y+1, low-lastlow, height, LowColour);
    if (high > lasthigh)                            // Draw an increasing high level
    {
      if (lasthigh > low)
        tft.fillRect(x+1+lasthigh, y+1, high-lasthigh, height, HighColour);
      else if ((lasthigh < low) && (high != low))
        tft.fillRect(x+1+low, y+1, high-low, height, HighColour);
    }
    else if (high < lasthigh)                       // Draw a decreasing high level
      tft.fillRect(x+1+high, y+1, lasthigh-high, height, blnkColour);
  }

  else if (low < lastlow)                           // Draw a decreasing low level
  {
    if (high <= lastlow)                            // Adjust high level
    {
      if (high != low)
        tft.fillRect(x+1+low, y+1, high-low, height, HighColour);  
      if (lasthigh != high) 
        tft.fillRect(x+1+high, y+1, lasthigh-high, height, blnkColour);
    }
    else if (high >= lastlow)                       // Adjust high level
    {
      tft.fillRect(x+1+low, y+1, lastlow-low, height, HighColour);
      if (high < lasthigh)
        tft.fillRect(x+1+high, y+1, lasthigh-high, height, blnkColour);
      else if (high > lasthigh)
        tft.fillRect(x+1+lasthigh, y+1, high-lasthigh, height, HighColour);
    }    
  }

  else if (high > lasthigh)                         // Draw an increasing high level while low level is unchanged
    tft.fillRect(x+1+lasthigh, y+1, high-lasthigh, height, HighColour);
  else if (high < lasthigh)                         // Draw a decreasing high level while low level is unchanged
    tft.fillRect(x+1+high, y+1, lasthigh-high, height, blnkColour);

  lastlow = low;
  lasthigh = high; 
}
void PowerMeter::erasegraph(void)
{
  if (lasthigh > 0)
  {
    tft.fillRect(x+1, y+1, lasthigh, height, blnkColour);
    lastlow  = 0;
    lasthigh = 0;
    lastmax  = 0;
  }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

//
//-----------------------------------------------------------------------------------------
//      Initialise SWR Meter, 
//      define left x and top y coordinates; 
//      length x; height y;
//      graph, lowSWR, midSWR and highSWR colour 
//-----------------------------------------------------------------------------------------
//
void VSWRmeter::init (int16_t xcoord, int16_t ycoord, int16_t xlen, int16_t ylen, 
                int16_t graphcolour, int16_t lowcolour, int16_t midcolour, int16_t highcolour)
{
  x           = xcoord; 
  y           = ycoord;
  len         = xlen;
  height      = ylen;
  GaugeColour = graphcolour;
  lSWRcolour  = lowcolour;
  mSWRcolour  = midcolour; 
  hSWRcolour  = highcolour; 
}
void VSWRmeter::init (int16_t xcoord, int16_t ycoord, int16_t xlen, int16_t ylen)
{
  x           = xcoord; 
  y           = ycoord;
  len         = xlen;
  height      = ylen;
}

//
//-----------------------------------------------------------------------------------------
//      Draw SWR Meter Scale
//      input arguments are starting x&y positions
//-----------------------------------------------------------------------------------------
//
void VSWRmeter::scale(void) 
{
  if (erased)                 // Draw complete Graph if requested and not already drawn
  {
    drawscale(GaugeColour);   // Always the same
    erased = false;
  }
}
void VSWRmeter::erase(void)
{
  if (!erased)
  {
    graph(2.0, 3.0, 0.0);
    drawscale(blnkColour);
    midthresh = 0;
    highthresh = 0;
    erased = true;    
  }
}
void VSWRmeter::drawscale(int16_t colour) 
{  
  int16_t  offs;
  bool     subdecimal;
  double   a;
  double   scalemark[] = { 1, 1.5, 2, 3, 4, 5, 6, 7, 8, 10 };
  double   scaletick[] = { 1.1, 1.2, 1.3, 1.4, 1.6, 1.7, 1.8, 1.9,
                           2.2, 2.4, 2.6, 2.8, 3.2, 3.4, 3.6, 3.8,
                           4.5, 5.5, 6.5,   9 };
  tft.setFont(Arial_8);
  tft.setTextColor(colour);

  // Draw the Graph Outline
  tft.drawRect(x, y,len+2, height+2, colour);
  // Draw the Scale line below
  tft.drawFastHLine(x, y+height+2, len+2, colour);
  // Draw the Scale
  for (uint8_t i = 0; i < 10; i++)         // Draw full size scale ticks and numbers
  { 
    offs = x+1 + len * log10(scalemark[i]);
    tft.drawFastVLine(offs, y+height+2, 5, colour);

    if ((scalemark[i] < 10) &&             // Decide whether we have subdecimals or not
        (modf(scalemark[i], &a) > 0.05) &&
        (modf(scalemark[i], &a) < 0.95)) 
      subdecimal = true;
    else subdecimal = false;
    
    if (subdecimal) offs -= 10;             // Position text and print
    else if (scalemark[i]>=10) offs -= 11;
    else offs -= 2;    
    tft.setCursor(offs-3, y+height+18);                
    if (scalemark[i]<10) tft.print(scalemark[i], subdecimal);
    else 
      {
        tft.setCursor(offs-6, y+height+18);                
        tft.print("SWR");                  // Print indicator at highest mark      
      }
  }
  for (uint8_t i = 0; i < 20; i++)         // Draw half size scale ticks
  {
    offs = x+1 + len * log10(scaletick[i]);
    tft.drawFastVLine(offs, y+height+2, 3, colour);
  }
}

//
//-----------------------------------------------------------------------------------------
//      Draw SWR Meter Graph
//      input arguments are starting x&y positions, swr value, mid and high indications.
//      swr, mid and high as values between 1 and infinite.  mid and high for colour
//      indication above set value, typically 2.0 and 3.0
//-----------------------------------------------------------------------------------------
//
void VSWRmeter::graph(double mid, double high, double swr)
{
  int16_t barlow;                                 // Length of SWR bar on graph
  int16_t barmid  = 0;                            // Indicate yellow above this swr level (mid threshold)
  int16_t barhigh = 0;                            // Indicate red above this swr level (high threshold)
       
  // Set sane limits
  if (swr < 1.0) swr = 1.0;
  if (mid > high) mid = high;
  
  // We need to force a blank slate to redraw everything if scale thresholds have changed
  if ((midthresh != mid) || (highthresh != high))
  {
    swr = 1.0;                                    // Set SWR at 1.0:1 (eq. 0)                 

    // Prepare scale thresholds
    scalemid  = len * log10(mid);                 // Recalculate Mid and High threshold in bar lengths
    scalehigh = len * log10(high);
    lastlow = scalemid;                           // Set previous value as the highest
    lastmid = scalehigh-scalemid;                 // possible value, to force a redraw
    lasthigh = len-scalehigh;
    midthresh = mid;                              // And store threshold values for next comparison
    highthresh = high;   
  }
  
  // Prepare input values
  if (swr > 10) swr = 10;
  barlow  = len * log10(swr);                     // Determine overall bar length
  barhigh = barlow - scalehigh;                   // Determine length of high bar
  if (barhigh < 0) barhigh = 0;
  if (barhigh > 0) barmid = scalehigh - scalemid; // Determine length of mid bar
  else barmid = barlow - scalemid;
  if (barmid < 0) barmid = 0;
  if (barlow > scalemid) barlow = scalemid;       // Truncate low bar if needed

  // Fill the bargraph
  if (barlow > lastlow)                                                             // Low bar increasing
    tft.fillRect(x+1+lastlow, y+1, barlow-lastlow, height, lSWRcolour);             // Fill difference between old and new
  else if (barlow < lastlow)                                                        // Low bar decreasing
    tft.fillRect(x+1+barlow, y+1, lastlow-barlow, height, blnkColour);              // Empty difference between old and new

  if (barmid > lastmid)                                                             // Mid bar increasing
    tft.fillRect(x+1+lastmid+scalemid, y+1, barmid-lastmid, height, mSWRcolour);    // Fill difference between old and new   
  else if (barmid < lastmid)                                                        // Mid bar decreasing
    tft.fillRect(x+1+barmid+scalemid, y+1, lastmid-barmid, height, blnkColour);     // Empty difference between old and new

  if (barhigh > lasthigh)                                                           // High bar increasing
    tft.fillRect(x+1+lasthigh+scalehigh, y+1, barhigh-lasthigh, height,hSWRcolour); // Fill difference between old and new    
  else if (barhigh < lasthigh)                                                      // High bar decreasing
    tft.fillRect(x+1+barhigh+scalehigh, y+1, lasthigh-barhigh, height, blnkColour); // Empty difference between old and new

  lastlow = barlow;    
  lastmid = barmid;   
  lasthigh= barhigh;     
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

//
//-----------------------------------------------------------------------------------------
//      Initialise Modulation Scope, 
//      define left x and top y coordinates; 
//      length x; height y;
//      border, trace, and peak colour 
//-----------------------------------------------------------------------------------------
//
void ModulationScope::init (int16_t xcoord, int16_t ycoord, int16_t xlen, int16_t ylen, 
                            int16_t bordercolour, int16_t tracecolour, int16_t peakcolour)
{
  borderColour= bordercolour;
  traceColour = tracecolour;
  peakColour  = peakcolour; 
  init(xcoord, ycoord, xlen, ylen);
}
void ModulationScope::init (int16_t xcoord, int16_t ycoord, int16_t xlen, int16_t ylen)
{
  x           = xcoord; 
  y           = ycoord;
  len         = xlen;
  if (len > SCOPE_BUFSIZE) len = SCOPE_BUFSIZE;
  height      = ylen;
  y_axis0     = y+1 + height/2;
  x_axis0     = x+1;
  y_max        = height/2;
}

//
//-----------------------------------------------------------------------------------------
//      Erase Modulation Scope
//-----------------------------------------------------------------------------------------
//
void ModulationScope::erase(void)
{
  if (!erased)
  {
    tft.fillRect(x, y,len+2, height+15, blnkColour); // A simple/crude draw blank rectangle
    for (uint16_t a=0; a<len; a++) olddata[a] = 0;   // Erase scope redraw data
    in_pos = 0;
    out_pos = 0;
    erased = true;
    drawn = false;   
  
    tft.setFont(Arial_8); 
    tft.setTextColor(blnkColour);
    tft.setCursor(x+len - 170, y+height + 15);
    tft.print("Modulation Scope");
  
  }
}
void ModulationScope::rate(int16_t rate)
{  
  rateDivisor = rate;
}

//
//-----------------------------------------------------------------------------------------
//      Update the Modulation Scope circulr buffer with one measurement
//-----------------------------------------------------------------------------------------
//
void ModulationScope::adddata(double level, double fullscale)
{
  if (rateCount <= 0)                           // Only add data every 1/rateDivisor time.
  {
    rateCount = rateDivisor;
              
    if (fullscale < level) fullscale = level;   // Set sane limits
    
    switch (R.low_power_floor)                  // Set minimum fullscale at 50x selected Low Power Floor
    {
      case FLOOR_TEN_mW:
        if (fullscale<500.0) fullscale = 500.0; // 500 mW
        break;
      case FLOOR_ONE_mW:
        if (fullscale < 50.0) fullscale = 50.0; // 50 mW
        break;
      case FLOOR_100_uW:
        if (fullscale < 5) fullscale = 5;       // 5 mW
        break;
      case FLOOR_TEN_uW:
        if (fullscale < 0.5) fullscale = 0.5;   // 500 uW
        break;
      case FLOOR_ONE_uW:
      default:
        if (fullscale < .05) fullscale = .05;   // 50 uW      
    }    

    newdata[in_pos++] = level/fullscale*y_max;  // Prepare and store incoming level
    if (in_pos >= len) in_pos = 0; 
  }
  rateCount--; 
}

//
//-----------------------------------------------------------------------------------------
//      Draw (if needed) and Update the TFT with current Modulation Scope data pending in circular buffer
//-----------------------------------------------------------------------------------------
//
void ModulationScope::draw(void) 
{
  if (!drawn)
  {
    tft.setFont(Arial_8); 
    tft.drawRect(x, y,len+2, height+3, borderColour);  // Draw the Scope Outline
    tft.setTextColor(borderColour);
    tft.setCursor(x+len - 170, y+height + 15);
    tft.print("Modulation Scope");               // and print title
    erased = false;
    drawn = true;
  }
}

void ModulationScope::update(void)
{
  int16_t y_start, y_stop, y_len;              // Coordinates to draw
       
  draw();                                      // Draw scope widget if need be
  
  while (out_pos != in_pos)                    // Only draw every 1/rateDivisor time.
  {
 
    if (newdata[out_pos] > olddata[out_pos])   // Draw/Add Above y0
    {
      y_start = y_axis0 - newdata[out_pos];
      y_stop  = y_axis0 - olddata[out_pos];
      y_len = y_stop - y_start;
      if (y_len>0) tft.drawFastVLine(x_axis0+out_pos, y_start+1, y_len, traceColour);
      tft.drawPixel(x_axis0+out_pos,y_start, peakColour);
      
      y_start  = y_axis0 + olddata[out_pos];   // Draw/Add Below y0
      y_stop = y_axis0 + newdata[out_pos];
      y_len = y_stop - y_start;
      if (y_len>0) tft.drawFastVLine(x_axis0+out_pos, y_start,  y_len, traceColour);

      tft.drawPixel(x_axis0+out_pos,y_stop, peakColour);
    }
    else if (newdata[out_pos] < olddata[out_pos])
    {
      y_start  = y_axis0 - olddata[out_pos];   // Erase Above y0
      y_stop = y_axis0 - newdata[out_pos];
      y_len = y_stop - y_start;
      if (y_len>0) tft.drawFastVLine(x_axis0+out_pos, y_start,  y_len, blnkColour);
      tft.drawPixel(x_axis0+out_pos,y_stop, peakColour);
      
      y_start = y_axis0 + newdata[out_pos];    // Erase Below y0
      y_stop  = y_axis0 + olddata[out_pos];
      y_len = y_stop - y_start;
      if (y_len>0) tft.drawFastVLine(x_axis0+out_pos, y_start+1, y_len, blnkColour);

      tft.drawPixel(x_axis0+out_pos,y_start, peakColour);   
    }
    else if (newdata[out_pos] == 0) tft.drawPixel(x_axis0+out_pos,y_axis0, peakColour);

    olddata[out_pos] = newdata[out_pos];
    out_pos++;
    if (out_pos >= len) out_pos = 0;
  }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

//
//-----------------------------------------------------------------------------------------
//      Init size, shape and colour of Text LCD.
//      Variables are: columns, rows; upper left corner x, y; Font_name, FG Colour, BG Colour
//-----------------------------------------------------------------------------------------
//
// NOTE: Max size of character array (columns * rows) is fixed at LCD_TEXTSIZE characters
// variables are columns, rows; upper left corner x, y; Font_name, FG Colour, BG Colour
void TextBox::init(int16_t _col, int16_t _row, int16_t _xoffs, int16_t _yoffs, 
                   const GFXfont &f, int16_t _fontColour, int16_t _blnkColour)
{
  xoffs =      _xoffs; 
  yoffs =      _yoffs;
  Col   =      _col;
  Row   =      _row;
  font  =      &f;
  fontColour = _fontColour;
  blnkColour = _blnkColour;

  tft.setFont(font);             // Determine font size
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(0,0);
  tft.print("a");                 // Print an arbitrary character to determine
  fontXsize = tft.getCursorX();   // size of each char, using monospace font
  tft.println();
  fontYsize = tft.getCursorY() -4;
  tft.setCursor(0,0);
}

void TextBox::init (void) 
{ 
  font = &FONT_12; 
  tft.setFont(font);             // Determine font size
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(0,0);
  tft.print("a");                 // Print an arbitrary character to determine
  fontXsize = tft.getCursorX();   // size of each char, using monospace font
  tft.println();
  fontYsize = tft.getCursorY() -4;
  tft.setCursor(0,0);
} 


//
//-----------------------------------------------------------------------------------------
//      Move characters to TFT LCD from virtual LCD
//      This function only updates character positions that have changed
//-----------------------------------------------------------------------------------------
//
void TextBox::transfer(void)
{ 
  uint16_t line, column, character=0;

  tft.setFont(font);
  for (line = 0; line < Row; line++)
  {
    for (column = 0; column < Col; column++)
    {
      if (virt_lcd[character] != text_lcd[character]) // We have a new character to write out
      {
        tft.setTextColor(blnkColour);
        tft.setCursor(xoffs + fontXsize*column, yoffs + fontYsize*line + fontYsize);
        tft.print(text_lcd[character]);               // Erase
        tft.setTextColor(fontColour);
        tft.setCursor(xoffs + fontXsize*column, yoffs + fontYsize*line + fontYsize);
        text_lcd[character] = virt_lcd[character];
        tft.print(text_lcd[character]);               // Write new
      }
      character++;
      if (character>=TEXTBOXSIZE) character=TEXTBOXSIZE; // Redundant, make sure we don't go too far
    }
  }
}

//
//-----------------------------------------------------------------------------------------
//      Print to a Virtual LCD - LCD_TEXTSIZE character long string representing a Column*Row LCD
//-----------------------------------------------------------------------------------------
//
void TextBox::write(char ch)
{
  uint8_t virt_pos;
  
  // Print to the ColxRow size virtual LCD
  virt_pos = virt_x + Col*virt_y;                     // Determine position on virt LCD
  if ((virt_pos >= TEXTBOXSIZE) || (virt_pos >= (Col*Row))) virt_pos = 0; // At end, wrap around to beginning 
  virt_lcd[virt_pos++] = ch;                          // Place character on virt LCD

  // After print, derive new x,y coordinates in our Column by Row matrix
  virt_x = virt_pos;
  for(virt_y = 0; virt_y < Row && virt_x >= Col;virt_y++)         
  {
    virt_x -=Col;
  }
}
void TextBox::print(const char *ch_in)
{
  uint8_t virt_len;

  virt_len = strlen(ch_in);                           // Print to the Row*Col sized virtual LCD
  for (uint8_t i = 0; i < virt_len; i++)
  {
    write(ch_in[i]);
  }
}

void TextBox::print(String ch_in)
{
  uint8_t virt_len;
  
  virt_len = ch_in.length();                           // Print to the Row*Col sized virtual LCD
  for (uint8_t i = 0; i < virt_len; i++)
  {
    write(ch_in.charAt(i));
  }
}


//
//-----------------------------------------------------------------------------------------
//      SetCursor virtual LCD
//-----------------------------------------------------------------------------------------
//
void TextBox::setCursor(int16_t x, int16_t y)
{
  virt_x = x;
  virt_y = y;
}
//
//-----------------------------------------------------------------------------------------
//      Clear virtual LCD
//-----------------------------------------------------------------------------------------
//
void TextBox::clear(void)
{
  uint16_t lcdsize = Row * Col;
  if (lcdsize > TEXTBOXSIZE) lcdsize = TEXTBOXSIZE;
  for (uint8_t i=0; i<lcdsize; i++) virt_lcd[i]=' ';  // Print a lot of spaces to virt LCD
  virt_x = 0;
  virt_y = 0;
}

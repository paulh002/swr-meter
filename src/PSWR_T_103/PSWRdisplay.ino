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
//*********************************************************************************

//
//-----------------------------------------------------------------------------
//  Erase everything on screen
//-----------------------------------------------------------------------------
//
void eraseDisplay(void)
{
  if (flag.picture)
  {
    flag.picture = false;
    tft.fillRect(0,0, 320,240, ILI9341_BLACK);// A simple/crude draw blank rectangle
  }

  // TODO: The below could be streamlined - due to the full clear above to remove picture, if present
  VirtLCDb.clear();
  VirtLCDw.clear();                         // Erase text
  VirtLCDy.clear();
  VirtLCDlargeY.clear();
  VirtLCDlargeR.clear();
  VirtLCDb.transfer();
  VirtLCDw.transfer();
  VirtLCDy.transfer();
  VirtLCDlargeY.transfer();
  VirtLCDlargeR.transfer();

  Meter1.erase();                           // Erase meters, if present
  Meter2.erase();
  Meter3.erase();
  Meter4.erase();
  Meter5.erase();
  SWR.erase();
  ModScope.erase();
}


//
//-----------------------------------------------------------------------------
//  Screensaver display
//-----------------------------------------------------------------------------
//
void screensaver(void)
{
  static int16_t count;                     // Force screensaver print when run first time

  flag.screensaver_on = true;
  
  if (flag.idle_refresh)                    // Force screensaver reprint if flag
  {
    flag.idle_refresh = false;              // Clear flag
    count = SLEEPTIME;                      // and force a reprint
    eraseDisplay();                         // Erase everything
  }

  if (Menu_exit_timer == 0)                 // Announcement timer not active
  {
    if (count++ >= SLEEPTIME/2)             // We reprint the output every fiftieth time (approx 5 sec)
    {
      count = 0;
      VirtLCDw.clear();
      VirtLCDy.clear();
      VirtLCDw.transfer();
      VirtLCDy.transfer();
      VirtLCDlargeY.clear();
      VirtLCDlargeR.clear();
      VirtLCDlargeY.transfer();
      VirtLCDlargeR.transfer();
      //tft.writeRect(40,0,240,240,(uint16_t*) Moon240x240); // Draw a pretty picture on screen
      flag.picture = true;                  // Indicate that we have picture on screen
      VirtLCDy.setCursor(rand() % (21 - strlen(R.idle_disp)), rand() % 10) ;   
      VirtLCDy.print(R.idle_disp);
    }
  }
}


//
//-----------------------------------------------------------------------------
//  Display Mode Intro
//-----------------------------------------------------------------------------
//
void lcd_display_mode_intro(const char *line1, const char *line2, const char *line3, const char *line4)
{
  eraseDisplay();
  VirtLCDw.setCursor(0,1);
  VirtLCDw.print(line1);
  VirtLCDy.setCursor(0,3);
  VirtLCDy.print(line2);
  VirtLCDy.setCursor(0,4);
  VirtLCDy.print(line3);
  VirtLCDy.setCursor(0,5);
  VirtLCDy.print(line4);
}


//
//-----------------------------------------------------------------------------------------
//  Determine Power Meter Scale, based on highest instantaneous power reading 
// during the last 30 seconds (routine is called once every 1/50th of a second)
//
//  The scale ranges are user definable, up to 3 ranges per decade
//    e.g. 11, 22 and 55 gives:
//    ... 11mW, 22mW, 55mW, 110mW ... 1.1W, 2.2W, 5.5W 11W 22W 55W 110W ...
//    If all three values set as "2", then: 
//    ... 2W, 20W, 200W ...
//    The third and largest value has to be less than ten times the first value
//  Output is in units of milliWatts (mW). Max 4kW, min 1uW
//-----------------------------------------------------------------------------------------
//
double scale_BAR(double p /* power in milliwatts */)
{
  #define  SCALE_BUFFER  150        // Keep latest 150 power readings in buffer

  // For measurement of peak and average power
  static   uint32_t peak_buff[SCALE_BUFFER];// 30 second window
  static   uint16_t a;              // Entry counter
  uint32_t          max;            // Find max value
  uint32_t          scale;          // Scale output
  
  #if AD8307_INSTALLED
  uint32_t          decade;         // Used to determine range decade, in units of 1uW
  
  // Determine how low the meter range can go, based on the power level floor setting
  if      (R.low_power_floor == FLOOR_ONE_uW) decade = 5;      // Lowest scale (x10 uW) when 1 uW
  else if (R.low_power_floor == FLOOR_TEN_uW) decade = 50;     // Lowest scale at 10 uW
  else if (R.low_power_floor == FLOOR_100_uW) decade = 500;    // Lowest scale at 100 uW
  else if (R.low_power_floor == FLOOR_ONE_mW) decade = 5000;   // Lowest scale at 1 mW
  else if (R.low_power_floor == FLOOR_TEN_mW) decade = 50000;  // Lowest scale at 10 mW
  else decade = 1;                                             // No scale floor
  #else
  uint32_t          decade = 10000; // Determine range decade, lowest is 10mW
  #endif

  // Store peak value in circular buffer
  peak_buff[a] = p * 1000.0;        // Max 4kW, resolution down to 1uW
  a++;
  if (a == SCALE_BUFFER) a = 0;

  // Retrieve the max value out of the measured window
  max = 0;
  for (uint16_t b = 0; b < SCALE_BUFFER; b++)
  {
    if (max < peak_buff[b]) max = peak_buff[b];
  }

  // Determine range decade
  while ((decade * R.ScaleRange[2]) < max)
  {
    decade = decade * 10;
  }
  // Determine scale limit to use, within the decade previously determined
  if    (max >= (decade * R.ScaleRange[1])) scale = decade * R.ScaleRange[2];
  else if (max >= (decade * R.ScaleRange[0])) scale = decade * R.ScaleRange[1];
  else  scale = decade * R.ScaleRange[0];
  return scale/1000.0;              // Return value is in mW
}


//
//-----------------------------------------------------------------------------------------
//  Determine range for the Power Meter, pW to kW
//-----------------------------------------------------------------------------------------
//
void scalePowerMeter(double fullscale, double *out_fullscale, char *range)
{
 uint8_t r = 3;
 const char *indicator[] = { "pW", "nW", "uW", "mW", "W", "kW", "MW", "GW" /* hah! */ };
 
 // If we have less than 1mW  
  while (fullscale < 1.0)
  {
    fullscale *= 1000;
    r -= 1;
  }
  while (fullscale >= 1000)
  {
    fullscale /= 1000;
    r += 1;    
  }
  if (r < 0) r = 0; // Enforce boundaries
  if (r > 7) r = 7;
  
  *out_fullscale = fullscale;
  strcpy(range,indicator[r]);
}


//
//-----------------------------------------------------------------------------
//  Print Power Output level in a large Font
//  Yellow if Normal SWR Normal, Red if SWR Alarm
//-----------------------------------------------------------------------------
//
void power_print_large(const char *str)
{
  static bool previousSWRalarmState;

  // Clear and Flush if Alarm State (colour) change
  if (flag.swr_alarm != previousSWRalarmState)
  {
    VirtLCDlargeY.clear();
    VirtLCDlargeR.clear();
    VirtLCDlargeY.transfer();
    VirtLCDlargeR.transfer();
    previousSWRalarmState = flag.swr_alarm;
  }
  if (flag.swr_alarm)                 // Print Red if Alarm
    VirtLCDlargeR.print(str);
  else                                // Print Yellow if not
    VirtLCDlargeY.print(str);
}


//
//-----------------------------------------------------------------------------
//  Display: Bargraph, Power in Watts, SWR & PEP Power
//  PEP Power always displayed and used for scale selection
//  power variable can be anything passed to function (power_mv_avg, power_mw_pk, etc...)
//-----------------------------------------------------------------------------
//
void lcd_display_clean(const char * introtext, const char * power_display_indicator, double power)
{
  static uint16_t mode_timer;         // Used to time the Display Mode intro
  double scale;                       // Progress bar scale
  double swr_alm;                     // SWR Alarm indication above this point by colour of graph
  double swr_mid = 2.0;               // Default SWR midlevel colour changeover point
  static bool alternate;              // Used to alternate LCD output between Graph and Text - loadspreading
  double adj_scale;                   // Adjust Power scales into ranges of 0 - 1000, uW, mW, W and kW
  char   range[5];
  
  //------------------------------------------
  // Display mode intro for a time
  if(flag.mode_display)
  {
    if(flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      mode_timer = 0;                 // New mode, reset timer
      lcd_display_mode_intro("Mode:"," ",introtext,"and SWR Bargraph");
    }

    mode_timer++;
    if (mode_timer >= MODE_INTRO_TIME)// Done with Mode Intro. MODE_INTRO_TIME in 10ths of seconds
    {
      mode_timer = 0;
      flag.mode_display = false;      // Clear display change mode
      flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode
      eraseDisplay();                 // Immediate blank slate to make room for Meters
      Meter1.init(5, 0,300,50);       // Draw Meters
      SWR.init   (5,75,300,50);
    }
  }

  //----------------------------------------------
  // Display Power if level is above useful threshold
  #if AD8307_INSTALLED
  else if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))
  #else  
  else if ((power_mw > MIN_PWR_FOR_METER) || (flag.power_detected))
  #endif
  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;
      eraseDisplay();                 // Immediate blank slate to make room for Meters
      //VirtLCDw.clear();
      //VirtLCDy.clear();
      //VirtLCDw.transfer();
      //VirtLCDy.transfer();
    }
    alternate ^= 1;                   // Alternate between the Text outputs to spread load

    if (alternate)                    // Print Graphs every second time
    {
      //------------------------------------------
      // Prepare and Print/Update Power Meter
      scale = scale_BAR(power_mw_long); // Determine scale setting
      scalePowerMeter(scale, &adj_scale, range);
      Meter1.scale(adj_scale, range);   // Redraw the bargraph scale if needed
      Meter1.graph(power,power_mw_pep,scale);
    
      //------------------------------------------
      // SWR Meter
      SWR.scale();                    // Draw SWR bargraph;
      swr_alm = R.SWR_alarm_trig/10.0;// Set colour changeover points for swr mid and swr alarm
      if (swr_alm < swr_mid) swr_mid = swr_alm;
      SWR.graph(swr_mid, swr_alm, swr_avg);      
    }
    else                              // And print text every second time
    {
      //------------------------------------------
      // SWR Printout
      VirtLCDw.setCursor(7,9);        // Clear junk in line, if any
      VirtLCDw.print("   ");
      VirtLCDw.setCursor(0,9);
      VirtLCDw.print("SWR:");
      print_swr();                    // and print the "SWR value"
      VirtLCDw.print(lcd_buf);
      
      //------------------------------------------
      // Power Indication
      VirtLCDy.setCursor(0,7);
      VirtLCDy.print(power_display_indicator);
      VirtLCDy.setCursor(2,8);
      if (Reverse) VirtLCDy.print("-");// If reverse power, then indicate
      else         VirtLCDy.print(" ");
      VirtLCDlargeY.setCursor(0,0);    // Print Power in Large font, yellow or red
      VirtLCDlargeR.setCursor(0,0);
      print_p_mw(power);
      power_print_large(lcd_buf);
  
      //------------------------------------------
      // Power indication, 1 second PEP
      VirtLCDy.setCursor(19,9);       // Clear junk in line, if any
      VirtLCDy.print(" ");
      VirtLCDy.setCursor(9,9);
      VirtLCDy.print(" PEP:");
      VirtLCDy.setCursor(14,9);
      print_p_mw(power_mw_pep);
      VirtLCDy.print(lcd_buf);  

      //------------------------------------------
      // Display band and TX indicator
      display_band();
    }
  }
  else                                // Screensaver display
  {
    Meter1.erase();
    SWR.erase();
    VirtLCDlargeY.clear();
    VirtLCDlargeR.clear();
    screensaver();
  }
}

//
//-----------------------------------------------------------------------------
//  Display: Bargraph, Power in dBm, SWR & PEP Power
//-----------------------------------------------------------------------------
//
void lcd_display_clean_dBm(void)
{
  static uint16_t mode_timer = 0;     // Used to time the Display Mode intro
  double scale;                       // Progress bar scale
  double swr_alm;                     // SWR Alarm indication above this point by colour of graph
  double swr_mid = 2.0;               // Default SWR midlevel colour changeover point
  double adj_scale;                   // Adjust Power scales into ranges of 0 - 1000, uW, mW, W and kW
  char range[5];
  
  //------------------------------------------
  // Display mode intro for a time
  if(flag.mode_display)
  {
    if(flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      mode_timer = 0;                 // New mode, reset timer
      lcd_display_mode_intro("Mode:"," ","dBm Meter,  Power","and SWR Bargraph");
    }

    mode_timer++;
    if (mode_timer >= MODE_INTRO_TIME)// Done with Mode Intro. MODE_INTRO_TIME in 10ths of seconds
    {
      mode_timer = 0;
      flag.mode_display = false;      // Clear display change mode
      flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode
      eraseDisplay();
      Meter1.init(5, 0,300,50);       // Draw Meters
      SWR.init   (5,75,300,50);
    }
  }

  //----------------------------------------------
  // Display Power if level is above useful threshold
  #if AD8307_INSTALLED
  else if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))
  #else  
  else if ((power_mw > MIN_PWR_FOR_METER) || (flag.power_detected))
  #endif
  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;
      eraseDisplay();                 // Immediate blank slate to make room for Meters
      //VirtLCDw.clear();
      //VirtLCDy.clear();
      //VirtLCDw.transfer();
      //VirtLCDy.transfer();
    }
    //------------------------------------------
    // Prepare and Print/Update Power Meter
    scale = scale_BAR(power_mw_long); // Determine scale setting
    scalePowerMeter(scale, &adj_scale, range);
    Meter1.scale(adj_scale, range);   // Redraw the bargraph scale if needed
    Meter1.graph(power_mw,power_mw_pep,scale);

    //------------------------------------------
    // SWR Meter
    SWR.scale();                      // Draw SWR bargraph;
    swr_alm = R.SWR_alarm_trig/10.0;  // Set colour changeover points for swr mid and swr alarm
    if (swr_alm < swr_mid) swr_mid = swr_alm;
    SWR.graph(swr_mid, swr_alm, swr_avg);
    
    //------------------------------------------
    // SWR Printout
    VirtLCDw.setCursor(7,9);          // Clear junk in line, if any
    VirtLCDw.print("   ");
    VirtLCDw.setCursor(0,9);
    VirtLCDw.print("SWR ");
    print_swr();                      // and print the "SWR value"
    VirtLCDw.print(lcd_buf);

    //------------------------------------------
    // Power Indication
    VirtLCDy.setCursor(0,7);
    VirtLCDy.print("Power in dB:");
    print_dbm(power_db*10.0);
    VirtLCDy.print(lcd_buf);

    //------------------------------------------
    // Power indication, PEP
    VirtLCDy.setCursor(9,9);
    VirtLCDy.print("PEP");
    print_dbm(power_db_pep*10.0);
    VirtLCDy.print(lcd_buf);

    //------------------------------------------
    // Display band and TX indicator
    display_band();
  }
  else                                // Screensaver display
  {
    Meter1.erase();
    SWR.erase();
    VirtLCDlargeY.clear();
    VirtLCDlargeR.clear();
    screensaver();
  }
}


//
//-----------------------------------------------------------------------------
//  Display Forward and Reflected Power
//  (input power is in mW)
//-----------------------------------------------------------------------------
//
void lcd_display_mixed()
{
  static uint16_t mode_timer = 0;     // Used to time the Display Mode intro
  double scale;                       // Progress bar scale
  double swr_alm;                     // SWR Alarm indication above this point by colour of graph
  double swr_mid = 2.0;               // Default SWR midlevel colour changeover point
  double adj_scale;                   // Adjust Power scales into ranges of 0 - 1000, uW, mW, W and kW
  char  range[5];
  
  //------------------------------------------
  // Display mode intro for a time
  if(flag.mode_display)
  {
    if(flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      mode_timer = 0;                 // New mode, reset timer
      lcd_display_mode_intro("Mode:","Forward and","     Reflected Power","Bargraph Meter");
    }
 
    mode_timer++;
    if (mode_timer >= MODE_INTRO_TIME)// Done with Mode Intro. MODE_INTRO_TIME in 10ths of seconds
    {
      mode_timer = 0;
      flag.mode_display = false;      // Clear display change mode
      flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode
      eraseDisplay();
      Meter1.init(5, 0,300,35);       // Draw Meters
      Meter2.init(5,60,300,15);
      SWR.init   (5,100,300,15);
    }
  }

 //----------------------------------------------
  // Display Power if level is above useful threshold
  #if AD8307_INSTALLED
  else if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))
  #else  
  else if ((power_mw > MIN_PWR_FOR_METER) || (flag.power_detected))
  #endif
  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;
      eraseDisplay();                 // Immediate blank slate to make room for Meters
      //VirtLCDw.clear();
      //VirtLCDy.clear();
      //VirtLCDw.transfer();
      //VirtLCDy.transfer();
    }
    
   //------------------------------------------
    // Prepare and Print/Update Power Meters
    scale = scale_BAR(fwd_power_mw);     // Determine scale setting
    scalePowerMeter(scale, &adj_scale, range);
    Meter1.scale(adj_scale, range);      // Redraw the bargraph scale if needed
    Meter1.graph(fwd_power_mw, 0,scale); // No PEP in this mode
    
    Meter2.scale(adj_scale, range);      // Redraw the bargraph scale if needed
    Meter2.graph(ref_power_mw, 0,scale); // No PEP in the reverse direction
    
    //------------------------------------------
    // SWR Meter
    SWR.scale();                         // Draw SWR bargraph;
    swr_alm = R.SWR_alarm_trig/10.0;     // Set colour changeover points for swr mid and swr alarm
    if (swr_alm < swr_mid) swr_mid = swr_alm;
    SWR.graph(swr_mid, swr_alm, swr_avg);
    
    //------------------------------------------
    // Forward Power Indication
    VirtLCDy.setCursor(0,7);
    VirtLCDy.print("Fwd:");
    VirtLCDlargeY.setCursor(0,0);      // Print Power in Large font, yellow or red
    VirtLCDlargeR.setCursor(0,0);
    print_p_mw(fwd_power_mw);
    power_print_large(lcd_buf);
    

    //------------------------------------------
    // Reflected Power indication
    VirtLCDy.setCursor(19,9);          // Clear junk in line, if any
    VirtLCDy.print(" ");
    VirtLCDy.setCursor(10,9);
    VirtLCDy.print("Ref:");
    VirtLCDy.setCursor(14,9);
    print_p_mw(ref_power_mw);
    VirtLCDy.print(lcd_buf);

   //------------------------------------------
    // SWR Printout
    VirtLCDw.setCursor(7,9);          // Clear junk in line, if any
    VirtLCDw.print("   ");
    VirtLCDw.setCursor(0,9);
    VirtLCDw.print("SWR:");
    print_swr();                      // and print the "SWR value"
    VirtLCDw.print(lcd_buf);

    //------------------------------------------
    // Display band and TX indicator
    display_band();
  }	

  else                                // Screensaver display
  {
    Meter1.erase();
    Meter2.erase();
    SWR.erase();
    VirtLCDlargeY.clear();
    VirtLCDlargeR.clear();
    screensaver();
  }
}


//
//-----------------------------------------------------------------------------
//  Display Modulation Scope
//-----------------------------------------------------------------------------
//
void lcd_display_modscope(void)
{
  static uint16_t mode_timer = 0;     // Used to time the Display Mode intro
  
  //------------------------------------------
  // Display mode intro for a time
  if(flag.mode_display)
  {
    if(flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      mode_timer = 0;                 // New mode, reset timer
      lcd_display_mode_intro("Mode:"," ","Modulation Scope"," ");
    }

    mode_timer++;
    if (mode_timer >= MODE_INTRO_TIME)// Done with Mode Intro. MODE_INTRO_TIME in 10ths of seconds
    {
      mode_timer = 0;
      flag.mode_display = false;      // Clear display change mode
      flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode
      eraseDisplay();
      modScopeActive = true;          // Start Modulation Scope
    }
  }

  //----------------------------------------------
  // Display Power if level is above useful threshold
  #if AD8307_INSTALLED
  else if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))
  #else  
  else if ((power_mw > MIN_PWR_FOR_METER) || (flag.power_detected))
  #endif
  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;
      eraseDisplay();                 // Immediate blank slate to make room for Meters
      //VirtLCDw.clear();
      //VirtLCDy.clear();
      //VirtLCDw.transfer();
      //VirtLCDy.transfer();
    }
    
    //------------------------------------------
    // Update scope with pending data
    ModScope.update();
    
    //------------------------------------------
    // SWR Printout
    VirtLCDw.setCursor(7,9);          // Clear junk in line, if any
    VirtLCDw.print("   ");
    VirtLCDw.setCursor(0,9);
    VirtLCDw.print("SWR ");
    print_swr();                      // and print the "SWR value"
    VirtLCDw.print(lcd_buf);

    //------------------------------------------
    // Power Indication
    VirtLCDy.setCursor(0,8);
    VirtLCDy.print("Power     Pk  ");
    print_p_mw(power_mw_pk);
    VirtLCDy.print(lcd_buf);

    //------------------------------------------
    // Power indication, PEP
    VirtLCDy.setCursor(9,9);
    VirtLCDy.print(" PEP ");
    print_p_mw(power_mw_pep);
    VirtLCDy.print(lcd_buf);
  }
  else                                // Screensaver display
  {
    ModScope.erase();
    VirtLCDlargeY.clear();
    VirtLCDlargeR.clear();
    screensaver();
  }
}


//
//-----------------------------------------------------------------------------
//  Display Config and measured input voltages etc...
//-----------------------------------------------------------------------------
//
void lcd_display_debug(void)
{
  double    output_voltage;
  uint16_t  v, v_sub;

  VirtLCDy.setCursor(0,0);
  VirtLCDy.print("Debug Display:");

  //------------------------------------------
  // Forward voltage
  VirtLCDw.setCursor(0,2);
  output_voltage = fwd * adc_ref / 4096;
  v_sub = output_voltage * 1000;
  v = v_sub / 1000;
  v_sub = v_sub % 1000;
  sprintf(lcd_buf,"Fwd AD:%4u   %u.%03uV ", fwd, v, v_sub);
  VirtLCDw.print(lcd_buf);
  //------------------------------------------
  // Reverse voltage
  VirtLCDw.setCursor(0,3);
  output_voltage = rev * adc_ref / 4096;
  v_sub = output_voltage * 1000;
  v = v_sub / 1000;
  v_sub = v_sub % 1000;
  sprintf(lcd_buf,"Ref AD:%4u   %u.%03uV ", rev, v, v_sub);
  VirtLCDw.print(lcd_buf);

  #if AD8307_INSTALLED
  //------------------------------------------
  // Calibrate 1
  VirtLCDw.setCursor(0,4);
  sprintf (lcd_buf,"%4d ",R.cal_AD[0].db10m);
  VirtLCDw.print(lcd_buf);
  //sprintf (lcd_buf,"F:%5ld,R:%5ld",R.cal_AD[0].Fwd,R.cal_AD[0].Rev);
  sprintf (lcd_buf,"F:%01.03f,R:%01.03f",R.cal_AD[0].Fwd,R.cal_AD[0].Rev);
  VirtLCDw.print(lcd_buf);
  //------------------------------------------
  // Calibrate 2
  VirtLCDw.setCursor(0,5);
  sprintf (lcd_buf,"%4d ",R.cal_AD[1].db10m);
  VirtLCDw.print(lcd_buf);
  //sprintf (lcd_buf,"F:%5ld,R:%5ld",R.cal_AD[1].Fwd,R.cal_AD[1].Rev);
  sprintf (lcd_buf,"F:%01.03f,R:%01.03f",R.cal_AD[1].Fwd,R.cal_AD[1].Rev);
  VirtLCDw.print(lcd_buf);
  //------------------------------------------
  // Fwd and Ref Power  
  VirtLCDw.setCursor(0,6);
  VirtLCDw.print("Fwd");
  print_p_mw((int32_t) fwd_power_mw);
  VirtLCDw.print(lcd_buf);
  VirtLCDw.print("  Ref");
  print_p_mw((int32_t) ref_power_mw);
  VirtLCDw.print(lcd_buf);
  
  #else
  //------------------------------------------
  // Fwd and Ref Power
  VirtLCDw.setCursor(0,4);
  VirtLCDw.print("Fwd");
  print_p_mw((int32_t) fwd_power_mw);
  VirtLCDw.print(lcd_buf);
  VirtLCDw.print("  Ref");
  print_p_mw((int32_t) ref_power_mw);
  VirtLCDw.print(lcd_buf);
  //------------------------------------------
  // Calibrate value
  VirtLCDw.setCursor(11,5);
  sprintf (lcd_buf,"Cal  %4d",R.meter_cal);
  VirtLCDw.print(lcd_buf);
  #endif
}


//
//-----------------------------------------------------------------------------
//  Debug Display 2.  All sorts of Gauges...
//-----------------------------------------------------------------------------
//
bool dbg2init = false;                // Firs time init indicator

void lcd_display_debug2(void)
{
  double scale;                       // Progress bar scale
  double swr_alm;                     // SWR Alarm indication above this point by colour of graph
  double swr_mid = 2.0;               // Default SWR midlevel colour changeover point
  double adj_scale;                   // Adjust Power scales into ranges of 0 - 1000, uW, mW, W and kW
  char  range[5];

  if (!dbg2init)
  {
      dbg2init = true;
      #if TOUCHSCREEN_ENABLED
      eraseTouchButtons();                                                     // Buttons invisible, but still working - sloppy!!!
      #endif
      eraseDisplay();
      Meter1.init(25,  0,280,15, ILI9341_WHITE, ILI9341_ORANGE, ILI9341_GREEN);// Combined FWD and REV meter
      Meter2.init(25, 40,280,15);                                              // PEP and inst
      Meter3.init(25, 80,280,15);                                              // PEP and Pk
      Meter4.init(25,120,280,15);                                              // PEP and 100ms AVG
      Meter5.init(25,160,280,15);                                              // PEP and 1s AVG
      SWR.init   (25,200,280,15);
      lcd_display_debug2_text(ILI9341_WHITE);                                  // Text for gauges
  }

  //------------------------------------------
  // Prepare and Print/Update Power Meters
  scale = scale_BAR(fwd_power_mw);                                             // Determine scale setting
  scalePowerMeter(scale, &adj_scale, range);
  Meter1.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter1.graph(ref_power_mw, fwd_power_mw,scale);                              // No PEP in this mode
  
  Meter2.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter2.graph(power_mw, power_mw_pep,scale);

  Meter3.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter3.graph(power_mw_pk, 0,scale);

  Meter4.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter4.graph(power_mw_avg, 0,scale);

  Meter5.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter5.graph(power_mw_1savg, 0,scale);
  
  //------------------------------------------
  // SWR Meter
  SWR.scale();                                                                 // Draw SWR bargraph;
  swr_alm = R.SWR_alarm_trig/10.0;                                             // Set colour changeover points for swr mid and swr alarm
  if (swr_alm < swr_mid) swr_mid = swr_alm;
  SWR.graph(swr_mid, swr_alm, swr);
}
//------------------------------------------
void lcd_display_debug_erase(void)
{
  dbg2init = false;
  #if TOUCHSCREEN_ENABLED
  eraseTouchButtons();
  #endif
  lcd_display_debug2_text(ILI9341_BLACK);                                      // Erase text
  Meter1.erase();
  Meter2.erase();
  Meter3.erase();
  Meter4.erase();
  Meter5.erase();
  SWR.erase();
  Meter1.init(5, 0,300,50, ILI9341_WHITE, ILI9341_YELLOW, ILI9341_BLUE );      // Return Meter1 to normal
  SWR.init   (5,75,300,50);
}
//------------------------------------------
void lcd_display_debug2_text(int16_t textcolour)
{
  tft.setFont(&FONT_8); 
  tft.setTextColor(textcolour);
  tft.setCursor( 0, 0);
  tft.print("fwd");
  tft.setCursor( 0, 14);
  tft.print("ref");
  
  tft.setCursor( 0, 40);
  tft.print("pep");
  tft.setCursor( 0, 54);
  tft.print("inst");
  
  tft.setCursor( 0, 85);
  tft.print("pk");
  tft.setCursor( 0, 120);
  tft.print("avg");
  tft.setCursor( 0, 134);
  tft.print("0.1s");
  tft.setCursor( 0, 160);
  tft.print("avg");
  tft.setCursor( 0, 174);
  tft.print("1s");  
}

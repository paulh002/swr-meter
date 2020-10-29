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

//
//-----------------------------------------------------------------------------
//			Print SWR, returns string in lcd_buf
//-----------------------------------------------------------------------------
//
void print_swr(void)
{
  uint16_t swr_sub;
  uint16_t swr_sup;
   
  // Format for 2 sub-decimals
  swr_sub = swr_avg * 100;
  swr_sup = swr_sub / 100;
  swr_sub = swr_sub % 100;

  if (swr_avg < 2.0)
  {
    sprintf(lcd_buf,"%u.%02u",swr_sup,swr_sub);  
  }
  else if (swr_avg <= 10.0)
  {
    swr_sub = swr_sub / 10;
    sprintf(lcd_buf," %u.%01u",swr_sup,swr_sub);  
  }
  else if (swr_avg <= 1000.0)
  {
    sprintf(lcd_buf,"%4u",(uint16_t) swr_avg);
  }
  else
  {
    //sprintf(lcd_buf," N/A");
    sprintf(lcd_buf,"9999");
  }
}


//
//-----------------------------------------------------------------------------
//			Print dBm, accepts 10x dBm input value, 
//			returns string in lcd_buf
//-----------------------------------------------------------------------------
//
void print_dbm(int16_t db10m)
{
  int16_t pwrdb_tenths = db10m;

  if (pwrdb_tenths < 0) pwrdb_tenths *= -1;
  int16_t pwrdb = pwrdb_tenths / 10;
  pwrdb_tenths = pwrdb_tenths % 10;

  if (db10m <= -100)
  {
    sprintf(lcd_buf,"-%2u.%1udBm",pwrdb,pwrdb_tenths);
  }
  else if (db10m < 0)
  {
    sprintf(lcd_buf," -%1u.%1udBm",pwrdb,pwrdb_tenths);
  }
  else sprintf(lcd_buf,"%3u.%1udBm",pwrdb,pwrdb_tenths);
}


//
//-----------------------------------------------------------------------------
//			Print Power, input value is in milliWatts, 
//			returns string in lcd_buf
//-----------------------------------------------------------------------------
//
void print_p_mw(double pwr)
{
  int8_t r = 3;                   // Start in the mW range
  uint8_t offs=0;
  const char *indicator[] = { "pW", "nW", "uW", "mW", "W", "kW", "MW", "GW" /* hah! */ };
  double p;
 
  if (pwr < 0) pwr *= -1;         // ABS() for float
  p = pwr;
  
  #if AD8307_INSTALLED            // Only relevant when not diode detectors
  // If lowpower print threshold has NOT been set at 10 uW and power is below 1mW
  if (R.low_power_floor != FLOOR_TEN_uW)
  {
    while ((p < 1.0) && (r > 0))  // Power levels below one milliwatt, down to pW
    {
      p *= 1000.0;
      r -= 1;
    }
  }
  #endif

  while ((p >= 1000) && (r < 7))  // Power levels above one watt, up to GW
  {
    p /= 1000.0;
    r +=1;    
  }
  
  if (r == 4)                     // Different format if "W"
  {
    lcd_buf[0]=' ';
    offs=1;    
  }
  
  #if AD8307_INSTALLED            // These are only relevant when not diode detectors
  // If lowpower print threshold has been set at 1 uW and power is below that threshold
  if ((R.low_power_floor == FLOOR_ONE_uW) && (pwr < 0.001))
    sprintf(lcd_buf+offs,"  0 uW");
  // If lowpower print threshold has been set at 10 uW and power is below that threshold
  else if ((R.low_power_floor == FLOOR_TEN_uW) && (pwr < 0.01))
    sprintf(lcd_buf+offs,"  0 uW");
  // If lowpower print threshold has been set at 100 uW and power is below that threshold
  else if ((R.low_power_floor == FLOOR_100_uW) && (pwr < 0.1))
    sprintf(lcd_buf+offs,"  0 uW");
  // If lowpower print threshold has been set at 1 mW and power is below that threshold
  else if ((R.low_power_floor == FLOOR_ONE_mW) && (pwr < 1.0))
    sprintf(lcd_buf+offs,"  0 mW");
  // If lowpower print threshold has been set at 1 mW and power is below that threshold
  else if ((R.low_power_floor == FLOOR_TEN_mW) && (pwr < 10.0))
    sprintf(lcd_buf+offs,"  0 mW");
  else
  #endif
  // 9.995 rather than 10.00 for correct round-up when two subdecimal formatting
  {
    if     (p >= 99.95) sprintf(lcd_buf+offs," %3u%s",   (uint16_t)p, indicator[r]);
    else if(p >= 9.995) sprintf(lcd_buf+offs,"%2.01f%s", p, indicator[r]);
    else if(p <  9.995) sprintf(lcd_buf+offs,"%1.02f%s", p, indicator[r]);
    if (strlen(lcd_buf) > 6) lcd_buf[6] = '\0'; // Belt and braces
  }
}

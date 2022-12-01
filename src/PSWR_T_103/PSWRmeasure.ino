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
//-----------------------------------------------------------------------------------------
//
//      Power and SWR Meter Functions
//
//      Measure Forward and Reverse Power
//      to determine Power and SWR 
//
//-----------------------------------------------------------------------------------------
//

#include "PSWR_T.h"

int8_t  ad7991_addr = 0;              // Address of AD7991 I2C connected A/D, 0 if none detected
#define AD7991_0  0x28                // I2C Address of AD7991-0
#define AD7991_1  0x29                // I2C Address of AD7991-1

//
//-----------------------------------------------------------------------------------------
//                                Initialize builtin ADCs.
//-----------------------------------------------------------------------------------------
//
void adc_init(void)
{ 
  analogSetAttenuation(ADC_11db);
  analogReadResolution(12);
  adc_ref = ADC_RES;  
}


#if WIRE_ENABLED
//
//-----------------------------------------------------------------------------------------
//                    Scan I2C bus for AD7991 - 4x 12bit AD
//
// Return which type detected (AD7991-0 or AD7991-1) or 0 if nothing found
//-----------------------------------------------------------------------------------------
//
uint8_t I2C_Init(void)
{
  uint8_t found;                      // Returns status of I2C bus scan

  // Scan for AD7991-0 or AD7991-1
  ad7991_addr = AD7991_1;             // We start testing for an AD7991-1
  found = 2;						              //  Assume it will be found
  Wire.beginTransmission(ad7991_addr);
  if(Wire.endTransmission() != 0)     // If error
  {
    ad7991_addr = AD7991_0;           // AD7991-0 was not detected
    found = 1;                        // We may have an AD7991-0	
  }
  Wire.beginTransmission(ad7991_addr);
  if(Wire.endTransmission() != 0)     // If error
  {
    ad7991_addr = 0;                  // AD7991-1 was not detected
    found = 0;                        // No I2C capable device was detected
  }

  // If Power & SWR Code and AD7991 12 bit ADC, the program it to use a 2.6V reference connected
  // to ADC channel 4 and only read ADC channels 1 and 2 consecutively.
  //
  if (found)                          // If AD7991 ADC was found, then configure it...
  {
    Wire.beginTransmission(ad7991_addr);
    Wire.write(0x38);                 // Set ADCs 1 and 2 for consecutive reads and REF_SEL = external
    Wire.endTransmission();
    adc_ref = AD7991REF;              // Readjust the ADC reference to the external REF voltage
  }
  return found;
}
#endif

//-----------------------------------------------------------------------------------------
//                            Poll the AD7991 2 x ADC chip
//                                  or alternately
//              use built-in 12 bit A/D converters if I2C device not present
//                 This function is called from the Interrupt function, and
//              reads the A/D inputs and transfers them to two circular buffers
//-----------------------------------------------------------------------------------------
void adc_poll_and_feed_circular(void)
{
  #if WIRE_ENABLED  
  uint16_t          adc_in[4];
  uint8_t           read_B[4];
  uint8_t           i=0;
  #endif
                                              // reads from the two builtin A/D converters
  #if WIRE_ENABLED  
  //-----------------------------------------------------------------------------
  // use I2C connected AD7991 12-bit AD converter, if it was detected during init
  // Each I2C poll of the AD7991 is measured to take 140us
  if (ad7991_addr)
  {
    Wire.requestFrom(ad7991_addr, 4);
    while (Wire.available()) read_B[i++] = Wire.read();

    // The output of the 12bit ADCs is contained in two consecutive byte pairs
    // read from the AD7991.  In theory, the second could be read before the first.
    // Each of the AD7991 four builtin ADCs has an address identifier (0 to 3)
    // in the uppermost 4 bits of the first byte.  The lowermost 4 bits of the first
    // byte are bits 8-12 of the A/D output.
    // In this routine we only read the two first ADCs, as set up in the I2C_Init()
    //
    adc_in[(read_B[0] >> 4) & 0x03] = (read_B[0] & 0x0f) * 0x100 + read_B[1];
    adc_in[(read_B[2] >> 4) & 0x03] = (read_B[2] & 0x0f) * 0x100 + read_B[3];
    measure.fwd[measure.incount] = adc_in[0]; // Input from AD7991 is 12 bit resolution
    measure.rev[measure.incount] = adc_in[1]; // contained in bit positions 0 to 11.
    measure.incount++;                        // 8 bit value, rolls over at 256
  }
  else
  #endif
  //----------------------------------------------------------------------------
  // If no I2C, then use builtin A/D converters (12 bit resolution)
  // Each poll of the builtin ADC is measured to take 130us if averaging set at 8.
  {
    int16_t result_fwd = (int16_t)random(1025, 3500); // analogRead(Pfwd);
    int16_t result_ref = (int16_t)random(0, result_fwd); // analogRead(Pref);  // ref=ADC0, fwd=ADC1
    
    //int16_t result_fwd = analogRead(Pfwd);
    //int16_t result_ref = analogRead(Pref);  // ref=ADC0, fwd=ADC1
    
    //Serial.println(result_fwd);
    measure.fwd[measure.incount] = result_fwd;
    measure.rev[measure.incount] = result_ref;
    measure.incount++;                        // 8 bit value, rolls over at 256
  }
}

//
//---------------------------------------------------------------------------------
// Process circular buffers being fed by the Interrupt function
//---------------------------------------------------------------------------------
//
void pswr_sync_from_interrupt(void)
{
  uint8_t in;

//  noInterrupts();
  in = measure.incount;
//  interrupts();

  while (measure.outcount != in)              // Read from circular buffer, while new input available
  {
//    noInterrupts();
    fwd = measure.fwd[measure.outcount];      // Transfer data from circular buffers
    rev = measure.rev[measure.outcount];
//    interrupts();
    measure.outcount++;                       // 8 bit value, rolls over at 256
    
    determine_power_pep_pk();                 // Determine Instantaneous power, pep, pk and avg

    if (modScopeActive)                       // Modulation Scope
    {
      ModScope.adddata(power_mw, power_mw_long);    
    }

//    noInterrupts();                           // Perhaps a bit redundant - squeeze every last drop while we're at it
    in = measure.incount;
//    interrupts();
  }
}


//
//-----------------------------------------------------------------------------------------
//                Prepare various types of power and calculate SWR
//                (these are things that can be run outside of the
//                 interrupt function, to optimize time management)
//-----------------------------------------------------------------------------------------
//
void calc_SWR_and_power(void)
{
  static double  swr_avg_buf[AVG_BUFSWR];        // a buffer of instantaneous swr measurements, for swr smoothing
  static double  swr_plus;                       // averaging: all swr measurements within a short window added together
  static uint16_t a;                             // swr avg: short ring buffer counter
  
  power_mw_pk   = pow(10,power_db_pk/10.0);
  power_mw_pep  = pow(10,power_db_pep/10.0);   
  power_mw_long = pow(10,power_db_long/10.0);   
  
  // Only calculate SWR if meaningful power

  if ((power_mw > MIN_PWR_FOR_SWR_CALC) || (power_mw < -MIN_PWR_FOR_SWR_CALC))
  {
    // Calculate SWR
    swr = (1+(r_inst/f_inst))/(1-(r_inst/f_inst));

    // Check for high SWR and set alarm flag if trigger value is exceeded
    // If trigger is 40 (4:1), then Alarm function is Off
    if ((R.SWR_alarm_trig != 40) && ((swr*10) >= R.SWR_alarm_trig) && (power_mw > R.SWR_alarm_pwr_thresh))
    {
      if (flag.swr_alarm)                        // This is the second time in a row, raise the SWR ALARM
      {
        digitalWrite(R_Led,flag.swr_alarm);      // Turn on the Alarm LED
      }
      flag.swr_alarm = true;                     // Set SWR flag - may be a one shot fluke
    }
    //else if (flag.swr_alarm)                   // Clear SWR Flag if High SWR condition does not exist
    //{
    //  flag.swr_alarm = false;                  // Alarm is cleared with a Touch Screen Push Action
    //}
  }
  
  // If some power present, but not enough for an accurate SWR reading, then use
  // recent measured value
  else if ((power_mw > MIN_PWR_FOR_SWR_SHOW) || (power_mw < -MIN_PWR_FOR_SWR_SHOW))
  {
    // Do nothing, in other words, swr remains the same
  }
  else
  {
    // No power present, set SWR to 1.0
    // swr = 1.0;
  }

  //------------------------------------------
  // Average swr reading over short period (100 ms)
  swr_avg_buf[a++] = swr;                       // Add the newest value onto ring buffer
  swr_plus = swr_plus + swr;                    // Add latest value to the total sum of all measurements in [100ms]
  if (a == AVG_BUFSWR) a = 0;                   // wrap around
  swr_plus = swr_plus - swr_avg_buf[a];         // and subtract the oldest value in the ring buffer from the total sum
  swr_avg = swr_plus / (AVG_BUFSWR-1);          // And finally, find the average
}


#if AD8307_INSTALLED
//
//-----------------------------------------------------------------------------------------
//                Convert Voltage Reading into Power when using AD8307
//-----------------------------------------------------------------------------------------
//
void pswr_determine_dBm(void)
{
  double  delta_db;
  double  delta_F, delta_R;
  double  delta_Fdb, delta_Rdb;
  double  temp;

  // Calculate the slope gradient between the two calibration points:
  //
  // (dB_Cal1 - dB_Cal2)/(V_Cal1 - V_Cal2) = slope_gradient
  //
  delta_db = (double)((R.cal_AD[1].db10m - R.cal_AD[0].db10m)/10.0);
  delta_F = R.cal_AD[1].Fwd - R.cal_AD[0].Fwd;
  delta_Fdb = delta_db/delta_F;
  delta_R = R.cal_AD[1].Rev - R.cal_AD[0].Rev;
  delta_Rdb = delta_db/delta_R;
  //
  // measured dB values are: (V - V_Cal1) * slope_gradient + dB_Cal1
  ad8307_FdBm = (adc_ref * (fwd/4096.0) - R.cal_AD[0].Fwd) * delta_Fdb + R.cal_AD[0].db10m/10.0;
  ad8307_RdBm = (adc_ref * (rev/4096.0) - R.cal_AD[0].Rev) * delta_Rdb + R.cal_AD[0].db10m/10.0;

  // Test for direction of power - Always designate the higher power as "forward"
  // while setting the "Reverse" flag on reverse condition.
  if (ad8307_FdBm > ad8307_RdBm)        // Forward direction
  {
    Reverse = false;
  }
  else                                  // Reverse direction
  {
    temp = ad8307_RdBm;
    ad8307_RdBm = ad8307_FdBm;
    ad8307_FdBm = temp;
    Reverse = true;
  }
}
#endif

//
//-----------------------------------------------------------------------------------------
//                Determine Forward and Reflected power
//                from 2x AD8307 or from Diode Detectors.
//                Also determine PEP, 100ms peak and Average, 100ms and 1s.
//-----------------------------------------------------------------------------------------
//
void determine_power_pep_pk(void)
{
  // For measurement of peak and average power
  static int32_t  buff_short[BUF_SHORT];        // voltage db information in a 100 ms window
  static int32_t  pepbuff[PEP_BUFFER];          // voltage db information in a 1, 2.5 or 5 second window
  static int32_t  longbuff[LONG_BUFFER];        // voltage db information in a 30 second window
  int32_t pk;                                   // Keep track of Max (1s) and Peak (100ms) dB voltage
  static double   p_avg_buf[AVG_BUFSHORT];      // a short buffer of all instantaneous power measurements, for short average
  static double   p_avg_buf1s[AVG_BUF1S];       // a one second buffer of instantanous power measurements, for 1s average
  static double   p_1splus;                     // averaging: all power measurements within a 1s window added together
  static double   p_plus;                       // averaging: all power measurements within a shorter window added together
  static uint16_t a;                            // Pk:  100ms ring buffer counter
  static uint16_t b;                            // PEP: ring buffer counter
  static uint16_t c;                            // Long: ring buffer counter
  static uint16_t d;                            // avg: short ring buffer counter
  static uint16_t e;                            // avg: 1s ring buffer counter

  #if AD8307_INSTALLED     
  //---------------------------------------------------------------------------------
  // Process Forward and Reflected power measurements from 2x AD8307
  //---------------------------------------------------------------------------------

  pswr_determine_dBm();                         // Determine dBm from AD8307 voltages
  
  // Instantaneous forward voltage and power, milliwatts and dBm
  f_inst = pow(10,ad8307_FdBm/20.0);		        // (We use voltage later on, for SWR calc)
  fwd_power_mw = SQR(f_inst);			              // P_mw = (V*V) (current and resistance have already been factored in)

  // Instantaneous reverse voltage and power
  r_inst = pow(10,(ad8307_RdBm)/20.0);
  ref_power_mw = SQR(r_inst);

  // We need some sane boundaries (4kW) to determine reasonable variable defs for further calculations
  if (fwd_power_mw > 4000000) fwd_power_mw = 4000000;
  if (ref_power_mw > 4000000) ref_power_mw = 4000000;
  
  // Instantaneous Real Power Output
  power_mw = fwd_power_mw - ref_power_mw;
  power_db = 10 * log10(power_mw);

  #else
  //---------------------------------------------------------------------------------
  // Measure Forward and Reflected power from Diode Detecotrs
  //---------------------------------------------------------------------------------
  // Test for direction of power - Always designate the higher power as "forward"
  // while setting the "Reverse" flag on reverse condition.
  if (fwd > rev)                                // Forward direction
  {
    Reverse = false;
  }
  else                                          // Reverse direction
  {
    uint16_t temp = rev;
    rev = fwd;
    fwd = temp;
    Reverse = true;
  }

  // Instantaneous forward voltage and power, milliwatts
  //
  // Establish actual measured voltage at diode
  f_inst = (double) fwd * adc_ref/4096.0;
  // Convert to VRMS in Bridge
  if (f_inst >= D_VDROP) f_inst = 1/1.4142135 * (f_inst - D_VDROP) + D_VDROP;
  // Take Bridge Coupling into account
  f_inst = f_inst * BRIDGE_COUPLING * R.meter_cal/100.0;
  // Convert into milliwatts
  fwd_power_mw = 1000 * SQR(f_inst)/50.0;
    
  // Instantaneous reflected voltage and power
  //
  // Establish actual measured voltage at diode
  r_inst = (double) rev * adc_ref/4096.0;
  // Convert to VRMS in Bridge
  if (r_inst >= D_VDROP) r_inst = 1/1.4142135 * (r_inst - D_VDROP) + D_VDROP;
  // Take Bridge Coupling into account
  r_inst = r_inst * BRIDGE_COUPLING * R.meter_cal/100.0;
  // Convert into milliwatts
  ref_power_mw = 1000 * SQR(r_inst)/50.0;
  
  // Instantaneous Real Power Output
  power_mw = fwd_power_mw - ref_power_mw;
  if (power_mw <  0) power_mw = power_mw * -1;
  power_db = 10 * log10(power_mw);

  #endif
   
  //------------------------------------------
  // Find peaks and averages

  // Multiply by 100 to make suitable for integer value
  // Store dB value in a ring buffer
  buff_short[a++] = 100 * power_db;             // Get precision of two subdecimals
  if (a == BUF_SHORT)    a = 0;

  //------------------------------------------
  // Find Peak value within a 100ms sliding window
  pk =-0x7fffffff;
  for (uint8_t x = 0; x < BUF_SHORT; x++)
  {
    if (pk < buff_short[x]) pk = buff_short[x];
  }
  power_db_pk = pk / 100.0;

  //------------------------------------------
  // Feed and Retrieve Max Value within a 1 to 5 second sliding window, 
  // while using a resolution granularity of BUF_SHORT (100ms)
  if (a == 0)                                   // Once every BUF_SHORT*SAMPLE_TIMER time
  {
    pepbuff[b++] = 100 * power_db_pk;           // Feed the PEP buffer with Pk values, precision of two subdecimals
    if (b >= R.PEP_period) b = 0;
   
    pk =-0x7fffffff;
    for (uint16_t x = 0; x < R.PEP_period; x++) // Retrieve a new PEP value
    {
      if (pk < pepbuff[x]) pk = pepbuff[x];
    }
    power_db_pep = pk / 100.0;
  }
  if (power_db_pep < power_db_pk)               // Correct for slow reaction of the above
    power_db_pep = power_db_pk;
  
  //------------------------------------------
  // Feed and Retrieve Max Value within a 30 second (or longer) sliding window
  if (b == 0)                                   // Once every PEP (1s, 2.5s or 5s) time
  {
    longbuff[c++] = 100 *power_db_pep;          // Feed the long buffer with PEP values, precision of two subdecimals
    if (c >= (LONG_BUFFER/(R.PEP_period/10))) c = 0;

    pk =-0x7fffffff;
    for (uint16_t x = 0; x < (LONG_BUFFER/(R.PEP_period/10)); x++)  // Retrieve a new LONG value
    {
      if (pk < longbuff[x]) pk = longbuff[x];
    }
    power_db_long = pk / 100.0;
  }
  if (power_db_long < power_db_pep)             // Correct for slow reaction of the above
    power_db_long = power_db_pep;
  
  //------------------------------------------
  // Determine averages, using a FIFO + add and subtract buffer methology

  //------------------------------------------
  // Average power buffers, short period (100 ms)
  p_avg_buf[d++] = power_mw;                    // Add the newest value onto ring buffer
  p_plus = p_plus + power_mw;                   // Add latest value to the total sum of all measurements in [100ms]
  if (d == AVG_BUFSHORT) d = 0;                 // wrap around
  p_plus = p_plus - p_avg_buf[d];               // and subtract the oldest value in the ring buffer from the total sum
  power_mw_avg = p_plus / (AVG_BUFSHORT-1);     // And finally, find the short period average

  //------------------------------------------
  // Average power buffers, 1 second
  p_avg_buf1s[e++] = power_mw;                  // Add the newest value onto ring buffer
  p_1splus = p_1splus + power_mw;               // Add latest value to the total sum of all measurements in 1s
  if (e == AVG_BUF1S) e = 0;                    // wrap around
  p_1splus = p_1splus - p_avg_buf1s[e];         // and subtract the oldest value in the ring buffer from the total sum
  power_mw_1savg = p_1splus / (AVG_BUF1S-1);    // And finally, find the one second period average
}

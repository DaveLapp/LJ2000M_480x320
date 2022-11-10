//*********************************************************************************
//**
//** Project.........: A menu driven Multi Display RF Power and SWR Meter
//**                   using a Tandem Match Coupler and 2x AD8307; or
//**                   diode detectors.
//**
//** Copyright (c) 2015  Loftur E. Jonasson  (tf3lj [at] arrl [dot] net)
//** Copyright (c) 2017-2021  J.G. Holstein
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
//**  Platform........: Teensy 4.0 (http://www.pjrc.com)
//**
//** Initial version.: 0.50, 2013-09-29  Loftur Jonasson, TF3LJ / VE2LJX
//**                   (beta version)
//**Version T_2.06  ILI9488 and FT6206 touchscreen, sample timer 50us max power 9kW

//-----------------------------------------------------------------------------
//  Erase everything on screen
//-----------------------------------------------------------------------------

void eraseDisplay(void)
{


  if (!flag.config_mode) {

    VirtLCDw.clear();                         // Erase text
    VirtLCDdebug.clear();
    VirtLCDy.clear();
    VirtLCDmessage.clear();
    VirtLCDlargeY.clear();
    VirtLCDlargeR.clear();
    VirtLCDpwr.clear();
    VirtLCDMode.clear();
    VirtLCDAlarm.clear(); //text SWR in YELLOW and  Red background
    VirtLCDMode.clear();
    VirtLCDcal.clear();
    VirtLCDdBm.clear();
    VirtLCDswr.clear();  //text SWR in Black and  Green background
    VirtLCDw.transfer();
    VirtLCDy.transfer();
    VirtLCDmessage.transfer();
    VirtLCDlargeY.transfer();
    VirtLCDlargeR.transfer();
    VirtLCDpwr.transfer();
    VirtLCDMode.transfer();
    VirtLCDcal.transfer();
    VirtLCDdBm.transfer();
    VirtLCDswr_G.clear();
    VirtLCDswr_G.transfer();
    VirtLCDswr.clear();
   
    Meter1.erase();                           // Erase meters, if present
    Meter2.erase();
    Meter3.erase();
    Meter4.erase();
    Meter5.erase();
    SWR.erase();
    ModScope.erase();

    if (flag.swr_alarm == false) {
      tft.fillRect(300, 0, 162, 80, 0x96FB);
    }else{
      tft.fillRect(300, 0, 162, 80, RED);
    }
  
    tft.fillRoundRect(10, 275, 70, 40, 1,0x20C3);  // Setup button
    tft.drawRoundRect(10, 275, 70, 40, 1, DKGREY );
    tft.setFont(DroidSansMono_12);
    tft.setTextColor(LTGREY, 0x20C3);
    tft.setCursor(20, 289);
    tft.print("SETUP");
    tft.fillRect(10, 0, 285, 80, LTBLUE); // power
  }
}

//-----------------------------------------------------------------------------
//  Screensaver display
//-----------------------------------------------------------------------------

void screensaver(void){

  if (flag.swr_alarm == false){

  
  if ( flag.screensaver_on == false) {
     tft.fillScreen(ILI9488_BLACK);
     flag.screensaver_on = true;
  }
  
  

  if (flag.idle_refresh && screensaver_time.hasPassed(screensaver_time_long)) // Force screensaver reprint if flag
   {
    flag.idle_refresh = false;              // Clear flag
    eraseDisplay();                         // Erase everything
    tft.fillScreen(ILI9488_BLACK);
   }
  
  if (Menu_exit_timer == 0)                 // Announcement timer not active
  {
    if (screensaver_time.hasPassed(screensaver_time_long)) 
    {        
      VirtLCDmessage.clear();
      VirtLCDmessage.transfer();
      VirtLCDmessage.setCursor(rand() % (30 - strlen(R.idle_disp)), rand() % 10) ;
      VirtLCDmessage.print(R.idle_disp);
      
      screensaver_time.start(); 
    }
   }
  }
}
//-----------------------------------------------------------------------------
//  Display Mode Intro
//-----------------------------------------------------------------------------
//

void lcd_display_mode_intro(const char *line1, const char *line2, const char *line3, const char *line4)
{
  eraseDisplay();
  VirtLCDMode.setCursor(0, 1); // Mode
  VirtLCDMode.print(line1);
  VirtLCDMode.setCursor(0, 1);
  VirtLCDMode.print(line2);
  VirtLCDMode.setCursor(0, 1);
  VirtLCDMode.print(line3);
  fpbuttons();

}
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

double scale_BAR(double p /* power in milliwatts */)
{
#define  SCALE_BUFFER  150        // Keep latest 150 power readings in buffer

  // For measurement of peak and average power
  static   uint64_t peak_buff[SCALE_BUFFER];// 30 second window
  static   uint64_t a;              // Entry counter
  uint64_t          max;            // Find max value
  uint64_t          scale;          // Scale output

  //#if AD8307_INSTALLED
  uint64_t          decade;         // Used to determine range decade, in units of 1uW

  // Determine how low the meter range can go, based on the power level floor setting
  if      (R.low_power_floor == FLOOR_ONE_uW) decade = 5;      // Lowest scale (x10 uW) when 1 uW
  else if (R.low_power_floor == FLOOR_TEN_uW) decade = 50;     // Lowest scale at 10 uW
  else if (R.low_power_floor == FLOOR_100_uW) decade = 500;    // Lowest scale at 100 uW
  else if (R.low_power_floor == FLOOR_ONE_mW) decade = 5000;   // Lowest scale at 1 mW
  else if (R.low_power_floor == FLOOR_TEN_mW) decade = 50000;  // Lowest scale at 10 mW
  else decade = 1;                                             // No scale floor

  // Store peak value in circular buffer
  peak_buff[a] = p * 1000.0;        // Max 9.9 kW, resolution down to 1uW

  a++;
  if (a == SCALE_BUFFER) a = 0;

  // Retrieve the max value out of the measured window
  max = 0;
  for (uint64_t b = 0; b < SCALE_BUFFER; b++)
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
  return scale / 1000.0;            // Return value is in mW
}

//-----------------------------------------------------------------------------------------
//  Determine range for the Power Meter, pW to kW
//-----------------------------------------------------------------------------------------

void scalePowerMeter(double fullscale, double *out_fullscale, char *range)
{
  uint16_t r = 3;
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
  strcpy(range, indicator[r]);
}

//-----------------------------------------------------------------------------
//  Print Power Output level in a large Font
//  Yellow if Normal SWR Normal, Red if SWR Alarm
//-----------------------------------------------------------------------------

void power_print_large(const char *str )
{
  static bool previousSWRalarmState;
  //Serial.println("large print");
  // Clear and Flush if Alarm State (colour) change
  if (flag.swr_alarm != previousSWRalarmState)
  {
    VirtLCDlargeY.clear();
    VirtLCDlargeR.clear();
    VirtLCDlargeY.transfer();
    VirtLCDlargeR.transfer();


    previousSWRalarmState = flag.swr_alarm;
  }
  if (flag.swr_alarm) {
    VirtLCDlargeR.print(str);
  } else {                               // Print Yellow if not
    VirtLCDlargeY.print(str);
  }
}

//-----------------------------------------------------------------------------
//  Display: Bargraph, Power in Watts, SWR & PEP Power
//  PEP Power always displayed and used for scale selection
//  power variable can be anything passed to function (power_mv_avg, power_mw_pk, etc...)
//-----------------------------------------------------------------------------

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
  if (flag.mode_display)
  {
    if (flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      mode_timer = 0;                 // New mode, reset timer
      Meter1.init(10, 85, 450, 20);  // Draw Meters
      SWR.init(10, 140, 450, 20);
      fpbuttons();
      lcd_display_mode_intro("", " ", introtext, "and SWR Bargraph");
    }

    mode_timer++;
    if (mode_timer >= MODE_INTRO_TIME)// Done with Mode Intro. MODE_INTRO_TIME in 10ths of seconds
    {
      mode_timer = 0;
      flag.mode_display = false;      // Clear display change mode
      flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode

      Meter1.init(10, 85, 450, 20);  // Draw Meters
      SWR.init(10, 140, 450, 20);
    }
  }

  //----------------------------------------------
  // Display Power if level is above useful threshold

  if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))
    
  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;
      eraseDisplay();// Immediate blank slate to make room for Meters
      fpbuttons();


    }
    if (mode_display == 1 ) {
      VirtLCDMode.setCursor(0, 0);  // reprint after Config menu
      VirtLCDMode.print("100ms Peak Power" );
    }
    if (mode_display == 2 ) {
      VirtLCDMode.setCursor(0, 0);  // reprint after Config menu
      VirtLCDMode.print("100ms Average Power" );
    }
    if (mode_display == 3 ) {
      VirtLCDMode.setCursor(0, 0);  // reprint after Config menu
      VirtLCDMode.print("1s Average Power" );
    }
    if (mode_display == 4 ) {
      VirtLCDMode.setCursor(0, 0);  // reprint after Config menu
      VirtLCDMode.print("Instantaneous Power" );
    }
    if (mode_display == 5 ) {
      VirtLCDMode.setCursor(0, 0);  // reprint after Config menu
      VirtLCDMode.print("Power in dBm" );
    }

    alternate ^= 1;                   // Alternate between the Text outputs to spread load

    if (alternate)                    // Print Graphs every second time
    {

      //------------------------------------------
      // Prepare and Print/Update Power Meter
      scale = scale_BAR(power_mw_long); // Determine scale setting
      scalePowerMeter(scale, &adj_scale, range);
      Meter1.scale(adj_scale, range);   // Redraw the bargraph scale if needed
      Meter1.graph(power, power_mw_pep, scale);

      //------------------------------------------
      // SWR Meter
      SWR.scale();                    // Draw SWR bargraph;
      swr_alm = R.SWR_alarm_trig / 10.0; // Set colour changeover points for swr mid and swr alarm
      if (swr_alm < swr_mid) swr_mid = swr_alm;
      SWR.graph(swr_mid, swr_alm, swr_avg);
    }
    else                              // And print text every second time
    {
      // SWR Printout
      if (flag.swr_alarm == false) {
        VirtLCDswr_G.setCursor(4, 0);
        print_swr();                    // and print the "SWR value"
        VirtLCDswr_G.print(lcd_buf);
      } else {

        VirtLCDswr_R.transfer();
        VirtLCDswr_R.setCursor(4, 0);
        print_swr();                    // and print the "SWR value"
        VirtLCDswr_R.print(lcd_buf);
      }


      //------------------------------------------
      // Power Indication

      //VirtLCDpwr.setCursor(4, 1);
      //VirtLCDpwr.print(power_display_indicator);
      //VirtLCDpwr.setCursor(8, 1);
      // if (Reverse) VirtLCDpwr.print("-");// If reverse power, then indicate
      //else         VirtLCDpwr.print(" ");
      // VirtLCDlargeY.setCursor(0, 0);   // Print Power in Large font, yellow or red
      // VirtLCDlargeR.setCursor(0, 0);
      print_p_mw(power);

      if (power_indicator.check() == 1) {
        power_indicator.reset();
        power_print_large(lcd_buf);
      }
      //********************************************************power_indicator.check**************************************************************//
      //------------------------------------------
      // Power indication, 1 second PEP
      VirtLCDpwr.setCursor(13, 2);
      VirtLCDpwr.print("PEP:");
      VirtLCDpwr.setCursor(17, 2);
      print_p_mw(power_mw_pep);
      VirtLCDpwr.print(lcd_buf);
    }
  }
  else                                // Screensaver display
  {
    Meter1.erase();
    SWR.erase();
    screensaver();
  }
}
//-----------------------------------------------------------------------------
//  Display: Bargraph, Power in dBm, SWR & PEP Power
//-----------------------------------------------------------------------------

void lcd_display_clean_dBm(void)
{
  double scale;                       // Progress bar scale
  double swr_alm;                     // SWR Alarm indication above this point by colour of graph
  double swr_mid = 2.0;               // Default SWR midlevel colour changeover point
  double adj_scale;                   // Adjust Power scales into ranges of 0 - 1000, uW, mW, W and kW
  char range[5];
  //------------------------------------------

  // Display mode intro for a time
  //if (flag_one_shot == true){fpbuttons(); flag_one_shot = false;}
  if (flag.mode_display)
  {
    if (flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      fpbuttons();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    lcd_display_mode_intro(" ", "", "", "");        //lcd_display_mode_intro("Mode: Power in dBm", "", "", "");
    flag.mode_display = false;      // Clear display change mode
    flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode
  }

  //----------------------------------------------
  // Display Power if level is above useful threshold

  if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))
  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;
      eraseDisplay();                 // Immediate blank slate to make room for Meters
      fpbuttons();
    }
    Meter1.init(10, 85, 450, 20);
    SWR.init(10, 140, 450, 20);
    VirtLCDMode.setCursor(0, 1);
    VirtLCDMode.print("Power in dBm"); // reprint after Config menu

    //------------------------------------------
    // Prepare and Print/Update Power Meter
    scale = scale_BAR(power_mw_long); // Determine scale setting
    scalePowerMeter(scale, &adj_scale, range);
    Meter1.scale(adj_scale, range);   // Redraw the bargraph scale if needed
    Meter1.graph(power_mw, power_mw_pep, scale);

    //------------------------------------------
    // SWR Meter
    SWR.scale();                      // Draw SWR bargraph;
    swr_alm = R.SWR_alarm_trig / 10.0; // Set colour changeover points for swr mid and swr alarm
    if (swr_alm < swr_mid) swr_mid = swr_alm;
    SWR.graph(swr_mid, swr_alm, swr_avg);

    //------------------------------------------

    // SWR Printout
    if (flag.swr_alarm == false) {
      VirtLCDswr_G.setCursor(4, 0);
      print_swr();                    // and print the "SWR value"
      VirtLCDswr_G.print(lcd_buf);
    } else {

      VirtLCDswr_R.transfer();
      VirtLCDswr_R.setCursor(4, 0);
      print_swr();                    // and print the "SWR value"
      VirtLCDswr_R.print(lcd_buf);
    }

    //------------------------------------------
    // Power Indication

    print_dbm(power_db * 10.0);

    if (power_indicator.check() == 1) {

      VirtLCDdBm.transfer();
      VirtLCDdBm.setCursor(0, 0);
      VirtLCDdBm.print(lcd_buf);

    }

    //------------------------------------------
    // Power indication, PEP
    VirtLCDpwr.setCursor(12, 2);
    VirtLCDpwr.print("PEP:");
    VirtLCDpwr.setCursor(16, 2);
    print_dbm(power_db_pep * 10.0);
    VirtLCDpwr.print(lcd_buf);

  }
  else                                // Screensaver display
  {
    Meter1.erase();
    SWR.erase();
    screensaver();
  }
}
//-----------------------------------------------------------------------------
//  Display Forward and Reflected Power
//  (input power is in mW)
//-----------------------------------------------------------------------------

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
  //if (flag_one_shot == true){fpbuttons(); flag_one_shot = false;}
  if (flag.mode_display)
  {
    if (flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      mode_timer = 0;                 // New mode, reset timer
      eraseDisplay();

      fpbuttons();

      Meter1.init(10, 85, 450, 20);
      SWR.init(10, 140, 450, 20);
      Meter2.init(10, 195, 450, 20);

    }
    mode_timer++;
    if (mode_timer >= MODE_INTRO_TIME)// Done with Mode Intro. MODE_INTRO_TIME in 10ths of seconds
    {
      mode_timer = 0;
      flag.mode_display = false;      // Clear display change mode
      flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode
      Meter1.init(10, 85, 450, 20);
      SWR.init(10, 140, 450, 20);
      Meter2.init(10, 195, 450, 20);
    }
  }

  //----------------------------------------------
  // Display Power if level is above useful threshold

  if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))

  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;

      eraseDisplay();                 // Immediate blank slate to make room for Meters
      fpbuttons();
    }
    if (mode_display == 6 ) {
      VirtLCDMode.setCursor(0, 1);  // reprint after Config menu
      VirtLCDMode.print("FWD and REF Power" );
    }

    //------------------------------------------
    // Prepare and Print/Update Power Meters
    scale = scale_BAR(fwd_power_mw);     // Determine scale setting
    scalePowerMeter(scale, &adj_scale, range);
    Meter1.scale(adj_scale, range);      // Redraw the bargraph scale if needed
    Meter1.graph(fwd_power_mw, 0, scale); // No PEP in this mode

    Meter2.scale(adj_scale, range);      // Redraw the bargraph scale if needed
    Meter2.graph(ref_power_mw, 0, scale); // No PEP in the reverse direction

    //------------------------------------------
    // SWR Meter
    SWR.scale();                         // Draw SWR bargraph;
    swr_alm = R.SWR_alarm_trig / 10.0;   // Set colour changeover points for swr mid and swr alarm
    if (swr_alm < swr_mid) swr_mid = swr_alm;
    SWR.graph(swr_mid, swr_alm, swr_avg);

    //------------------------------------------
    // Forward Power Indication
    //VirtLCDpwr.setCursor(4, 1);
    //VirtLCDpwr.print("Fwd:");
    VirtLCDlargeY.setCursor(0, 0);     // Print Power in Large font, yellow or red
    VirtLCDlargeR.setCursor(0, 0);
    print_p_mw(fwd_power_mw);
    if (power_indicator.check() == 1) {
      power_print_large(lcd_buf);
      //------------------------------------------
      // Reflected Power indication
      VirtLCDpwr.setCursor(13, 2);
      VirtLCDpwr.print("Ref:");
      VirtLCDpwr.setCursor(17, 2);
      print_p_mw(ref_power_mw);
      VirtLCDpwr.print(lcd_buf);
    }
    // SWR Printout
    if (flag.swr_alarm == false) {
      VirtLCDswr_G.setCursor(4, 0);
      print_swr();                    // and print the "SWR value"
      VirtLCDswr_G.print(lcd_buf);
    } else {

      VirtLCDswr_R.transfer();
      VirtLCDswr_R.setCursor(4, 0);
      print_swr();                    // and print the "SWR value"
      VirtLCDswr_R.print(lcd_buf);
    }

  }
  else                                // Screensaver display
  {
    Meter1.erase();
    Meter2.erase();
    SWR.erase();
    screensaver();
  }
}

//-----------------------------------------------------------------------------
//  Display Modulation Scope
//-----------------------------------------------------------------------------

void lcd_display_modscope(void)
{
  static uint16_t mode_timer = 0;     // Used to time the Display Mode intro

  //------------------------------------------
  // Display mode intro for a time
  //if (flag_one_shot == true){fpbuttons(); flag_one_shot = false;}
  if (flag.mode_display)
  {
    if (flag.mode_change)
    {
      flag.mode_change = false;       // Clear display change mode
      mode_timer = 0;                 // New mode, reset timer
      lcd_display_mode_intro("", " ", "Modulation Scope", " ");
    }
    mode_timer++;
    if (mode_timer >= MODE_INTRO_TIME)// Done with Mode Intro. MODE_INTRO_TIME in 10ths of seconds
    {
      mode_timer = 0;
      flag.mode_display = false;      // Clear display change mode
      flag.idle_refresh = true;       // Force screensaver reprint upon exit, if screensaver mode
      //eraseDisplay();
      modScopeActive = true;          // Start Modulation Scope
    }
  }

  //----------------------------------------------
  // Display Power if level is above useful threshold

  if ((power_mw > R.idle_disp_thresh) || (flag.power_detected))

  {
    if (flag.screensaver_on)
    {
      flag.screensaver_on = false;

      eraseDisplay();      // Immediate blank slate to make room for Meters
      fpbuttons();
    }
    VirtLCDMode.setCursor(0, 1); VirtLCDMode.print("Modulation Scope"); // reprint after Config menu

    //------------------------------------------
    // Update scope with pending data
    ModScope.update();


    print_p_mw(power_mw_pk);
    if (power_indicator.check() == 1) {
      VirtLCDlargeY.setCursor(0, 0);
      VirtLCDlargeY.print(lcd_buf);
    }
    // SWR Printout
    if (flag.swr_alarm == false) {
      VirtLCDswr_G.setCursor(4, 0);
      print_swr();                    // and print the "SWR value"
      VirtLCDswr_G.print(lcd_buf);
    } else {

      VirtLCDswr_R.transfer();
      VirtLCDswr_R.setCursor(4, 0);
      print_swr();                    // and print the "SWR value"
      VirtLCDswr_R.print(lcd_buf);
    }

    //------------------------------------------
    // Power Indication
    VirtLCDpwr.setCursor(13, 1);
    VirtLCDpwr.print("Pk :");
    print_p_mw(power_mw_pk);
    VirtLCDpwr.print(lcd_buf);



    //------------------------------------------
    // Power indication, PEP
    VirtLCDpwr.setCursor(13, 2);
    VirtLCDpwr.print("PEP:");
    print_p_mw(power_mw_pep);
    VirtLCDpwr.print(lcd_buf);
  }
  else                                // Screensaver display
  {
    ModScope.erase();
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

  eraseTouchButtons();


  VirtLCDy.clear();
  VirtLCDmessage.clear();
  VirtLCDdebug.clear();
  VirtLCDw.clear();

  double    output_voltage;
  uint16_t  v, v_sub;

  VirtLCDy.setCursor(4, 0);
  VirtLCDy.print("Debug Display:");
  fpbuttons();


  //------------------------------------------
  // Forward voltage
  VirtLCDdebug.setCursor(0, 1);
  output_voltage = fwd * adc_ref / 4096;
  v_sub = output_voltage * 1000;
  v = v_sub / 1000;
  v_sub = v_sub % 1000;
  sprintf(lcd_buf, "Fwd AD:%4u   %u.%03uV ", fwd, v, v_sub);
  VirtLCDdebug.print(lcd_buf);
  //------------------------------------------
  // Reverse voltage
  VirtLCDdebug.setCursor(0, 2);
  output_voltage = rev * adc_ref / 4096;
  v_sub = output_voltage * 1000;
  v = v_sub / 1000;
  v_sub = v_sub % 1000;
  sprintf(lcd_buf, "Ref AD:%4u   %u.%03uV ", rev, v, v_sub);
  VirtLCDdebug.print(lcd_buf);

  //#if AD8307_INSTALLED
  //------------------------------------------
  // Calibrate 1
  VirtLCDdebug.setCursor(0, 3);
  sprintf (lcd_buf, "%4d ", R.cal_AD[0].db10m);
  VirtLCDdebug.print(lcd_buf);

  sprintf (lcd_buf, "F:%01.03f, R:%01.03f", R.cal_AD[0].Fwd, R.cal_AD[0].Rev);
  VirtLCDdebug.print(lcd_buf);
  //------------------------------------------
  // Calibrate 2
  VirtLCDdebug.setCursor(0, 4);
  sprintf (lcd_buf, "%4d ", R.cal_AD[1].db10m);
  VirtLCDdebug.print(lcd_buf);
  sprintf (lcd_buf, "F:%01.03f, R:%01.03f", R.cal_AD[1].Fwd, R.cal_AD[1].Rev);
  VirtLCDdebug.print(lcd_buf);
  //------------------------------------------
  // Fwd and Ref Power
  VirtLCDdebug.setCursor(0, 5);
  VirtLCDdebug.print("Fwd ");
  print_p_mw((int32_t) fwd_power_mw);
  VirtLCDdebug.print(lcd_buf);
  VirtLCDdebug.print("  Ref ");
  print_p_mw((int32_t) ref_power_mw);
  VirtLCDdebug.print(lcd_buf);

  tft.setFont(DroidSansMono_14);
  tft.setTextColor(YELLOW);
  tft.setCursor(160, 280);
  tft.print("Touch for Exit");


  if (TouchZ == 0) {
    exit_time.restart();
  }

  if (exit_time.hasPassed(10) && TouchZ  == 1) {
    exit_time.start();
    touch.exit = true;
    tft.fillScreen(ILI9488_BLACK);
    flag.config_mode = false;
    eraseDisplay();
    exit_time = false;

  }
}

//-----------------------------------------------------------------------------
//  Debug Display 2.  All sorts of Gauges...
//-----------------------------------------------------------------------------

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
    eraseTouchButtons();

    eraseDisplay();
    Meter1.init(15,  20, 430, 20, WHITE, ORANGE, GREEN); // Combined FWD and REV meter
    Meter2.init(15, 70,  430, 20);                                            // PEP and inst
    Meter3.init(15, 120, 430, 20);                                            // PEP and Pk
    Meter4.init(15, 170, 430, 20);                                           // PEP and 100ms AVG
    Meter5.init(15, 220, 430, 20);                                           // PEP and 1s AVG
    SWR.init   (15, 270, 430, 20);
    lcd_display_debug2_text(YELLOW);                                  // Text for gauges

  }
  if (TouchZ == 0) {
    exit_time.restart();
  }

  if (exit_time.hasPassed(500) && TouchZ  == 1) {
    exit_time.start();
    flag.short_push = true;
    touch.exit = true;

    eraseDisplay();
    exit_time = false;
  }
  //------------------------------------------
  // Prepare and Print/Update Power Meters

  scale = scale_BAR(fwd_power_mw);                                             // Determine scale setting

  scalePowerMeter(scale, &adj_scale, range);
  Meter1.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter1.graph(ref_power_mw, fwd_power_mw, scale);                             // No PEP in this mode

  Meter2.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter2.graph(power_mw, power_mw_pep, scale);

  Meter3.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter3.graph(power_mw_pk, 0, scale);

  Meter4.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter4.graph(power_mw_avg, 0, scale);

  Meter5.scale(adj_scale, range);                                              // Redraw the bargraph scale if needed
  Meter5.graph(power_mw_1savg, 0, scale);

  //------------------------------------------
  // SWR Meter
  SWR.scale();                                                                 // Draw SWR bargraph;
  swr_alm = R.SWR_alarm_trig / 10.0;                                           // Set colour changeover points for swr mid and swr alarm
  if (swr_alm < swr_mid) swr_mid = swr_alm;
  SWR.graph(swr_mid, swr_alm, swr);
}
//------------------------------------------
void lcd_display_debug_erase(void)
{
  dbg2init = false;

  lcd_display_debug2_text(BLACK);                                      // Erase text
  Meter1.erase();
  Meter2.erase();
  Meter3.erase();
  Meter4.erase();
  Meter5.erase();
  SWR.erase();

  if (flag_debug > 5) {
    Meter1.init(20, 5, 725, 35);             // Return to meters Forward and Reflected
    Meter2.init(20, 105, 725, 35);           // when coming from debug mode
    SWR.init   (20, 205, 725, 35);           // this was a bug in version 1.02

  }
  if (flag_debug < 5) {
    Meter1.init(20, 5, 725, 70);
    SWR.init   (20, 135, 725, 70);
  }
}
//------------------------------------------
void lcd_display_debug2_text(int16_t textcolour)
{
  tft.setFont(DroidSansMono_16);
  tft.setTextColor(textcolour);
  tft.setCursor( 7, 10);
  tft.print("fwd");
  tft.setCursor( 7, 35);
  tft.print("ref");
  tft.setCursor( 7, 90);
  tft.print("pep");
  tft.setCursor( 7, 115);
  tft.print("inst");
  tft.setCursor( 7, 180);
  tft.print("pk");
  tft.setCursor( 7, 250);
  tft.print("avg");
  tft.setCursor( 7, 275);
  tft.print("0.1s");
  tft.setCursor( 7, 330);
  tft.print("avg");
  tft.setCursor( 7, 355);
  tft.print("1s");
}
void swr_alarm() {

  tft.fillRect(300, 0, 162, 80, RED);
  VirtLCDAlarm.clear();
  VirtLCDAlarm.transfer();
  VirtLCDswr_R.transfer();
  VirtLCDswr_R.clear();

  digitalWrite(R_Led, flag.swr_alarm);     // Turn on the Alarm LED
  analogWrite(14, 128);
  digitalWrite(9, HIGH); // PTT ouput
}

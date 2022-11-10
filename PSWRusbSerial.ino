//*********************************************************************************
//**
//** Project.........: A menu driven Multi Display RF Power and SWR Meter
//**                   using a Tandem Match Coupler and 2x AD8307; or
//**                   diode detectors.
//**
//** Copyright (c) 2014  Loftur E. Jonasson  (tf3lj [at] arrl [dot] net)
//** Copyright (c) 2020  J.G. Holstein  
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
//*********************************************************************************

#include <stdio.h>
#include <string.h>
//
//-----------------------------------------------------------------------------------------
//                      Send power and SWR measurement data to the Computer
//
//                      Format of transmitted data is:
//                      Incident (real transmitted) power, VSWR
//-----------------------------------------------------------------------------------------
//
//------------------------------------------
// Instantaneous Power (unformatted) and SWR
void usb_poll_data(void)
{
  //------------------------------------------
  // Power indication, incident power
  if (Reverse)   Serial.print("-");
  Serial.print(power_mw/1000,8);
  Serial.print(F(", "));
  Serial.println(swr,2);
}

//------------------------------------------
// Instantaneous Power (formatted, mW - kW)
void usb_poll_inst(void)
{
  print_p_mw(power_mw);
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

//------------------------------------------
// Peak (100ms) Power (formatted, mW - kW)
void usb_poll_pk(void)
{
  print_p_mw(power_mw_pk);
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

//------------------------------------------
// PEP (1s) Power (formatted, mW - kW)
void usb_poll_pep(void)
{
  print_p_mw(power_mw_pep);
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

//------------------------------------------
// AVG (100ms) Power (formatted, mW - kW)
void usb_poll_avg(void)
{
  //------------------------------------------
  // Power indication, 100ms average power, formatted, pW-kW
  print_p_mw(power_mw_avg);
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

//------------------------------------------
// AVG (1s) Power (formatted, mW - kW)
void usb_poll_1savg(void)
{
  //------------------------------------------
  // Power indication, 1s average power, formatted, pW-kW
  print_p_mw(power_mw_1savg);
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

void usb_poll_instdb(void)
{
  //------------------------------------------
  // Power indication, instantaneous power, formatted, dB
  print_dbm((int16_t) (power_db*10.0));
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

void usb_poll_pkdb(void)
{
  //------------------------------------------
  // Power indication, 100ms peak power, formatted, dB
  print_dbm((int16_t) (power_db_pk*10.0));
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

void usb_poll_pepdb(void)
{
  //------------------------------------------
  // Power indication, PEP power, formatted, dB
  print_dbm((int16_t) (power_db_pep*10.0));
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

void usb_poll_avgdb(void)
{
  //------------------------------------------
  // Power indication, 100ms average power, formatted, dB
  print_dbm((int16_t) (log10(power_mw_avg)*100.0));
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}
void usb_poll_1savgdb(void)
{
  //------------------------------------------
  // Power indication, 1s average power, formatted, dB
  print_dbm((int16_t) (log10(power_mw_1savg)*100.0));
  Serial.print(lcd_buf);
  //------------------------------------------
  // SWR indication
  Serial.print(F(", VSWR"));
  print_swr();
  Serial.println(lcd_buf);
}

//-----------------------------------------------------------------------------------------
//                      Send Power and SWR measurement data to the Computer
//
//                      Long and Human Readable Format
//-----------------------------------------------------------------------------------------

void usb_poll_long(void)
{
  
  
// forward power  
    print_p_mw(fwd_power_mw);
    Serial.print(lcd_buf);                   // array 0 in Pascal
    Serial.print(F(","));
  
// reflected power 
    print_p_mw(ref_power_mw);
    Serial.print(lcd_buf);                   // array 1 in Pascal
    Serial.print(F(","));
    

// pep power 
    print_p_mw(power_mw_pep);
    Serial.print(lcd_buf);                    // array 2 in Pascal
    Serial.print(F(","));
 
    print_p_mw(power_mw_avg);                 // array 3 in Pascal
    Serial.print(lcd_buf);
    
    Serial.print(F(","));
    
    print_dbm((int16_t) (power_db*10.0));
    Serial.print(lcd_buf);                    // array 4 in Pascal
    Serial.print(F(","));
    print_swr();
    Serial.print(lcd_buf);                    // array 5 in Pascal
    Serial.print(F(","));

    // 100 ms peak power          
    print_p_mw(power_mw_pk);
    Serial.print(lcd_buf);                    // array 6 in Pascal
    Serial.print(F(","));
    
// 100 ms peak power                  
    print_p_mw(power_mw_pk);
    pwr100ms = atoi (lcd_buf); 
     Serial.print(pwr100ms);                  // array 7 in Pascal
     Serial.print(F(","));

// Get swr alarm trigger   
    if (R.SWR_alarm_trig < 40){
      Serial.print(R.SWR_alarm_trig/10.0,1);  // array 8 in Pascal
      Serial.print(F(","));
   }                                                
    if (R.SWR_alarm_trig == 40){
      Serial.print("Off");                    // array 8 in Pascal
      Serial.print(F(","));
    }

// SWR alarm trigger
   if (flag.swr_alarm == true){
     Serial.print( "SWR Alarm triggered");    // array 9 in Pascal
     Serial.print(F(","));
   }
   if (flag.swr_alarm == false){
      Serial.print(F(","));
   }
// Range indicator for graphical interface 
        if (p_range_indc == "pW"){range_indicator = 1;}
        if (p_range_indc == "nW"){range_indicator = 2;}
        if (p_range_indc == "uW"){range_indicator = 3;}       // array 10 in Pascal
        if (p_range_indc == "mW"){range_indicator = 4;}
        if (p_range_indc == "W" ){range_indicator = 5;}
        if (p_range_indc == "kW"){range_indicator = 6;}
        if (p_range_indc == "MW"){range_indicator = 7;}
    Serial.print(range_indicator);
    Serial.print(F(","));
   
   if (state == false){
    Serial.print("COUPLER 1");
     Serial.print(F(","));
   }                                       // array 11 in Pascal
    if (state == true){
    Serial.print("COUPLER 2");
     Serial.print(F(","));
   }

// Reflected power no mWatt in string
     print_p_mw(ref_power_mw);
     refl_bar = atoi (lcd_buf); 
     Serial.print( refl_bar);              // array 12 in Pascal
     Serial.print(F(","));                  
    
     
// Instantaneous power   
    print_p_mw(power_mw);
    Serial.print(lcd_buf);                
    Serial.print(F(","));                // Instantaneous Power  13 in Pascal
     
// Voltage FWD     
     double  output_voltage;
     output_voltage = fwd * adc_ref / 4096;
     Serial.print(output_voltage,4);
     Serial.print(F(","));                  // voltage FWD array 14 in Pascal
     
// Voltage REV         
     double  output_voltage_rev;
     output_voltage_rev = rev * adc_ref / 4096;  // voltage REV array 15 in Pascal
     Serial.print(output_voltage_rev,4);
     Serial.print(F(","));    
                   
// SWR Alarm Power                       //  array 16 in Pascal

     Serial.print(R.SWR_alarm_pwr_thresh);
     Serial.print(F(","));  


// PEP period                        // array 17 in Pascal
     
     Serial.print(R.PEP_period/(PEP_BUFFER/5.0),1);
     Serial.print(F(","));   

// Calibration read value             // array 18
   
    Serial.print(lcd_buf_cal);         
    Serial.print(F(","));  

// Calibration actual value             // array 19

    Serial.print(lcd_buf_actual);         
    Serial.print(F(","));      

// Calibration status string           // array 20
    if (status_ind_cal == 0){Serial.print("calibrate using one accurate level");}
    if (status_ind_cal == 1){Serial.print("set higher of two accurate levels");}
    if (status_ind_cal == 2){Serial.print("set lower of two accurate levels");}
    if (status_ind_cal == 4){Serial.print("single Level Cal");}
    if (status_ind_cal == 5){Serial.print("first Cal SetPoint");}
    if (status_ind_cal == 6){Serial.print("second Cal SetPoint");}  
    Serial.print(F(",")); 
    
// Signal quality                       // array 21

    if (signal_quality == 0){Serial.print("No signal");}
    if (signal_quality == 1){Serial.print("Signal detected");}
    if (signal_quality == 2){Serial.print("Reverse detected");}
    if (signal_quality == 3){Serial.print("Poor signal quality");}
    Serial.print(F(",")); 
// Display status                                                     // array 22

    if (display_attached == true){
      Serial.print("Display Connected");
      
      
      //tft.brightness(R.disp.tft_backlight * 25 + 5);
    }
    if (display_attached == false){
      Serial.print("Display Disonnected");
      //tft.displayOn(0);
      //tft.brightness(0);
    }
    Serial.print(F(",")); 


// Scale get 1st range                                                // array 23
    sprintf(lcd_buf,"%4u",R.ScaleRange[0]);
    Serial.print(lcd_buf);
    Serial.print(F(",")); 

// Scale get 2nd range                                                // array 24
    sprintf(lcd_buf,"%4u",R.ScaleRange[1]);
    Serial.print(lcd_buf);
    Serial.print(F(",")); 


// Scale get 3rd range                                                 // array 25
    sprintf(lcd_buf,"%4u",R.ScaleRange[2]);
    Serial.print(lcd_buf); 
    Serial.print(F(",")); 


// Display intensity                                                   // array 26
    //intensity = R.disp.tft_backlight;
    //Serial.print(intensity * 10);//Serial.print(" %");
    Serial.print(F(",")); 

// Screensaver thresholds
   Serial.print(current_selection_threshold);                          // array 27
   Serial.print(F(",")); 
   
// Low power floor                                                     // array 28

    Serial.print(R.low_power_floor);
    Serial.print(F(",")); 


// Calibration setting
    sprintf(lcd_buf,"%4d  %01.03f  %01.03f  %4d  %01.03f  %01.03f",    // array 29
        R.cal_AD[0].db10m,R.cal_AD[0].Fwd,R.cal_AD[0].Rev,
        R.cal_AD[1].db10m,R.cal_AD[1].Fwd,R.cal_AD[1].Rev);
    Serial.print(lcd_buf);
    Serial.println();
    
}
//------------------------------------------
// AD debug report - prints out raw AD values
void usb_poll_ad_debug(void)
{
  Serial.print(F("AD values: "));
  Serial.print(fwd);
  Serial.print(F(", "));
  Serial.println(rev);
}
//------------------------------------------
// Prints the selected PSWR report type on a continuous basis, once every 100 milliseconds
void usb_cont_report(void)
{
  if (R.usb_report_cont)
  {
    if (R.usb_report_type == REPORT_DATA) usb_poll_data();

    else if (R.usb_report_type == REPORT_INST) usb_poll_inst();
    else if (R.usb_report_type == REPORT_PK) usb_poll_pk();
    else if (R.usb_report_type == REPORT_PEP) usb_poll_pep();
    else if (R.usb_report_type == REPORT_AVG) usb_poll_avg();
    else if (R.usb_report_type == REPORT_1SAVG) usb_poll_1savg();

    else if (R.usb_report_type == REPORT_INSTDB) usb_poll_instdb();
    else if (R.usb_report_type == REPORT_PKDB) usb_poll_pkdb();
    else if (R.usb_report_type == REPORT_PEPDB) usb_poll_pepdb();
    else if (R.usb_report_type == REPORT_AVGDB) usb_poll_avgdb();
    else if (R.usb_report_type == REPORT_1SAVGDB) usb_poll_1savgdb();

    else if (R.usb_report_type == REPORT_LONG) usb_poll_long();
    else if (R.usb_report_type == REPORT_AD_DEBUG) usb_poll_ad_debug();
  }
}


//-----------------------------------------------------------------------------------------
//      Print a complete list of USB commands upon request
//-----------------------------------------------------------------------------------------

void usb_print_help(void)
{
  Serial.println(F(
            "Available USB commands:\r\n"
            "\r\n"
            "$ppoll             Poll for one single USB serial report, inst power (unformatted).\r\n"
            "$pinst             Poll for one single USB serial report, inst power (human readable).\r\n"
            "$ppk               Poll for one single USB serial report, 100ms peak power (human readable).\r\n"
            "$ppep              Poll for one single USB serial report, pep power (human readable).\r\n"
            "$pavg              Poll for one single USB serial report, 100ms avg power (human readable).\r\n"
            "$p1savg            Poll for one single USB serial report, 1s avg power (human readable).\r\n"
            "$pinstdb           Poll for one single USB serial report, inst power in dB (human readable).\r\n"
            "$ppkdb             Poll for one single USB serial report, 100ms peak power in dB (human readable).\r\n"
            "$ppepdb            Poll for one single USB serial report, pep power in dB (human readable).\r\n"
            "$pavgdb            Poll for one single USB serial report, 100ms avg power in dB (human readable).\r\n"
            "$p1savgdb          Poll for one single USB serial report, 1s avg power in dB (human readable).\r\n"
            "$plong             Poll for one single USB serial report, actual power (inst, pep and avg)\r\n"
            "                   as well as fwd power, reflected power and SWR (long form).\r\n"
            "\r\n"
            "$pcont             USB serial reporting in a continuous mode, 10 times per second.\r\n"
            "\r\n"
            "                   $ppoll, $pinst, $ppk, $ppep, $pavg or $plong entered after $pcont will\r\n"
            "                   switch back to single shot mode.\r\n"
            "\r\n"
            "$sleepmsg=abcdefg  Where abcdefg is a free text string to be displayed when\r\n"
            "                   in screensaver mode, up to 20 characters max.\r\n"         
            #if AD8307_INSTALLED                // --------------Only used with AD8307:            
            "$sleeppwrset x     Power above the level defined here will turn the display into meter mode.\r\n"
            "                   x = 0.001, 0.01, 0.1, 1 or 10 mW (milliWatts).\r\n"
            "$sleeppwrget       Return current value.\r\n"
            #endif
            "\r\n"
            "$tuneset x         x = 1.1 to 4.0. SWR tune threshold.\r\n"
            "$tuneget           Return current value.\r\n"
            "\r\n"
            "$alarmset x        x = 1.5 to 3.9. 4 will inactivate SWR Alarm function.\r\n"
            "$alarmget          Return current value.\r\n"
            "$alarmreset        Deactivate Alarm if activated.\r\n"
            "$alarmpowerset x   x = 1, 10, 100, 1000 or 10000 mW (milliwatts).\r\n"
            "$alarmpowerget     Return current value.\r\n"
            "\r\n"
            "$pepperiodset x    x = 1, 2.5 or 5 seconds.  PEP sampling period.\r\n"
            "$pepperiodget      Return current value.\r\n"
            "\r\n"
            #if AD8307_INSTALLED                // --------------Only used with AD8307:            
            "$calset cal1 AD1-1 AD2-1 cal2 AD1-2 AD2-2\r\n"
            "                   Write new calibration values to the meter.\r\n"
            "$calget            Retrieve calibration values.\r\n"
            "\r\n"
            "                   Format of calibration values is:\r\n"
            "                   cal1 AD1-1 AD2-1 cal2 AD1-2 AD2-2\r\n"
            "                                 where:\r\n"
            "                   cal1 and cal2 are calibration setpoints 1 and 2 in 10x dBm\r\n"
            "                                 and\r\n"
            "                   ADx-1 and ADx-2 are the corresponding AD voltage values for\r\n"
            "                   AD1 (forward direction) and AD2 (reverse direction).\r\n"
            "                   (\r\n"
            "                      normally the AD1 and AD2 voltage values for each setpoint would be the same,\r\n"
            "                      however by doing reverse calibration through the Controller Menu functions\r\n"
            "                      it is possible to balance any small differences there might be between the\r\n"
            "                      two AD8307 outputs.\r\n"
            "                      Note that I have not found this to be necessary at all :)\r\n"
            "                   )\r\n"
            #else
            "$calset cal        Set calibration multiplier (10 - 250). 100 equals a voltage multiplier of 1.\r\n"
            "$calget            Retrieve calibration multiplier.\r\n"
            #endif
            "\r\n"
            "$scaleget          Retrieve user definable scale ranges.\r\n"
            "$scaleset          Write new  scale ranges.\r\n"
            "\r\n"
            "                   The scale ranges are user definable, up to 3 ranges per decade,\r\n"
            "                   e.g. 11, 22 and 55 gives:\r\n"
            "                   ... 11mW, 22mW, 55mW, 110mW ... 1.1W, 2.2W, 5.5W 11W 22W 55W 110W ...\r\n"
            "                   If all three values set as \"2\", then\r\n"
            "                   ... 2W, 20W, 200W ...\r\n"
            "                   The third and largest value has to be less than ten times the first value.\r\n"
            "\r\n"
            "$addebug           read raw AD input - also works with $pcont, same as $ppoll etc...\r\n"
            "\r\n"
            "$version           Report version and date of firmware.\r\n"
            "$help              Display the above instructions.\r\n"
            "\r\n" ));                   
// Debug ################################################# 
//          "$memorywipe        full EEPROM wipe - force a "factory reset".\r\n"
//          "$softreset         soft reset of Controller - same as turning off and on. Probably not very useful
}

//
//-----------------------------------------------------------------------------------------
//      Parse and act upon an incoming USB command; USB port enumerates as a COM port
//-----------------------------------------------------------------------------------------
//
char incoming_command_string[50];                             // Input from USB Serial
void usb_parse_incoming(void)
{
  char *pEnd;
  uint16_t inp_val;
  double inp_double;

  if (!strcasecmp("ppoll",incoming_command_string))           // Poll, if Continuous mode, then switch into Polled Mode
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_DATA)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_DATA;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);      
    }
    usb_poll_data();                                          // Send data over USB
  }
  else if (!strcasecmp("pinst",incoming_command_string))      // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed 
    if ((R.usb_report_type != REPORT_INST)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_INST;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);        
    }
    usb_poll_inst();
  }
  else if (!strcasecmp("ppk",incoming_command_string))        // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_PK)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_PK;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);    
    }
    usb_poll_pk();
  }
  else if (!strcasecmp("ppep",incoming_command_string))       // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_PEP)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_PEP;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R); 
    }
    usb_poll_pep();
  }
  else if (!strcasecmp("pavg",incoming_command_string))       // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_AVG)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_AVG;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R); 
    }
    usb_poll_avg();
  }
  else if (!strcasecmp("p1savg",incoming_command_string))     // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_1SAVG)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_1SAVG;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R); 
    }
    usb_poll_1savg();
  }
  else if (!strcasecmp("pinstdb",incoming_command_string))    // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_INSTDB)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_INSTDB;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);
    }
    usb_poll_instdb();
  }
  else if (!strcasecmp("ppkdb",incoming_command_string))      // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_PKDB)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_PKDB;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);
    }
    usb_poll_pkdb();
  }
  else if (!strcasecmp("ppepdb",incoming_command_string))     // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_PEPDB)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_PEPDB;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);
    }
    usb_poll_pepdb();
  }
  else if (!strcasecmp("pavgdb",incoming_command_string))     // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_AVGDB)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_AVGDB;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);
    }
    usb_poll_avgdb();
  }
  else if (!strcasecmp("p1savgdb",incoming_command_string))   // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_1SAVGDB)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_1SAVGDB;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);
    }
    usb_poll_1savgdb();
  }
  else if (!strcasecmp("plong",incoming_command_string))      // Poll for one single Human Readable report
  {
    // Disable continuous USB report mode ($pcont) if previously set
    // and Write report mode to EEPROM, if changed
    if ((R.usb_report_type != REPORT_LONG)||(R.usb_report_cont))
    {
      EEPROM_readAnything(1,R);
      R.usb_report_type = REPORT_LONG;
      R.usb_report_cont = false;
      EEPROM_writeAnything(1,R);
    }
    usb_poll_long();
  }
  else if (!strcasecmp("addebug",incoming_command_string))    // Poll for one single Human Readable report
  {
     // Disable continuous USB report mode ($pcont) if previously set
     if (R.usb_report_cont)
    {
      R.usb_report_cont = false;
    }
    R.usb_report_type = REPORT_AD_DEBUG;
    usb_poll_ad_debug();
  }
  else if (!strcasecmp("pcont",incoming_command_string))      // Switch into Continuous Mode
  {
    // Enable continuous USB report mode ($pcont), and write to EEPROM, if previously disabled
    if (!R.usb_report_cont)
    {
      EEPROM_readAnything(1,R);
      R.usb_report_cont = true;
      EEPROM_writeAnything(1,R);
    }
  }
else if (!strncasecmp("savhreshold",incoming_command_string,11))   // Retrieve scale limits
  {
     
     
     current_selection_threshold = strtod(incoming_command_string+11,&pEnd);
     if (current_selection_threshold == 0) R.idle_disp_thresh = 0;
     else if (current_selection_threshold == 1) R.idle_disp_thresh = 0.001; 
     else if (current_selection_threshold == 2) R.idle_disp_thresh = 0.01;
     else if (current_selection_threshold == 3) R.idle_disp_thresh = 0.1;
     else if (current_selection_threshold == 4) R.idle_disp_thresh = 1;
     else if (current_selection_threshold == 5) R.idle_disp_thresh = 10;
      
      
      EEPROM_readAnything(1, eeprom_R);
    if (eeprom_R.idle_disp_thresh != R.idle_disp_thresh)// New Value
    {
      EEPROM_writeAnything(1, R);
    }  
  
   }

// Low power floor
   else if (!strncasecmp("floor",incoming_command_string,5))   // Retrieve scale limits
  {
     
     
     R.low_power_floor = strtod(incoming_command_string+5,&pEnd);
           
      EEPROM_readAnything(1, eeprom_R);
    if (eeprom_R.low_power_floor != R.low_power_floor)// New Value
    {
      EEPROM_writeAnything(1, R);
    }  
  
   }
   
  #if AD8307_INSTALLED
  else if (!strcasecmp("calget",incoming_command_string))     // Retrieve calibration values
  {
    //Serial.print(F("AD8307 Cal: "));
    sprintf(lcd_buf,"%4d %01.03f %01.03f %4d %01.03f %01.03f", 
        R.cal_AD[0].db10m,R.cal_AD[0].Fwd,R.cal_AD[0].Rev,
        R.cal_AD[1].db10m,R.cal_AD[1].Fwd,R.cal_AD[1].Rev);
    //Serial.println(lcd_buf);
  }
  else if (!strncasecmp("calset",incoming_command_string,6))  // Write new calibration values
  {
    EEPROM_readAnything(1,R);
    R.cal_AD[0].db10m = strtol(incoming_command_string+6,&pEnd,10);
    R.cal_AD[0].Fwd = strtod(pEnd,&pEnd);
    R.cal_AD[0].Rev = strtod(pEnd,&pEnd);
    R.cal_AD[1].db10m = strtol(pEnd,&pEnd,10);
    R.cal_AD[1].Fwd = strtod(pEnd,&pEnd);
    R.cal_AD[1].Rev = strtod(pEnd,&pEnd);
    EEPROM_writeAnything(1,R);
  }
  #else
  else if (!strcasecmp("calget",incoming_command_string))     // Retrieve calibration values
  {
    //Serial.print(F("Meter Cal: "));
    //Serial.println(R.meter_cal/100.0,2);
  }
  else if (!strncasecmp("calset",incoming_command_string,6))  // Write new calibration values
  {
    inp_val = strtod(incoming_command_string+6,&pEnd) *100;
    if ((inp_val >= 10) && (inp_val < 251))
    {
      EEPROM_readAnything(1,R);
      R.meter_cal = inp_val;
      EEPROM_writeAnything(1,R);
    }
  }  
  #endif

  else if (!strcasecmp("scaleget",incoming_command_string))   // Retrieve scale limits
  {
    Serial.print(F("Scale: "));
    sprintf(lcd_buf,"%4u,%4u,%4u",
        R.ScaleRange[0],R.ScaleRange[1],R.ScaleRange[2]);
    Serial.println(lcd_buf);
  }
  else if (!strncasecmp("scaleset",incoming_command_string,8))// Write new scale limits
  {
    uint8_t r1, r2, r3;  
  
    r1 = strtol(incoming_command_string+8,&pEnd,10);
    r2 = strtol(pEnd,&pEnd,10);
    r3 = strtol(pEnd,&pEnd,10);
    // Bounds dependencies check and adjust
    //
    // Scales 2 and 3 cannot ever be larger than 9.9 times Scale 1
    // Scale 2 is equal to or larger than Scale 1
    // Scale 3 is equal to or larger than Scale 2
    // If two scales are equal, then only two Scale Ranges in effect
    // If all three scales are equal, then only one Scale Range is in effect
    // If Scale 1 is being adjusted, Scales 2 and 3 can be pushed up or down as a consequence
    // If Scale 2 is being adjusted up, Scale 3 can be pushed up
    // If Scale 3 is being adjusted down, Scale 2 can be pushed down
    if (r2 >= r1*10) r2 = r1*10 - 1;
    if (r3 >= r1*10) r3 = r1*10 - 1;
    // Ranges 2 and 3 cannot be smaller than range 1
    if (r2 < r1) r2 = r1;
    if (r3 < r1) r3 = r1;
    // Range 2 cannot be larger than range 3
    if (r2 > r3) r3 = r2;

    EEPROM_readAnything(1,R);
    R.ScaleRange[0] = r1;
    R.ScaleRange[1] = r2;
    R.ScaleRange[2] = r3;
    EEPROM_writeAnything(1,R);
  }

  else if (!strncasecmp("sleepmsg=",incoming_command_string,9))// A new "sleep message" string was received
  {
    EEPROM_readAnything(1,R);
    // Copy up to 20 characters of the received string
    strncpy(R.idle_disp, incoming_command_string+9,20);
    // and store in EEPROM
    EEPROM_writeAnything(1,R);
  }
 
  //
  // The below are a bit redundant, as they are fully manageable by the rotary encoder:
  //
  //    $sleeppwrset x    Power below the level defined here will put display into screensaver mode.
  //                      x = 0.001, 0.01, 0.1, 1 or 10 mW (milliwatts)
  //    $sleeppwrget      Return current value  
  else if (!strncasecmp("sleeppwrset",incoming_command_string,11))
  {
    // Write value if valid
    inp_double = strtod(incoming_command_string+11,&pEnd);
    if ((inp_double==0)||(inp_double==0.001)||(inp_double==0.01)||(inp_double==0.1)||(inp_double==1)||(inp_double==10))
    {
      EEPROM_readAnything(1,R);
      R.idle_disp_thresh = (float) inp_double;
      EEPROM_writeAnything(1,R);
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  else if (!strncasecmp("call",incoming_command_string,9))// Call name
  {
    EEPROM_readAnything(1,R);
    // Copy up to 20 characters of the received string
    strncpy(R.idle_disp, incoming_command_string+9,20);
    // and store in EEPROM
    EEPROM_writeAnything(1,R);
  }


  
  else if (!strcasecmp("sleeppwrget",incoming_command_string))
  {
    Serial.print(F("IdleDisplayThreshold (mW): "));
    Serial.println(R.idle_disp_thresh,3);
  }
 

  //    $alarmset x       x = 1.5 to 3.9. 4 will inactivate SWR Alarm function
  //    $alarmget         Return current value
  else if (!strncasecmp("alarmset",incoming_command_string,8))
  {
    // Write value if valid
    inp_double = strtod(incoming_command_string+8,&pEnd);
    inp_val = inp_double * 10;
    if ((inp_val>14) && (inp_val<=40))
    {
      EEPROM_readAnything(1,R);
      R.SWR_alarm_trig = inp_val;
      EEPROM_writeAnything(1,R);
    }
  }
  else if (!strcasecmp("alarmget",incoming_command_string))
  {
    Serial.print(F("SWR_Alarm_Trigger: "));
    Serial.println(R.SWR_alarm_trig/10.0,1);
  }
  else if (!strcasecmp("alarmreset",incoming_command_string))
  {
    digitalWrite(R_Led,false);                                // Clear SWR Alarm LED
    
    analogWrite(14, 0);
    tft.fillRect(300, 0, 162, 80, 0x96FB);
    flag_excl_swr = true;
    flag_swr_bg = false;
    VirtLCDAlarm.transfer();
    VirtLCDswr_G.clear();
    VirtLCDswr_G.transfer();
    digitalWrite(9, LOW); // PTT ouput
    swr = 1.001; // rounding to 1.0
    flag.swr_alarm = false;
    flag_oneshot_swr = false;
    
  }
   // set coupler 1
   else if (!strcasecmp("cpl1",incoming_command_string))
  {
    state =false;
     digitalWrite(coupler_sw, 1);
    if (flag.screensaver_on == false){fpbuttons();
    }
  }
   // set coupler 2
   else if (!strcasecmp("cpl2",incoming_command_string))
  {
    state =true;
     digitalWrite(coupler_sw, 0);
    if (flag.screensaver_on == false){ fpbuttons();
    }
  }
  //    $alarmpowerset x    x = 1, 10, 100, 1000 or 10000 mW (milliwatts)
  //    $alarmpowerget      Return current value
  else if (!strncasecmp("alarmpowerset",incoming_command_string,13))
  {
    // Write value if valid
    inp_val = strtol(incoming_command_string+13,&pEnd,10);
    if ((inp_val==1)||(inp_val==10)||(inp_val==100)||(inp_val==1000)||(inp_val==10000))
    {
      EEPROM_readAnything(1,R);
      R.SWR_alarm_pwr_thresh = inp_val;
      EEPROM_writeAnything(1,R);
    }
  }
  else if (!strcasecmp("alarmpowerget",incoming_command_string))
  {
    Serial.print(F("SWR_Alarm_Power_Threshold (mW): "));
    Serial.println(R.SWR_alarm_pwr_thresh);
  }

  //    $pepperiodget x    x = 1, 2.5 or 5 seconds.  PEP sampling period
  //    $pepperiodset      Return current value
  else if (!strncasecmp("pepperiodset",incoming_command_string,12))
  {
    // Write value if valid
    inp_double = strtod(incoming_command_string+12,&pEnd);
    inp_val = inp_double*(PEP_BUFFER/5.0);
    if ((inp_val==(PEP_BUFFER/5.0))||(inp_val==(PEP_BUFFER/2.0))||(inp_val==PEP_BUFFER))
    {
      EEPROM_readAnything(1,R);
      R.PEP_period = inp_val;
      EEPROM_writeAnything(1,R);
    }
  }
  else if (!strcasecmp("pepperiodget",incoming_command_string))
  {
    Serial.print(F("PEP_period (seconds): "));
    Serial.println(R.PEP_period/(PEP_BUFFER/5.0),1);
  }

  else if (!strcasecmp("version",incoming_command_string))    // Poll for one single Human Readable report
  {
    Serial.println(F("Teensy 4.0 based Power & SWR Meter, by PD0LEW"));   
    Serial.print(F("Version "));
    Serial.print(VERSION);
    Serial.print(F(" "));
    Serial.println(DATE);
    Serial.println();
  }

  else if (!strcasecmp("memorywipe",incoming_command_string)) // Full reset of Memory
  {
    Serial.println(F("$memorywipe"));
    // Force a full EEPROM update upon reboot by storing 0xff in the first address
    EEPROM.write(0,0xff);
    SOFT_RESET();
  }
  else if (!strcasecmp("softreset",incoming_command_string))  // Reset Microcontroller
  {
    Serial.println(F("$softreset"));                          // Probably not very useful
    SOFT_RESET();
  }
  
  else if (!strcasecmp("help",incoming_command_string))       // Print out USB command help
  {
    usb_print_help();
  }
} 

//-----------------------------------------------------------------------------------------
//      Monitor USB Serial port for an incoming USB command
//-----------------------------------------------------------------------------------------

void usb_read_serial(void)
{
  static uint8_t a;                   // Indicates number of chars received in an incoming command
  static uint8_t Incoming;            // BOOL, true or false
  uint8_t ReceivedChar;
  uint8_t waiting;                    // Number of chars waiting in receive buffer

  waiting = Serial.available();       // Find out how many characters are waiting to be read. 

  if (waiting && !Incoming)           // Scan for a command attention symbol -> '$'
  {
    ReceivedChar = Serial.read();
    // A valid incoming message starts with an "attention" symbol = '$'.
    // in other words, any random garbage received on USB port is ignored.
    if (ReceivedChar == '$')          // Start command symbol was received,
    {                                 // we can begin processing input command
      Incoming = true;
      a = 0;
      waiting--;
    }
  }

  // Input command is on its way.  One or more characters are waiting to be read
  // and Incoming flag has been set. Read any available bytes from the USB OUT endpoint
  while (waiting && Incoming)
  {
    ReceivedChar = Serial.read();
    waiting--;

    if (a == sizeof(incoming_command_string)-1)  // Line is too long, discard input
    {
      Incoming = false;
      a = 0;
    }
    // Check for End of line
    else if ((ReceivedChar=='\r') || (ReceivedChar=='\n') || (ReceivedChar==';'))
    {
      incoming_command_string[a] = 0; // Terminate line
      usb_parse_incoming();           // Parse the command
      Incoming = false;
      a = 0;
    }
    else                              // Receive message, char by char
    {
      incoming_command_string[a] = ReceivedChar;
    }
    a++;                              // String length count++
  }
}

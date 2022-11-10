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
//** Version T_2.06  ILI9488 and FT6206 touchscreen, sample timer 50us max power 9kW
//** Keep wires to display as short as posible
//*********************************************************************************

/**********************************************************************************

    Teensy T4.0 Pinout with ILI9488 SPI tft_off display

                               pin numbers     I/O       pin numbers
                                       \     /       \      /
                                        -------------------
                                       1| GND        Vin   |28  +5VDC
                                  RX1  2| 0         Agnd   |27
                                  TX1  3| 1         3.3V   |26
                                       4| 2         23/A9  |25
                                       5| 3         22/A8  |24
                                       6| 4         21/A7  |23
                                       7| 5         20/A6  |22
                                       8| 6         19/A5  |21  SCL0  AD7991
                            PTT        9| 7         18/A4  |20  SDA0  AD7991
                            tft_DC    10| 8         17/A3  |19  SDA1  FT6206 touchscreen change Wire to Wire1
                                      11| 9         16/A2  |18  SCL1  FT6206 touchscreen change Wire to Wire1
                            tft_CS    12| 10        15/A1  |17
                            tft_MOSI  13| 11        14/A0  |16  Buzzer
                            tft_MISO  14| 12        13/LED |15  tft_SCLK
                                        --------------------

   ILI9488 SPI tft connections,back side left upcorner
    -------------------------------------------------------|
                                                           |
          GND   1 o    o 2   VCC                           |
                  o    o                                   |
                  o    o                                   |
                  o    o                                   |
                  o    o                                   |
                  o    o        Connect pin 29 to +5V      |
                  o    o        this is for the backlite   |
                  o    o                                   |
                  o    o                                   |
                  o    o                                   |
                  o    o                                   |
          CS   23 o    o 24 SCK                            |
          DC   25 o    o 26 nc                             |
          MOSI 27 o    o 28 MISO                           |
      Backlite 29 o    o 30 SCL0 (I2C)                     |
         SDA0  31 o    o                                   |
         (I2C)    o    o                                   |
                  o    o                                   |
                  o    o                                   |
               39 o    o 40                                |
  ---------------------------------------------------------|
 ****************************************************/
// Set SPI clock in ILI9488_t3.h
// #define ILI9488_SPICLOCK 72000000
// #define ILI9488_SPICLOCK_READ 4000000

/* This must be added in WireIMXRT.cpp to get the I2C clock on 2 MHz. J.G. Holstein @ 09-04-2021

  } else if (frequency < 1000000) {
    // 400 kHz
    port->MCCR0 = LPI2C_MCCR0_CLKHI(26) | LPI2C_MCCR0_CLKLO(28) |
      LPI2C_MCCR0_DATAVD(12) | LPI2C_MCCR0_SETHOLD(18);
    port->MCFGR1 = LPI2C_MCFGR1_PRESCALE(0);
    port->MCFGR2 = LPI2C_MCFGR2_FILTSDA(2) | LPI2C_MCFGR2_FILTSCL(2) |
      LPI2C_MCFGR2_BUSIDLE(3600); // idle timeout 150 us
    port->MCFGR3 = LPI2C_MCFGR3_PINLOW(CLOCK_STRETCH_TIMEOUT * 24 / 256 + 1);
  } else {
    // 2 MHz
                port->MCCR0 = LPI2C_MCCR0_CLKHI(3) | LPI2C_MCCR0_CLKLO(4) |
                LPI2C_MCCR0_DATAVD(2) | LPI2C_MCCR0_SETHOLD(3);
                port->MCFGR1 = LPI2C_MCFGR1_PRESCALE(0);
                port->MCFGR2 = LPI2C_MCFGR2_FILTSDA(1) | LPI2C_MCFGR2_FILTSCL(1) |
                LPI2C_MCFGR2_BUSIDLE(1200); // idle timeout 50 us
                port->MCFGR3 = LPI2C_MCFGR3_PINLOW(CLOCK_STRETCH_TIMEOUT * 24 / 256 + 1);
  }
*/

#include <SPI.h>
#include <ILI9488_t3.h>
bool display_attached = true;
#include <Adafruit_FT6206.h>
//#include "T4_PowerButton.h"

#include "PSWR_T.h"
uint16_t TouchY;
uint16_t TouchX;
bool     TouchZ;

int EncI;
int EncQ;
int EnactSW;

#define I2C Wire
Adafruit_FT6206 ctp = Adafruit_FT6206();

int flag_debug = 0;
int flag_display_functions = 0;
int flag_reset = 1;
String  p_range_indc ;
int range_indicator;
long time = 0;
unsigned long debounce = 100;
int reading;
int previous = 0;
int state = 0;
int flag_loopctr  = 0;
bool draw_new = true;
bool flag_cal  = false ;
bool flag_one_shot;
bool flag_config = false;
bool flag_excl_coupler = false;
bool flag_excl_loop = false;
bool flag_excl_config = false;
bool flag_loopcontroller = false;
bool flag_coupler = false;
bool refresh ;
bool flag_oneshot_swr = false;
bool flag_swr_bg = false;
int FirstBut = 0;
int LastBut = 3;
bool  flag_excl_swr;
int status_ind_cal =0;
int signal_quality = 0;
int tft_brightness = 110;
int button_bg = 1;
int progress_bar;

int pwr100ms; // no mWatt print in string
int refl_bar; // no mWatt print in string
int coupler_sw = 15;    // output 15 switch between coupler 1 and coupler 2
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
double      swr = 1.0;      // SWR as an absolute value
double      swr_avg = 1.0;  // SWR average over 10 ms (smoothed value)

uint16_t    menu_level = 0; // Used with PSWRmenu. Keep track of which menu we are in
char        lcd_buf[200];    // Used to process data to be passed to LCD and USB Serial
char        lcd_buf_cal[10];
char        lcd_buf_actual[10]; 
uint16_t    power_timer;    // Used to stay out of Screensaver
uint16_t    Menu_exit_timer;// Used for a timed display when returning from Menu
uint8_t     mode_display;   // Active display
flags       flag;           // Various op flags
touch_flags touch;          // Touchscreen flags
int current_selection_threshold;
int current_selection_floor;
bool        Reverse;        // BOOL: True if reverse power is greater than forward power
bool        modScopeActive; // BOOL: Feed stuff to the Modulation Scope
volatile bool X_LedState;   // BOOL: Debug LED

//-----------------------------------------------------------------------------------------
// Variables in ram/flash rom (default)
var_t  R  = {
  {
    { // Meter calibration if 2x AD8307
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
  (uint8_t) METER_CAL * 100, // Calibration fudge of diode detector style meter
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
  // e.g. 50us * 460 * 1 = 0,0375 seconds for a full sweep
  {
    0,                      // 1 for Upside down, else 0
    7                       // PWM value for tft `backlight. 10 max, 0 min
  }
};
//-----------------------------------------------------------------------------------------
// Instanciate an Encoder Object for legacy reasons, always available - whether using it or not
Encoder   Enc(EncI, EncQ);              // Only used if PUSHBUTTON_ENABLED

//-----------------------------------------------------------------------------------------
// Timers for various tasks:
Metro     pollMetro = Metro(POLL_TIMER);// Power/SWR calc and TFT LCD printout timer
Metro     pushMetro = Metro(1);         // 1 millisecond timer for Pushbutton management
Metro     slowMetro = Metro(100);       // 100 millisecond timer for various tasks
Metro     power_indicator = Metro(300);
Metro     power_dBm = Metro(300);
Chrono    exit_time = false ;
Chrono    acceleration_time;
Chrono    readbuttons_time;
uint16_t buttons_time;
Chrono    screensaver_time;                  // time between message shifting on screen
Chrono    screensaver_begin_time;            // time delay to start screensaver
uint16_t  screensaver_time_long   = 1500;    // change position of the screensaver message onscreen in this case 1.5 sec.(time in ms)
uint16_t  screensaver_time_start  = 1;       // wait time (min) before start screensaver 1 min.


//-----------------------------------------------------------------------------------------

#define TFT_RST 255
#define TFT_DC 8
#define TFT_CS 10
#define TC_INT 3
ILI9488_t3 tft = ILI9488_t3(&SPI, TFT_CS, TFT_DC, TFT_RST );

//-----------------------------------------------------------------------------------------
// Instanciate Power and SWR Meter gauges, virtual text lcd like handling for TFT LCD
// and a Modulation Scope widget
PowerMeter      Meter1;                 // A graphical Power Meter Gauge
PowerMeter      Meter2;                 // A second Power Meter Gauge, when needed
PowerMeter      Meter3;                 // A third Power Meter Gauge, when needed
PowerMeter      Meter4;                 // A fourth Power Meter Gauge, when needed
PowerMeter      Meter5;                 // A fifth Power Meter Gauge, when needed
VSWRmeter       SWR;                    // An SWR Meter Gauge
TextBox         VirtLCDw;               // Text LCD to TFT, white
TextBox         VirtLCDadG;             // AD7991 detected in GREEN
TextBox         VirtLCDadR;             // AD7991 detected in RED
TextBox         VirtLCDy;               // Text LCD to TFT, yellow
TextBox         VirtLCDlargeY;          // Text LCD to TFT, yellow
TextBox         VirtLCDlargeR;          // Text LCD to TFT, red
TextBox         VirtLCDpwr;             // Text LCD to TFT, Power indication label
TextBox         VirtLCDdBm;             //
TextBox         VirtLCDswr_G;           //
TextBox         VirtLCDswr_R;           //
TextBox         VirtLCDMode;            // Mode text in Power window
TextBox         VirtLCDAlarm;           // SWR ALARM text
TextBox         VirtLCDswr;             // SWR text
TextBox         VirtLCDcal;             //
TextBox         VirtLCDdebug;
TextBox         VirtLCDmessage;
ModulationScope ModScope;

//-----------------------------------------------------------------------------------------
// Forward & Reverse Voltage Measure and collect Function ( Interrupt driven )
//-----------------------------------------------------------------------------------------

void powerSampler(void)
{
#if INTR_LOOP_THRU_LED                    // Blink LED every time going through here 
  digitalWrite(X_Led, X_LedState ^= 1);   // Set the LED
#endif
  adc_poll_and_feed_circular();           // Read fwd and Rev AD, external or internal and feed circular buffer
#if INTR_LOOP_THRU_LED                    // Blink LED every time going through here 
  digitalWrite(X_Led, X_LedState ^= 1);   // Reset the LED
#endif
}

// frontpanel buttons main screen

void fpbuttons() {

  if ( flag.config_mode == false ) {

    tft.fillRoundRect(10, 275, 70, 40, 1,0x20C3);  // Setup button
    tft.drawRoundRect(10, 275, 70, 40, 1, DKGREY );
    tft.setFont(DroidSansMono_12);
    tft.setTextColor(LTGREY, 0x20C3);
    tft.setCursor(20, 289);
    tft.print("SETUP");
    tft.fillRect(10, 0, 285, 80, LTBLUE); // power
  }
}

//-----------------------------------------------------------------------------------------
// Top level task
// runs in an endless loop
//-----------------------------------------------------------------------------------------

void loop() {

  if (flag.swr_alarm == true && flag_oneshot_swr == false && !flag.config_mode ) {
    swr_alarm();
    flag_oneshot_swr = true;
  }

  if ((power_mw < R.idle_disp_thresh || flag.mode_display) && (flag_oneshot_swr == false)){
    swr =1.001;
   }

  

#if FAST_LOOP_THRU_LED                    // Blink a LED every time
  digitalWrite(X_Led, X_LedState ^= 1);     // Blink a led
#endif

  // Retrieve a point
  
  TS_Point p = ctp.getPoint();
 
  TouchX = p.y;
  TouchY = p.x;
  TouchZ = p.z;

  TouchX = map( TouchX, 0, 480, 480, 0);

  if (readbuttons_time.hasPassed(buttons_time) && flag.config_mode == true && menu_level != 1000 && menu_level != 1001) {
    ReadButtons(draw_new);
    readbuttons_time.restart();  //when Exit Config Menu
    if (draw_new)
    draw_new = false;
  }

  if ( TouchZ == true && flag.swr_alarm == true && flag.config_mode == false && TouchX > 300 && TouchY < 80) {

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


#if PUSHBUTTON_ENABLED
  static uint8_t  multi_button;             // State of Multipurpose Enact Switch
  // (Multipurpose pushbutton)
  static uint8_t  old_multi_button;         // Last state of Multipurpose Enact Switch
#endif

  //-------------------------------------------------------------------------------
  // Here we do routines which are to be run through as often as possible
  //-------------------------------------------------------------------------------

  pswr_sync_from_interrupt();                 // Read and process circular buffers containing adc input,
  // calculate forward and reverse power, pep, pk and avg

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

    if ((power_mw > R.idle_disp_thresh) || (flag.mode_display))

    {

      flag.power_detected = true; screensaver_begin_time.start();
      
    }
    if (screensaver_begin_time.hasPassed(screensaver_time_start * 60000) && flag.swr_alarm == false) {
      flag.power_detected = false;
     }


#if TOUCHSCREEN_ENABLED
    //-------------------------------------------------------------------------------
    // Touch Screen Management
    //
    // Exclude mode switching for frontpanel buttons
    if ( !flag.config_mode ) {

      if (flag.swr_alarm == true) {
        VirtLCDAlarm.transfer();
        VirtLCDswr.clear();
        VirtLCDswr.transfer();
        VirtLCDAlarm.setCursor(0, 0);
        VirtLCDAlarm.print("SWR Alarm");
      }

      if ( !flag_excl_config && !flag_excl_swr) {
        manage_Touchscreen();
      }
      if (flag.swr_alarm == false) {

        VirtLCDAlarm.clear();
        VirtLCDAlarm.transfer();
       
        VirtLCDswr.transfer();
        VirtLCDswr.setCursor(0, 0);
        VirtLCDswr.print("SWR");
      }
      flag_excl_config = TouchX > 5  && TouchX < 95  && TouchY > 270   && TouchY < 320 ;// Exclude mode switching SETUP menu button
      flag_excl_swr  = TouchX > 300  && TouchX < 480  && TouchY > 0   &&  TouchY < 80;  // Exclude mode switching SWR reset button

      if (flag_excl_config == true ) {
      
        refresh = true;  //repaint Config menu buttons
        VirtLCDswr.clear();
        VirtLCDswr.transfer();
        VirtLCDpwr.clear();
        VirtLCDpwr.transfer();
        VirtLCDlargeY.clear();
        VirtLCDlargeR.clear();
        VirtLCDlargeY.transfer();
        VirtLCDlargeR.transfer();
        tft.fillScreen(ILI9488_BLACK);
        flag.config_mode = true;
         
      }
    }

    if (!flag_excl_config && !flag_excl_swr && TouchZ) {

      if (!flag.config_mode  )
      {

        if (touch.pushed )
        {
          mode_display++;
          if (mode_display > MAX_MODE) mode_display = 1;
          flag.mode_newdefault = true;       // Indicate that patience timer for a potential new
          flag.mode_change = true;           // Force Mode Intro Display whenever Mode has been changed
          flag.mode_display = true;
        }
        touch.pushed = false;
      }
    }
    //-------------------------------------------------------------------------------
    // Configuration Menu Management
    else
    {
      if (touch.up )
      {
        Enc.write(-ENC_RESDIVIDE);          // Emulate encoder counterclockwise
        touch.up = false;
      }
      if (touch.up )
      {
        Enc.write(+ENC_RESDIVIDE);          // Arrow keys
        touch.up = false;
      }
      if (touch.down )
      {
        Enc.write(+ENC_RESDIVIDE);          // Emulate encoder counterclockwise
        touch.down = false;
      }
      if (touch.down )
      {
        Enc.write(-ENC_RESDIVIDE);          // Arrow keys
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

        eraseDisplay();

        if (flag.swr_alarm == false && flag.screensaver_on == false) {
          VirtLCDswr_G.clear();
          VirtLCDswr_G.transfer();
          tft.fillRect(300, 0, 162, 80, 0x96FB);
          tft.fillRect(10, 0, 285, 80, LTBLUE);
        }
        if (flag.swr_alarm == true) {
          VirtLCDswr_R.clear();
          VirtLCDswr_R.transfer();
          tft.fillRect(300, 0, 162, 80, RED);
          tft.fillRect(10, 0, 285, 80, LTBLUE);
        }

           Menu_exit_timer = 0;            // Show on LCD for 1 second
           flag.config_mode = false;        // We're done with Menu, EXIT
           flag.mode_change = true;         // Signal for display cleanup
           flag.menu_lcd_upd = false;       // Make ready for next time
        
        if (menu_level == 1001)             // Clean up from Debug2 Menu
          lcd_display_debug_erase();
          menu_level = 0;                   // We're done with all Menu levels
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
        tft.fillScreen(ILI9488_BLACK);
        
      }
    }

    //-------------------------------------------------------------------
    // Various Menu (rotary encoder) selectable display/function modes


    if (Menu_exit_timer == 0)
    {
      if (flag.config_mode)                     // Pushbutton Configuration Menu
      {
        ConfigMenu();
      }
      else if (mode_display == POWER_BARPK)     // 100ms Peak Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("100ms Peak Power", "", power_mw_pk); flag_debug = 1;
      }
      else if (mode_display == POWER_BARAVG)    // 1s Average Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("100ms Average Power", "", power_mw_avg); flag_debug = 2;
      }
      else if (mode_display == POWER_BARAVG1S)  // 1s Average Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("1s Average Power", "", power_mw_1savg);  flag_debug = 3;
      }
      else if (mode_display == POWER_BARINST)   // Instantaneous Power, Bargraph, PWR, SWR, PEP
      {
        lcd_display_clean("Instantaneous Power", "    ", power_mw); flag_debug = 4;
      }
      else if (mode_display == POWER_CLEAN_DBM) // Power Meter in dBm
      {
        lcd_display_clean_dBm(); flag_debug = 5;
      }
      else if (mode_display == POWER_MIXED)     // Fwd, Reflected and SWR
      {
        lcd_display_mixed(); flag_debug = 7;
      }
      else if (mode_display == MODULATIONSCOPE) // Modulation Scope
      {
        lcd_display_modscope(); flag_debug = 8;
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
      VirtLCDmessage.transfer();
      VirtLCDpwr.transfer();
      VirtLCDdBm.transfer();
      VirtLCDMode.transfer();
      VirtLCDAlarm.transfer();
      VirtLCDcal.transfer();
      VirtLCDdebug.transfer();
      VirtLCDswr.transfer();
    }
    else
    {
      VirtLCDlargeY.transfer();
      VirtLCDlargeR.transfer();
      VirtLCDswr_R.transfer();
      VirtLCDswr_G.transfer();

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

    // Do stuff when not in Config Menu
    if (!flag.config_mode)
    {

      if (Enc.read() / ENC_RESDIVIDE != 0)  // Encoder turned to cycle through modes
      {
        flag.mode_change = true;            // Force Mode Intro Display whenever Mode has been changed
        flag.mode_display = true;
        flag.mode_newdefault = true;        // Indicate that patience timer for a potential new
        // default mode to be saved into EEPROM is running
        // Mode switching travels only one click at a time, ignoring extra clicks
        if (Enc.read() / ENC_RESDIVIDE > 0)
        {
          mode_display++;
          if (mode_display > MAX_MODE) mode_display = 1;
          Enc.write(0);                     // Reset data from Encoder
        }
        else if (Enc.read() / ENC_RESDIVIDE < 0)
        {
          mode_display--;
          if (mode_display == 0) mode_display = MAX_MODE;
          Enc.write(0);                     // Reset data from Encoder
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
          EEPROM_writeAnything(1, R);
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------------------
//      Setup Ports, timers, start the works and never return, unless reset
//                          by the watchdog timer
//                   then - do everything, all over again
//-----------------------------------------------------------------------------------------
void myCallback(void) {
  digitalWriteFast( 2, 1);
  Serial.println ("Callback called - Switching off now.");
  delay(100);
}
void setup()
{
  //set_arm_power_button_callback(&myCallback); //Start callback
  uint8_t coldstart;
  // Enable LEDs and set to LOW = Off
  pinMode(R_Led, OUTPUT);                   // SWR Alarm
  digitalWrite(R_Led, LOW);
  digitalWrite(coupler_sw, 1);
  


  coldstart = EEPROM.read(0);               // Grab the coldstart byte indicator in EEPROM for
  // comparison with the COLDSTART_REFERENCE
  // Initialize all memories if first upload or if COLDSTART_REF has been modified
  // either through PSWR_A.h or through Menu functions
  if (coldstart != COLDSTART_REF)
  {
    EEPROM.write(0, COLDSTART_REF);         // COLDSTART_REF in first byte indicates all initialized
    EEPROM_writeAnything(1, R);             // Write default settings into EEPROM
  }
  else                                      // EEPROM contains stored data, retrieve the data
  {
    EEPROM_readAnything(1, R);              // Read the stored data
  }
  tft.begin();
  //digitalWrite(2, HIGH); // backlite 255 = 100% 
  
  tft.setRotation(3);
  tft.fillScreen(ILI9488_BLACK);
  tft.writeRect(0, 0, 480, 320, (uint16_t*) earth480x320); // Draw a pretty picture on screen

  tft.setTextColor(YELLOW);
  tft.setFont(DroidSansMono_18);
  tft.setCursor(100, 270);
  tft.print("Power and SWR Meter");
  tft.setCursor(188, 295);
  tft.setFont(DroidSansMono_16);
  tft.print("LJ-2000M"); // your call here
  tft.setTextColor(YELLOW);
  tft.setFont(DroidSansMono_12);
  tft.setTextColor(WHITE);
  tft.setCursor(340, 10);
  tft.print("Version: "); tft.print(VERSION);
  tft.setCursor(340, 30);
  tft.print("Date: "); tft.print(DATE);
  tft.setCursor(340, 50);
  tft.print("J.G. Holstein"); 
  Serial.begin(115200);
  delay(2000);
  for (int progress_bar = 0; progress_bar <= 480; progress_bar++) {
    delay (2);
    tft.fillRoundRect(0, 315, progress_bar, 5, 1, RED);  // Setup button
  }

  ctp.begin(254);

  // Start I2C on port SDA0/SCL0 (pins 18/19) - 2000 kHz
  I2C.begin();
  I2C.setClock(2000000);

  uint8_t i2c_status = I2C_Init();          // Initialize I2C comms


  VirtLCDw.init(50, 12, 125, 1, DroidSansMono_14, WHITE, BLACK);        //  WHITE text
  VirtLCDdebug.init(23, 12, 105, 60, DroidSansMono_16, WHITE, BLACK);   //  DEBUG window in WHITE value
  VirtLCDy.init(30, 12, 100, 1, DroidSansMono_14, YELLOW, BLACK);       //  YELLOW value
  VirtLCDpwr.init( 30, 15, 1, 200, DroidSansMono_24, WHITE, BLACK);     //  power label in WHITE value
  VirtLCDdBm.init( 9, 1, 10, 11, DroidSansMono_40, BLUE, LTBLUE);       //  dBm in BLUE value
  VirtLCDswr_G.init(4, 1, 310, 11, DroidSansMono_40, 0x1B86, 0x96FB);   //  swr value GREEN value
  VirtLCDswr_R.init(4, 1, 310, 11, DroidSansMono_40, WHITE, RED);       //  swr value WHITE value
  VirtLCDMode.init(70, 1, 13, 63, LiberationMono_14, BLACK, LTBLUE);    //  mode label Power BLACK

  VirtLCDadG.init(20, 1, 120, 150, DroidSansMono_18, GREEN, BLACK);     // AD7991 Detected in GREEN
  VirtLCDadR.init(20, 1, 120, 150, DroidSansMono_18, RED, BLACK);       // No AD7991 Detected in RED
  //-------------------------------------------------------------------------------------------------------------------//
  VirtLCDAlarm.init(70, 1, 305, 63, LiberationMono_14, YELLOW, 0x96FB); //  Alarm label Alarm YELLOW
  VirtLCDswr.init(70, 1, 305, 63, LiberationMono_14, BLACK, 0x96FB);    //  Alarm label Alarm YELLOW
  //-------------------------------------------------------------------------------------------------------------------//
  VirtLCDmessage.init(50, 20, 25, 1, DroidSansMono_18, YELLOW, BLACK); // Screensaver message 
  
  VirtLCDcal.init(20, 1, 340, 1, DroidSansMono_14, WHITE, BLACK);       //  Calibration
  VirtLCDlargeY.init ( 6, 1, 55, 11, DroidSansMono_40, BLUE, LTBLUE);   // Large power BLUE value
  VirtLCDlargeR.init ( 6, 1, 55, 11, DroidSansMono_40, BLUE, LTBLUE);   // Large power BLUE value
  ModScope.init(10, 86, 451, 135, WHITE, GREEN, YELLOW);
  ModScope.rate(R.modscopeDivisor);                                     // Modulation scope rate divisor
  VirtLCDadG.setCursor(0, 0);                                           // Center justify in line
  VirtLCDadR.setCursor(0, 0);                                           // Center justify in line
 

  
  tft.fillScreen(ILI9488_BLACK);

#if Wire_ENABLED                          // I2C scan report

  if      (i2c_status == 1)   VirtLCDadG.print(" AD7991 Detected");     // print in GREEN
  else if (i2c_status == 2)   VirtLCDadG.print("AD7991-1 Detected");    // print in GREEN
  else                        VirtLCDadR.print("No AD7991 Detected");   // print in RED
  VirtLCDadG.transfer();
  VirtLCDadR.transfer();

  delay(1500);

  tft.fillScreen(ILI9488_BLACK);
  tft.fillRect(300, 0, 162, 80, 0x96FB);
#endif

  mode_display = R.mode_display;            // Active Display Mode
  flag.mode_change = true;                  // Force a Display of Mode Intro when starting up
  flag.mode_display = true;
  VirtLCDdebug.clear();
  VirtLCDw.clear();                         // Prep for going live
  VirtLCDy.clear();
  VirtLCDmessage.clear();
  VirtLCDswr_G.clear();
  VirtLCDswr_R.clear();
  VirtLCDpwr.clear();
  VirtLCDdBm.clear();
  VirtLCDMode.clear();
  VirtLCDswr.clear();
  VirtLCDAlarm.clear();
  VirtLCDcal.clear();
  VirtLCDlargeY.clear();
  VirtLCDlargeR.clear();
  

  Timer1.initialize(SAMPLE_TIMER);          // Init the adc sample timer interrupt function
  Timer1.attachInterrupt(powerSampler);     // Start the works
}

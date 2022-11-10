//*********************************************************************************
//**
//** General purpose buttons made by J.G. Holstein @ 2017
//**
//**
//** Copyright (c) 2021  J.G. Holstein
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
//**
//**Version T_2.06  ILI9488 and FT6206 touchscreen, sample timer 50us max power 9kW
//*********************************************************************************

//General purpose buttons
//

//*********************************************Buttons**********************************************
struct NewBut {
  short x;
  short y;
  short w;
  short h;
  short xoff;
  short font;
  String offtext;
  int offtxtcol;
  int offbutcol;
  String ontext;
  int ontxtcol;
  int onbutcol;
  short onoff;
};
void ReadButtons(bool redraw)
{
    if (refresh == true) {
      TouchZ=false; 
      refresh = false;
    }
   // Last button number used
  static short ButAction[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static short Button[11]    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static String ButText[1];

  // See http://fontawesome.io/icons/ for the Symbol button code
  // x-position, y-position, width, height, x-offset, font_Awesome part (0xfff if it is a text button), offtext, offtxtcol, offbutcol, ontext, ontxtcol, onbutcol, onoff

  static NewBut But_Param[12] = {
    /* Menu Config buttons */
    /* Button 0*/{10, 200, 70, 70, 2,  0xf00, char(0x10), LTBLUE, DKGREEN, char(0x10), BLUE, LTBLUE, 0}, // Accept button
    /* Button 1*/{10, 10, 70, 70, 2,   0x100, char(0x2A), LTBLUE, BLUE,    char(0x2A), BLUE, LTBLUE, 0}, // Arrow up button
    /* Button 2*/{10, 105, 70, 70, 2,  0x100, char(0x2B), LTBLUE, BLUE,    char(0x2B), BLUE, LTBLUE, 0}, // Arrow down button
    /* Button 3*/{400, 200, 70, 70, 2, 0xfff, "EXIT",     BLACK, ORANGE,  "EXIT", BLUE, LTBLUE, 0},      // Exit menu

    ///********************************NOT USED **********************************************//
    /* Spare buttons*/
    /* Button 4*/ {400, 10,  70, 50, 1, 0xf01, char(0x5a),    LTBLUE, BLACK, char(0x10), BLACK, LTBLUE, 0},     // 
    /* Button 5*/ {90, 260, 70, 50, 1, 0x1,  "5",    LTBLUE, BLUE, "5", BLUE, LTBLUE, 0},     // 
    /* Button 6*/ {170,260, 70, 50, 1, 0x1,  "6",    LTBLUE, BLUE, "6", BLUE, LTBLUE, 0},     // 
    /* Button 7*/ {250, 260, 70, 50, 1, 0x1, "7",    LTBLUE, BLUE, "7", BLUE, LTBLUE, 1},     // 
    /* Button 8*/ {330, 260, 70, 50, 1, 0x1, "8",    LTBLUE, BLUE, "8", BLUE, LTBLUE, 0},     // 
    /* Button 9*/ {400, 268, 70, 40, 1, 0x1, "9",    LTBLUE, BLUE, "9", BLUE, LTBLUE, 0},     // 
    /* Button 10*/{655, 370, 125, 80, 2, 0x1,"10",   LTBLUE, BLUE, "10", BLUE, LTBLUE, 0},    //         
    /* Button 11*/{670, 370, 125, 80, 2, 0x1,"11",   LTBLUE, BLUE, "11", BLUE, LTBLUE, 0},    //              

  };
  //###### buttons  ######
  for (int i = FirstBut; i <= LastBut; i++) {  // choose between Config Menu buttons or spare buttons
     if ((Button[i] == 0 || redraw))
    {
      if ( TouchZ == true && flag_cal == false)
      {
        break;
      }
      if (flag_cal)
      {
        accelaration();
        Button[i] = 1;
      }
      if (!flag_cal)
      {
        buttons_time = 100;
        Button[i] = 1;
      }
     if (  TouchZ == false)
      {
        if (ButAction[i] == 0) {
          TXTtestButton(But_Param[i].x, But_Param[i].y, But_Param[i].w, But_Param[i].h, But_Param[i].xoff, But_Param[i].font, But_Param[i].offtext, But_Param[i].onoff, But_Param[i].offtxtcol, But_Param[i].offbutcol);
        }
        Button[i] = 1;
      }
    }
    if (Button[i] == 1 && TouchX > But_Param[i].x && TouchX < But_Param[i].x + But_Param[i].w && TouchY > But_Param[i].y && TouchY < But_Param[i].y + But_Param[i].h && TouchZ == true) {
      TXTtestButton(But_Param[i].x, But_Param[i].y, But_Param[i].w, But_Param[i].h, But_Param[i].xoff, But_Param[i].font, But_Param[i].ontext, But_Param[i].onoff, But_Param[i].ontxtcol, But_Param[i].onbutcol);
      Button[i] = 0;
      switch (i) {
        case 0:     // accept button
          if (ButAction[i] == 0  ) {
             touch.enact = true;
             
            Button[i] = 0;
           }
          break;
        case 1:  // arrow up button
          if (ButAction[i] == 0 ) {
            touch.up = true;
            Button[i] = 0;
          }
          break;
        case 2:  // arrow down button
          if (ButAction[i] == 0) {
            touch.down = true;
            Button[i] = 0;
          }
          break;
        case 3:  // exit menu button Config
          if  (ButAction[i] == 0) {
              //eraseDisplay();
              tft.fillScreen(ILI9488_BLACK);
              
              touch.exit = true;
              flag.mode_display = true;
              VirtLCDw.clear();
              VirtLCDy.clear();
              VirtLCDw.transfer();
              VirtLCDy.transfer();
              
              Button[i] = 0;
          }
          break;
        
        //**************************************Spare**************************************//

        case 4:  // Button 1
          if  (ButAction[i] == 0 ) {
             
            Serial.println("Arrow Up");
            }else{
             Button[i] = 0;
          }
          break;
        case 5:  //
          if  (ButAction[i] == 0) {
            Serial.println("Arrow Down");
            Button[i] = 0;
          }
          break;
        case 6:  //
          if  (ButAction[i] == 0) {
            Button[i] = 0;
          }
          break;
        case 7:  //                      // Auto Tune  ON/OFF
          if  (ButAction[i] == 0) {
             Serial.println( "ON");
            }else{ 
             Serial.println( "OFF");   

            Button[i] = 0;
          }
          break;

        case 8:  // exit menu button
          if  (ButAction[i] == 0) {

            Serial.println("button3");
            Button[i] = 0;
          }
          break;
        case 9:  // Arrow Up
          if  (ButAction[i] == 0  ) {

            Serial.println("button4");
            Button[i] = 0;
          }
          break;
        case 10:  // Exit
          if  (ButAction[i] == 0  ) {

            eraseDisplay();
            fpbuttons();
            flag_loopctr = 0;
            Serial.println("Exit");
            Button[i] = 0;
          }
          break;
      }
      if (But_Param[i].onoff == 1) {
        ButAction[i] = 1 - ButAction[i];
      }
    }
  }
}
void accelaration() {
  if (TouchZ == false) {
    buttons_time = 200;
    acceleration_time.restart();
  }
  if (acceleration_time.hasPassed(4000) && TouchZ == true) {
    acceleration_time.restart();
    buttons_time = 1;
  }
}
//##################################################################################################################################################################
//######                                Subroutine to draw a graphic button on the screen (See http://fontawesome.io/icons/)                                  ######
//##################################################################################################################################################################
void TXTtestButton(short x, short y, short w, short h, short xoff, short font, String text, short onoff, int txtcol, int butcol)
{
  static short yoff;
  
  if (font == 0xf00) {
    tft.setFont(AwesomeF080_28);
    yoff = (h - 14) / 2 - 13;
    xoff = (w - text.length()) / 3.3;
  }
  if (font == 0x100) {
    tft.setFont(AwesomeF080_28);
    yoff = (h - 14) / 2 - 13;                   
    xoff = (w - text.length()) / 3.3;
  }
  if (font == 0x2) {
    yoff = (h - 32) / 2 - 8;
    xoff = (w - text.length()) / 3.3;
  }
  if (font == 0xfff) {
    tft.setFont(DroidSansMono_12);
    xoff = (w - text.length() * 10) / 2;
    yoff = (h - 10) / 2.0;
  }
   if (font == 0xf01) {
    tft.setFont(AwesomeF000_28);
    yoff = (h - 14) / 2 - 12;
    xoff = (w - text.length()) / 3.3;
  }
   if (flag.config_mode == true ){
      tft.fillRoundRect(x, y, w, h, 5, butcol);
      tft.drawRoundRect(x, y, w, h, 5, WHITE);
      tft.setCursor(x + xoff, y + yoff);
      tft.setTextColor(txtcol);
      tft.print(text);
   }
   if (flag.config_mode == false){
      tft.fillRoundRect(x, y, w, h, 4, butcol);
      tft.drawRoundRect(x, y, w, h, 4, WHITE);
      tft.setCursor(x + xoff, y + yoff);
      tft.setTextColor(txtcol, butcol);
      tft.print(text);
   }
}

//**************************End of buttons***********************************************************-`

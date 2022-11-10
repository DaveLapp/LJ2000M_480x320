//*********************************************************************************
//**
//** Project.........: A menu driven Multi Display RF Power and SWR Meter
//**                   using a Tandem Match Coupler and 2x AD8307; or
//**                   diode detectors.
//**
//** Copyright (C) 2016  Loftur E. Jonasson  (tf3lj [at] arrl [dot] net)
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
//** Platform........: Teensy 4.0 (http://www.pjrc.com)
//**
//** Initial version.: 0.00, 2016-04-15  Loftur Jonasson, TF3LJ / VE2LJX / VE2AO
//**Version T_2.06  ILI9488 and FT6206 touchscreen, sample timer 50us max power 9kW
//*********************************************************************************

void manage_Touchscreen(void)
{
  static   bool          push;

  //-------------------------------------------------------------------
  // Manage Touch Screen when not in Configuration Menu

  if (!flag.config_mode)
  {

    if (TouchZ == false   )

    {
      if (push)
      {
        push = false;
        touch.pushed = true;
      }
      return;
    }

    if (!push)
    {
      push = true;
    }
       touch.upper = true;
    }
 
}
void eraseTouchButtons(void)       // A simple draw black box to erase
{

 
  tft.fillRoundRect(0, 0, 88,320, 1, BLACK);
  tft.fillRoundRect(400, 0, 75, 320, 1, BLACK);
  touch.onscreen = false;
}

/*  Gamepad.h
 *   
 *  Based on the advanced HID library for Arduino: 
 *  https://github.com/NicoHood/HID
 *  Copyright (c) 2014-2015 NicoHood
 * 
 *  Copyright (c) 2020 Mikael Norrgård <http://daemonbite.com>
 *
 *  GNU GENERAL PUBLIC LICENSE
 *  Version 3, 29 June 2007
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 */

//Modified to output as an absolute position mouse.
//Matheus Fraguas (sonik-br)

#pragma once

#include "HID.h"

//extern const char* gp_serial;

// The numbers after colon are bit fields, meaning how many bits the field uses.
// Remove those if there are problems
typedef struct {

  union
  {
    struct {
      uint8_t buttons;// : 3;
      uint16_t x;
      uint16_t y;
    };
  };
  
} MouseAbsReport;


class MouseAbsolute_ : public PluggableUSBModule
{  
  private:
    uint8_t reportId;
    const char* gp_serial = "RZordPsGun";

  protected:
    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    uint8_t getShortName(char *name);
    
    uint8_t epType[1];
    uint8_t protocol;
    uint8_t idle;
    
  public:
    MouseAbsReport _Report;
    MouseAbsolute_(void);
    void reset(void);
    void send();
    void setXAxis(const uint16_t value);
    void setYAxis(const uint16_t value);
    void setButton(const uint8_t button, uint8_t value);
    void pressButton(const uint8_t button);
    void releaseButton(const uint8_t button);
};

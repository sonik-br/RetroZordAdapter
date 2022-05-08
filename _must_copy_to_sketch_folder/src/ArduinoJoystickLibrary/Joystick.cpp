/*
  Joystick.cpp

  Copyright (c) 2015-2017, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * Modified by sonik-br
 * Just a base class. Report data must be implemented on derived class.
 * Can use a mix of usb endpoints and composite hid.
 * Added usb serial id. Based on code from Mikael Norrgård <http://daemonbite.com>.
*/

#include "Joystick.h"

uint8_t Joystick_::getNextIndex(const char* serial, const uint8_t totalControllers)
{
  // Select the endpoint to be used
  const uint8_t maxOnEachEndpoint = (totalControllers + (MAX_ENDPOINTS - 1)) / MAX_ENDPOINTS;
  uint8_t i = 0;
  for(i = 0; i < MAX_ENDPOINTS; i++) {

    //Fill the first one, then the next...
    if(_endpointInitialized[i] < maxOnEachEndpoint) {
      
        //Increment counter and create instance if needed
      if(_endpointInitialized[i]++ == 0)
        _endpointPool[i] = new DynamicHID_(serial);

      break;
    }
  }
  //returns the selected endpoint
  return i;
}
//static properties
bool Joystick_::_useComposite(false);
DynamicHID_* Joystick_::_endpointPool[MAX_ENDPOINTS];
uint8_t Joystick_::_endpointInitialized[MAX_ENDPOINTS];

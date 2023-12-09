/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#pragma once

#include <stdint.h>

#include "HIDDescriptors.h"

typedef struct __attribute((packed, aligned(1)))
{
	uint8_t id;
	uint16_t buttons;
	
	// digital direction, use the dir_* constants(enum)
	// 8 = center, 0 = up, 1 = up/right, 2 = right, 3 = right/down
	// 4 = down, 5 = down/left, 6 = left, 7 = left/up

	uint8_t direction;
} JogconMouse0Report;

typedef struct __attribute((packed, aligned(1)))
{
	uint8_t id;
	int8_t x;
	int8_t y;

} JogconMouse1Report;

static const uint8_t jogcon_mouse_report_descriptor[] PROGMEM =
{
  0x05, 0x01,                       // USAGE_PAGE (Generic Desktop)
  0x09, 0x05,                       // USAGE (Gamepad)
  0xa1, 0x01,                       // COLLECTION (Application)
	0x85, 0x01,                     // REPORT_ID (1)
    0xa1, 0x00,                     // COLLECTION (Physical)

      0x05, 0x09,                   // USAGE_PAGE (Button)
      0x19, 0x01,                   // USAGE_MINIMUM (Button 1)
      0x29, 0x0C,                   // USAGE_MAXIMUM (Button 12)
	  //0x29, 0x10,                   // USAGE_MAXIMUM (Button 16)
      0x15, 0x00,                   // LOGICAL_MINIMUM (0)
      0x25, 0x01,                   // LOGICAL_MAXIMUM (1)
      0x95, 0x10,                   // REPORT_COUNT (16)
      0x75, 0x01,                   // REPORT_SIZE (1)
      0x81, 0x02,                   // INPUT (Data,Var,Abs)
    
      0x05, 0x01,                   // USAGE_PAGE (Generic Desktop)
      0x09, 0x39,                   // Usage (Hat switch)
      0x15, 0x00,                   // Logical Minimum (0)
      0x25, 0x07,                   // Logical Maximum (7)
      0x35, 0x00,                   // Physical Minimum (0)
      0x46, 0x3B, 0x01,             // Physical Maximum (315)
      0x75, 0x08,                   // Report Size (8)
      0x95, 0x01,                   // Report Count (1)
      0x65, 0x14,                   // Unit (20)
      0x81, 0x42,                   // Input (variable,absolute,null_state)

    0xc0,                           // END_COLLECTION
  0xc0,                             // END_COLLECTION 


  0x05, 0x01,                       // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,                       // USAGE (mouse)
  0xa1, 0x01,                       // COLLECTION (Application)
  	0x85, 0x02,                     // REPORT_ID (2)
      0x09, 0x01,                   // USAGE (pointer)
      0xa1, 0x00,                   // COLLECTION (Physical) 
        0x09, 0x30,                 // USAGE (X)
        0x09, 0x31,                 // USAGE (Y)
		0x15, 0x81,                 // LOGICAL_MINIMUM (-127)
		0x25, 0x7F,                 // LOGICAL_MAXIMUM (127)
		0x35, 0x81,                 // PHYSICAL_MINIMUM (-127)
		0x45, 0x7F,                 // PHYSICAL_MINIMUM (127)
        0x95, 0x02,                 // REPORT_COUNT (2)
        0x75, 0x08,                 // REPORT_SIZE (8)
        0x81, 0x06,                 // INPUT (Data,Var,Rel)
      0xc0,                         // END_COLLECTION
  0xc0,                             // END_COLLECTION 
};

#define HID_JOGCON_MOUSE_INTERFACE(n) \
		/* interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12 */ \
	9,				               /* bLength */ \
	4,				               /* bDescriptorType */ \
	(GAMEPAD_INTERFACE+n),         /* bInterfaceNumber */ \
	0,				               /* bAlternateSetting */ \
	1,				               /* bNumEndpoints */ \
	0x03,			               /* bInterfaceClass (0x03 = HID) */ \
	0x00,			               /* bInterfaceSubClass (0x00 = No Boot) */ \
	0x00,			               /* bInterfaceProtocol (0x00 = No Protocol) */ \
	0,				               /* iInterface */ \
		/* HID interface descriptor, HID 1.11 spec, section 6.2.1 */ \
	9,							   /* bLength */ \
	0x21,						   /* bDescriptorType */ \
	0x11, 0x01,					   /* bcdHID */ \
	0,							   /* bCountryCode */ \
	1,							   /* bNumDescriptors */ \
	0x22,						   /* bDescriptorType */ \
	sizeof(jogcon_mouse_report_descriptor), /* wDescriptorLength */ \
	0,								\
		/* endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13 */ \
	7,						 	   /* bLength */ \
	5,						       /* bDescriptorType */ \
	(GAMEPAD_ENDPOINT+n) | 0x80,   /* bEndpointAddress */ \
	0x03,					       /* bmAttributes (0x03=intr) */ \
	GAMEPAD_SIZE, 0,		       /* wMaxPacketSize */ \
	1,						       /* bInterval (1 ms) */


//#define CONFIG1_DESC_SIZE		(9+9+9+7)
// static uint8_t hid_jogcon_configuration_descriptor[] =
// {
// 	    // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
// 	9,						       // bLength;
// 	2,						       // bDescriptorType;
// 	LSB(CONFIG1_DESC_SIZE),        // wTotalLength
// 	MSB(CONFIG1_DESC_SIZE),
// 	1,	                           // bNumInterfaces
// 	1,	                           // bConfigurationValue
// 	0,	                           // iConfiguration
// 	0x80,                          // bmAttributes
// 	50,	                           // bMaxPower

// 	HID_JOGCON_INTERFACE(0)
// 	HID_JOGCON_INTERFACE(1)
// 	//HID_JOGCON_INTERFACE(2)
// 	//HID_JOGCON_INTERFACE(3)
// 	//HID_JOGCON_INTERFACE(4)

// };

static const uint8_t hid_jogcon_mouse_configuration_descriptor[] PROGMEM =
{
	    // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9,						       // bLength;
	2,						       // bDescriptorType;
	LSB(CONFIG1_DESC_SIZE),        // wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	1,	                           // bNumInterfaces
	1,	                           // bConfigurationValue
	0,	                           // iConfiguration
	0x80,                          // bmAttributes
	50,	                           // bMaxPower


		// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,				               // bLength
	4,				               // bDescriptorType
	GAMEPAD_INTERFACE,             // bInterfaceNumber
	0,				               // bAlternateSetting
	1,				               // bNumEndpoints
	0x03,			               // bInterfaceClass (0x03 = HID)
	0x00,			               // bInterfaceSubClass (0x00 = No Boot)
	0x00,			               // bInterfaceProtocol (0x00 = No Protocol)
	0,				               // iInterface
		// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,							   // bLength
	0x21,						   // bDescriptorType
	0x11, 0x01,					   // bcdHID
	0,							   // bCountryCode
	1,							   // bNumDescriptors
	0x22,						   // bDescriptorType
	sizeof(jogcon_mouse_report_descriptor), // wDescriptorLength
	0,
		// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,						 	   // bLength
	5,						       // bDescriptorType
	GAMEPAD_ENDPOINT | 0x80,       // bEndpointAddress
	0x03,					       // bmAttributes (0x03=intr)
	GAMEPAD_SIZE, 0,		       // wMaxPacketSize
	1						       // bInterval (1 ms)







	//HID_INTERFACE(0)
	//HID_INTERFACE(1)
	//HID_INTERFACE(2)
	//HID_INTERFACE(3)
	//HID_INTERFACE(4)

	/*
		// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,				               // bLength
	4,				               // bDescriptorType
	GAMEPAD_INTERFACE,             // bInterfaceNumber
	0,				               // bAlternateSetting
	1,				               // bNumEndpoints
	0x03,			               // bInterfaceClass (0x03 = HID)
	0x00,			               // bInterfaceSubClass (0x00 = No Boot)
	0x00,			               // bInterfaceProtocol (0x00 = No Protocol)
	0,				               // iInterface
		// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,							   // bLength
	0x21,						   // bDescriptorType
	0x11, 0x01,					   // bcdHID
	0,							   // bCountryCode
	1,							   // bNumDescriptors
	0x22,						   // bDescriptorType
	sizeof(hid_report_descriptor), // wDescriptorLength
	0,
		// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,						 	   // bLength
	5,						       // bDescriptorType
	GAMEPAD_ENDPOINT | 0x80,       // bEndpointAddress
	0x03,					       // bmAttributes (0x03=intr)
	GAMEPAD_SIZE, 0,		       // wMaxPacketSize
	1						       // bInterval (1 ms)
	*/
};
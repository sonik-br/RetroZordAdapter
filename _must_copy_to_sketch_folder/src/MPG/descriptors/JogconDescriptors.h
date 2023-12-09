/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#pragma once

#include <stdint.h>

#include "HIDDescriptors.h"

typedef struct __attribute((packed, aligned(1)))
{
	// uint8_t f_spn_l_btn : 1; //fake spinner left
	// uint8_t f_spn_r_btn : 1; //fake spinner right

	// uint8_t select_btn : 1;
	// uint8_t start_btn : 1;

	// uint8_t empty_btn : 4;

	// uint8_t l2_btn : 1;
	// uint8_t r2_btn : 1;
	// uint8_t l1_btn : 1;
	// uint8_t r1_btn : 1;

	// uint8_t triangle_btn : 1;
	// uint8_t circle_btn : 1;
	// uint8_t cross_btn : 1;
	// uint8_t square_btn : 1;

	uint16_t buttons;

	int8_t l_x_axis;
	int8_t l_y_axis;
	int8_t spinner_axis;
	int8_t paddle_axis;
	
	// digital direction, use the dir_* constants(enum)
	// 8 = center, 0 = up, 1 = up/right, 2 = right, 3 = right/down
	// 4 = down, 5 = down/left, 6 = left, 7 = left/up

	uint8_t direction;
} JogconReport;

static const uint8_t jogcon_report_descriptor[] PROGMEM =
{
  0x05, 0x01,                       // USAGE_PAGE (Generic Desktop)
  0x09, 0x04,                       // USAGE (Joystick) (Maybe change to gamepad? I don't think so but...)
  0xa1, 0x01,                       // COLLECTION (Application)
    0xa1, 0x00,                     // COLLECTION (Physical)
    
      0x05, 0x09,                   // USAGE_PAGE (Button)
      0x19, 0x01,                   // USAGE_MINIMUM (Button 1)
      //0x29, 0x0C,                   // USAGE_MAXIMUM (Button 12)
	  0x29, 0x10,                   // USAGE_MAXIMUM (Button 16)
      0x15, 0x00,                   // LOGICAL_MINIMUM (0)
      0x25, 0x01,                   // LOGICAL_MAXIMUM (1)
      0x95, 0x10,                   // REPORT_COUNT (16)
      0x75, 0x01,                   // REPORT_SIZE (1)
      0x81, 0x02,                   // INPUT (Data,Var,Abs)
    
      0x05, 0x01,                   // USAGE_PAGE (Generic Desktop)
      0x09, 0x01,                   // USAGE (pointer)
      0xa1, 0x00,                   // COLLECTION (Physical) 
        0x09, 0x30,                 // USAGE (X)
        0x09, 0x31,                 // USAGE (Y)
        0x15, 0x80,                 // LOGICAL_MINIMUM (-128)
        0x25, 0x7F,                 // LOGICAL_MAXIMUM (127)
        0x95, 0x02,                 // REPORT_COUNT (2)
        0x75, 0x08,                 // REPORT_SIZE (8)
        0x81, 0x02,                 // INPUT (Data,Var,Abs)
      0xc0,                         // END_COLLECTION

      0x09, 0x37,                   // USAGE (Dial)
      0x15, 0x80,                   // LOGICAL_MINIMUM (-128)
      0x25, 0x7F,                   // LOGICAL_MAXIMUM (127)
      0x95, 0x01,                   // REPORT_COUNT (1)
      0x75, 0x08,                   // REPORT_SIZE (8)
      0x81, 0x06,                   // INPUT (Data,Var,Rel)

      0x09, 0x38,                   // USAGE (Wheel)
      0x15, 0x00,                   // LOGICAL_MINIMUM (0)
      0x26, 0xFF, 0x00,             // LOGICAL_MAXIMUM (255)
      0x95, 0x01,                   // REPORT_COUNT (1)
      0x75, 0x08,                   // REPORT_SIZE (8)
      0x81, 0x02,                   // INPUT (Data,Var,Abs)

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
};

#define HID_JOGCON_INTERFACE(n) \
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
	sizeof(jogcon_report_descriptor), /* wDescriptorLength */ \
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
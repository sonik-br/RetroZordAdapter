/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#pragma once

#include <stdint.h>
#include <avr/pgmspace.h>

#define HID_ENDPOINT_SIZE 64

// Mac OS-X and Linux automatically load the correct drivers.  On
// Windows, even though the driver is supplied by Microsoft, an
// INF file is needed to load the driver.  These numbers need to
// match the INF file.
// #define VENDOR_ID		0x10C4
// #define PRODUCT_ID		0x82C0

// Arduino Leonardo
// #define VENDOR_ID		0x2341
// #define PRODUCT_ID		0x8036

// RetroZord
// Use it on personal projects only! If doing a commercial product, use your own vid:pid!
#define VENDOR_ID		0x1209
#define PRODUCT_ID		0x595A



typedef struct __attribute((packed, aligned(1)))
{
	uint8_t left_duration;  //large motor
	uint8_t left_power;
	uint8_t right_duration; //small motor
	uint8_t right_power;
} RumbleReport;

/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define ENDPOINT0_SIZE	64

#define GAMEPAD_INTERFACE	0
#define GAMEPAD_ENDPOINT	1
#define GAMEPAD_SIZE		64

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

// HAT report (4 bits)
#define HID_HAT_UP        0x00
#define HID_HAT_UPRIGHT   0x01
#define HID_HAT_RIGHT     0x02
#define HID_HAT_DOWNRIGHT 0x03
#define HID_HAT_DOWN      0x04
#define HID_HAT_DOWNLEFT  0x05
#define HID_HAT_LEFT      0x06
#define HID_HAT_UPLEFT    0x07
#define HID_HAT_NOTHING   0x08

// Button report (16 bits)
#define HID_MASK_SQUARE   (1U <<  0)
#define HID_MASK_CROSS    (1U <<  1)
#define HID_MASK_CIRCLE   (1U <<  2)
#define HID_MASK_TRIANGLE (1U <<  3)
#define HID_MASK_L1       (1U <<  4)
#define HID_MASK_R1       (1U <<  5)
#define HID_MASK_L2       (1U <<  6)
#define HID_MASK_R2       (1U <<  7)
#define HID_MASK_SELECT   (1U <<  8)
#define HID_MASK_START    (1U <<  9)
#define HID_MASK_L3       (1U << 10)
#define HID_MASK_R3       (1U << 11)
#define HID_MASK_PS       (1U << 12)
#define HID_MASK_TP       (1U << 13)

// Switch analog sticks only report 8 bits
#define HID_JOYSTICK_MIN 0x00
#define HID_JOYSTICK_MID 0x80
#define HID_JOYSTICK_MAX 0xFF

#define BTN_GAMEPAD        1 << 0
#define BTN_SOUTH        1 << 0
//#define BTN_A            BTN_SOUTH
#define BTN_EAST        1 << 1
//#define BTN_B            BTN_EAST
//#define BTN_C            1 << 2
#define BTN_NORTH        1 << 2
//#define BTN_X            BTN_NORTH
#define BTN_WEST        1 << 3
//#define BTN_Y            BTN_WEST
//#define BTN_Z            1 << 5
#define BTN_TL            1 << 4
#define BTN_TR            1 << 5
#define BTN_TL2            1 << 6
#define BTN_TR2            1 << 7
#define BTN_SELECT        1 << 8
#define BTN_START        1 << 9
#define BTN_MODE        1 << 10
#define BTN_THUMBL        1 << 11
#define BTN_THUMBR        1 << 12

typedef struct __attribute((packed, aligned(1)))
{
	uint32_t buttons : 24;
	uint8_t hat;
	uint8_t lx;
	uint8_t ly;
	uint8_t rx;
	uint8_t ry;
	//uint8_t trigger;
	
	// uint8_t lt;
	// uint8_t rt;
} HIDReport;

static const uint8_t hid_string_language[]     = { 0x09, 0x04 };
static const uint8_t hid_string_manufacturer[] = "Open Stick Community";
static const uint8_t hid_string_product[]      = "GP2040-CE (D-Input)";
static const uint8_t hid_string_version[]      = "1.0";

static const uint8_t *hid_string_descriptors[] =
{
	hid_string_language,
	hid_string_manufacturer,
	hid_string_product,
	hid_string_version
};

static const uint8_t hid_device_descriptor[] PROGMEM =
{
		18,								  // bLength
		1,								  // bDescriptorType
		0x00, 0x02,						  // bcdUSB
		0,								  // bDeviceClass
		0,								  // bDeviceSubClass
		0,								  // bDeviceProtocol
		ENDPOINT0_SIZE,					  // bMaxPacketSize0
		LSB(VENDOR_ID), MSB(VENDOR_ID),	  // idVendor
		LSB(PRODUCT_ID), MSB(PRODUCT_ID), // idProduct
		0x00, 0x01,						  // bcdDevice
		1,								  // iManufacturer
		2,								  // iProduct
		3,								  // iSerialNumber
		1								  // bNumConfigurations
};

static const uint8_t hid_report_descriptor[] PROGMEM =
{
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	0x09, 0x05,        // Usage (Game Pad)
	0xA1, 0x01,        // Collection (Application)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x35, 0x00,        //   Physical Minimum (0)
	0x45, 0x01,        //   Physical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x18,        //   Report Count (24)
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (0x01)
	0x29, 0x18,        //   Usage Maximum (0x18)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x25, 0x07,        //   Logical Maximum (7)
	0x46, 0x3B, 0x01,  //   Physical Maximum (315)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
	0x09, 0x39,        //   Usage (Hat switch)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x65, 0x00,        //   Unit (None)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x46, 0xFF, 0x00,  //   Physical Maximum (255)
	
	0x09, 0x30,        //   Usage (X)
	0x09, 0x31,        //   Usage (Y)

	0x09, 0x35,        //   Usage (Rz)
	0x09, 0x36,        //   Usage (Slider)

	//0x09, 0x32,        //   Usage (Z)

	
	// 0x09, 0x32,        //   Usage (Z)
	// 0x09, 0x33,        //   Usage (Rx)

	// 0x09, 0x34,        //   Usage (Ry)
	// 0x09, 0x35,        //   Usage (Rz)


	 //0x09, 0x33,        //   Usage (Rx)
	 //0x09, 0x34,        //   Usage (Ry)
	
	// 0x09, 0x35,        //   Usage (Rz)
	//0x09, 0x36,        //   Usage (Slider)
	 



	0x75, 0x08,        //   Report Size (8)
	0x95, 0x04,        //   Report Count (4)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              // End Collection
};

static const uint8_t hid_hid_descriptor[] PROGMEM =
{
		0x09,								 // bLength
		0x21,								 // bDescriptorType (HID)
		0x11, 0x01,							 // bcdHID 1.11
		0x00,								 // bCountryCode
		0x01,								 // bNumDescriptors
		0x22,								 // bDescriptorType[0] (HID)
		sizeof(hid_report_descriptor), 0x00, // wDescriptorLength[0] 90 (dynamically updated)
};

static const uint8_t HID_CONFIG_MAIN_LENGTH = 9;
static const uint8_t HID_CONFIG_INTERFACE_LENGTH = 25;

#define HID_INTERFACE(n) \
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
	sizeof(hid_report_descriptor), /* wDescriptorLength */ \
	0,								\
		/* endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13 */ \
	7,						 	   /* bLength */ \
	5,						       /* bDescriptorType */ \
	(GAMEPAD_ENDPOINT+n) | 0x80,   /* bEndpointAddress */ \
	0x03,					       /* bmAttributes (0x03=intr) */ \
	GAMEPAD_SIZE, 0,		       /* wMaxPacketSize */ \
	1,						       /* bInterval (1 ms) */


#define CONFIG1_DESC_SIZE		(9+9+9+7)
static const uint8_t hid_configuration_descriptor[] PROGMEM =
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

static const uint8_t hid_configuration_interface_descriptor[] PROGMEM  =
{
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
};
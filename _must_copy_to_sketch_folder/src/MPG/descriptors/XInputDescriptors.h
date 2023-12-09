/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#pragma once

#include <stdint.h>
#include <avr/pgmspace.h>

#define XINPUT_ENDPOINT_SIZE 20

// Buttons 1 (8 bits)
// TODO: Consider using an enum class here.
#define XBOX_MASK_UP    (1U << 0)
#define XBOX_MASK_DOWN  (1U << 1)
#define XBOX_MASK_LEFT  (1U << 2)
#define XBOX_MASK_RIGHT (1U << 3)
#define XBOX_MASK_START (1U << 4)
#define XBOX_MASK_BACK  (1U << 5)
#define XBOX_MASK_LS    (1U << 6)
#define XBOX_MASK_RS    (1U << 7)

// Buttons 2 (8 bits)
// TODO: Consider using an enum class here.
#define XBOX_MASK_LB    (1U << 0)
#define XBOX_MASK_RB    (1U << 1)
#define XBOX_MASK_HOME  (1U << 2)
//#define UNUSED        (1U << 3)
#define XBOX_MASK_A     (1U << 4)
#define XBOX_MASK_B     (1U << 5)
#define XBOX_MASK_X     (1U << 6)
#define XBOX_MASK_Y     (1U << 7)

typedef struct __attribute((packed, aligned(1)))
{
	uint8_t report_id;
	uint8_t report_size;
	uint8_t buttons1;
	uint8_t buttons2;
	uint8_t lt;
	uint8_t rt;
	int16_t lx;
	int16_t ly;
	int16_t rx;
	int16_t ry;
	uint8_t _reserved[6];
} XInputReport;

static const uint8_t xinput_string_language[]    = { 0x09, 0x04 };
static const uint8_t xinput_string_manfacturer[] = "Microsoft";
static const uint8_t xinput_string_product[]     = "XInput STANDARD GAMEPAD";
static const uint8_t xinput_string_version[]     = "1.0";

static const uint8_t *xinput_string_descriptors[] =
{
	xinput_string_language,
	xinput_string_manfacturer,
	xinput_string_product,
	xinput_string_version
};

static const uint8_t xinput_device_descriptor[] PROGMEM =
{
	0x12,       // bLength
	0x01,       // bDescriptorType (Device)
	0x00, 0x02, // bcdUSB 2.00
	0xFF,	      // bDeviceClass
	0xFF,	      // bDeviceSubClass
	0xFF,	      // bDeviceProtocol
	0x40,	      // bMaxPacketSize0 64
	0x5E, 0x04, // idVendor 0x045E
	0x8E, 0x02, // idProduct 0x028E
	0x14, 0x01, // bcdDevice 2.14
	0x01,       // iManufacturer (String Index)
	0x02,       // iProduct (String Index)
	0x03,       // iSerialNumber (String Index)
	0x01,       // bNumConfigurations 1
};

static const uint8_t xinput_configuration_descriptor[] =
{
	// Configuration Descriptor
	0x09,        // bLength
	0x02,        // bDescriptorType (CONFIGURATION)
	0x31, 0x00,  // wTotalLength (49)
	0x01,        // bNumInterfaces
	0x01,        // bConfigurationValue
	0x00,        // iConfiguration
	0xA0,        // bmAttributes
	0xFA,        // bMaxPower

	/* ---------------------------------------------------- */
	// Interface 0: Control Data
	0x09,        // bLength
	0x04,        // bDescriptorType (INTERFACE)
	0x00,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x02,        // bNumEndpoints
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x01,        // bInterfaceProtocol
	0x00,        // iInterface

	// Unknown Descriptor (If0)
	0x11,        // bLength
	0x21,        // bDescriptorType
	0x00, 0x01, 0x01, 0x25,  // ???
	0x81,        // bEndpointAddress (IN, 1)
	0x14,        // bMaxDataSize
	0x00, 0x00, 0x00, 0x00, 0x13,  // ???
	0x02,        // bEndpointAddress (OUT, 2)
	0x08,        // bMaxDataSize
	0x00, 0x00,  // ???

	// Endpoint 1: Control Surface Send
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x81,        // bEndpointAddress (IN, 1)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x04,        // bInterval

	// Endpoint 1: Control Surface Receive
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x02,        // bEndpointAddress (OUT, 2)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x08,        // bInterval
};

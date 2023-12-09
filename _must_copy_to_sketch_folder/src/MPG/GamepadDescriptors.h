/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#pragma once

#define MAX_HID_INTERFACES 5 //5

#include <string.h>
#include "GamepadEnums.h"
#include "descriptors/HIDDescriptors.h"
#include "descriptors/SwitchDescriptors.h"
#include "descriptors/XInputDescriptors.h"
#include "descriptors/PS3Descriptors.h"
#include "descriptors/NegconDescriptors.h"
#include "descriptors/JogconDescriptors.h"
#include "descriptors/JogconMouseDescriptors.h"
#include "descriptors/GunconDescriptors.h"

// Default value used for networking, override if necessary
static uint8_t macAddress[6] = { 0x02, 0x02, 0x84, 0x6A, 0x96, 0x00 };

static const uint8_t *getConfigurationDescriptor(uint16_t *size, InputMode mode, const uint8_t interfaces)
{
	static uint8_t descriptorBuffer[sizeof(ps3_configuration_descriptor) + (MAX_HID_INTERFACES * sizeof(ps3_configuration_interface_descriptor))];

	switch (mode)
	{
		case INPUT_MODE_XINPUT:
			*size = sizeof(xinput_configuration_descriptor);
			return xinput_configuration_descriptor;

		case INPUT_MODE_SWITCH:
			// *size = switch_configuration_descriptor[2];
			// return switch_configuration_descriptor;
      		memcpy_P(descriptorBuffer, switch_configuration_descriptor, sizeof(switch_configuration_descriptor));
			descriptorBuffer[2] = sizeof(switch_configuration_descriptor) + (interfaces * sizeof(switch_configuration_interface_descriptor)); //wTotalLength
      		descriptorBuffer[4] = interfaces; //bNumInterfaces
			for(uint8_t i = 0; i < interfaces; ++i) {
				memcpy_P(descriptorBuffer + sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)), switch_configuration_interface_descriptor, sizeof(switch_configuration_interface_descriptor));
				descriptorBuffer[sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
				descriptorBuffer[sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)) + 16] = sizeof(switch_report_descriptor); //wDescriptorLength
				descriptorBuffer[sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
			}
			*size = sizeof(switch_configuration_descriptor) + (interfaces * sizeof(switch_configuration_interface_descriptor));
			return descriptorBuffer;

		case INPUT_MODE_PS3:
			//*size = ps3_configuration_descriptor[2];
			//return ps3_configuration_descriptor;
      		memcpy_P(descriptorBuffer, ps3_configuration_descriptor, sizeof(ps3_configuration_descriptor));
			descriptorBuffer[2] = sizeof(ps3_configuration_descriptor) + (interfaces * sizeof(ps3_configuration_interface_descriptor)); //wTotalLength
      		descriptorBuffer[4] = interfaces; //bNumInterfaces
			for(uint8_t i = 0; i < interfaces; ++i) {
				memcpy_P(descriptorBuffer + sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)), ps3_configuration_interface_descriptor, sizeof(ps3_configuration_interface_descriptor));
				descriptorBuffer[sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
				descriptorBuffer[sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)) + 16] = sizeof(ps3_report_descriptor); //wDescriptorLength
				descriptorBuffer[sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
			}
			*size = sizeof(ps3_configuration_descriptor) + (interfaces * sizeof(ps3_configuration_interface_descriptor));
			return descriptorBuffer;

		case INPUT_MODE_HID_NEGCON:
			// *size = hid_negcon_configuration_descriptor[2];
			// return hid_negcon_configuration_descriptor;
      		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
      		descriptorBuffer[4] = interfaces; //bNumInterfaces
			for(uint8_t i = 0; i < interfaces; ++i) {
				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(negcon_report_descriptor); //wDescriptorLength
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
			}
			*size = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON:
			// *size = hid_jogcon_configuration_descriptor[2];
			// return hid_jogcon_configuration_descriptor;
      		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
      		descriptorBuffer[4] = interfaces; //bNumInterfaces
			for(uint8_t i = 0; i < interfaces; ++i) {
				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(jogcon_report_descriptor); //wDescriptorLength
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
			}
			*size = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON_MOUSE:
			// *size = hid_jogcon_mouse_configuration_descriptor[2];
			// return hid_jogcon_mouse_configuration_descriptor;
      		memcpy_P(descriptorBuffer, hid_jogcon_mouse_configuration_descriptor, sizeof(hid_jogcon_mouse_configuration_descriptor));
			*size = sizeof(hid_jogcon_mouse_configuration_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_GUNCON:
			// *size = hid_guncon_configuration_descriptor[2];
			// return hid_guncon_configuration_descriptor;
      		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
      		descriptorBuffer[4] = interfaces; //bNumInterfaces
			for(uint8_t i = 0; i < interfaces; ++i) {
				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(guncon_report_descriptor); //wDescriptorLength
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
			}
			*size = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));
			return descriptorBuffer;

		default:
			//*size = hid_configuration_descriptor[2];
			//return hid_configuration_descriptor;
      		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
      		descriptorBuffer[4] = interfaces; //bNumInterfaces
			for(uint8_t i = 0; i < interfaces; ++i) {
				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(hid_report_descriptor); //wDescriptorLength
				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
			}
			*size = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));
			return descriptorBuffer;
	}
}

static const uint8_t *getDeviceDescriptor(uint16_t *size, InputMode mode)
{
	static uint8_t descriptorBuffer[18];

	switch (mode)
	{
		case INPUT_MODE_XINPUT:
			// *size = sizeof(xinput_device_descriptor);
			// return xinput_device_descriptor;
			memcpy_P(descriptorBuffer, xinput_device_descriptor, sizeof(xinput_device_descriptor));
			*size = sizeof(xinput_device_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_SWITCH:
			// *size = sizeof(switch_device_descriptor);
			// return switch_device_descriptor;
			memcpy_P(descriptorBuffer, switch_device_descriptor, sizeof(switch_device_descriptor));
			*size = sizeof(switch_device_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_PS3:
			// *size = sizeof(ps3_device_descriptor);
			// return ps3_device_descriptor;
			memcpy_P(descriptorBuffer, ps3_device_descriptor, sizeof(ps3_device_descriptor));
			*size = sizeof(ps3_device_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_NEGCON:
			// *size = sizeof(hid_device_descriptor);
			// return hid_device_descriptor;
			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
			*size = sizeof(hid_device_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON:
			// *size = sizeof(hid_device_descriptor);
			// return hid_device_descriptor;
			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
			*size = sizeof(hid_device_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON_MOUSE:
			// *size = sizeof(hid_device_descriptor);
			// return hid_device_descriptor;
			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
			*size = sizeof(hid_device_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_GUNCON:
			// *size = sizeof(hid_device_descriptor);
			// return hid_device_descriptor;
			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
			*size = sizeof(hid_device_descriptor);
			return descriptorBuffer;

		default:
			// *size = sizeof(hid_device_descriptor);
			// return hid_device_descriptor;
			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
			*size = sizeof(hid_device_descriptor);
			return descriptorBuffer;
	}
}

static const uint8_t *getHIDDescriptor(uint16_t *size, InputMode mode)
{
	static uint8_t descriptorBuffer[9];

	switch (mode)
	{
		case INPUT_MODE_SWITCH:
			// *size = sizeof(switch_hid_descriptor);
			// return switch_hid_descriptor;
			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
			descriptorBuffer[7] = sizeof(switch_report_descriptor); //wDescriptorLength
			*size = sizeof(hid_hid_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_PS3:
			// *size = sizeof(ps3_hid_descriptor);
			// return ps3_hid_descriptor;
			memcpy_P(descriptorBuffer, ps3_hid_descriptor, sizeof(ps3_hid_descriptor));
			descriptorBuffer[7] = sizeof(ps3_report_descriptor); //wDescriptorLength
			*size = sizeof(ps3_hid_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_NEGCON:
			// *size = sizeof(negcon_hid_descriptor);
			// return negcon_hid_descriptor;
			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
			descriptorBuffer[7] = sizeof(negcon_report_descriptor); //wDescriptorLength
			*size = sizeof(hid_hid_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON:
			// *size = sizeof(jogcon_hid_descriptor);
			// return jogcon_hid_descriptor;
			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
			descriptorBuffer[7] = sizeof(jogcon_report_descriptor); //wDescriptorLength
			*size = sizeof(hid_hid_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON_MOUSE:
			// *size = sizeof(jogcon_mouse_hid_descriptor);
			// return jogcon_mouse_hid_descriptor;
			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
			descriptorBuffer[7] = sizeof(jogcon_mouse_report_descriptor); //wDescriptorLength
			*size = sizeof(hid_hid_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_GUNCON:
			// *size = sizeof(guncon_hid_descriptor);
			// return guncon_hid_descriptor;
			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
			descriptorBuffer[7] = sizeof(guncon_report_descriptor); //wDescriptorLength
			*size = sizeof(hid_hid_descriptor);
			return descriptorBuffer;

		default:
			// *size = sizeof(hid_hid_descriptor);
			// return hid_hid_descriptor;
			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
			descriptorBuffer[7] = sizeof(hid_report_descriptor); //wDescriptorLength
			*size = sizeof(hid_hid_descriptor);
			return descriptorBuffer;
	}
}

static const uint8_t *getHIDReport(uint16_t *size, InputMode mode)
{
	static uint8_t descriptorBuffer[150]; //hid:112, switch:86, negcon:89

	switch (mode)
	{
		case INPUT_MODE_SWITCH:
			// *size = sizeof(switch_report_descriptor);
			// return switch_report_descriptor;
			memcpy_P(descriptorBuffer, switch_report_descriptor, sizeof(switch_report_descriptor));
			*size = sizeof(switch_report_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_PS3:
			// *size = sizeof(ps3_report_descriptor);
			// return ps3_report_descriptor;
			memcpy_P(descriptorBuffer, ps3_report_descriptor, sizeof(ps3_report_descriptor));
			*size = sizeof(ps3_report_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_NEGCON:
			// *size = sizeof(hid_negcon_report_descriptor);
			// return hid_negcon_report_descriptor;
			memcpy_P(descriptorBuffer, negcon_report_descriptor, sizeof(negcon_report_descriptor));
			*size = sizeof(negcon_report_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON:
			// *size = sizeof(hid_jogcon_report_descriptor);
			// return hid_jogcon_report_descriptor;
			memcpy_P(descriptorBuffer, jogcon_report_descriptor, sizeof(jogcon_report_descriptor));
			*size = sizeof(jogcon_report_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_JOGCON_MOUSE:
			// *size = sizeof(hid_jogcon_mouse_report_descriptor);
			// return hid_jogcon_mouse_report_descriptor;
			memcpy_P(descriptorBuffer, jogcon_mouse_report_descriptor, sizeof(jogcon_mouse_report_descriptor));
			*size = sizeof(jogcon_mouse_report_descriptor);
			return descriptorBuffer;

		case INPUT_MODE_HID_GUNCON:
			// *size = sizeof(hid_guncon_report_descriptor);
			// return hid_guncon_report_descriptor;
			memcpy_P(descriptorBuffer, guncon_report_descriptor, sizeof(guncon_report_descriptor));
			*size = sizeof(guncon_report_descriptor);
			return descriptorBuffer;

		default:
			// *size = sizeof(hid_report_descriptor);
			// return hid_report_descriptor;
			memcpy_P(descriptorBuffer, hid_report_descriptor, sizeof(hid_report_descriptor));
			*size = sizeof(hid_report_descriptor);
			return descriptorBuffer;
	}
}

// Convert ASCII string into UTF-16
static const uint16_t *convertStringDescriptor(uint16_t *payloadSize, const char *str, int charCount)
{
	static uint16_t payload[32];

	// Cap at max char
	if (charCount > 31)
		charCount = 31;

	for (uint8_t i = 0; i < charCount; i++)
		payload[1 + i] = str[i];

	// first byte is length (including header), second byte is string type
	*payloadSize = (2 * charCount + 2);
	payload[0] = (0x03 << 8) | *payloadSize;
	return payload;
}

static const uint16_t *getStringDescriptor(uint16_t *size, InputMode mode, uint8_t index)
{
	uint8_t charCount;
	char *str;

	if (index == 0)
	{
		str = (char *)xinput_string_descriptors[0];
		charCount = 1;
	}
	else if (index == 5)
	{
		// Convert MAC address into UTF-16
		for (int i = 0; i < 6; i++)
		{
			str[1 + charCount++] = "0123456789ABCDEF"[(macAddress[i] >> 4) & 0xf];
			str[1 + charCount++] = "0123456789ABCDEF"[(macAddress[i] >> 0) & 0xf];
		}
	}
	else
	{
		switch (mode)
		{
			case INPUT_MODE_XINPUT:
				str = (char *)xinput_string_descriptors[index];
				break;

			case INPUT_MODE_SWITCH:
				str = (char *)switch_string_descriptors[index];
				break;

			case INPUT_MODE_PS3:
				str = (char *)ps3_string_descriptors[index];
				break;

			default:
				str = (char *)hid_string_descriptors[index];
				break;
		}

		charCount = strlen(str);
	}

	return convertStringDescriptor(size, str, charCount);
}

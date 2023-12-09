// #include "GamepadDescriptors.h"

// static uint16_t getConfigurationDescriptor(const uint8_t *buffer, InputMode mode, const uint8_t interfaces)
// {
// 	static uint8_t descriptorBuffer[sizeof(hid_configuration_descriptor) + (MAX_HID_INTERFACES * sizeof(hid_configuration_interface_descriptor))];

// 	switch (mode)
// 	{
// 		case INPUT_MODE_XINPUT:
// 			buffer = xinput_configuration_descriptor;
// 			return sizeof(xinput_configuration_descriptor);

// 		case INPUT_MODE_SWITCH:
// 			// buffer = switch_configuration_descriptor;
// 			// return switch_configuration_descriptor[2];
//       		memcpy_P(descriptorBuffer, switch_configuration_descriptor, sizeof(switch_configuration_descriptor));
// 			descriptorBuffer[2] = sizeof(switch_configuration_descriptor) + (interfaces * sizeof(switch_configuration_interface_descriptor)); //wTotalLength
//       		descriptorBuffer[4] = interfaces; //bNumInterfaces
// 			for(uint8_t i = 0; i < interfaces; ++i) {
// 				memcpy_P(descriptorBuffer + sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)), switch_configuration_interface_descriptor, sizeof(switch_configuration_interface_descriptor));
// 				descriptorBuffer[sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
// 				descriptorBuffer[sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)) + 16] = sizeof(switch_report_descriptor); //wDescriptorLength
// 				descriptorBuffer[sizeof(switch_configuration_descriptor) + (i * sizeof(switch_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
// 			}
// 			buffer = descriptorBuffer;
// 			return sizeof(switch_configuration_descriptor) + (interfaces * sizeof(switch_configuration_interface_descriptor));

// 		case INPUT_MODE_PS3:
// 			//buffer = ps3_configuration_descriptor;
// 			//return ps3_configuration_descriptor[2];
//       		memcpy_P(descriptorBuffer, ps3_configuration_descriptor, sizeof(ps3_configuration_descriptor));
// 			descriptorBuffer[2] = sizeof(ps3_configuration_descriptor) + (interfaces * sizeof(ps3_configuration_interface_descriptor)); //wTotalLength
//       		descriptorBuffer[4] = interfaces; //bNumInterfaces
// 			for(uint8_t i = 0; i < interfaces; ++i) {
// 				memcpy_P(descriptorBuffer + sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)), ps3_configuration_interface_descriptor, sizeof(ps3_configuration_interface_descriptor));
// 				descriptorBuffer[sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
// 				descriptorBuffer[sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)) + 16] = sizeof(ps3_report_descriptor); //wDescriptorLength
// 				descriptorBuffer[sizeof(ps3_configuration_descriptor) + (i * sizeof(ps3_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
// 			}
// 			buffer = descriptorBuffer;
// 			return sizeof(ps3_configuration_descriptor) + (interfaces * sizeof(ps3_configuration_interface_descriptor));

// 		case INPUT_MODE_HID_NEGCON:
// 			//buffer = hid_negcon_configuration_descriptor;
// 			//return sizeof(hid_negcon_configuration_descriptor);
//       		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
// 			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
//       		descriptorBuffer[4] = interfaces; //bNumInterfaces
// 			for(uint8_t i = 0; i < interfaces; ++i) {
// 				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(negcon_report_descriptor); //wDescriptorLength
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
// 			}
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));

// 		case INPUT_MODE_HID_JOGCON:
// 			//buffer = hid_jogcon_configuration_descriptor;
// 			//return sizeof(hid_jogcon_configuration_descriptor);
//       		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
// 			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
//       		descriptorBuffer[4] = interfaces; //bNumInterfaces
// 			for(uint8_t i = 0; i < interfaces; ++i) {
// 				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(jogcon_report_descriptor); //wDescriptorLength
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
// 			}
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));

// 		case INPUT_MODE_HID_JOGCON_MOUSE:
// 			//buffer = hid_jogcon_mouse_configuration_descriptor;
// 			//return sizeof(hid_jogcon_mouse_configuration_descriptor);
//       		memcpy_P(descriptorBuffer, hid_jogcon_mouse_configuration_descriptor, sizeof(hid_jogcon_mouse_configuration_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_jogcon_mouse_configuration_descriptor);

// 		case INPUT_MODE_HID_GUNCON:
// 			//buffer = hid_guncon_configuration_descriptor;
// 			//return sizeof(hid_guncon_configuration_descriptor);
//       		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
// 			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
//       		descriptorBuffer[4] = interfaces; //bNumInterfaces
// 			for(uint8_t i = 0; i < interfaces; ++i) {
// 				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(guncon_report_descriptor); //wDescriptorLength
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
// 			}
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));

// 		default:
// 			//buffer = hid_configuration_descriptor;
// 			//return hid_configuration_descriptor[2];
//       		memcpy_P(descriptorBuffer, hid_configuration_descriptor, sizeof(hid_configuration_descriptor));
// 			descriptorBuffer[2] = sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor)); //wTotalLength
//       		descriptorBuffer[4] = interfaces; //bNumInterfaces
// 			for(uint8_t i = 0; i < interfaces; ++i) {
// 				memcpy_P(descriptorBuffer + sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)), hid_configuration_interface_descriptor, sizeof(hid_configuration_interface_descriptor));
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 2] = GAMEPAD_INTERFACE + i; //bInterfaceNumber
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 16] = sizeof(hid_report_descriptor); //wDescriptorLength
// 				descriptorBuffer[sizeof(hid_configuration_descriptor) + (i * sizeof(hid_configuration_interface_descriptor)) + 20] = (GAMEPAD_ENDPOINT + i) | 0x80; //bEndpointAddress
// 			}
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_configuration_descriptor) + (interfaces * sizeof(hid_configuration_interface_descriptor));
// 	}
// }

// static uint16_t getDeviceDescriptor(const uint8_t *buffer, InputMode mode)
// {
// 	static uint8_t descriptorBuffer[18];

// 	switch (mode)
// 	{
// 		case INPUT_MODE_XINPUT:
// 			// buffer = xinput_device_descriptor;
// 			// return sizeof(xinput_device_descriptor);
// 			memcpy_P(descriptorBuffer, xinput_device_descriptor, sizeof(xinput_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(xinput_device_descriptor);

// 		case INPUT_MODE_SWITCH:
// 			// buffer = switch_device_descriptor;
// 			// return sizeof(switch_device_descriptor);
// 			memcpy_P(descriptorBuffer, switch_device_descriptor, sizeof(switch_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(switch_device_descriptor);

// 		case INPUT_MODE_PS3:
// 			// buffer = ps3_device_descriptor;
// 			// return sizeof(ps3_device_descriptor);
// 			memcpy_P(descriptorBuffer, ps3_device_descriptor, sizeof(ps3_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(ps3_device_descriptor);

// 		case INPUT_MODE_HID_NEGCON:
// 			// buffer = hid_device_descriptor;
// 			// return sizeof(hid_device_descriptor);
// 			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_device_descriptor);

// 		case INPUT_MODE_HID_JOGCON:
// 			// buffer = hid_device_descriptor;
// 			// return sizeof(hid_device_descriptor);
// 			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_device_descriptor);

// 		case INPUT_MODE_HID_JOGCON_MOUSE:
// 			// buffer = hid_device_descriptor;
// 			// return sizeof(hid_device_descriptor);
// 			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_device_descriptor);

// 		case INPUT_MODE_HID_GUNCON:
// 			// buffer = hid_device_descriptor;
// 			// return sizeof(hid_device_descriptor);
// 			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_device_descriptor);

// 		default:
// 			// buffer = hid_device_descriptor;
// 			// return sizeof(hid_device_descriptor);
// 			memcpy_P(descriptorBuffer, hid_device_descriptor, sizeof(hid_device_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_device_descriptor);
// 	}
// }

// static uint16_t getHIDDescriptor(const uint8_t *buffer, InputMode mode)
// {
// 	static uint8_t descriptorBuffer[9];

// 	switch (mode)
// 	{
// 		case INPUT_MODE_SWITCH:
// 			// buffer = switch_hid_descriptor;
// 			// return sizeof(switch_hid_descriptor);
// 			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
// 			descriptorBuffer[7] = sizeof(switch_report_descriptor); //wDescriptorLength
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_hid_descriptor);

// 		case INPUT_MODE_PS3:
// 			// buffer = ps3_hid_descriptor;
// 			// return sizeof(ps3_hid_descriptor);
// 			memcpy_P(descriptorBuffer, ps3_hid_descriptor, sizeof(ps3_hid_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(ps3_hid_descriptor);

// 		case INPUT_MODE_HID_NEGCON:
// 			// buffer = negcon_hid_descriptor;
// 			// return sizeof(negcon_hid_descriptor);
// 			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
// 			descriptorBuffer[7] = sizeof(negcon_report_descriptor); //wDescriptorLength
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_hid_descriptor);

// 		case INPUT_MODE_HID_JOGCON:
// 			// buffer = jogcon_hid_descriptor;
// 			// return sizeof(jogcon_hid_descriptor);
// 			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
// 			descriptorBuffer[7] = sizeof(jogcon_report_descriptor); //wDescriptorLength
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_hid_descriptor);

// 		case INPUT_MODE_HID_JOGCON_MOUSE:
// 			// buffer = jogcon_mouse_hid_descriptor;
// 			// return sizeof(jogcon_mouse_hid_descriptor);
// 			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
// 			descriptorBuffer[7] = sizeof(jogcon_mouse_report_descriptor); //wDescriptorLength
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_hid_descriptor);

// 		case INPUT_MODE_HID_GUNCON:
// 			// buffer = guncon_hid_descriptor;
// 			// return sizeof(guncon_hid_descriptor);
// 			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
// 			descriptorBuffer[7] = sizeof(guncon_report_descriptor); //wDescriptorLength
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_hid_descriptor);

// 		default:
// 			// buffer = hid_hid_descriptor;
// 			// return sizeof(hid_hid_descriptor);
// 			memcpy_P(descriptorBuffer, hid_hid_descriptor, sizeof(hid_hid_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_hid_descriptor);
// 	}
// }

// static uint16_t getHIDReport(const uint8_t *buffer, InputMode mode)
// {
// 	static uint8_t descriptorBuffer[150]; //hid:112, switch:86, negcon:89

// 	switch (mode)
// 	{
// 		case INPUT_MODE_SWITCH:
// 			// buffer = switch_report_descriptor;
// 			// return sizeof(switch_report_descriptor);
// 			memcpy_P(descriptorBuffer, switch_report_descriptor, sizeof(switch_report_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(switch_report_descriptor);

// 		case INPUT_MODE_PS3:
// 			// buffer = ps3_report_descriptor;
// 			// return sizeof(ps3_report_descriptor);
// 			memcpy_P(descriptorBuffer, ps3_report_descriptor, sizeof(ps3_report_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(ps3_report_descriptor);

// 		case INPUT_MODE_HID_NEGCON:
// 			// buffer = hid_negcon_report_descriptor;
// 			// return sizeof(hid_negcon_report_descriptor);
// 			memcpy_P(descriptorBuffer, negcon_report_descriptor, sizeof(negcon_report_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(negcon_report_descriptor);

// 		case INPUT_MODE_HID_JOGCON:
// 			// buffer = hid_jogcon_report_descriptor;
// 			// return sizeof(hid_jogcon_report_descriptor);
// 			memcpy_P(descriptorBuffer, jogcon_report_descriptor, sizeof(jogcon_report_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(jogcon_report_descriptor);

// 		case INPUT_MODE_HID_JOGCON_MOUSE:
// 			// buffer = hid_jogcon_mouse_report_descriptor;
// 			// return sizeof(hid_jogcon_mouse_report_descriptor);
// 			memcpy_P(descriptorBuffer, jogcon_mouse_report_descriptor, sizeof(jogcon_mouse_report_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(jogcon_mouse_report_descriptor);

// 		case INPUT_MODE_HID_GUNCON:
// 			// buffer = hid_guncon_report_descriptor;
// 			// return sizeof(hid_guncon_report_descriptor);
// 			memcpy_P(descriptorBuffer, guncon_report_descriptor, sizeof(guncon_report_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(guncon_report_descriptor);

// 		default:
// 			// buffer = hid_report_descriptor;
// 			// return sizeof(hid_report_descriptor);
// 			memcpy_P(descriptorBuffer, hid_report_descriptor, sizeof(hid_report_descriptor));
// 			buffer = descriptorBuffer;
// 			return sizeof(hid_report_descriptor);
// 	}
// }

// static uint16_t getStringDescriptor(const uint16_t *buffer, InputMode mode, uint8_t index)
// {
// 	static uint8_t descriptorStringBuffer[64]; // Max 64 bytes

// 	const char *value;
// 	uint16_t size;
// 	uint8_t charCount;

// 	switch (mode)
// 	{
// 		case INPUT_MODE_XINPUT:
// 			value = (const char *)xinput_string_descriptors[index];
// 			size = sizeof(xinput_string_descriptors[index]);
// 			break;

// 		case INPUT_MODE_SWITCH:
// 			value = (const char *)switch_string_descriptors[index];
// 			size = sizeof(switch_string_descriptors[index]);
// 			break;

// 		case INPUT_MODE_PS3:
// 			value = (const char *)ps3_string_descriptors[index];
// 			size = sizeof(ps3_string_descriptors[index]);
// 			break;

// 		case INPUT_MODE_HID_NEGCON:
// 			value = (const char *)hid_string_descriptors[index];
// 			size = sizeof(hid_string_descriptors[index]);
		
// 		case INPUT_MODE_HID_JOGCON:
// 			value = (const char *)hid_string_descriptors[index];
// 			size = sizeof(hid_string_descriptors[index]);

// 		case INPUT_MODE_HID_JOGCON_MOUSE:
// 			value = (const char *)hid_string_descriptors[index];
// 			size = sizeof(hid_string_descriptors[index]);

// 		case INPUT_MODE_HID_GUNCON:
// 			value = (const char *)hid_string_descriptors[index];
// 			size = sizeof(hid_string_descriptors[index]);

// 		default:
// 			value = (const char *)hid_string_descriptors[index];
// 			size = sizeof(hid_string_descriptors[index]);
// 			break;
// 	}

// 	// Cap at max char
// 	charCount = strlen(value);
// 	if (charCount > 31)
// 		charCount = 31;

// 	for (uint8_t i = 0; i < charCount; i++)
// 		descriptorStringBuffer[1 + i] = value[i];


// 	// first byte is length (including header), second byte is string type
// 	descriptorStringBuffer[0] = (2 * charCount + 2);
// 	descriptorStringBuffer[1] = 0x03;

// 	// Cast temp buffer to final result
// 	buffer = (uint16_t *)descriptorStringBuffer;

// 	return size;
// }

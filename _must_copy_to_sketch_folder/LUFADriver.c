#include "LUFADriver.h"

static HIDReport hidReportData;
static PS3Report ps3ReportData;
static SwitchReport switchReportData;
static XInputReport xinputReportData;
static NegconReport negconReportData;
static JogconReport jogconReportData;
static JogconMouse0Report jogconmouse0ReportData;
static JogconMouse1Report jogconmouse1ReportData;
static GunconReport gunconReportData;

static InputMode inputMode;
static void *reportData;
static uint8_t reportSize;
static uint8_t lastReportBytes[MAX_HID_INTERFACES][64] = {0};

static uint8_t bNumInterfaces;
static bool use_ep_out;

//https://github.com/bootsector/PS3PadMicro
static uint8_t ps3_magic_bytes[8] = { 0x21, 0x26, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00 };

// Configures hardware and peripherals, such as the USB peripherals.
void setupHardware(InputMode mode, const uint8_t interfaces, const char* id)
{
  inputMode = mode;
  bNumInterfaces = interfaces;
  use_ep_out = (mode == INPUT_MODE_HID && bNumInterfaces <= 3) || mode == INPUT_MODE_XINPUT;
  //strcpy(USB_STRING_VERSION, id);
  USB_STRING_VERSION = (char*)id;

  // We need to disable watchdog if enabled by bootloader/fuses.
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  // We need to disable clock division before initializing the USB hardware.
  clock_prescale_set(clock_div_1);

  // We can then initialize our hardware and peripherals, including the USB stack.
  USB_Init();

  // Enable interrupts after USB init
  GlobalInterruptEnable();
}

void sendReport(void *data, uint8_t size, const uint8_t interface)
{
  reportData = data;
  reportSize = size;
  if (
    //memcmp(lastReportBytes[interface], reportData, reportSize) != 0 && // Did the report change?
    USB_DeviceState == DEVICE_STATE_Configured              // Is USB ready?
  )
  {
    uint8_t endpoint_out = XINPUT_EPADDR_OUT;
    if (inputMode != INPUT_MODE_XINPUT) {
      endpoint_out = ENDPOINT_DIR_OUT | ((interface * 2) + 2);
    }

    if (use_ep_out)
    {
      Endpoint_SelectEndpoint(endpoint_out);
      if (Endpoint_IsConfigured() && Endpoint_IsOUTReceived())
      {
        if (Endpoint_IsReadWriteAllowed())
        {
          switch (inputMode)
          {
            case INPUT_MODE_XINPUT:
              {
                //xinput report data format from:
                //https://github.com/dmadison/ArduinoXInput
  
                uint8_t rxdata[8];
                Endpoint_Read_Stream_LE(&rxdata, sizeof(rxdata), NULL);
                if (rxdata[0] == 0x00 && rxdata[1] == 0x08) { //Rumble data
                  rumble_left[interface] = rxdata[3];
                  rumble_right[interface] = rxdata[4];
                }
                //            else if (rxdata[0] == 0x01 && rxdata[1] == 0x03) { //Led data
                //              //led = rxdata[2];
                //            }
                break;
              }
            case INPUT_MODE_HID:
              {
                uint8_t rxdata[5];
                if (Endpoint_Read_Stream_LE(&rxdata, sizeof(rxdata), NULL) == ENDPOINT_RWSTREAM_NoError) {
                  if (rxdata[0] == 0x05) { //Rumble data. Stadia style
                    rumble_left[interface] = rxdata[2];
                    rumble_right[interface] = rxdata[4];
                  }
                }
                break;
              }
            default:
              {
                //https://github.com/felis/USB_Host_Shield_2.0/blob/master/PS3USB.cpp
                //todo implement ps3 rumble
  
                SwitchOutReport JoystickOutputData;
                Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL);
                //At this point, we can react to this data.
                break;
              }
          }
        }
  
        Endpoint_ClearOUT();
      }
    }

    if (memcmp(lastReportBytes[interface], reportData, reportSize) != 0) { // Did the report change?
      
      if (inputMode == INPUT_MODE_HID_JOGCON_MOUSE) //force single interface
        Endpoint_SelectEndpoint(ENDPOINT_DIR_IN | 1);
      else if (use_ep_out)
        Endpoint_SelectEndpoint(ENDPOINT_DIR_IN | ((interface * 2) + 1));
      else
        Endpoint_SelectEndpoint(ENDPOINT_DIR_IN | (interface + 1));

      if (Endpoint_IsConfigured() && Endpoint_IsINReady())
      {
        Endpoint_Write_Stream_LE(reportData, reportSize, NULL);
        Endpoint_ClearIN();
        memcpy(lastReportBytes[interface], reportData, reportSize);
        memset(reportData, 0, reportSize);
      }
    }
  }

  USB_USBTask();
}

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex, const void **const address)
{
  const uint8_t descriptorType = (wValue >> 8);
  const uint8_t descriptorIndex = (wValue & 0xFF);
  uint16_t size = NO_DESCRIPTOR;

  switch (descriptorType)
  {
    case DTYPE_Device:
      *address = getDeviceDescriptor(&size, inputMode);
      break;

    case DTYPE_Configuration:
      *address = getConfigurationDescriptor(&size, inputMode, bNumInterfaces);
      break;

    case DTYPE_String:
      switch (descriptorIndex)
      {
        case 1:
          if (inputMode == INPUT_MODE_SWITCH)
            *address = convertStringDescriptor(&size, USB_SWITCH_STRING_MANUFACTURER, strlen(USB_SWITCH_STRING_MANUFACTURER));
          else
            *address = convertStringDescriptor(&size, USB_STRING_MANUFACTURER, strlen(USB_STRING_MANUFACTURER));
          break;
        case 2:
          if (inputMode == INPUT_MODE_SWITCH)
            *address = convertStringDescriptor(&size, USB_SWITCH_STRING_PRODUCT, strlen(USB_SWITCH_STRING_PRODUCT));
          else
            *address = convertStringDescriptor(&size, USB_STRING_PRODUCT, strlen(USB_STRING_PRODUCT));
          break;
        case 3:
            if (inputMode == INPUT_MODE_PS3)
              *address = convertStringDescriptor(&size, USB_STRING_PS3_VERSION, strlen(USB_STRING_PS3_VERSION));
            else
              *address = convertStringDescriptor(&size, USB_STRING_VERSION, strlen(USB_STRING_VERSION));
          break;
        default:
          *address = getStringDescriptor(&size, inputMode, descriptorIndex);
          break;
      }
      break;

    case HID_DTYPE_HID:
      *address = getHIDDescriptor(&size, inputMode);
      break;

    case HID_DTYPE_Report:
      *address = getHIDReport(&size, inputMode);
      break;
  }

  return size;
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void)
{
  // We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void)
{
  // We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void)
{
  switch (inputMode)
  {
    case INPUT_MODE_XINPUT:
      Endpoint_ConfigureEndpoint(XINPUT_EPADDR_IN, EP_TYPE_INTERRUPT, 32, 1);
      Endpoint_ConfigureEndpoint(XINPUT_EPADDR_OUT, EP_TYPE_INTERRUPT, 32, 8);
      break;

    default:
      //Endpoint_ConfigureEndpoint((ENDPOINT_DIR_OUT | i), EP_TYPE_INTERRUPT, HID_ENDPOINT_SIZE, 1);
      for (uint8_t i = 0; i < bNumInterfaces; ++i)
      {
        if (use_ep_out) {
          Endpoint_ConfigureEndpoint(ENDPOINT_DIR_IN  | ((i * 2) + 1), EP_TYPE_INTERRUPT, HID_ENDPOINT_SIZE, 1);
          Endpoint_ConfigureEndpoint(ENDPOINT_DIR_OUT | ((i * 2) + 2), EP_TYPE_INTERRUPT, HID_ENDPOINT_SIZE, 1);
        } else {
          Endpoint_ConfigureEndpoint(ENDPOINT_DIR_IN  | (i + 1), EP_TYPE_INTERRUPT, HID_ENDPOINT_SIZE, 1);
        }
        
        if (inputMode == INPUT_MODE_HID_JOGCON_MOUSE)
          break;
      }
      break;
  }
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void)
{
  switch (USB_ControlRequest.bRequest)
  {
    case HID_REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        Endpoint_ClearSETUP();

        if (USB_ControlRequest.wValue == 0x0300) {
//          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(ps3_magic_bytes, sizeof(ps3_magic_bytes));
//          Endpoint_ClearOUT();
        } else {

          switch (inputMode)
          {
            case INPUT_MODE_XINPUT:
              Endpoint_Write_Control_Stream_LE(&xinputReportData, XINPUT_ENDPOINT_SIZE);
              break;

            case INPUT_MODE_SWITCH:
              Endpoint_Write_Control_Stream_LE(&switchReportData, HID_ENDPOINT_SIZE);
              break;

            case INPUT_MODE_PS3:
              Endpoint_Write_Control_Stream_LE(&ps3ReportData, HID_ENDPOINT_SIZE);
              break;

            case INPUT_MODE_HID:
              Endpoint_Write_Control_Stream_LE(&hidReportData, HID_ENDPOINT_SIZE);
              break;

            case INPUT_MODE_HID_NEGCON:
              Endpoint_Write_Control_Stream_LE(&negconReportData, HID_ENDPOINT_SIZE);
              break;

            case INPUT_MODE_HID_JOGCON:
              Endpoint_Write_Control_Stream_LE(&jogconReportData, HID_ENDPOINT_SIZE);
              break;

            case INPUT_MODE_HID_JOGCON_MOUSE:
            {
              //todo needs testing. no idea if/when this is called
              jogconmouse0ReportData.id = 1;
              jogconmouse1ReportData.id = 2;
              if(USB_ControlRequest.wIndex == 0)
                Endpoint_Write_Control_Stream_LE(&jogconmouse0ReportData, HID_ENDPOINT_SIZE);
              else
                Endpoint_Write_Control_Stream_LE(&jogconmouse1ReportData, HID_ENDPOINT_SIZE);

              break;
            }

            case INPUT_MODE_HID_GUNCON:
              Endpoint_Write_Control_Stream_LE(&gunconReportData, HID_ENDPOINT_SIZE);
              break;
              
          }

        }

        Endpoint_ClearOUT();
      }
      break;

      case HID_REQ_SetReport:
      {
        if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) //if ((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_TYPE) == REQTYPE_CLASS)
        {
          uint8_t report_type = (USB_ControlRequest.wValue >> 8);
          uint8_t report_id   = (USB_ControlRequest.wValue & 0xFF);
          uint8_t interface = USB_ControlRequest.wIndex;
          if ((report_type == HID_REPORT_ITEM_Out || report_type == HID_REPORT_ITEM_Feature) && (report_id == 5)) //Rumble data. Stadia style
          {
              uint16_t report_size = USB_ControlRequest.wLength;
              uint8_t output_buffer[report_size];
              Endpoint_ClearSETUP();
              Endpoint_Read_Control_Stream_LE(&output_buffer, report_size);
              Endpoint_ClearIN();
              rumble_left[interface] = output_buffer[2];
              rumble_right[interface] = output_buffer[4];
          }
        }
      }
      break;
  }
}

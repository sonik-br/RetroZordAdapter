/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#include "MPG.h"

static PS3Report ps3Report
{
	.square_btn = 0, .cross_btn = 0, .circle_btn = 0, .triangle_btn = 0,
	.l1_btn = 0, .r1_btn = 0, .l2_btn = 0, .r2_btn = 0,
	.select_btn = 0, .start_btn = 0, .l3_btn = 0, .r3_btn = 0, .ps_btn = 0,
	.direction = 0x08,
	.l_x_axis = 0x80, .l_y_axis = 0x80, .r_x_axis = 0x80, .r_y_axis = 0x80,
	.right_axis = 0x00, .left_axis = 0x00, .up_axis = 0x00, .down_axis = 0x00,
	.triangle_axis = 0x00, .circle_axis = 0x00, .cross_axis = 0x00, .square_axis = 0x00,
	.l1_axis = 0x00, .r1_axis = 0x00, .l2_axis = 0x00, .r2_axis = 0x00
};

static HIDReport hidReport
{
	.buttons = 0,
	.hat = HID_HAT_NOTHING,
	.lx = HID_JOYSTICK_MID,
	.ly = HID_JOYSTICK_MID,
	.rx = HID_JOYSTICK_MID,
	.ry = HID_JOYSTICK_MID,
	//.trigger = HID_JOYSTICK_MID,
	// .lt = HID_JOYSTICK_MIN,
	// .rt = HID_JOYSTICK_MIN,
};

static SwitchReport switchReport
{
	.buttons = 0,
	.hat = SWITCH_HAT_NOTHING,
	.lx = SWITCH_JOYSTICK_MID,
	.ly = SWITCH_JOYSTICK_MID,
	.rx = SWITCH_JOYSTICK_MID,
	.ry = SWITCH_JOYSTICK_MID,
	.vendor = 0,
};

static XInputReport xinputReport
{
	.report_id = 0,
	.report_size = XINPUT_ENDPOINT_SIZE,
	.buttons1 = 0,
	.buttons2 = 0,
	.lt = 0,
	.rt = 0,
	.lx = GAMEPAD_JOYSTICK_MID,
	.ly = GAMEPAD_JOYSTICK_MID,
	.rx = GAMEPAD_JOYSTICK_MID,
	.ry = GAMEPAD_JOYSTICK_MID,
	._reserved = { },
};

static NegconReport negconReport
{
	.square_btn = 0, .cross_btn = 0, .circle_btn = 0, .triangle_btn = 0,
	.l1_btn = 0, .r1_btn = 0, .l2_btn = 0, .r2_btn = 0,
	.select_btn = 0, .start_btn = 0, .l3_btn = 0, .r3_btn = 0, .ps_btn = 0,
	.direction = 0x08,
	.l_x_axis = 0x80, .l_y_axis = 0x80, .r_x_axis = 0x80, .r_y_axis = 0x80,
	.paddle_axis = 0x80,
};

static JogconReport jogconReport
{
	.buttons = 0,
	// .f_spn_l_btn = 0, .f_spn_r_btn = 0,
	// .select_btn = 0, .start_btn = 0,
	// .empty_btn = 0,
	// .l2_btn = 0, .r2_btn = 0, .l1_btn = 0, .r1_btn = 0,
	// .triangle_btn = 0, .circle_btn = 0, .cross_btn = 0, .square_btn = 0,
	.l_x_axis = 0x00, .l_y_axis = 0x00,
	.spinner_axis = 0x00, .paddle_axis = 0x00,
	.direction = 0x08,
};

static JogconMouse0Report jogconMouse0Report
{
	.id = 1,
	.buttons = 0,
	.direction = 0x08,
};

static JogconMouse1Report jogconMouse1Report
{
	.id = 2,
	.x = 0, .y = 0,
};

static GunconReport gunconReport
{
	.buttons = 0,
	.x = 0, .y = 0,
};


void *MPG::getReport(const uint8_t index)
{
	switch (options.inputMode)
	{
		case INPUT_MODE_XINPUT:
			return getXInputReport(index);

		case INPUT_MODE_SWITCH:
			return getSwitchReport(index);

		case INPUT_MODE_PS3:
			return getPS3Report(index);

		case INPUT_MODE_HID_NEGCON:
			return getNegconReport(index);
		
		case INPUT_MODE_HID_JOGCON:
			return getJogconReport(index);

		case INPUT_MODE_HID_JOGCON_MOUSE:
		{
			if (index == 0)
				return getJogconMouse0Report(index);
			else
				return getJogconMouse1Report(index);
		}

		case INPUT_MODE_HID_GUNCON:
			return getGunconReport(index);

		default:
			return getHIDReport(index);
	}
}


uint16_t MPG::getReportSize(const uint8_t index)
{
	switch (options.inputMode)
	{
		case INPUT_MODE_XINPUT:
			return sizeof(XInputReport);

		case INPUT_MODE_SWITCH:
			return sizeof(SwitchReport);

		case INPUT_MODE_PS3:
			return sizeof(PS3Report);

		case INPUT_MODE_HID_NEGCON:
			return sizeof(NegconReport);

		case INPUT_MODE_HID_JOGCON:
			return sizeof(JogconReport);

		case INPUT_MODE_HID_JOGCON_MOUSE:
		{
			if (index == 0)
				return sizeof(JogconMouse0Report);
			else
				return sizeof(JogconMouse1Report);
		}

		case INPUT_MODE_HID_GUNCON:
			return sizeof(GunconReport);

		default:
			return sizeof(HIDReport);
	}
}


PS3Report *MPG::getPS3Report(const uint8_t index)
{
	switch (state[index].dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        ps3Report.direction = HID_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   ps3Report.direction = HID_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     ps3Report.direction = HID_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: ps3Report.direction = HID_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      ps3Report.direction = HID_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  ps3Report.direction = HID_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      ps3Report.direction = HID_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    ps3Report.direction = HID_HAT_UPLEFT;    break;
		default:                                     ps3Report.direction = HID_HAT_NOTHING;   break;
	}

	ps3Report.cross_btn    = pressedB1(index);
	ps3Report.circle_btn   = pressedB2(index);
	ps3Report.square_btn   = pressedB3(index);
	ps3Report.triangle_btn = pressedB4(index);
	ps3Report.l1_btn       = pressedL1(index);
	ps3Report.r1_btn       = pressedR1(index);
	ps3Report.l2_btn       = pressedL2(index);
	ps3Report.r2_btn       = pressedR2(index);
	ps3Report.select_btn   = pressedS1(index);
	ps3Report.start_btn    = pressedS2(index);
	ps3Report.l3_btn       = pressedL3(index);
	ps3Report.r3_btn       = pressedR3(index);
	ps3Report.ps_btn       = pressedA1(index);
//	ps3Report.cross_btn = pressedA2();

	ps3Report.l_x_axis = static_cast<uint8_t>(state[index].lx >> 8);
	ps3Report.l_y_axis = static_cast<uint8_t>(state[index].ly >> 8);
	ps3Report.r_x_axis = static_cast<uint8_t>(state[index].rx >> 8);
	ps3Report.r_y_axis = static_cast<uint8_t>(state[index].ry >> 8);

	if (hasAnalogTriggers[index])
	{
		ps3Report.l2_axis = state[index].lt;
		ps3Report.r2_axis = state[index].rt;
	}
	else
	{
		ps3Report.l2_axis = pressedL2(index) ? 0xFF : 0;
		ps3Report.r2_axis = pressedR2(index) ? 0xFF : 0;
	}

	return &ps3Report;
}

SwitchReport *MPG::getSwitchReport(const uint8_t index)
{
	switch (state[index].dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        switchReport.hat = SWITCH_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   switchReport.hat = SWITCH_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     switchReport.hat = SWITCH_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: switchReport.hat = SWITCH_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      switchReport.hat = SWITCH_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  switchReport.hat = SWITCH_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      switchReport.hat = SWITCH_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    switchReport.hat = SWITCH_HAT_UPLEFT;    break;
		default:                                     switchReport.hat = SWITCH_HAT_NOTHING;   break;
	}

	switchReport.buttons = 0
		| (pressedB1(index) ? SWITCH_MASK_B       : 0)
		| (pressedB2(index) ? SWITCH_MASK_A       : 0)
		| (pressedB3(index) ? SWITCH_MASK_Y       : 0)
		| (pressedB4(index) ? SWITCH_MASK_X       : 0)
		| (pressedL1(index) ? SWITCH_MASK_L       : 0)
		| (pressedR1(index) ? SWITCH_MASK_R       : 0)
		| (pressedL2(index) ? SWITCH_MASK_ZL      : 0)
		| (pressedR2(index) ? SWITCH_MASK_ZR      : 0)
		| (pressedS1(index) ? SWITCH_MASK_MINUS   : 0)
		| (pressedS2(index) ? SWITCH_MASK_PLUS    : 0)
		| (pressedL3(index) ? SWITCH_MASK_L3      : 0)
		| (pressedR3(index) ? SWITCH_MASK_R3      : 0)
		| (pressedA1(index) ? SWITCH_MASK_HOME    : 0)
		| (pressedA2(index) ? SWITCH_MASK_CAPTURE : 0)
	;

	switchReport.lx = static_cast<uint8_t>(state[index].lx >> 8);
	switchReport.ly = static_cast<uint8_t>(state[index].ly >> 8);
	switchReport.rx = static_cast<uint8_t>(state[index].rx >> 8);
	switchReport.ry = static_cast<uint8_t>(state[index].ry >> 8);

	return &switchReport;
}

HIDReport *MPG::getHIDReport(const uint8_t index)
{
	switch (state[index].dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        hidReport.hat = SWITCH_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   hidReport.hat = SWITCH_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     hidReport.hat = SWITCH_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: hidReport.hat = SWITCH_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      hidReport.hat = SWITCH_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  hidReport.hat = SWITCH_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      hidReport.hat = SWITCH_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    hidReport.hat = SWITCH_HAT_UPLEFT;    break;
		default:                                     hidReport.hat = SWITCH_HAT_NOTHING;   break;
	}

	// hidReport.buttons = 0
	// 	| (pressedB1(index) ? SWITCH_MASK_B       : 0)
	// 	| (pressedB2(index) ? SWITCH_MASK_A       : 0)
	// 	| (pressedB3(index) ? SWITCH_MASK_Y       : 0)
	// 	| (pressedB4(index) ? SWITCH_MASK_X       : 0)
	// 	| (pressedL1(index) ? SWITCH_MASK_L       : 0)
	// 	| (pressedR1(index) ? SWITCH_MASK_R       : 0)
	// 	| (pressedL2(index) ? SWITCH_MASK_ZL      : 0)
	// 	| (pressedR2(index) ? SWITCH_MASK_ZR      : 0)
	// 	| (pressedS1(index) ? SWITCH_MASK_MINUS   : 0)
	// 	| (pressedS2(index) ? SWITCH_MASK_PLUS    : 0)
	// 	| (pressedL3(index) ? SWITCH_MASK_L3      : 0)
	// 	| (pressedR3(index) ? SWITCH_MASK_R3      : 0)
	// 	| (pressedA1(index) ? SWITCH_MASK_HOME    : 0)
	// 	| (pressedA2(index) ? SWITCH_MASK_CAPTURE : 0)
	// ;

	hidReport.buttons = 0
		| (pressedB1(index) ? BTN_SOUTH   : 0)
		| (pressedB2(index) ? BTN_EAST    : 0)
		| (pressedB3(index) ? BTN_NORTH   : 0)
		| (pressedB4(index) ? BTN_WEST    : 0)
		| (pressedL1(index) ? BTN_TL      : 0)
		| (pressedR1(index) ? BTN_TR      : 0)
		| (pressedL2(index) ? BTN_TL2     : 0)
		| (pressedR2(index) ? BTN_TR2     : 0)
		| (pressedS1(index) ? BTN_SELECT  : 0)
		| (pressedS2(index) ? BTN_START   : 0)
		| (pressedL3(index) ? BTN_THUMBL  : 0)
		| (pressedR3(index) ? BTN_THUMBR  : 0)
		| (pressedA1(index) ? BTN_MODE    : 0)
		// | (pressedA2(index) ? SWITCH_MASK_CAPTURE : 0)
	;

	//todo add all extra buttons (NTT, JAGUAR)

	hidReport.lx = static_cast<uint8_t>(state[index].lx >> 8);
	hidReport.ly = static_cast<uint8_t>(state[index].ly >> 8);
	hidReport.rx = static_cast<uint8_t>(state[index].rx >> 8);
	hidReport.ry = static_cast<uint8_t>(state[index].ry >> 8);

	//hidReport.trigger = HID_JOYSTICK_MID;

	// if (hasAnalogTriggers[index])
	// {
 	// 	//combine the two axes and use only half precision for each
	// 	hidReport.trigger = ((state[index].lt - state[index].rt) >> 1) + HID_JOYSTICK_MID;
	// 	// hidReport.lt = state[index].lt;
	// 	// hidReport.rt = state[index].rt;
	// }
	// else
	// {
	// 	if (pressedL2(index) && !pressedR2(index))
	// 		hidReport.trigger = HID_JOYSTICK_MAX;
	// 	else if (!pressedL2(index) && pressedR2(index))
	// 		hidReport.trigger = HID_JOYSTICK_MIN;
	// 	else
	// 		hidReport.trigger = HID_JOYSTICK_MID;
	// 	// hidReport.lt = pressedL2(index) ? 0xFF : 0;
	// 	// hidReport.rt = pressedR2(index) ? 0xFF : 0;
	// }

	return &hidReport;
}


XInputReport *MPG::getXInputReport(const uint8_t index)
{
	xinputReport.buttons1 = 0
		| (pressedUp(index)    ? XBOX_MASK_UP    : 0)
		| (pressedDown(index)  ? XBOX_MASK_DOWN  : 0)
		| (pressedLeft(index)  ? XBOX_MASK_LEFT  : 0)
		| (pressedRight(index) ? XBOX_MASK_RIGHT : 0)
		| (pressedS2(index)    ? XBOX_MASK_START : 0)
		| (pressedS1(index)    ? XBOX_MASK_BACK  : 0)
		| (pressedL3(index)    ? XBOX_MASK_LS    : 0)
		| (pressedR3(index)    ? XBOX_MASK_RS    : 0)
	;

	xinputReport.buttons2 = 0
		| (pressedL1(index) ? XBOX_MASK_LB   : 0)
		| (pressedR1(index) ? XBOX_MASK_RB   : 0)
		| (pressedA1(index) ? XBOX_MASK_HOME : 0)
		| (pressedB1(index) ? XBOX_MASK_A    : 0)
		| (pressedB2(index) ? XBOX_MASK_B    : 0)
		| (pressedB3(index) ? XBOX_MASK_X    : 0)
		| (pressedB4(index) ? XBOX_MASK_Y    : 0)
	;

	xinputReport.lx = static_cast<int16_t>(state[index].lx) + INT16_MIN;
	xinputReport.ly = static_cast<int16_t>(~state[index].ly) + INT16_MIN;
	xinputReport.rx = static_cast<int16_t>(state[index].rx) + INT16_MIN;
	xinputReport.ry = static_cast<int16_t>(~state[index].ry) + INT16_MIN;

	if (hasAnalogTriggers[index])
	{
		xinputReport.lt = state[index].lt;
		xinputReport.rt = state[index].rt;
	}
	else
	{
		xinputReport.lt = pressedL2(index) ? 0xFF : 0;
		xinputReport.rt = pressedR2(index) ? 0xFF : 0;
	}

	return &xinputReport;
}

NegconReport *MPG::getNegconReport(const uint8_t index)
{
	switch (state[index].dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        negconReport.direction = HID_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   negconReport.direction = HID_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     negconReport.direction = HID_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: negconReport.direction = HID_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      negconReport.direction = HID_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  negconReport.direction = HID_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      negconReport.direction = HID_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    negconReport.direction = HID_HAT_UPLEFT;    break;
		default:                                     negconReport.direction = HID_HAT_NOTHING;   break;
	}

	negconReport.cross_btn    = pressedB1(index);
	negconReport.circle_btn   = pressedB2(index);
	negconReport.square_btn   = pressedB3(index);
	negconReport.triangle_btn = pressedB4(index);
	negconReport.l1_btn       = pressedL1(index);
	negconReport.r1_btn       = pressedR1(index);
	negconReport.l2_btn       = pressedL2(index);
	negconReport.r2_btn       = pressedR2(index);
	negconReport.select_btn   = pressedS1(index);
	negconReport.start_btn    = pressedS2(index);
	negconReport.l3_btn       = pressedL3(index);
	negconReport.r3_btn       = pressedR3(index);
	negconReport.ps_btn       = pressedA1(index);

	negconReport.l_x_axis = static_cast<uint8_t>(state[index].lx >> 8);
	negconReport.l_y_axis = static_cast<uint8_t>(state[index].ly >> 8);
	negconReport.r_x_axis = static_cast<uint8_t>(state[index].rx >> 8);
	negconReport.r_y_axis = static_cast<uint8_t>(state[index].ry >> 8);
	negconReport.paddle_axis = static_cast<uint8_t>(state[index].lx >> 8);

	return &negconReport;
}

JogconReport *MPG::getJogconReport(const uint8_t index)
{
	switch (state[index].dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        jogconReport.direction = HID_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   jogconReport.direction = HID_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     jogconReport.direction = HID_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: jogconReport.direction = HID_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      jogconReport.direction = HID_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  jogconReport.direction = HID_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      jogconReport.direction = HID_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    jogconReport.direction = HID_HAT_UPLEFT;    break;
		default:                                     jogconReport.direction = HID_HAT_NOTHING;   break;
	}

	jogconReport.buttons = 0
		| (pressedB1(index) ? BTN_SOUTH   : 0)
		| (pressedB2(index) ? BTN_EAST    : 0)
		| (pressedB3(index) ? BTN_NORTH   : 0)
		| (pressedB4(index) ? BTN_WEST    : 0)
		| (pressedL1(index) ? BTN_TL      : 0)
		| (pressedR1(index) ? BTN_TR      : 0)
		| (pressedL2(index) ? BTN_TL2     : 0)
		| (pressedR2(index) ? BTN_TR2     : 0)
		| (pressedS1(index) ? BTN_SELECT  : 0)
		| (pressedS2(index) ? BTN_START   : 0)
		| (pressedL3(index) ? BTN_THUMBL  : 0)
		| (pressedR3(index) ? BTN_THUMBR  : 0)
		// | (pressedA1(index) ? BTN_MODE    : 0)
		// | (pressedA2(index) ? SWITCH_MASK_CAPTURE : 0)
	;

	// jogconReport.cross_btn    = pressedB1(index);
	// jogconReport.circle_btn   = pressedB2(index);
	// jogconReport.square_btn   = pressedB3(index);
	// jogconReport.triangle_btn = pressedB4(index);
	// jogconReport.l1_btn       = pressedL1(index);
	// jogconReport.r1_btn       = pressedR1(index);
	// jogconReport.l2_btn       = pressedL2(index);
	// jogconReport.r2_btn       = pressedR2(index);
	// jogconReport.select_btn   = pressedS1(index);
	// jogconReport.start_btn    = pressedS2(index);
	// jogconReport.f_spn_l_btn  = pressedL3(index);
	// jogconReport.f_spn_r_btn  = pressedR3(index);
	//jogconReport.ps_btn       = pressedA1(index);
	//jogconReport.buttons = state[index].buttons >> 8;

	//jogconReport.l_x_axis = static_cast<uint8_t>(state[index].lx >> 8);
	//jogconReport.l_y_axis = static_cast<uint8_t>(state[index].ly >> 8);
	//jogconReport.r_x_axis = static_cast<uint8_t>(state[index].rx >> 8);
	//jogconReport.r_y_axis = static_cast<uint8_t>(state[index].ry >> 8);
	jogconReport.paddle_axis = static_cast<int8_t>(state[index].lx);
	jogconReport.spinner_axis = static_cast<int8_t>(state[index].aux);

	return &jogconReport;
}

JogconMouse0Report *MPG::getJogconMouse0Report(const uint8_t index)
{
	jogconMouse0Report.id    = 1;

	switch (state[index].dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        jogconMouse0Report.direction = HID_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   jogconMouse0Report.direction = HID_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     jogconMouse0Report.direction = HID_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: jogconMouse0Report.direction = HID_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      jogconMouse0Report.direction = HID_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  jogconMouse0Report.direction = HID_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      jogconMouse0Report.direction = HID_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    jogconMouse0Report.direction = HID_HAT_UPLEFT;    break;
		default:                                     jogconMouse0Report.direction = HID_HAT_NOTHING;   break;
	}

	jogconMouse0Report.buttons = 0
		| (pressedB1(index) ? BTN_SOUTH   : 0)
		| (pressedB2(index) ? BTN_EAST    : 0)
		| (pressedB3(index) ? BTN_NORTH   : 0)
		| (pressedB4(index) ? BTN_WEST    : 0)
		| (pressedL1(index) ? BTN_TL      : 0)
		| (pressedR1(index) ? BTN_TR      : 0)
		| (pressedL2(index) ? BTN_TL2     : 0)
		| (pressedR2(index) ? BTN_TR2     : 0)
		| (pressedS1(index) ? BTN_SELECT  : 0)
		| (pressedS2(index) ? BTN_START   : 0)
		| (pressedL3(index) ? BTN_THUMBL  : 0)
		| (pressedR3(index) ? BTN_THUMBR  : 0)
		// | (pressedA1(index) ? BTN_MODE    : 0)
		// | (pressedA2(index) ? SWITCH_MASK_CAPTURE : 0)
	;
	
	return &jogconMouse0Report;
}

JogconMouse1Report *MPG::getJogconMouse1Report(const uint8_t index)
{
	jogconMouse1Report.id = 2;
	jogconMouse1Report.x  = static_cast<int8_t>(state[0].aux);
	jogconMouse1Report.y  = 0;

	return &jogconMouse1Report;
}

GunconReport *MPG::getGunconReport(const uint8_t index)
{
	gunconReport.buttons = 0
		| (pressedB1(index) ? BTN_SOUTH   : 0)
		| (pressedB2(index) ? BTN_EAST    : 0)
		| (pressedB3(index) ? BTN_NORTH   : 0)
		| (pressedB4(index) ? BTN_WEST    : 0)
		// | (pressedL1(index) ? BTN_TL      : 0)
		// | (pressedR1(index) ? BTN_TR      : 0)
		// | (pressedL2(index) ? BTN_TL2     : 0)
		// | (pressedR2(index) ? BTN_TR2     : 0)
		// | (pressedS1(index) ? BTN_SELECT  : 0)
		// | (pressedS2(index) ? BTN_START   : 0)
		// | (pressedL3(index) ? BTN_THUMBL  : 0)
		// | (pressedR3(index) ? BTN_THUMBR  : 0)
		// | (pressedA1(index) ? BTN_MODE    : 0)
		// | (pressedA2(index) ? SWITCH_MASK_CAPTURE : 0)
	;
	gunconReport.x  = state[index].lx;
	gunconReport.y  = state[index].ly;

	return &gunconReport;
}

GamepadHotkey MPG::hotkey(const uint8_t index)
{
	static GamepadHotkey lastAction = HOTKEY_NONE;

	GamepadHotkey action = HOTKEY_NONE;
	if (pressedF1(index))
	{
		switch (state[index].dpad & GAMEPAD_MASK_DPAD)
		{
			case GAMEPAD_MASK_LEFT:
				action = HOTKEY_DPAD_LEFT_ANALOG;
				options.dpadMode = DPAD_MODE_LEFT_ANALOG;
				state[index].dpad = 0;
				state[index].buttons &= ~(f1Mask);
				break;

			case GAMEPAD_MASK_RIGHT:
				action = HOTKEY_DPAD_RIGHT_ANALOG;
				options.dpadMode = DPAD_MODE_RIGHT_ANALOG;
				state[index].dpad = 0;
				state[index].buttons &= ~(f1Mask);
				break;

			case GAMEPAD_MASK_DOWN:
				action = HOTKEY_DPAD_DIGITAL;
				options.dpadMode = DPAD_MODE_DIGITAL;
				state[index].dpad = 0;
				state[index].buttons &= ~(f1Mask);
				break;

			case GAMEPAD_MASK_UP:
				action = HOTKEY_HOME_BUTTON;
				state[index].dpad = 0;
				state[index].buttons &= ~(f1Mask);
				state[index].buttons |= GAMEPAD_MASK_A1; // Press the Home button
				break;
		}
	}
	else if (pressedF2(index))
	{
		switch (state[index].dpad & GAMEPAD_MASK_DPAD)
		{
			case GAMEPAD_MASK_DOWN:
				action = HOTKEY_SOCD_NEUTRAL;
				options.socdMode = SOCD_MODE_NEUTRAL;
				state[index].dpad = 0;
				state[index].buttons &= ~(f2Mask);
				break;

			case GAMEPAD_MASK_UP:
				action = HOTKEY_SOCD_UP_PRIORITY;
				options.socdMode = SOCD_MODE_UP_PRIORITY;
				state[index].dpad = 0;
				state[index].buttons &= ~(f2Mask);
				break;

			case GAMEPAD_MASK_LEFT:
				action = HOTKEY_SOCD_LAST_INPUT;
				options.socdMode = SOCD_MODE_SECOND_INPUT_PRIORITY;
				state[index].dpad = 0;
				state[index].buttons &= ~(f2Mask);
				break;

			case GAMEPAD_MASK_RIGHT:
				if (lastAction != HOTKEY_INVERT_Y_AXIS)
					options.invertYAxis = !options.invertYAxis;
				action = HOTKEY_INVERT_Y_AXIS;
				state[index].dpad = 0;
				state[index].buttons &= ~(f2Mask);
				break;
		}
	}

	lastAction = action;
	return action;
}

void MPG::process(const uint8_t index)
{
	uint16_t analogCenter = options.inputMode == INPUT_MODE_SWITCH ? ((SWITCH_JOYSTICK_MID << 8) + SWITCH_JOYSTICK_MID) : GAMEPAD_JOYSTICK_MID;

	state[index].dpad = runSOCDCleaner(options.socdMode, state[index].dpad);

	switch (options.dpadMode)
	{
		case DpadMode::DPAD_MODE_LEFT_ANALOG:
			if (!hasRightAnalogStick[index]) {
				state[index].rx = analogCenter;
				state[index].ry = analogCenter;
			}
			state[index].lx = dpadToAnalogX(state[index].dpad);
			state[index].ly = dpadToAnalogY(state[index].dpad);
			state[index].dpad = 0;
			break;

		case DpadMode::DPAD_MODE_RIGHT_ANALOG:
			if (!hasLeftAnalogStick[index]) {
				state[index].lx = analogCenter;
				state[index].ly = analogCenter;
			}
			state[index].rx = dpadToAnalogX(state[index].dpad);
			state[index].ry = dpadToAnalogY(state[index].dpad);
			state[index].dpad = 0;
			break;

		default:
			if (!hasLeftAnalogStick[index]) {
				state[index].lx = analogCenter;
				state[index].ly = analogCenter;
			}
			if (!hasRightAnalogStick[index]) {
				state[index].rx = analogCenter;
				state[index].ry = analogCenter;
			}
			break;
	}
}

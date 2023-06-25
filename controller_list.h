/**************************************************************************/
/*  controller_list.h                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                          EIRTeam.InputGlyphs                           */
/*                         https://ph.eirteam.moe                         */
/**************************************************************************/
/* Copyright (c) 2023-present Álex Román (EIRTeam) & contributors.        */
/*                                                                        */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef CONTROLLER_LIST_H
#define CONTROLLER_LIST_H

#include "input_glyphs.h"

typedef struct
{
	unsigned int device_id;
	InputType controller_type;
	const char *name;
} ControllerDescription_t;

#define k_eControllerType_PS3Controller InputType::PS3_CONTROLLER
#define k_eControllerType_PS4Controller InputType::PS4_CONTROLLER
#define k_eControllerType_PS5Controller InputType::PS5_CONTROLLER
#define k_eControllerType_XInputPS4Controller InputType::PS4_CONTROLLER
#define k_eControllerType_UnknownNonSteamController InputType::UNKNOWN
#define k_eControllerType_XBox360Controller InputType::XBOX_360_CONTROLLER
#define k_eControllerType_XBoxOneController InputType::XBOX_ONE_CONTROLLER
#define k_eControllerType_AppleController InputType::GENERIC_XINPUT_CONTROLLER
#define k_eControllerType_SwitchJoyConLeft InputType::SWITCH_PRO_CONTROLLER
#define k_eControllerType_SwitchJoyConRight InputType::SWITCH_PRO_CONTROLLER
#define k_eControllerType_SwitchJoyConPair InputType::SWITCH_PRO_CONTROLLER
#define k_eControllerType_SwitchProController InputType::SWITCH_PRO_CONTROLLER
#define k_eControllerType_SwitchInputOnlyController InputType::SWITCH_PRO_CONTROLLER
#define k_eControllerType_XInputSwitchController InputType::SWITCH_PRO_CONTROLLER
#define k_eControllerType_MobileTouch InputType::STEAM_CONTROLLER
#define k_eControllerType_SteamController InputType::STEAM_CONTROLLER
#define k_eControllerType_SteamControllerV2 InputType::STEAM_CONTROLLER

#include "thirdparty/controller_list_sdl.h"

#endif // CONTROLLER_LIST_H

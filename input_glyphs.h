/**************************************************************************/
/*  input_glyphs.h                                                        */
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

#ifndef INPUT_GLYPHS_H
#define INPUT_GLYPHS_H

#include "core/variant/binder_common.h"
#include <iterator>

enum HBInputType {
	UNKNOWN,
	STEAM_CONTROLLER,
	XBOX_360_CONTROLLER,
	XBOX_ONE_CONTROLLER,
	GENERIC_XINPUT_CONTROLLER,
	PS3_CONTROLLER,
	PS4_CONTROLLER,
	PS5_CONTROLLER,
	SWITCH_PRO_CONTROLLER,
	STEAM_DECK_CONTROLLER,
	INPUT_TYPE_MAX
};
VARIANT_ENUM_CAST(HBInputType);

// TODO:
// d-pad move
enum HBInputOrigin {
	INPUT_ORIGIN_INVALID = -1,
	INPUT_ORIGIN_A,
	INPUT_ORIGIN_B,
	INPUT_ORIGIN_X,
	INPUT_ORIGIN_Y,
	INPUT_ORIGIN_VIEW, // Select/Back
	INPUT_ORIGIN_MENU, // Start
	INPUT_ORIGIN_LEFTBUMPER,
	INPUT_ORIGIN_RIGHTBUMPER,
	INPUT_ORIGIN_LEFTTRIGGER_PULL,
	INPUT_ORIGIN_LEFTTRIGGER_CLICK,
	INPUT_ORIGIN_RIGHTTRIGGER_PULL,
	INPUT_ORIGIN_RIGHTTRIGGER_CLICK,
	INPUT_ORIGIN_LEFTSTICK_MOVE,
	INPUT_ORIGIN_LEFTSTICK_CLICK,
	INPUT_ORIGIN_LEFTSTICK_DPADNORTH,
	INPUT_ORIGIN_LEFTSTICK_DPADSOUTH,
	INPUT_ORIGIN_LEFTSTICK_DPADWEST,
	INPUT_ORIGIN_LEFTSTICK_DPADEAST,
	INPUT_ORIGIN_RIGHTSTICK_MOVE,
	INPUT_ORIGIN_RIGHTSTICK_CLICK,
	INPUT_ORIGIN_RIGHTSTICK_DPADNORTH,
	INPUT_ORIGIN_RIGHTSTICK_DPADSOUTH,
	INPUT_ORIGIN_RIGHTSTICK_DPADWEST,
	INPUT_ORIGIN_RIGHTSTICK_DPADEAST,
	INPUT_ORIGIN_DPAD_NORTH,
	INPUT_ORIGIN_DPAD_SOUTH,
	INPUT_ORIGIN_DPAD_WEST,
	INPUT_ORIGIN_DPAD_EAST,
	INPUT_ORIGIN_DPAD_MOVE,
	INPUT_ORIGIN_MISC1, // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button
	INPUT_ORIGIN_PADDLE1, // Xbox Elite paddle P1 (upper left, facing the back)
	INPUT_ORIGIN_PADDLE2, // Xbox Elite paddle P3 (upper right, facing the back)
	INPUT_ORIGIN_PADDLE3, // Xbox Elite paddle P2 (lower left, facing the back)
	INPUT_ORIGIN_PADDLE4, // Xbox Elite paddle P4 (lower right, facing the back)
	INPUT_ORIGIN_TRACKPAD_CLICK,
	INPUT_ORIGIN_COUNT,
};
VARIANT_ENUM_CAST(HBInputOrigin);

namespace HBInputGlyphs {
const HBInputOrigin godot_button_to_input_origin_lut[] = {
	INPUT_ORIGIN_A,
	INPUT_ORIGIN_B,
	INPUT_ORIGIN_X,
	INPUT_ORIGIN_Y,
	INPUT_ORIGIN_VIEW,
	INPUT_ORIGIN_INVALID,
	INPUT_ORIGIN_MENU,
	INPUT_ORIGIN_LEFTSTICK_CLICK,
	INPUT_ORIGIN_RIGHTSTICK_CLICK,
	INPUT_ORIGIN_LEFTBUMPER,
	INPUT_ORIGIN_RIGHTBUMPER,
	INPUT_ORIGIN_DPAD_NORTH,
	INPUT_ORIGIN_DPAD_SOUTH,
	INPUT_ORIGIN_DPAD_WEST,
	INPUT_ORIGIN_DPAD_EAST,
	INPUT_ORIGIN_MISC1,
	INPUT_ORIGIN_PADDLE1,
	INPUT_ORIGIN_PADDLE2,
	INPUT_ORIGIN_PADDLE3,
	INPUT_ORIGIN_PADDLE4,
	INPUT_ORIGIN_TRACKPAD_CLICK,
};
static_assert(std::size(godot_button_to_input_origin_lut) == (int)JoyButton::SDL_MAX);
}; //namespace HBInputGlyphs

static const char *_debug_input_origin_names[] = {
	"INPUT_ORIGIN_A",
	"INPUT_ORIGIN_B",
	"INPUT_ORIGIN_X",
	"INPUT_ORIGIN_Y",
	"INPUT_ORIGIN_VIEW", // Select/Back
	"INPUT_ORIGIN_MENU", // Start
	"INPUT_ORIGIN_LEFTBUMPER",
	"INPUT_ORIGIN_RIGHTBUMPER",
	"INPUT_ORIGIN_LEFTTRIGGER_PULL",
	"INPUT_ORIGIN_LEFTTRIGGER_CLICK",
	"INPUT_ORIGIN_RIGHTTRIGGER_PULL",
	"INPUT_ORIGIN_RIGHTTRIGGER_CLICK",
	"INPUT_ORIGIN_LEFTSTICK_MOVE",
	"INPUT_ORIGIN_LEFTSTICK_CLICK",
	"INPUT_ORIGIN_LEFTSTICK_DPADNORTH",
	"INPUT_ORIGIN_LEFTSTICK_DPADSOUTH",
	"INPUT_ORIGIN_LEFTSTICK_DPADWEST",
	"INPUT_ORIGIN_LEFTSTICK_DPADEAST",
	"INPUT_ORIGIN_RIGHTSTICK_MOVE",
	"INPUT_ORIGIN_RIGHTSTICK_CLICK",
	"INPUT_ORIGIN_RIGHTSTICK_DPADNORTH",
	"INPUT_ORIGIN_RIGHTSTICK_DPADSOUTH",
	"INPUT_ORIGIN_RIGHTSTICK_DPADWEST",
	"INPUT_ORIGIN_RIGHTSTICK_DPADEAST",
	"INPUT_ORIGIN_DPAD_NORTH",
	"INPUT_ORIGIN_DPAD_SOUTH",
	"INPUT_ORIGIN_DPAD_WEST",
	"INPUT_ORIGIN_DPAD_EAST",
	"INPUT_ORIGIN_DPAD_MOVE",
	"INPUT_ORIGIN_MISC1", // Xbox Series X share button", PS5 microphone button", Nintendo Switch Pro capture button", Amazon Luna microphone button
	"INPUT_ORIGIN_PADDLE1", // Xbox Elite paddle P1 (upper left", facing the back)
	"INPUT_ORIGIN_PADDLE2", // Xbox Elite paddle P3 (upper right", facing the back)
	"INPUT_ORIGIN_PADDLE3", // Xbox Elite paddle P2 (lower left", facing the back)
	"INPUT_ORIGIN_PADDLE4", // Xbox Elite paddle P4 (lower right", facing the back)
	"INPUT_ORIGIN_TRACKPAD_CLICK",
};

static_assert(std::size(_debug_input_origin_names) == HBInputOrigin::INPUT_ORIGIN_COUNT);

enum HBInputGlyphStyle {
	// Base-styles - cannot mix
	GLYPH_STYLE_KNOCKOUT = 0x0,
	GLYPH_STYLE_LIGHT = 0x1,
	GLYPH_STYLE_DARK = 0x2,
	GLYPH_STYLE_THEME_COUNT = 0x3,
	// Modifiers
	// Default ABXY/PS equivalent glyphs have a solid fill w/ color matching the physical buttons on the device
	GLYPH_STYLE_NEUTRAL_COLOR_ABXY = 0x10, // ABXY Buttons will match the base style color instead of their normal associated color
	GLYPH_STYLE_SOLID_ABXY = 0x20, // ABXY Buttons will have a solid fill
};

VARIANT_ENUM_CAST(HBInputGlyphStyle);

enum HBInputGlyphSize {
	GLYPH_SIZE_SMALL,
	GLYPH_SIZE_MEDIUM,
	GLYPH_SIZE_LARGE,
	GLYPH_SIZE_MAX,
};
VARIANT_ENUM_CAST(HBInputGlyphSize);

#endif // INPUT_GLYPHS_H

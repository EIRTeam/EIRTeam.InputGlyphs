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
#include "input_glyphs_constants.h"
#include <iterator>

enum InputGlyphStyle {
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

VARIANT_BITFIELD_CAST(InputGlyphStyle);

enum InputGlyphSize {
	GLYPH_SIZE_SMALL,
	GLYPH_SIZE_MEDIUM,
	GLYPH_SIZE_LARGE,
	GLYPH_SIZE_MAX,
};
VARIANT_ENUM_CAST(InputGlyphSize);

#endif // INPUT_GLYPHS_H

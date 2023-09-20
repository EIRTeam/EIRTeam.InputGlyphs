/**************************************************************************/
/*  input_glyphs_source.h                                                 */
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

#ifndef INPUT_GLYPHS_SOURCE_H
#define INPUT_GLYPHS_SOURCE_H

#include "core/object/ref_counted.h"
#include "input_glyphs.h"
#include "input_glyphs_constants.h"
#include "scene/resources/texture.h"

class InputGlyphsSource : public RefCounted {
	GDCLASS(InputGlyphsSource, RefCounted);

protected:
	static Ref<InputGlyphsSource> (*_create_func)();

public:
	virtual Ref<Texture2D> get_input_glyph(const InputGlyphsConstants::InputType &p_input_type, const InputGlyphsConstants::InputOrigin &p_input_origin, const BitField<InputGlyphStyle> &p_glyphs_style, const InputGlyphSize &p_size) = 0;
	static Ref<InputGlyphsSource> create();
	virtual InputGlyphsConstants::InputType identify_joy(int p_device) const = 0;
};

class InputGlyphsSourceBuiltin : public InputGlyphsSource {
	GDCLASS(InputGlyphsSourceBuiltin, InputGlyphsSource);

private:
	virtual Ref<Texture2D> get_input_glyph(const InputGlyphsConstants::InputType &p_input_type, const InputGlyphsConstants::InputOrigin &p_input_origin, const BitField<InputGlyphStyle> &p_glyphs_style, const InputGlyphSize &p_size) override;

public:
	static Ref<InputGlyphsSource> _create_current() {
		Ref<InputGlyphsSourceBuiltin> ref;
		ref.instantiate();
		return ref;
	}

	static void make_current() {
		_create_func = _create_current;
	}
	virtual InputGlyphsConstants::InputType identify_joy(int identify_joy) const override;
};

#endif // INPUT_GLYPHS_SOURCE_H

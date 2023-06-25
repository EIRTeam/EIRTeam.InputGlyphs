/**************************************************************************/
/*  input_glyph_texture_rect.h                                            */
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

#ifndef INPUT_GLYPH_TEXTURE_RECT_H
#define INPUT_GLYPH_TEXTURE_RECT_H

#include "input_glyphs.h"
#include "scene/gui/texture_rect.h"

class InputGlyphTextureRect : public TextureRect {
	GDCLASS(InputGlyphTextureRect, TextureRect);

private:
	bool override_glyph_style = false;
	int glyph_style_override = InputGlyphStyle::GLYPH_STYLE_KNOCKOUT;
	InputOrigin origin = InputOrigin::INPUT_ORIGIN_A;
	StringName action_name = "ui_left";

	int _get_glyph_style_with_override() const;
	void _queue_texture_update();
	void _on_input_glyphs_changed();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	bool get_override_glyph_style() const;
	void set_override_glyph_style(bool p_override_glyph_style);

	int get_glyph_style_override() const;
	void set_glyph_style_override(int p_glyph_style_override);

	StringName get_action_name() const;
	void set_action_name(const StringName &p_action_name);
	InputGlyphTextureRect();
};

#endif // INPUT_GLYPH_TEXTURE_RECT_H

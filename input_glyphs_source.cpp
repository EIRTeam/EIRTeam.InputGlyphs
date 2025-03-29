/**************************************************************************/
/*  input_glyphs_source.cpp                                               */
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

#include "input_glyphs_source.h"
#include "controller_list.h"
#include "input_glyph_svg_decode.h"
#include "modules/input_glyphs/glyphs.gen.h"
#include "scene/resources/image_texture.h"
#include "scene/resources/placeholder_textures.h"
#include <iterator>

Ref<InputGlyphsSource> (*InputGlyphsSource::_create_func)() = nullptr;

Ref<InputGlyphsSource> InputGlyphsSource::create() {
	if (!_create_func) {
		return nullptr;
	}
	return _create_func();
}

Ref<Texture2D> InputGlyphsSourceBuiltin::get_input_glyph(const InputGlyphsConstants::InputType &p_input_type, const InputGlyphsConstants::InputOrigin &p_input_origin, const BitField<InputGlyphStyle> &p_glyphs_style, const InputGlyphSize &p_size) {
	int theme = p_glyphs_style & 0b11;
	int abxy_overrides = p_glyphs_style & 0b110000;
	abxy_overrides = abxy_overrides >> 4;

	Vector2i size = Vector2i(32, 32);
	switch (p_size) {
		case GLYPH_SIZE_LARGE: {
			size = Vector2i(256, 256);
		} break;
		case GLYPH_SIZE_MEDIUM: {
			size = Vector2i(128, 128);
		} break;
		default:
		case GLYPH_SIZE_SMALL: {
			size = Vector2i(32, 32);
		} break;
	}

	if (p_input_origin == InputGlyphsConstants::INPUT_ORIGIN_INVALID) {
		Ref<PlaceholderTexture2D> placeholder = memnew(PlaceholderTexture2D);
		placeholder->set_size(size);
		return placeholder;
	}

	if (theme > 2) {
		Ref<PlaceholderTexture2D> placeholder = memnew(PlaceholderTexture2D);
		placeholder->set_size(size);
		ERR_FAIL_V_MSG(placeholder, vformat("Invalid theme index: %d", theme));
	}

	int svg_index = -1;

	if (p_input_origin <= InputGlyphsConstants::InputOrigin::INPUT_ORIGIN_Y && abxy_overrides != 0) {
		svg_index = __glyph_icons_abxy_override_map[p_input_type][theme][((abxy_overrides - 1) * 4) + p_input_origin];
	} else {
		svg_index = __glyph_icons_map[p_input_type][theme][p_input_origin];
	}

	if (svg_index == -1) {
		Ref<PlaceholderTexture2D> placeholder = memnew(PlaceholderTexture2D);
		placeholder->set_size(size);
		return placeholder;
	}

	const char *svg_data = __glyph_icons[svg_index];

	Ref<Image> out_image;
	out_image.instantiate();
	// We convert it to a String and then into a PackedByteArray so we can get the proper bounds
	PackedByteArray pba = String(svg_data).to_utf8_buffer();
	if (InputGlyphSVGDecode::render_svg(out_image, pba, size) != OK) {
		Ref<PlaceholderTexture2D> placeholder = memnew(PlaceholderTexture2D);
		placeholder->set_size(size);
		return placeholder;
	}
	Ref<ImageTexture> image_texture = ImageTexture::create_from_image(out_image);
	image_texture->set_meta("glyph_path", "BUILT_IN");
	return image_texture;
}

static unsigned char nibble(unsigned char c) {
	// Shamelessly lifted from SDL
	if ((c >= '0') && (c <= '9')) {
		return c - '0';
	}

	if ((c >= 'A') && (c <= 'F')) {
		return c - 'A' + 0x0a;
	}

	if ((c >= 'a') && (c <= 'f')) {
		return c - 'a' + 0x0a;
	}

	/* received an invalid character, and no real way to return an error */
	/* AssertMsg1(false, "Q_nibble invalid hex character '%c' ", c); */
	return 0;
}

void controller_guid_string_decode(const String &p_guid_str, uint16_t *vendor, uint16_t *product, uint16_t *version) {
	// Shamelessly lifted from SDL
	uint8_t guid_data[16] = { 0 };
	int maxoutputbytes = sizeof(guid_data);
	size_t len = p_guid_str.length();
	uint8_t *p;
	size_t i;

	/* Make sure it's even */
	len = (len) & ~0x1;

	p = (uint8_t *)&guid_data;
	const char *pchGUID = p_guid_str.ascii().ptr();
	for (i = 0; (i < len) && ((p - (uint8_t *)&guid_data) < maxoutputbytes); i += 2, p++) {
		*p = (nibble((unsigned char)pchGUID[i]) << 4) | nibble((unsigned char)pchGUID[i + 1]);
	}

	uint16_t *guid_16 = (uint16_t *)guid_data;
	*vendor = guid_16[2];
	*product = guid_16[4];
	*version = guid_16[6];
}

InputGlyphsConstants::InputType InputGlyphsSourceBuiltin::identify_joy(int p_controller_idx) const {
	String guid_str = Input::get_singleton()->get_joy_guid(p_controller_idx);
	uint16_t vendor, product, version;
	controller_guid_string_decode(guid_str, &vendor, &product, &version);
	uint64_t controller_id = MAKE_CONTROLLER_ID(vendor, product);

	for (int i = 0; i < (int)std::size(arrControllers); i++) {
		if (arrControllers[i].device_id == controller_id) {
			return arrControllers[i].controller_type;
		}
	}

	return InputGlyphsConstants::UNKNOWN;
}

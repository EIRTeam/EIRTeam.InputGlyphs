/**************************************************************************/
/*  input_glyphs_singleton.h                                              */
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

#ifndef INPUT_GLYPHS_SINGLETON_H
#define INPUT_GLYPHS_SINGLETON_H

#include "core/input/input_event.h"
#include "core/object/class_db.h"
#include "core/object/object.h"

#include "input_glyphs_source.h"

#include "input_glyphs.h"

namespace HBInputGlyphs {
typedef uint64_t GlyphUID;
static GlyphUID calculate_guid(const HBInputOrigin &p_input_origin, const int &p_style, const HBInputGlyphSize &p_glyph_size) {
	GlyphUID uid;
	uid = p_input_origin << 7;
	uid |= p_style << 2;
	uid |= p_glyph_size;
	return uid;
}
} //namespace HBInputGlyphs

class InputGlyphsSingleton : public Object {
	GDCLASS(InputGlyphsSingleton, Object);

	struct GlyphInfo {
		HBInputOrigin origin;
		int style;
		HBInputGlyphSize size;
		HBInputGlyphs::GlyphUID get_uid() {
			return HBInputGlyphs::calculate_guid(origin, style, size);
		}
	};

	HashMap<HBInputGlyphs::GlyphUID, Ref<Texture2D>> loaded_glyphs;

	struct GlyphLoadTask {
		bool aborted = false;
		Mutex task_mutex;
		GlyphInfo glyph_info;
		WorkerThreadPool::TaskID task_id;
		HBInputType input_type;
		Ref<Texture2D> texture;
	};

	Mutex mutex;
	HashMap<HBInputGlyphs::GlyphUID, GlyphLoadTask *> current_tasks;

	static void _join_task(WorkerThreadPool::TaskID p_task_id);
	static void _glyph_loaded_callback(GlyphLoadTask *p_task);

	static InputGlyphsSingleton *singleton;
	void _input_event(const Ref<InputEvent> &p_input_event);
	HBInputType _get_input_type() const;

public:
	Ref<InputGlyphsSource> glyph_source;

private:
	HBInputType current_input_type = HBInputType::UNKNOWN;
	HBInputType forced_input_type = HBInputType::UNKNOWN;
	HBInputGlyphStyle default_glyph_style = HBInputGlyphStyle::GLYPH_STYLE_KNOCKOUT;
	HBInputGlyphSize default_glyph_size = HBInputGlyphSize::GLYPH_SIZE_SMALL;

	static void _load_glyph_thread(void *p_userdata);

	void _on_input_type_changed();

protected:
	static void _bind_methods();

public:
	void init();
	bool has_glyph_texture(const HBInputOrigin p_input_origin, BitField<HBInputGlyphStyle> p_style, const HBInputGlyphSize p_size = HBInputGlyphSize::GLYPH_SIZE_MAX);
	Ref<Texture2D> get_glyph_texture(const HBInputOrigin p_input_origin, BitField<HBInputGlyphStyle> p_style, const HBInputGlyphSize p_size = HBInputGlyphSize::GLYPH_SIZE_MAX);
	void request_glyph_texture_load(const HBInputOrigin p_input_origin, BitField<HBInputGlyphStyle> p_style, const HBInputGlyphSize p_size = HBInputGlyphSize::GLYPH_SIZE_MAX);
	HBInputOrigin get_origin_from_joy_event(const Ref<InputEvent> &p_input_event) const;
	void set_forced_input_type(HBInputType p_force_input_type);
	HBInputType get_forced_input_type() const;
	static InputGlyphsSingleton *get_singleton();
	InputGlyphsSingleton();
};

#endif // INPUT_GLYPHS_SINGLETON_H

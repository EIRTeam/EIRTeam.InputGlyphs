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

namespace InputGlyphs {
typedef uint64_t GlyphUID;
static GlyphUID calculate_guid(const InputOrigin &p_input_origin, const int &p_style, const InputGlyphSize &p_glyph_size) {
	GlyphUID uid;
	uid = p_input_origin << 6;
	uid |= p_style;
	uid |= p_glyph_size;
	return uid;
}
} //namespace InputGlyphs

class InputGlyphsSingleton : public Object {
	GDCLASS(InputGlyphsSingleton, Object);

	struct GlyphInfo {
		InputOrigin origin;
		int style;
		InputGlyphSize size;
		InputGlyphs::GlyphUID get_uid() {
			return InputGlyphs::calculate_guid(origin, style, size);
		}
	};

	HashMap<InputGlyphs::GlyphUID, Ref<Texture2D>> loaded_glyphs;

	struct GlyphLoadTask {
		bool aborted = false;
		Mutex task_mutex;
		GlyphInfo glyph_info;
		WorkerThreadPool::TaskID task_id;
		InputType input_type;
		Ref<Texture2D> texture;
	};

	Mutex mutex;
	HashMap<InputGlyphs::GlyphUID, GlyphLoadTask *> current_tasks;

	static void _join_task(WorkerThreadPool::TaskID p_task_id);
	static void _glyph_loaded_callback(GlyphLoadTask *p_task);

	static InputGlyphsSingleton *singleton;
	void _input_event(const Ref<InputEvent> &p_input_event);
	InputType _get_input_type() const;

public:
	Ref<InputGlyphsSource> glyph_source;

private:
	InputType current_input_type = InputType::UNKNOWN;
	InputType forced_input_type = InputType::UNKNOWN;
	InputGlyphStyle default_glyph_style = InputGlyphStyle::GLYPH_STYLE_KNOCKOUT;
	InputGlyphSize default_glyph_size = InputGlyphSize::GLYPH_SIZE_SMALL;

	static void _load_glyph_thread(void *p_userdata);

	void _on_input_glyphs_changed();

protected:
	static void _bind_methods();

public:
	void init();
	Ref<InputEvent> get_event_for_action(const StringName &p_action, int p_skip = 0);
	bool has_glyph_texture(const InputOrigin p_input_origin, BitField<InputGlyphStyle> p_style, const InputGlyphSize p_size = InputGlyphSize::GLYPH_SIZE_MAX);
	Ref<Texture2D> get_glyph_texture(const InputOrigin p_input_origin, BitField<InputGlyphStyle> p_style, const InputGlyphSize p_size = InputGlyphSize::GLYPH_SIZE_MAX);
	void request_glyph_texture_load(const InputOrigin p_input_origin, BitField<InputGlyphStyle> p_style, const InputGlyphSize p_size = InputGlyphSize::GLYPH_SIZE_MAX);
	InputOrigin get_origin_from_joy_event(const Ref<InputEvent> &p_input_event) const;
	void set_forced_input_type(InputType p_force_input_type);
	InputType get_forced_input_type() const;
	static InputGlyphsSingleton *get_singleton();
	String input_type_to_localized_string(InputType p_origin) const;
	List<StringName> get_game_actions() const;
	InputGlyphsSingleton();
};

#endif // INPUT_GLYPHS_SINGLETON_H

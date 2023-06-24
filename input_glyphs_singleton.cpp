/**************************************************************************/
/*  input_glyphs_singleton.cpp                                            */
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

#include "input_glyphs_singleton.h"
#include "scene/main/scene_tree.h"
#include "scene/main/window.h"

HBInputGlyphsSingleton *HBInputGlyphsSingleton::singleton = nullptr;

void HBInputGlyphsSingleton::_join_task(WorkerThreadPool::TaskID p_task_id) {
	WorkerThreadPool::get_singleton()->wait_for_task_completion(p_task_id);
}

void HBInputGlyphsSingleton::_glyph_loaded_callback(GlyphLoadTask *p_task) {
	HBInputGlyphsSingleton *igs = HBInputGlyphsSingleton::get_singleton();
	p_task->task_mutex.lock();
	igs->mutex.lock();
	HBInputGlyphs::GlyphUID uid = p_task->glyph_info.get_uid();
	if (igs->current_tasks.has(uid) && !p_task->aborted) {
		igs->loaded_glyphs.insert(uid, p_task->texture);
		igs->current_tasks.erase(uid);
	}
	callable_mp_static(&HBInputGlyphsSingleton::_join_task).call_deferred(p_task->task_id);
	p_task->task_mutex.unlock();
	igs->mutex.unlock();
	memdelete(p_task);
}

void HBInputGlyphsSingleton::init() {
	SceneTree::get_singleton()->get_root()->connect("window_input", callable_mp(this, &HBInputGlyphsSingleton::_input_event));
	glyph_source = HBInputGlyphsSource::create();
	if (glyph_source.is_valid()) {
		_on_input_type_changed();
	}
}

void HBInputGlyphsSingleton::_input_event(const Ref<InputEvent> &p_input_event) {
	Ref<InputEventJoypadButton> joypad_button = p_input_event;
	if (joypad_button.is_valid()) {
		HBInputType new_input_type = glyph_source->identify_joy(joypad_button->get_device());
		if (current_input_type != new_input_type) {
			current_input_type = new_input_type;
			if (forced_input_type == HBInputType::UNKNOWN) {
				_on_input_type_changed();
			}
		}
	}
}

void HBInputGlyphsSingleton::_load_glyph_thread(void *p_userdata) {
	GlyphLoadTask *task = (GlyphLoadTask *)p_userdata;

	task->task_mutex.lock();
	if (!task->aborted) {
		Ref<HBInputGlyphsSource> glyph_src = HBInputGlyphsSingleton::get_singleton()->glyph_source;
		Ref<Texture2D> texture = glyph_src->get_input_glyph(task->input_type, task->glyph_info.origin, task->glyph_info.style, task->glyph_info.size);
		task->texture = texture;
	}
	task->task_mutex.unlock();

	_glyph_loaded_callback(task);
}

void HBInputGlyphsSingleton::_on_input_type_changed() {
	// Abort all current loadings, if any
	MutexLock lock(mutex);
	for (KeyValue<HBInputGlyphs::GlyphUID, GlyphLoadTask *> kv : current_tasks) {
		kv.value->task_mutex.lock();
		kv.value->aborted = true;
		kv.value->task_mutex.unlock();
	}
	current_tasks.clear();
	// Clean loaded glyphs
	loaded_glyphs.clear();
	emit_signal("input_type_changed");
}

void HBInputGlyphsSingleton::_bind_methods() {
	ADD_SIGNAL(MethodInfo("input_type_changed"));
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_INVALID);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_A);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_B);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_X);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_Y);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_VIEW); // Select/Back
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_MENU); // Start
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTBUMPER);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTBUMPER);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTTRIGGER_PULL);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTTRIGGER_CLICK);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTTRIGGER_PULL);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTTRIGGER_CLICK);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTSTICK_MOVE);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTSTICK_CLICK);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTSTICK_DPADNORTH);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTSTICK_DPADSOUTH);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTSTICK_DPADWEST);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_LEFTSTICK_DPADEAST);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTSTICK_MOVE);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTSTICK_CLICK);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTSTICK_DPADNORTH);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTSTICK_DPADSOUTH);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTSTICK_DPADWEST);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_RIGHTSTICK_DPADEAST);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_DPAD_NORTH);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_DPAD_SOUTH);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_DPAD_WEST);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_DPAD_EAST);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_DPAD_MOVE);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_MISC1); // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_PADDLE1); // Xbox Elite paddle P1 (upper left, facing the back)
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_PADDLE2); // Xbox Elite paddle P3 (upper right, facing the back)
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_PADDLE3); // Xbox Elite paddle P2 (lower left, facing the back)
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_PADDLE4); // Xbox Elite paddle P4 (lower right, facing the back)
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_TRACKPAD_CLICK);
	BIND_ENUM_CONSTANT(INPUT_ORIGIN_COUNT);

	BIND_ENUM_CONSTANT(UNKNOWN);
	BIND_ENUM_CONSTANT(STEAM_CONTROLLER);
	BIND_ENUM_CONSTANT(XBOX_360_CONTROLLER);
	BIND_ENUM_CONSTANT(XBOX_ONE_CONTROLLER);
	BIND_ENUM_CONSTANT(GENERIC_XINPUT_CONTROLLER);
	BIND_ENUM_CONSTANT(PS3_CONTROLLER);
	BIND_ENUM_CONSTANT(PS4_CONTROLLER);
	BIND_ENUM_CONSTANT(PS5_CONTROLLER);
	BIND_ENUM_CONSTANT(SWITCH_PRO_CONTROLLER);
	BIND_ENUM_CONSTANT(STEAM_DECK_CONTROLLER);
	BIND_ENUM_CONSTANT(INPUT_TYPE_MAX);
}

bool HBInputGlyphsSingleton::has_glyph_texture(const HBInputOrigin &p_input_origin, const int &p_style, const HBInputGlyphSize &p_size) {
	HBInputGlyphSize size = p_size == HBInputGlyphSize::GLYPH_SIZE_MAX ? default_glyph_size : p_size;
	GlyphInfo info;
	info.origin = p_input_origin;
	info.style = p_style;
	info.size = size;

	return loaded_glyphs.has(info.get_uid());
}

Ref<Texture2D> HBInputGlyphsSingleton::get_glyph_texture(const HBInputOrigin &p_input_origin, const int &p_style, const HBInputGlyphSize &p_size) {
	HBInputGlyphSize size = p_size == HBInputGlyphSize::GLYPH_SIZE_MAX ? default_glyph_size : p_size;
	GlyphInfo info;
	info.origin = p_input_origin;
	info.style = p_style;
	info.size = size;

	MutexLock lock(mutex);
	HBInputGlyphs::GlyphUID uid = info.get_uid();
	ERR_FAIL_COND_V(!loaded_glyphs.has(uid), memnew(ImageTexture));

	return loaded_glyphs[uid];
}

void HBInputGlyphsSingleton::request_glyph_texture_load(const HBInputOrigin &p_input_origin, const int &p_style, const HBInputGlyphSize &p_size) {
	HBInputGlyphSize size = p_size == HBInputGlyphSize::GLYPH_SIZE_MAX ? default_glyph_size : p_size;
	GlyphInfo info;
	info.origin = p_input_origin;
	info.style = p_style;
	info.size = size;

	MutexLock lock(mutex);
	if (current_tasks.has(info.get_uid()) || loaded_glyphs.has(info.get_uid())) {
		mutex.unlock();
		return;
	}

	GlyphLoadTask *load_task = memnew(GlyphLoadTask);
	load_task->glyph_info.origin = p_input_origin;
	load_task->glyph_info.size = size;
	load_task->glyph_info.style = p_style;

	load_task->input_type = forced_input_type != HBInputType::UNKNOWN ? forced_input_type : current_input_type;
	load_task->task_id = WorkerThreadPool::get_singleton()->add_native_task(&_load_glyph_thread, load_task, false, "Load glyph");
	current_tasks.insert(load_task->glyph_info.get_uid(), load_task);
}

HBInputOrigin HBInputGlyphsSingleton::get_origin_from_joy_event(const Ref<InputEvent> &p_input_event) const {
	Ref<InputEventJoypadButton> joy_button = p_input_event;
	HBInputOrigin origin = HBInputOrigin::INPUT_ORIGIN_INVALID;
	if (joy_button.is_valid()) {
		if (joy_button->get_button_index() < JoyButton::SDL_MAX) {
			origin = HBInputGlyphs::godot_button_to_input_origin_lut[(int)joy_button->get_button_index()];
		}
	}
	// TODO: Implement input event joy motion
	//Ref<InputEventJoypadMotion> joy_motion = p_input_event;
	return origin;
}

HBInputGlyphsSingleton *HBInputGlyphsSingleton::get_singleton() {
	return singleton;
}

HBInputGlyphsSingleton::HBInputGlyphsSingleton() {
	HBInputGlyphsSourceBuiltin::make_current();
	singleton = this;
}

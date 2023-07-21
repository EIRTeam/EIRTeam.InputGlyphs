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
#include "core/config/project_settings.h"
#include "core/input/input_map.h"
#include "scene/main/scene_tree.h"
#include "scene/main/window.h"
#include "scene/resources/image_texture.h"

InputGlyphsSingleton *InputGlyphsSingleton::singleton = nullptr;

void InputGlyphsSingleton::_join_task(WorkerThreadPool::TaskID p_task_id) {
	WorkerThreadPool::get_singleton()->wait_for_task_completion(p_task_id);
}

void InputGlyphsSingleton::_glyph_loaded_callback(GlyphLoadTask *p_task) {
	InputGlyphsSingleton *igs = InputGlyphsSingleton::get_singleton();
	p_task->task_mutex.lock();
	igs->mutex.lock();
	InputGlyphs::GlyphUID uid = p_task->glyph_info.get_uid();
	if (igs->current_tasks.has(uid) && !p_task->aborted) {
		igs->loaded_glyphs.insert(uid, p_task->texture);
		igs->current_tasks.erase(uid);
	}
	callable_mp_static(&InputGlyphsSingleton::_join_task).call_deferred(p_task->task_id);
	p_task->task_mutex.unlock();
	igs->mutex.unlock();
	memdelete(p_task);
}

void InputGlyphsSingleton::init() {
	SceneTree::get_singleton()->get_root()->connect("window_input", callable_mp(this, &InputGlyphsSingleton::_input_event));
	glyph_source = InputGlyphsSource::create();
	if (glyph_source.is_valid()) {
		_on_input_glyphs_changed();
	}
}

Ref<InputEvent> InputGlyphsSingleton::get_event_for_action(const StringName &p_action, int p_skip) {
	if (Engine::get_singleton()->is_editor_hint()) {
		StringName input_path = "input/" + p_action;
		if (!ProjectSettings::get_singleton()->has_setting(input_path)) {
			return Ref<InputEvent>();
		}
		Dictionary action = GLOBAL_GET(input_path);
		Array events = action.get("events", Array());

		for (int i = 0; i < events.size(); i++) {
			Ref<InputEvent> event = events[i];
			if (event.is_null()) {
				continue;
			}
			InputOrigin origin = get_origin_from_joy_event(event);
			if (origin > InputOrigin::INPUT_ORIGIN_INVALID) {
				return event;
			}
		}
	} else {
		const InputMap::Action *action = InputMap::get_singleton()->get_action_map().getptr(p_action);
		if (!action) {
			return Ref<InputEvent>();
		}
		for (const Ref<InputEvent> &input_ev : action->inputs) {
			InputOrigin origin = get_origin_from_joy_event(input_ev);
			if (origin > InputOrigin::INPUT_ORIGIN_INVALID) {
				return input_ev;
			}
		}
	}
	return Ref<InputEvent>();
}

void InputGlyphsSingleton::_input_event(const Ref<InputEvent> &p_input_event) {
	Ref<InputEventJoypadButton> joypad_button = p_input_event;
	if (joypad_button.is_valid() && forced_input_type != InputType::UNKNOWN) {
		InputType new_input_type = glyph_source->identify_joy(joypad_button->get_device());
		if (current_input_type != new_input_type) {
			current_input_type = new_input_type;
			_on_input_glyphs_changed();
		}
	}
}

InputType InputGlyphsSingleton::_get_input_type() const {
	if (forced_input_type != InputType::UNKNOWN) {
		return forced_input_type;
	}
	return current_input_type;
}

void InputGlyphsSingleton::_load_glyph_thread(void *p_userdata) {
	GlyphLoadTask *task = (GlyphLoadTask *)p_userdata;

	task->task_mutex.lock();
	if (!task->aborted) {
		Ref<InputGlyphsSource> glyph_src = InputGlyphsSingleton::get_singleton()->glyph_source;
		Ref<Texture2D> texture = glyph_src->get_input_glyph(task->input_type, task->glyph_info.origin, task->glyph_info.style, task->glyph_info.size);
		task->texture = texture;
	}
	task->task_mutex.unlock();

	_glyph_loaded_callback(task);
}

void InputGlyphsSingleton::_on_input_glyphs_changed() {
	// Abort all current loadings, if any
	MutexLock lock(mutex);
	for (KeyValue<InputGlyphs::GlyphUID, GlyphLoadTask *> kv : current_tasks) {
		kv.value->task_mutex.lock();
		kv.value->aborted = true;
		kv.value->task_mutex.unlock();
	}
	current_tasks.clear();
	// Clean loaded glyphs
	loaded_glyphs.clear();
	emit_signal("input_glyphs_changed");
}

void InputGlyphsSingleton::_bind_methods() {
	ADD_SIGNAL(MethodInfo("input_glyphs_changed"));
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

	BIND_ENUM_CONSTANT(GLYPH_SIZE_SMALL);
	BIND_ENUM_CONSTANT(GLYPH_SIZE_MEDIUM);
	BIND_ENUM_CONSTANT(GLYPH_SIZE_LARGE);
	BIND_ENUM_CONSTANT(GLYPH_SIZE_MAX);

	BIND_BITFIELD_FLAG(GLYPH_STYLE_KNOCKOUT);
	BIND_BITFIELD_FLAG(GLYPH_STYLE_LIGHT);
	BIND_BITFIELD_FLAG(GLYPH_STYLE_DARK);

	BIND_BITFIELD_FLAG(GLYPH_STYLE_NEUTRAL_COLOR_ABXY);
	BIND_BITFIELD_FLAG(GLYPH_STYLE_SOLID_ABXY);

	ClassDB::bind_method(D_METHOD("has_glyph_texture", "input_origin", "style", "size"), &InputGlyphsSingleton::has_glyph_texture, DEFVAL(InputGlyphSize::GLYPH_SIZE_MAX));
	ClassDB::bind_method(D_METHOD("get_glyph_texture", "input_origin", "style", "size"), &InputGlyphsSingleton::get_glyph_texture, DEFVAL(InputGlyphSize::GLYPH_SIZE_MAX));
	ClassDB::bind_method(D_METHOD("request_glyph_texture_load", "input_origin", "style", "size"), &InputGlyphsSingleton::request_glyph_texture_load, DEFVAL(InputGlyphSize::GLYPH_SIZE_MAX));
	ClassDB::bind_method(D_METHOD("get_origin_from_joy_event", "input_event"), &InputGlyphsSingleton::get_origin_from_joy_event);

	ClassDB::bind_method(D_METHOD("set_forced_input_type", "forced_input_type"), &InputGlyphsSingleton::set_forced_input_type);
	ClassDB::bind_method(D_METHOD("get_forced_input_type"), &InputGlyphsSingleton::get_forced_input_type);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "forced_input_type"), "set_forced_input_type", "get_forced_input_type");

	ClassDB::bind_method(D_METHOD("input_type_to_localized_string", "input_type"), &InputGlyphsSingleton::input_type_to_localized_string);
}

bool InputGlyphsSingleton::has_glyph_texture(const InputOrigin p_input_origin, BitField<InputGlyphStyle> p_style, const InputGlyphSize p_size) {
	InputGlyphSize size = p_size == InputGlyphSize::GLYPH_SIZE_MAX ? default_glyph_size : p_size;
	GlyphInfo info;
	info.origin = p_input_origin;
	info.style = p_style;
	info.size = size;

	return loaded_glyphs.has(info.get_uid());
}

Ref<Texture2D> InputGlyphsSingleton::get_glyph_texture(const InputOrigin p_input_origin, const BitField<InputGlyphStyle> p_style, const InputGlyphSize p_size) {
	InputGlyphSize size = p_size == InputGlyphSize::GLYPH_SIZE_MAX ? default_glyph_size : p_size;
	GlyphInfo info;
	info.origin = p_input_origin;
	info.style = p_style;
	info.size = size;

	MutexLock lock(mutex);
	InputGlyphs::GlyphUID uid = info.get_uid();
	ERR_FAIL_COND_V(!loaded_glyphs.has(uid), memnew(ImageTexture));

	print_line("PSTYLE", p_style);

	return loaded_glyphs[uid];
}

void InputGlyphsSingleton::request_glyph_texture_load(const InputOrigin p_input_origin, const BitField<InputGlyphStyle> p_style, const InputGlyphSize p_size) {
	InputGlyphSize size = p_size == InputGlyphSize::GLYPH_SIZE_MAX ? default_glyph_size : p_size;
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

	load_task->input_type = _get_input_type();
	load_task->task_id = WorkerThreadPool::get_singleton()->add_native_task(&_load_glyph_thread, load_task, false, "Load glyph");
	current_tasks.insert(load_task->glyph_info.get_uid(), load_task);
}

InputOrigin InputGlyphsSingleton::get_origin_from_joy_event(const Ref<InputEvent> &p_input_event) const {
	if (p_input_event.is_null()) {
		return InputOrigin::INPUT_ORIGIN_INVALID;
	}
	Ref<InputEventJoypadButton> joy_button = p_input_event;
	InputOrigin origin = InputOrigin::INPUT_ORIGIN_INVALID;
	if (joy_button.is_valid()) {
		if (joy_button->get_button_index() < JoyButton::SDL_MAX) {
			origin = InputGlyphTables::godot_button_to_input_origin_lut[(int)joy_button->get_button_index()];
		}
	}
	// TODO: Implement input event joy motion
	Ref<InputEventJoypadMotion> joy_motion = p_input_event;
	if (joy_motion.is_valid()) {
		int sign = SIGN(joy_motion->get_axis_value());
		print_line(joy_motion, sign);
		switch (joy_motion->get_axis()) {
			case JoyAxis::LEFT_X: {
				origin = sign > 0 ? INPUT_ORIGIN_LEFTSTICK_DPADEAST : INPUT_ORIGIN_LEFTSTICK_DPADWEST;
			} break;
			case JoyAxis::LEFT_Y: {
				origin = sign > 0 ? INPUT_ORIGIN_LEFTSTICK_DPADSOUTH : INPUT_ORIGIN_LEFTSTICK_DPADNORTH;
			} break;
			case JoyAxis::RIGHT_X: {
				origin = sign > 0 ? INPUT_ORIGIN_RIGHTSTICK_DPADEAST : INPUT_ORIGIN_RIGHTSTICK_DPADWEST;
			} break;
			case JoyAxis::RIGHT_Y: {
				origin = sign > 0 ? INPUT_ORIGIN_RIGHTSTICK_DPADSOUTH : INPUT_ORIGIN_RIGHTSTICK_DPADNORTH;
			} break;
			case JoyAxis::TRIGGER_LEFT: {
				origin = INPUT_ORIGIN_LEFTTRIGGER_PULL;
			} break;
			case JoyAxis::TRIGGER_RIGHT: {
				origin = INPUT_ORIGIN_RIGHTTRIGGER_PULL;
			} break;
			case JoyAxis::INVALID:
			case JoyAxis::SDL_MAX:
			case JoyAxis::MAX: {
			} break;
		}
	}
	return origin;
}

void InputGlyphsSingleton::set_forced_input_type(InputType p_force_input_type) {
	InputType input_type = _get_input_type();
	forced_input_type = p_force_input_type;
	if (input_type != p_force_input_type) {
		_on_input_glyphs_changed();
	}
}

InputType InputGlyphsSingleton::get_forced_input_type() const {
	return forced_input_type;
}

InputGlyphsSingleton *InputGlyphsSingleton::get_singleton() {
	return singleton;
}

String InputGlyphsSingleton::input_type_to_localized_string(InputType p_type) const {
	String ret;
	switch (p_type) {
		case UNKNOWN:
			ret = RTR("Unknown Controller");
			break;
		case STEAM_CONTROLLER:
			ret = RTR("Steam Controller");
			break;
		case XBOX_360_CONTROLLER:
			ret = RTR("Xbox 360 Controller");
			break;
		case XBOX_ONE_CONTROLLER:
			ret = RTR("Xbox One/Series X|S Controller");
			break;
		case GENERIC_XINPUT_CONTROLLER:
			ret = RTR("Generic Controller");
			break;
		case PS3_CONTROLLER:
			ret = RTR("PlayStation 3 Controller");
			break;
		case PS4_CONTROLLER:
			ret = RTR("PlayStation 4 Controller");
			break;
		case PS5_CONTROLLER:
			ret = RTR("PlayStation 5 Controller");
			break;
		case SWITCH_PRO_CONTROLLER:
			ret = RTR("Nintendo Switch Controller");
			break;
		case STEAM_DECK_CONTROLLER:
			ret = RTR("Steam Deck Controller");
			break;
		case INPUT_TYPE_MAX:
			break; // Can't happen, but silences warning
	}
	return ret;
}

List<StringName> InputGlyphsSingleton::get_game_actions() const {
	List<StringName> actions;
	if (Engine::get_singleton()->is_editor_hint()) {
		List<PropertyInfo> pinfo;
		ProjectSettings::get_singleton()->get_property_list(&pinfo);
		for (int i = 0; i < pinfo.size(); i++) {
			if (pinfo[i].name.begins_with("input/")) {
				StringName name = pinfo[i].name.substr(pinfo[i].name.find("/") + 1, pinfo[i].name.length());
				actions.push_back(name);
			}
		}
	} else {
		actions = InputMap::get_singleton()->get_actions();
	}
	return actions;
}

InputGlyphsSingleton::InputGlyphsSingleton() {
	InputGlyphsSourceBuiltin::make_current();
	singleton = this;
}

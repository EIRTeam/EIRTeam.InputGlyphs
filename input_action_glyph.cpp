/**************************************************************************/
/*  input_action_glyph.cpp                                                */
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

#include "input_action_glyph.h"
#include "core/config/project_settings.h"
#include "core/input/input_map.h"
#include "input_glyphs_singleton.h"

int HBInputActionGlyph::_get_glyph_style_with_override() {
	if (override_glyph_style) {
		return glyph_style_override;
	}
	int theme = GLOBAL_GET("eirteam/input_glyphs/theme");
	int abxy_overrides = GLOBAL_GET("eirteam/input_glyphs/abxy_overrides");
	return theme | abxy_overrides;
}

void HBInputActionGlyph::_queue_label_update() {
	if (!Engine::get_singleton()->is_editor_hint()) {
		const InputMap::Action *action = InputMap::get_singleton()->get_action_map().getptr(action_name);
		ERR_FAIL_COND_MSG(action == nullptr, vformat("InputActionGlyph: action %s does not exist.", action_name));
		origin = HBInputOrigin::INPUT_ORIGIN_INVALID;
		for (const Ref<InputEvent> &input_ev : action->inputs) {
			origin = HBInputGlyphsSingleton::get_singleton()->get_origin_from_joy_event(input_ev);
			if (origin > HBInputOrigin::INPUT_ORIGIN_INVALID) {
				break;
			}
		}
		ERR_FAIL_COND_MSG(origin == HBInputOrigin::INPUT_ORIGIN_INVALID, vformat("InputActionGlyph: Couldn't find an input origin for action %s. Using a placeholder instead.", action_name));
		set_process_internal(true);
	}
}

void HBInputActionGlyph::_on_input_type_changed() {
	_queue_label_update();
}

void HBInputActionGlyph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_override_glyph_style", "override_glyph_style"), &HBInputActionGlyph::set_override_glyph_style);
	ClassDB::bind_method(D_METHOD("get_override_glyph_style"), &HBInputActionGlyph::get_override_glyph_style);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "override_glyph_style"), "set_override_glyph_style", "get_override_glyph_style");

	ClassDB::bind_method(D_METHOD("set_glyph_style_override", "style"), &HBInputActionGlyph::set_glyph_style_override);
	ClassDB::bind_method(D_METHOD("get_glyph_style_override"), &HBInputActionGlyph::get_glyph_style_override);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "glyph_style_override", PROPERTY_HINT_FLAGS, "Light:1,Dark:2,Neutral ABXY:16,Solid ABXY:32", PROPERTY_USAGE_NO_EDITOR), "set_glyph_style_override", "get_glyph_style_override");

	ClassDB::bind_method(D_METHOD("get_action_name"), &HBInputActionGlyph::get_action_name);
	ClassDB::bind_method(D_METHOD("set_action_name", "action_name"), &HBInputActionGlyph::set_action_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "action_name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_action_name", "get_action_name");
}

void HBInputActionGlyph::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			rtl = memnew(RichTextLabel);
			add_child(rtl);
			rtl->set_autowrap_mode(TextServer::AUTOWRAP_OFF);
			rtl->set_fit_content(true);
			_queue_label_update();
			HBInputGlyphsSingleton::get_singleton()->connect("input_type_changed", callable_mp(this, &HBInputActionGlyph::_on_input_type_changed));
		} break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			int glyph_style = _get_glyph_style_with_override();
			if (!HBInputGlyphsSingleton::get_singleton()->has_glyph_texture(origin, glyph_style)) {
				HBInputGlyphsSingleton::get_singleton()->request_glyph_texture_load(origin, glyph_style);
			} else {
				glyph_texture = HBInputGlyphsSingleton::get_singleton()->get_glyph_texture(origin, glyph_style);
				rtl->clear();
				rtl->add_image(glyph_texture);
				rtl->add_text(text);
				set_process_internal(false);
			}
		} break;
	}
}

bool HBInputActionGlyph::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == SNAME("style_override_theme")) {
		int value = p_value;
		glyph_style_override &= ~(0b11);
		glyph_style_override |= value;
		return true;
	} else if (p_name == SNAME("style_override_abx_overrides")) {
		int value = p_value;
		glyph_style_override &= ~(0b110000);
		glyph_style_override |= value;
		return true;
	} else if (p_name == SNAME("action")) {
		int idx = p_value;
		action_name = InputMap::get_singleton()->get_actions()[idx];
		return true;
	}
	return false;
}

bool HBInputActionGlyph::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == SNAME("style_override_theme")) {
		r_ret = glyph_style_override & 0b11;
		return true;
	} else if (p_name == SNAME("style_override_abx_overrides")) {
		r_ret = glyph_style_override & 0b110000;
		return true;
	} else if (p_name == SNAME("action")) {
		List<StringName> actions = InputMap::get_singleton()->get_actions();
		for (int i = 0; i < actions.size(); i++) {
			if (action_name == actions[i]) {
				r_ret = i;
				break;
			}
		}
		return true;
	}
	return false;
}

void HBInputActionGlyph::_get_property_list(List<PropertyInfo> *p_list) const {
	if (override_glyph_style) {
		p_list->push_back(PropertyInfo(Variant::INT, "style_override_theme", PROPERTY_HINT_ENUM, "Kockout,Light,Dark", PROPERTY_USAGE_EDITOR));
		p_list->push_back(PropertyInfo(Variant::INT, "style_override_abx_overrides", PROPERTY_HINT_FLAGS, "Neutral Color ABXY:16, Solid ABXY:32", PROPERTY_USAGE_EDITOR));
	}

	List<StringName> action_list = InputMap::get_singleton()->get_actions();
	String enum_values;
	for (int i = 0; i < action_list.size(); i++) {
		enum_values += String(action_list[i]) + ",";
	}
	p_list->push_back(PropertyInfo(Variant::INT, "action", PROPERTY_HINT_ENUM, enum_values, PROPERTY_USAGE_EDITOR));
}

String HBInputActionGlyph::get_text() const {
	return text;
}

void HBInputActionGlyph::set_text(const String &p_text) {
	text = p_text;
	_queue_label_update();
}

HBInputActionGlyph::HBInputActionGlyph() {
	_queue_label_update();
}

int HBInputActionGlyph::get_glyph_style_override() const {
	return glyph_style_override;
}

void HBInputActionGlyph::set_glyph_style_override(const int &p_glyph_style_override) {
	glyph_style_override = p_glyph_style_override;
}

StringName HBInputActionGlyph::get_action_name() const {
	return action_name;
}

void HBInputActionGlyph::set_action_name(const StringName &p_action_name) {
	action_name = p_action_name;
}

bool HBInputActionGlyph::get_override_glyph_style() const {
	return override_glyph_style;
}

void HBInputActionGlyph::set_override_glyph_style(bool p_override_glyph_style) {
	override_glyph_style = p_override_glyph_style;
	notify_property_list_changed();
	_queue_label_update();
}

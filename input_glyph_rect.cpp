#include "input_glyph_rect.h"

#include "core/config/project_settings.h"
#include "input_glyphs_singleton.h"
#include "scene/theme/theme_db.h"

void InputGlyphRect::_shape_action_name_text() {
	if (action_text.is_empty()) {
		return;
	}
	action_name_text_buff->clear();

	Ref<Font> font = theme_cache.action_text_font;
	int font_size = theme_cache.action_text_font_size;
	if (font.is_null() || font_size == 0) {
		// Can't shape without a valid font and a non-zero size.
		return;
	}

	action_name_text_buff->add_string(action_text, font, font_size);
	update_minimum_size();
}

void InputGlyphRect::_shape_fallback_glyph_text() {
	if (fallback_glyph_string.is_empty()) {
		return;
	}
	fallback_text_buff->clear();

	Ref<Font> font = theme_cache.fallback_glyph_font;
	int font_size = theme_cache.fallback_glyph_font_size;
	if (font.is_null() || font_size == 0) {
		// Can't shape without a valid font and a non-zero size.
		return;
	}

	fallback_text_buff->add_string(fallback_glyph_string, font, font_size);
	update_minimum_size();
}

BitField<InputGlyphStyle> InputGlyphRect::_get_glyph_style_with_override() const {
	/*if (override_glyph_style) {
		return glyph_style_override;
	}*/
	int theme = GLOBAL_GET("eirteam/input_glyphs/theme");
	int abxy_overrides = GLOBAL_GET("eirteam/input_glyphs/abxy_overrides");
	return theme | abxy_overrides;
}

void InputGlyphRect::_on_input_glyphs_changed() {
	input_type = InputGlyphsSingleton::get_singleton()->get_visible_input_type();
	if (forced_input_type == InputGlyphsConstants::UNKNOWN) {
		_queue_glyph_update();
	}
}

InputGlyphsConstants::InputType InputGlyphRect::_get_input_type() const {
	InputGlyphsConstants::InputType type = input_type;
	if (forced_input_type != InputGlyphsConstants::UNKNOWN) {
		type = forced_input_type;
	}

	if (forced_joy_origin != InputGlyphsConstants::INPUT_ORIGIN_INVALID) {
		if (type == InputGlyphsConstants::KEYBOARD) {
			return InputGlyphsConstants::UNKNOWN;
		}
	}

	return type;
}

InputGlyphsConstants::InputOrigin InputGlyphRect::_get_joy_origin() const {
	if (forced_joy_origin != InputGlyphsConstants::INPUT_ORIGIN_INVALID) {
		return forced_joy_origin;
	}
	return joy_origin;
}

void InputGlyphRect::_bind_methods() {
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, InputGlyphRect, action_text_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, InputGlyphRect, fallback_glyph_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, InputGlyphRect, fallback_glyph_stylebox);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, InputGlyphRect, separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, InputGlyphRect, icon_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, InputGlyphRect, fallback_glyph_font_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, InputGlyphRect, action_text_font_size);
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_CONSTANT, InputGlyphRect, action_text_font_outline_size, "action_text_font_outline_size");
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, InputGlyphRect, action_text_font_outline_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, InputGlyphRect, action_text_font_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, InputGlyphRect, fallback_glyph_font_color);

	ClassDB::bind_method(D_METHOD("set_action_text", "action_text"), &InputGlyphRect::set_action_text);
	ClassDB::bind_method("get_action_text", &InputGlyphRect::get_action_text);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "action_text"), "set_action_text", "get_action_text");

	ClassDB::bind_method("set_action_name", &InputGlyphRect::set_action_name, "action_name");
	ClassDB::bind_method("get_action_name", &InputGlyphRect::get_action_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "action_name"), "set_action_name", "get_action_name");

	ClassDB::bind_method(D_METHOD("set_forced_input_type", "input_type"), &InputGlyphRect::set_forced_input_type);
	ClassDB::bind_method("get_forced_input_type", &InputGlyphRect::get_forced_input_type);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "forced_input_type", PROPERTY_HINT_ENUM, "UNKNOWN,STEAM_CONTROLLER,XBOX_360_CONTROLLER,XBOX_ONE_CONTROLLER,GENERIC_XINPUT_CONTROLLER,PS3_CONTROLLER,PS4_CONTROLLER,PS5_CONTROLLER,SWITCH_PRO_CONTROLLER,STEAM_DECK_CONTROLLER,KEYBOARD"), "set_forced_input_type", "get_forced_input_type");

	ClassDB::bind_method(D_METHOD("set_forced_joy_origin", "joy_origin"), &InputGlyphRect::set_forced_joy_origin);
	ClassDB::bind_method("get_forced_joy_origin", &InputGlyphRect::get_forced_joy_origin);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "forced_joy_origin"), "set_forced_joy_origin", "get_forced_joy_origin");

	ClassDB::bind_method(D_METHOD("set_action_skip_count", "skip_count"), &InputGlyphRect::set_action_skip_count);
	ClassDB::bind_method("get_action_skip_count", &InputGlyphRect::get_action_skip_count);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "action_skip_count"), "set_action_skip_count", "get_action_skip_count");

	ClassDB::bind_method(D_METHOD("set_disable_axis_direction_display", "disable_axis_direction_display"), &InputGlyphRect::set_disable_axis_direction_display);
	ClassDB::bind_method("get_disable_axis_direction_display", &InputGlyphRect::get_disable_axis_direction_display);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disable_axis_direction_display"), "set_disable_axis_direction_display", "get_disable_axis_direction_display");
}

void InputGlyphRect::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_shape_action_name_text();
			_shape_fallback_glyph_text();
		} break;
		case NOTIFICATION_READY: {
			input_type = InputGlyphsSingleton::get_singleton()->get_visible_input_type();
			InputGlyphsSingleton::get_singleton()->connect("input_glyphs_changed", callable_mp(this, &InputGlyphRect::_on_input_glyphs_changed));
			_queue_glyph_update();
		} break;
		case NOTIFICATION_DRAW: {
			RID ci = get_canvas_item();
			Size2 size = get_size();
			Point2 center = size * 0.5f;
			Vector2 pos;
			if (!action_text.is_empty()) {
				pos.x += action_name_text_buff->get_size().x;
				if (theme_cache.action_text_font_outline_size > 0) {
					action_name_text_buff->draw_outline(ci, Point2(0, center.y - action_name_text_buff->get_size().y * 0.5f), theme_cache.action_text_font_outline_size, theme_cache.action_text_font_outline_color);
				}
				action_name_text_buff->draw(ci, Point2(0, center.y - action_name_text_buff->get_size().y * 0.5f), theme_cache.action_text_font_color);
				pos.x += theme_cache.separation;
			}

			if (!fallback_glyph_string.is_empty()) {
				Size2 offset;
				Point2 stylebox_center = center;
				stylebox_center.x = pos.x;
				stylebox_center.y = center.y;
				if (theme_cache.fallback_glyph_stylebox.is_valid()) {
					offset = theme_cache.fallback_glyph_stylebox->get_offset();
					Size2 stylebox_size = fallback_text_buff->get_size() + theme_cache.fallback_glyph_stylebox->get_minimum_size();
					stylebox_size.x = MAX(stylebox_size.x, stylebox_size.y);
					Point2 stylebox_pos = pos;
					stylebox_pos.y = center.y - stylebox_size.y * 0.5f;
					theme_cache.fallback_glyph_stylebox->draw(ci, Rect2(stylebox_pos, stylebox_size));
					float margins[4] = {
						theme_cache.fallback_glyph_stylebox->get_margin(SIDE_LEFT),
						theme_cache.fallback_glyph_stylebox->get_margin(SIDE_TOP),
						theme_cache.fallback_glyph_stylebox->get_margin(SIDE_RIGHT),
						theme_cache.fallback_glyph_stylebox->get_margin(SIDE_BOTTOM),
					};
					Size2 remaining = stylebox_size - offset;
					remaining.x -= margins[SIDE_RIGHT];
					remaining.y -= margins[SIDE_BOTTOM];
					stylebox_center = stylebox_pos + offset + remaining * 0.5f;
				}
				fallback_text_buff->draw(ci, stylebox_center - fallback_text_buff->get_size() * 0.5f, theme_cache.fallback_glyph_font_color);
			} else if (glyph_texture.is_valid()) {
				glyph_texture->draw_rect(ci, Rect2(Point2(pos.x, center.y - theme_cache.icon_size * 0.5f), Size2(theme_cache.icon_size, theme_cache.icon_size)));
			}
		} break;
		case NOTIFICATION_THEME_CHANGED:
		case NOTIFICATION_TRANSLATION_CHANGED: {
			_shape_action_name_text();
			_shape_fallback_glyph_text();
			_queue_glyph_update();

			update_minimum_size();
			queue_redraw();
		} break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (glyph_update_queued) {
				int glyph_style = _get_glyph_style_with_override();
				if (!InputGlyphsSingleton::get_singleton()->has_glyph_texture(_get_input_type(), _get_joy_origin(), glyph_style, GLYPH_SIZE_MEDIUM)) {
					InputGlyphsSingleton::get_singleton()->request_glyph_texture_load(_get_input_type(), _get_joy_origin(), glyph_style, GLYPH_SIZE_MEDIUM);
				} else {
					glyph_texture = InputGlyphsSingleton::get_singleton()->get_glyph_texture(_get_input_type(), _get_joy_origin(), glyph_style, GLYPH_SIZE_MEDIUM);
					fallback_glyph_string.clear();

					queue_redraw();
					set_process_internal(false);
					glyph_update_queued = false;
					update_minimum_size();
				}
			}
		}
	}
}

void InputGlyphRect::_queue_glyph_update() {
	glyph_update_queued = true;

	if (forced_joy_origin != InputGlyphsConstants::InputOrigin::INPUT_ORIGIN_INVALID) {
		set_process_internal(true);
		return;
	}

	Ref<InputEvent> ev = InputGlyphsSingleton::get_singleton()->get_event_for_action(_get_input_type(), action_name, action_skip_count);

	if (!ev.is_valid()) {
		return;
	}
	if (ev->is_class("InputEventKey")) {
		fallback_glyph_string = InputGlyphsSingleton::get_singleton()->get_event_display_string(ev);
		glyph_update_queued = false;
		glyph_texture.unref();
		queue_redraw();
		_shape_fallback_glyph_text();
	} else {
		joy_origin = InputGlyphsSingleton::get_singleton()->get_origin_from_joy_event(ev);
		if (disable_axis_direction_display) {
			bool is_left_stick = joy_origin >= InputGlyphsConstants::INPUT_ORIGIN_LEFTSTICK_DPADNORTH && joy_origin <= InputGlyphsConstants::INPUT_ORIGIN_LEFTSTICK_DPADEAST;
			bool is_right_stick = joy_origin >= InputGlyphsConstants::INPUT_ORIGIN_RIGHTSTICK_DPADNORTH && joy_origin <= InputGlyphsConstants::INPUT_ORIGIN_RIGHTSTICK_DPADEAST;

			if (is_left_stick) {
				joy_origin = InputGlyphsConstants::INPUT_ORIGIN_LEFTSTICK_MOVE;
			} else if (is_right_stick) {
				joy_origin = InputGlyphsConstants::INPUT_ORIGIN_RIGHTSTICK_MOVE;
			}
		}
		set_process_internal(true);
	}
}

void InputGlyphRect::set_action_name(StringName p_action_name) {
	action_name = p_action_name;
	_queue_glyph_update();
}

StringName InputGlyphRect::get_action_name() const {
	return action_name;
}

Size2 InputGlyphRect::get_minimum_size() const {
	Size2 ms;

	if (!action_text.is_empty()) {
		Size2 string_size = action_name_text_buff->get_size();

		ms += string_size;

		ms.x += theme_cache.separation;
	}

	if (!fallback_glyph_string.is_empty()) {
		Size2 fallback_string_size = fallback_text_buff->get_size();

		if (theme_cache.fallback_glyph_stylebox.is_valid()) {
			Size2 glyph_stylebox_min_size = theme_cache.fallback_glyph_stylebox->get_minimum_size();
			ms.y = MAX(ms.y, fallback_string_size.y + glyph_stylebox_min_size.y);
			ms.x += MAX(ms.y, fallback_string_size.x + glyph_stylebox_min_size.x);
		} else {
			ms.x += fallback_string_size.x;
		}

		ms.y = MAX(theme_cache.icon_size, ms.y);

	} else {
		ms.y = MAX(ms.y, theme_cache.icon_size);
		ms.x += theme_cache.icon_size;
	}

	return ms;
}

InputGlyphRect::InputGlyphRect() {
	action_name_text_buff.instantiate();
	fallback_text_buff.instantiate();
}

String InputGlyphRect::get_action_text() const { return action_text; }

void InputGlyphRect::set_action_text(const String &p_action_text) {
	if (action_text != p_action_text) {
		action_text = p_action_text;
		_shape_action_name_text();
		update_minimum_size();
		queue_redraw();
	}
}

InputGlyphsConstants::InputType InputGlyphRect::get_forced_input_type() const {
	return forced_input_type;
}

void InputGlyphRect::set_forced_input_type(const InputGlyphsConstants::InputType p_forced_input_type) {
	if (p_forced_input_type != forced_input_type) {
		forced_input_type = p_forced_input_type;
		_queue_glyph_update();
	}
}

InputGlyphsConstants::InputOrigin InputGlyphRect::get_forced_joy_origin() const {
	return forced_joy_origin;
}

void InputGlyphRect::set_forced_joy_origin(const InputGlyphsConstants::InputOrigin p_forced_joy_origin) {
	if (forced_joy_origin != p_forced_joy_origin) {
		forced_joy_origin = p_forced_joy_origin;
		_queue_glyph_update();
	}
}

int InputGlyphRect::get_action_skip_count() const {
	return action_skip_count;
}

void InputGlyphRect::set_action_skip_count(int p_action_skip_count) {
	if (action_skip_count != p_action_skip_count) {
		action_skip_count = p_action_skip_count;
		_queue_glyph_update();
	}
}

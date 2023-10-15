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
	_queue_glyph_update();
}

void InputGlyphRect::_bind_methods() {
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, InputGlyphRect, action_text_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, InputGlyphRect, fallback_glyph_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, InputGlyphRect, fallback_glyph_stylebox);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, InputGlyphRect, separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, InputGlyphRect, fallback_glyph_font_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, InputGlyphRect, action_text_font_size);

	ClassDB::bind_method("set_action_text", &InputGlyphRect::set_action_text, "action_text");
	ClassDB::bind_method("get_action_text", &InputGlyphRect::get_action_text);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "action_text"), "set_action_text", "get_action_text");
}

void InputGlyphRect::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			InputGlyphsSingleton::get_singleton()->connect("input_glyphs_changed", callable_mp(this, &InputGlyphRect::_on_input_glyphs_changed));
		} break;
		case NOTIFICATION_DRAW: {
			RID ci = get_canvas_item();
			Size2 size = get_size();
			Point2 center = size * 0.5f;
			Vector2 pos;
			if (!action_text.is_empty()) {
				pos.x += action_name_text_buff->get_size().x;
				action_name_text_buff->draw(ci, Point2(0, center.y - action_name_text_buff->get_size().y * 0.5f));
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
				fallback_text_buff->draw(ci, stylebox_center - fallback_text_buff->get_size() * 0.5f);
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
				if (!InputGlyphsSingleton::get_singleton()->has_glyph_texture(joy_origin, glyph_style)) {
					InputGlyphsSingleton::get_singleton()->request_glyph_texture_load(joy_origin, glyph_style);
				} else {
					glyph_texture = InputGlyphsSingleton::get_singleton()->get_glyph_texture(joy_origin, glyph_style);
					fallback_glyph_string.clear();

					queue_redraw();
					set_process_internal(false);
					glyph_update_queued = false;
				}
			}
		}
	}
}

void InputGlyphRect::_queue_glyph_update() {
	glyph_update_queued = true;
	Ref<InputEvent> ev = InputGlyphsSingleton::get_singleton()->get_event_for_action(action_name);
	if (ev->is_class("InputEventKey")) {
		fallback_glyph_string = InputGlyphsSingleton::get_singleton()->get_event_display_string(ev);
		glyph_update_queued = false;
		glyph_texture = Ref<Texture2D>();
		queue_redraw();
		_shape_fallback_glyph_text();
	} else {
		joy_origin = InputGlyphsSingleton::get_singleton()->get_origin_from_joy_event(ev);
		set_process_internal(true);
	}
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

	} else {
		ms.y = MAX(ms.y, theme_cache.icon_size);
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

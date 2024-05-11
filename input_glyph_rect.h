#include "input_glyphs.h"
#include "scene/gui/control.h"
#include "scene/resources/text_line.h"

class InputGlyphRect : public Control {
	GDCLASS(InputGlyphRect, Control);

	Ref<TextLine> action_name_text_buff;
	Ref<TextLine> fallback_text_buff;

	struct ThemeCache {
		Ref<Font> fallback_glyph_font;
		int fallback_glyph_font_size = 0;
		Ref<Font> action_text_font;
		int action_text_font_size = 0;
		Ref<StyleBox> fallback_glyph_stylebox;
		int separation = 5;
		int icon_size = 32;
		Color action_text_font_color;
		int action_text_font_outline_size = 0;
		Color action_text_font_outline_color;
		Color fallback_glyph_font_color;
	} theme_cache;

	StringName action_name = "move_left";
	String action_text = "Action Text";
	String fallback_glyph_string = "Fallback";
	Ref<Texture2D> glyph_texture;
	InputGlyphsConstants::InputOrigin joy_origin = InputGlyphsConstants::InputOrigin::INPUT_ORIGIN_INVALID;
	InputGlyphsConstants::InputOrigin forced_joy_origin = InputGlyphsConstants::InputOrigin::INPUT_ORIGIN_INVALID;
	InputGlyphsConstants::InputType input_type = InputGlyphsConstants::InputType::GENERIC_XINPUT_CONTROLLER;
	InputGlyphsConstants::InputType forced_input_type = InputGlyphsConstants::InputType::UNKNOWN;
	int action_skip_count = 0;

	bool glyph_update_queued = false;
	bool disable_axis_direction_display = false;

private:
	void _shape_action_name_text();
	void _shape_fallback_glyph_text();
	BitField<InputGlyphStyle> _get_glyph_style_with_override() const;
	void _on_input_glyphs_changed();
	InputGlyphsConstants::InputType _get_input_type() const;
	InputGlyphsConstants::InputOrigin _get_joy_origin() const;

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _queue_glyph_update();
	void set_action_name(StringName p_action_name);
	StringName get_action_name() const;

public:
	virtual Size2 get_minimum_size() const override;
	InputGlyphRect();

	String get_action_text() const;
	void set_action_text(const String &p_action_text);

	InputGlyphsConstants::InputType get_forced_input_type() const;
	void set_forced_input_type(const InputGlyphsConstants::InputType p_forced_input_type);

	InputGlyphsConstants::InputOrigin get_forced_joy_origin() const;
	void set_forced_joy_origin(const InputGlyphsConstants::InputOrigin p_forced_joy_origin);

	int get_action_skip_count() const;
	void set_action_skip_count(int p_action_skip_count);

	bool get_disable_axis_direction_display() const {
		return disable_axis_direction_display;
	}
	void set_disable_axis_direction_display(bool p_disable_show_axis_direction) {
		if (p_disable_show_axis_direction != disable_axis_direction_display) {
			disable_axis_direction_display = p_disable_show_axis_direction;
			_queue_glyph_update();
		}
	}
};
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
		int separation = 0;
		int icon_size = 32;
	} theme_cache;

	StringName action_name = "move_left";
	String action_text = "Action Text";
	String fallback_glyph_string = "Fallback";
	Ref<Texture2D> glyph_texture;
	InputGlyphsConstants::InputOrigin joy_origin = InputGlyphsConstants::InputOrigin::INPUT_ORIGIN_INVALID;

	bool glyph_update_queued = false;

private:
	void _shape_action_name_text();
	void _shape_fallback_glyph_text();
	BitField<InputGlyphStyle> _get_glyph_style_with_override() const;
	void _on_input_glyphs_changed();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _queue_glyph_update();

public:
	virtual Size2 get_minimum_size() const override;
	InputGlyphRect();

	String get_action_text() const;
	void set_action_text(const String &p_action_text);
};
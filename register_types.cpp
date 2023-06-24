#include "register_types.h"
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/object/class_db.h"
#include "input_action_glyph.h"
#include "input_glyph_texture_rect.h"
#include "input_glyphs_singleton.h"

InputGlyphsSingleton *glyphs_singleton = nullptr;

void initialize_input_glyphs_module(ModuleInitializationLevel p_level) {
	if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GLOBAL_DEF_BASIC(PropertyInfo(Variant::INT, "eirteam/input_glyphs/theme", PROPERTY_HINT_ENUM, "Knockout,Light,Dark"), GLYPH_STYLE_KNOCKOUT);
	GLOBAL_DEF_BASIC(PropertyInfo(Variant::INT, "eirteam/input_glyphs/abxy_overrides", PROPERTY_HINT_FLAGS, "Neutral Color ABXY:16, Solid ABXY:32"), 0);
	GDREGISTER_ABSTRACT_CLASS(InputGlyphsSingleton);
	GDREGISTER_CLASS(InputGlyphTextureRect);
	GDREGISTER_CLASS(InputActionGlyph);
	glyphs_singleton = memnew(InputGlyphsSingleton);
	Engine::get_singleton()->add_singleton(Engine::Singleton("InputGlyphsSingleton", glyphs_singleton->get_singleton()));
}

void uninitialize_input_glyphs_module(ModuleInitializationLevel p_level) {
	if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	memdelete(glyphs_singleton);
}

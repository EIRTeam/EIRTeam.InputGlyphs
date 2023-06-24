#include "register_types.h"
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/object/class_db.h"
#include "input_action_glyph.h"
#include "input_glyph_texture_rect.h"
#include "input_glyphs_singleton.h"

HBInputGlyphsSingleton *glyphs_singleton = nullptr;

void initialize_input_glyphs_module(ModuleInitializationLevel p_level) {
	if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GLOBAL_DEF_BASIC(PropertyInfo(Variant::INT, "eirteam/input_glyphs/theme", PROPERTY_HINT_ENUM, "Knockout,Light,Dark"), GLYPH_STYLE_KNOCKOUT);
	GLOBAL_DEF_BASIC(PropertyInfo(Variant::INT, "eirteam/input_glyphs/abxy_overrides", PROPERTY_HINT_FLAGS, "Neutral Color ABXY:16, Solid ABXY:32"), 0);
	GDREGISTER_ABSTRACT_CLASS(HBInputGlyphsSingleton);
	GDREGISTER_CLASS(HBInputGlyphTextureRect);
	GDREGISTER_CLASS(HBInputActionGlyph);
	glyphs_singleton = memnew(HBInputGlyphsSingleton);
	Engine::get_singleton()->add_singleton(Engine::Singleton("InputGlyphs", glyphs_singleton->get_singleton()));
}

void uninitialize_input_glyphs_module(ModuleInitializationLevel p_level) {
	if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	memdelete(glyphs_singleton);
}

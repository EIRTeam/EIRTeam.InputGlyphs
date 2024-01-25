#include "register_types.h"
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/object/class_db.h"
#include "input_glyph_rect.h"
#include "input_glyphs_constants.h"
#include "input_glyphs_singleton.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#endif

InputGlyphsSingleton *glyphs_singleton = nullptr;

static void _on_editor_init() {
	InputGlyphsSingleton::get_singleton()->init();
	InputGlyphsSingleton::get_singleton()->set_forced_input_type(InputGlyphsConstants::InputType::PS5_CONTROLLER);
}

void initialize_input_glyphs_module(ModuleInitializationLevel p_level) {
	if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

#ifdef TOOLS_ENABLED
	EditorNode::add_init_callback(_on_editor_init);
#endif // TOOLS_ENABLED

	GLOBAL_DEF_BASIC(PropertyInfo(Variant::INT, "eirteam/input_glyphs/theme", PROPERTY_HINT_ENUM, "Knockout,Light,Dark"), GLYPH_STYLE_KNOCKOUT);
	GLOBAL_DEF_BASIC(PropertyInfo(Variant::INT, "eirteam/input_glyphs/abxy_overrides", PROPERTY_HINT_FLAGS, "Neutral Color ABXY:16, Solid ABXY:32"), 0);
	GDREGISTER_ABSTRACT_CLASS(InputGlyphsSingleton);
	GDREGISTER_ABSTRACT_CLASS(InputGlyphsConstants);
	GDREGISTER_CLASS(InputGlyphRect);
	glyphs_singleton = memnew(InputGlyphsSingleton);
	Engine::get_singleton()->add_singleton(Engine::Singleton("InputGlyphsSingleton", glyphs_singleton->get_singleton()));
}

void uninitialize_input_glyphs_module(ModuleInitializationLevel p_level) {
	if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	memdelete(glyphs_singleton);
}

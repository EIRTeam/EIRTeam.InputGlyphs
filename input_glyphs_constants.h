#ifndef INPUT_GLYPHS_CONSTANTS_H
#define INPUT_GLYPHS_CONSTANTS_H

#include "core/object/ref_counted.h"
#include <iterator>

class InputGlyphsConstants : public RefCounted {
	GDCLASS(InputGlyphsConstants, RefCounted);

public:
	enum InputOrigin {
		INPUT_ORIGIN_INVALID = -1,
		INPUT_ORIGIN_A,
		INPUT_ORIGIN_B,
		INPUT_ORIGIN_X,
		INPUT_ORIGIN_Y,
		INPUT_ORIGIN_VIEW, // Select/Back
		INPUT_ORIGIN_MENU, // Start
		INPUT_ORIGIN_LEFTBUMPER,
		INPUT_ORIGIN_RIGHTBUMPER,
		INPUT_ORIGIN_LEFTTRIGGER_PULL,
		INPUT_ORIGIN_LEFTTRIGGER_CLICK,
		INPUT_ORIGIN_RIGHTTRIGGER_PULL,
		INPUT_ORIGIN_RIGHTTRIGGER_CLICK,
		INPUT_ORIGIN_LEFTSTICK_MOVE,
		INPUT_ORIGIN_LEFTSTICK_CLICK,
		INPUT_ORIGIN_LEFTSTICK_DPADNORTH,
		INPUT_ORIGIN_LEFTSTICK_DPADSOUTH,
		INPUT_ORIGIN_LEFTSTICK_DPADWEST,
		INPUT_ORIGIN_LEFTSTICK_DPADEAST,
		INPUT_ORIGIN_RIGHTSTICK_MOVE,
		INPUT_ORIGIN_RIGHTSTICK_CLICK,
		INPUT_ORIGIN_RIGHTSTICK_DPADNORTH,
		INPUT_ORIGIN_RIGHTSTICK_DPADSOUTH,
		INPUT_ORIGIN_RIGHTSTICK_DPADWEST,
		INPUT_ORIGIN_RIGHTSTICK_DPADEAST,
		INPUT_ORIGIN_DPAD_NORTH,
		INPUT_ORIGIN_DPAD_SOUTH,
		INPUT_ORIGIN_DPAD_WEST,
		INPUT_ORIGIN_DPAD_EAST,
		INPUT_ORIGIN_DPAD_MOVE,
		INPUT_ORIGIN_MISC1, // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button
		INPUT_ORIGIN_PADDLE1, // Xbox Elite paddle P1 (upper left, facing the back)
		INPUT_ORIGIN_PADDLE2, // Xbox Elite paddle P3 (upper right, facing the back)
		INPUT_ORIGIN_PADDLE3, // Xbox Elite paddle P2 (lower left, facing the back)
		INPUT_ORIGIN_PADDLE4, // Xbox Elite paddle P4 (lower right, facing the back)
		INPUT_ORIGIN_TRACKPAD_CLICK,
		INPUT_ORIGIN_COUNT,
	};
	static constexpr InputOrigin godot_button_to_input_origin_lut[] = {
		INPUT_ORIGIN_A,
		INPUT_ORIGIN_B,
		INPUT_ORIGIN_X,
		INPUT_ORIGIN_Y,
		INPUT_ORIGIN_VIEW,
		INPUT_ORIGIN_INVALID,
		INPUT_ORIGIN_MENU,
		INPUT_ORIGIN_LEFTSTICK_CLICK,
		INPUT_ORIGIN_RIGHTSTICK_CLICK,
		INPUT_ORIGIN_LEFTBUMPER,
		INPUT_ORIGIN_RIGHTBUMPER,
		INPUT_ORIGIN_DPAD_NORTH,
		INPUT_ORIGIN_DPAD_SOUTH,
		INPUT_ORIGIN_DPAD_WEST,
		INPUT_ORIGIN_DPAD_EAST,
		INPUT_ORIGIN_MISC1,
		INPUT_ORIGIN_PADDLE1,
		INPUT_ORIGIN_PADDLE2,
		INPUT_ORIGIN_PADDLE3,
		INPUT_ORIGIN_PADDLE4,
		INPUT_ORIGIN_TRACKPAD_CLICK,
	};
	static_assert(std::size(godot_button_to_input_origin_lut) == (int)JoyButton::SDL_MAX);

	static constexpr char const *_debug_input_origin_names[] = {
		"INPUT_ORIGIN_A",
		"INPUT_ORIGIN_B",
		"INPUT_ORIGIN_X",
		"INPUT_ORIGIN_Y",
		"INPUT_ORIGIN_VIEW", // Select/Back
		"INPUT_ORIGIN_MENU", // Start
		"INPUT_ORIGIN_LEFTBUMPER",
		"INPUT_ORIGIN_RIGHTBUMPER",
		"INPUT_ORIGIN_LEFTTRIGGER_PULL",
		"INPUT_ORIGIN_LEFTTRIGGER_CLICK",
		"INPUT_ORIGIN_RIGHTTRIGGER_PULL",
		"INPUT_ORIGIN_RIGHTTRIGGER_CLICK",
		"INPUT_ORIGIN_LEFTSTICK_MOVE",
		"INPUT_ORIGIN_LEFTSTICK_CLICK",
		"INPUT_ORIGIN_LEFTSTICK_DPADNORTH",
		"INPUT_ORIGIN_LEFTSTICK_DPADSOUTH",
		"INPUT_ORIGIN_LEFTSTICK_DPADWEST",
		"INPUT_ORIGIN_LEFTSTICK_DPADEAST",
		"INPUT_ORIGIN_RIGHTSTICK_MOVE",
		"INPUT_ORIGIN_RIGHTSTICK_CLICK",
		"INPUT_ORIGIN_RIGHTSTICK_DPADNORTH",
		"INPUT_ORIGIN_RIGHTSTICK_DPADSOUTH",
		"INPUT_ORIGIN_RIGHTSTICK_DPADWEST",
		"INPUT_ORIGIN_RIGHTSTICK_DPADEAST",
		"INPUT_ORIGIN_DPAD_NORTH",
		"INPUT_ORIGIN_DPAD_SOUTH",
		"INPUT_ORIGIN_DPAD_WEST",
		"INPUT_ORIGIN_DPAD_EAST",
		"INPUT_ORIGIN_DPAD_MOVE",
		"INPUT_ORIGIN_MISC1", // Xbox Series X share button", PS5 microphone button", Nintendo Switch Pro capture button", Amazon Luna microphone button
		"INPUT_ORIGIN_PADDLE1", // Xbox Elite paddle P1 (upper left", facing the back)
		"INPUT_ORIGIN_PADDLE2", // Xbox Elite paddle P3 (upper right", facing the back)
		"INPUT_ORIGIN_PADDLE3", // Xbox Elite paddle P2 (lower left", facing the back)
		"INPUT_ORIGIN_PADDLE4", // Xbox Elite paddle P4 (lower right", facing the back)
		"INPUT_ORIGIN_TRACKPAD_CLICK",
	};

	static_assert(std::size(_debug_input_origin_names) == InputOrigin::INPUT_ORIGIN_COUNT);

	enum InputType {
		UNKNOWN,
		STEAM_CONTROLLER,
		XBOX_360_CONTROLLER,
		XBOX_ONE_CONTROLLER,
		GENERIC_XINPUT_CONTROLLER,
		PS3_CONTROLLER,
		PS4_CONTROLLER,
		PS5_CONTROLLER,
		SWITCH_PRO_CONTROLLER,
		STEAM_DECK_CONTROLLER,
		INPUT_TYPE_MAX
	};

protected:
	static void _bind_methods();
};

VARIANT_ENUM_CAST(InputGlyphsConstants::InputOrigin);
VARIANT_ENUM_CAST(InputGlyphsConstants::InputType);

#endif // INPUT_GLYPHS_CONSTANTS_H
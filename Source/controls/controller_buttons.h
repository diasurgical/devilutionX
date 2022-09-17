#pragma once
// Unifies joystick, gamepad, and keyboard controller APIs.

#include <cstdint>
#include <string>

namespace devilution {

// NOTE: A, B, X, Y refer to physical positions on an XBox 360 controller.
// A<->B and X<->Y are reversed on a Nintendo controller.
enum ControllerButton : uint8_t {
	ControllerButton_NONE,
	ControllerButton_IGNORE,
	ControllerButton_AXIS_TRIGGERLEFT,  // ZL (aka L2)
	ControllerButton_AXIS_TRIGGERRIGHT, // ZR (aka R2)
	ControllerButton_BUTTON_A,          // Bottom button
	ControllerButton_BUTTON_B,          // Right button
	ControllerButton_BUTTON_X,          // Left button
	ControllerButton_BUTTON_Y,          // TOP button
	ControllerButton_BUTTON_LEFTSTICK,
	ControllerButton_BUTTON_RIGHTSTICK,
	ControllerButton_BUTTON_LEFTSHOULDER,
	ControllerButton_BUTTON_RIGHTSHOULDER,
	ControllerButton_BUTTON_START,
	ControllerButton_BUTTON_BACK,
	ControllerButton_BUTTON_DPAD_UP,
	ControllerButton_BUTTON_DPAD_DOWN,
	ControllerButton_BUTTON_DPAD_LEFT,
	ControllerButton_BUTTON_DPAD_RIGHT
};

inline bool IsDPadButton(ControllerButton button)
{
	return button == ControllerButton_BUTTON_DPAD_UP
	    || button == ControllerButton_BUTTON_DPAD_DOWN
	    || button == ControllerButton_BUTTON_DPAD_LEFT
	    || button == ControllerButton_BUTTON_DPAD_RIGHT;
}

namespace controllerButtonIcon {
using namespace std;
const std::string Playstation_Triangle = "\uE000";
const std::string Playstation_Square = "\uE001";
const std::string Playstation_X = "\uE002";
const std::string Playstation_Circle = "\uE003";
const std::string Playstation_Options = "\uE004";
const std::string Playstation_Share = "\uE005";
const std::string Playstation_L2 = "\uE006";
const std::string Playstation_R2 = "\uE007";
const std::string Playstation_L1 = "\uE008";
const std::string Playstation_R1 = "\uE009";
const std::string Playstation_DPad_Up = "\uE00A";
const std::string Playstation_DPad_Right = "\uE00B";
const std::string Playstation_DPad_Down = "\uE00C";
const std::string Playstation_DPad_Left = "\uE00D";
const std::string Playstation_LStick_NW = "\uE00E";
const std::string Playstation_LStick_W = "\uE00F";
const std::string Playstation_LStick_SW = "\uE010";
const std::string Playstation_LStick_N = "\uE011";
const std::string Playstation_LStick = "\uE012";
const std::string Playstation_LStick_S = "\uE013";
const std::string Playstation_LStick_NE = "\uE014";
const std::string Playstation_LStick_E = "\uE015";
const std::string Playstation_LStick_SE = "\uE016";
const std::string Playstation_L3 = "\uE017";
const std::string Playstation_RStick_NW = "\uE018";
const std::string Playstation_RStick_W = "\uE019";
const std::string Playstation_RStick_SW = "\uE01A";
const std::string Playstation_RStick_N = "\uE01B";
const std::string Playstation_RStick = "\uE01C";
const std::string Playstation_RStick_S = "\uE01D";
const std::string Playstation_RStick_NE = "\uE01E";
const std::string Playstation_RStick_E = "\uE01F";
const std::string Playstation_RStick_SE = "\uE020";
const std::string Playstation_R3 = "\uE021";
const std::string Playstation_Touchpad = "\uE022";
const std::string Nintendo_X = "\uE023";
const std::string Nintendo_Y = "\uE024";
const std::string Nintendo_B = "\uE025";
const std::string Nintendo_A = "\uE026";
const std::string Nintendo_Plus = "\uE027";
const std::string Nintendo_Minus = "\uE028";
const std::string Nintendo_ZL = "\uE029";
const std::string Nintendo_ZR = "\uE02A";
const std::string Nintendo_L = "\uE02B";
const std::string Nintendo_R = "\uE02C";
const std::string Nintendo_DPad_Up = "\uE02D";
const std::string Nintendo_DPad_Right = "\uE02E";
const std::string Nintendo_DPad_Down = "\uE02F";
const std::string Nintendo_DPad_Left = "\uE030";
const std::string Nintendo_LStick_NW = "\uE031";
const std::string Nintendo_LStick_W = "\uE032";
const std::string Nintendo_LStick_SW = "\uE033";
const std::string Nintendo_LStick_N = "\uE034";
const std::string Nintendo_LStick = "\uE035";
const std::string Nintendoo_LStick_S = "\uE036";
const std::string Nintendo_LStick_NE = "\uE037";
const std::string Nintendo_LStick_E = "\uE038";
const std::string Nintendo_LStick_SE = "\uE039";
const std::string Nintendo_LStick_Click = "\uE03A";
const std::string Nintendo_RStick_NW = "\uE03B";
const std::string Nintendo_RStick_W = "\uE03C";
const std::string Nintendo_RStick_SW = "\uE03D";
const std::string Nintendo_RStick_N = "\uE03E";
const std::string Nintendo_RStick = "\uE03F";
const std::string Nintendo_RStick_S = "\uE040";
const std::string Nintendo_RStick_NE = "\uE041";
const std::string Nintendo_RStick_E = "\uE042";
const std::string Nintendo_RStick_SE = "\uE043";
const std::string Nintendo_RStick_Click = "\uE044";
const std::string Nintendo_Home = "\uE045";
const std::string Nintendo_Screenshot = "\uE046";
const std::string Nintendo_SL = "\uE047";
const std::string Nintendo_SR = "\uE048";
const std::string Xbox_Y = "\uE049";
const std::string Xbox_X = "\uE04A";
const std::string Xbox_A = "\uE04B";
const std::string Xbox_B = "\uE04C";
const std::string Xbox_Menu = "\uE04D";
const std::string Xbox_View = "\uE04E";
const std::string Xbox_LT = "\uE04F";
const std::string Xbox_RT = "\uE050";
const std::string Xbox_LB = "\uE051";
const std::string Xbox_RB = "\uE052";
const std::string Xbox_DPad_Up = "\uE053";
const std::string Xbox_Dpad_Right = "\uE054";
const std::string Xbox_Dpad_Down = "\uE055";
const std::string Xbox_Dpad_Left = "\uE056";
const std::string Xbox_LStick_NW = "\uE057";
const std::string Xbox_LStick_W = "\uE058";
const std::string Xbox_LStick_SW = "\uE059";
const std::string Xbox_LStick_N = "\uE05A";
const std::string Xbox_LStick = "\uE05B";
const std::string Xbox_LStick_NE = "\uE05C";
const std::string Xbox_LStick_E = "\uE05D";
const std::string Xbox_LStick_SE = "\uE05E";
const std::string Xbox_LStick_Click = "\uE05F";
const std::string Xbox_RStick_NW = "\uE060";
const std::string Xbox_RStick_W = "\uE061";
const std::string Xbox_RStick_SW = "\uE062";
const std::string Xbox_RStick_N = "\uE063";
const std::string Xbox_RStick = "\uE064";
const std::string Xbox_RStick_S = "\uE065";
const std::string Xbox_RStick_NE = "\uE066";
const std::string Xbox_RStick_E = "\uE067";
const std::string Xbox_RStick_SE = "\uE068";
const std::string Xbox_RStick_Click = "\uE069";
const std::string Xbox_Xbox = "\uE06A";
} // namespace controllerButtonIcon
} // namespace devilution

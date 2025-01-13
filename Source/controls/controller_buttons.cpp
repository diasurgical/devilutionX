#include "controller_buttons.h"

namespace devilution {
namespace {
namespace controller_button_icon {
[[maybe_unused]] const std::string_view Playstation_Triangle = "\uE000";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_Square = "\uE001";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_X = "\uE002";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_Circle = "\uE003";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_Options = "\uE004";    // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_Share = "\uE005";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_L2 = "\uE006";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_R2 = "\uE007";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_L1 = "\uE008";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_R1 = "\uE009";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_DPad_Up = "\uE00A";    // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_DPad_Right = "\uE00B"; // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_DPad_Down = "\uE00C";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_DPad_Left = "\uE00D";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_NW = "\uE00E";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_W = "\uE00F";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_SW = "\uE010";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_N = "\uE011";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick = "\uE012";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_S = "\uE013";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_NE = "\uE014";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_E = "\uE015";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_LStick_SE = "\uE016";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_L3 = "\uE017";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_NW = "\uE018";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_W = "\uE019";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_SW = "\uE01A";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_N = "\uE01B";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick = "\uE01C";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_S = "\uE01D";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_NE = "\uE01E";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_E = "\uE01F";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_RStick_SE = "\uE020";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_R3 = "\uE021";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Playstation_Touchpad = "\uE022";   // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_X = "\uE023";             // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_Y = "\uE024";             // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_B = "\uE025";             // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_A = "\uE026";             // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_Plus = "\uE027";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_Minus = "\uE028";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_ZL = "\uE029";            // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_ZR = "\uE02A";            // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_L = "\uE02B";             // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_R = "\uE02C";             // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_DPad_Up = "\uE02D";       // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_DPad_Right = "\uE02E";    // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_DPad_Down = "\uE02F";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_DPad_Left = "\uE030";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_NW = "\uE031";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_W = "\uE032";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_SW = "\uE033";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_N = "\uE034";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick = "\uE035";        // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_S = "\uE036";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_NE = "\uE037";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_E = "\uE038";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_SE = "\uE039";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_LStick_Click = "\uE03A";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_NW = "\uE03B";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_W = "\uE03C";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_SW = "\uE03D";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_N = "\uE03E";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick = "\uE03F";        // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_S = "\uE040";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_NE = "\uE041";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_E = "\uE042";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_SE = "\uE043";     // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_RStick_Click = "\uE044";  // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_Home = "\uE045";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_Screenshot = "\uE046";    // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_SL = "\uE047";            // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Nintendo_SR = "\uE048";            // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_Y = "\uE049";                 // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_X = "\uE04A";                 // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_A = "\uE04B";                 // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_B = "\uE04C";                 // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_Menu = "\uE04D";              // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_View = "\uE04E";              // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LT = "\uE04F";                // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RT = "\uE050";                // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LB = "\uE051";                // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RB = "\uE052";                // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_DPad_Up = "\uE053";           // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_DPad_Right = "\uE054";        // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_DPad_Down = "\uE055";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_DPad_Left = "\uE056";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_NW = "\uE057";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_W = "\uE058";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_SW = "\uE059";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_N = "\uE05A";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick = "\uE05B";            // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_NE = "\uE05C";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_E = "\uE05D";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_SE = "\uE05E";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_LStick_Click = "\uE05F";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_NW = "\uE060";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_W = "\uE061";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_SW = "\uE062";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_N = "\uE063";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick = "\uE064";            // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_S = "\uE065";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_NE = "\uE066";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_E = "\uE067";          // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_SE = "\uE068";         // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_RStick_Click = "\uE069";      // NOLINT(readability-identifier-naming)
[[maybe_unused]] const std::string_view Xbox_Xbox = "\uE06A";              // NOLINT(readability-identifier-naming)
} // namespace controller_button_icon

std::string_view ToGenericButtonText(ControllerButton button)
{
	switch (button) {
	case devilution::ControllerButton_BUTTON_A:
		return "A";
	case devilution::ControllerButton_BUTTON_B:
		return "B";
	case devilution::ControllerButton_BUTTON_X:
		return "X";
	case devilution::ControllerButton_BUTTON_Y:
		return "Y";
	case devilution::ControllerButton_BUTTON_START:
		return "Start";
	case devilution::ControllerButton_BUTTON_BACK:
		return "Select";
	case devilution::ControllerButton_AXIS_TRIGGERLEFT:
		return "LT";
	case devilution::ControllerButton_AXIS_TRIGGERRIGHT:
		return "RT";
	case devilution::ControllerButton_BUTTON_LEFTSHOULDER:
		return "LB";
	case devilution::ControllerButton_BUTTON_RIGHTSHOULDER:
		return "RB";
	case devilution::ControllerButton_BUTTON_LEFTSTICK:
		return "LS";
	case devilution::ControllerButton_BUTTON_RIGHTSTICK:
		return "RS";
	case devilution::ControllerButton_BUTTON_DPAD_UP:
		return "Up";
	case devilution::ControllerButton_BUTTON_DPAD_DOWN:
		return "Down";
	case devilution::ControllerButton_BUTTON_DPAD_LEFT:
		return "Left";
	case devilution::ControllerButton_BUTTON_DPAD_RIGHT:
		return "Right";
	case devilution::ControllerButton_NONE:
		return "None";
	case devilution::ControllerButton_IGNORE:
		return "Ignored";
	default:
		return "Unknown";
	}
}

std::string_view ToPlayStationIcon(ControllerButton button)
{
	switch (button) {
	case devilution::ControllerButton_BUTTON_A:
		return controller_button_icon::Playstation_X;
	case devilution::ControllerButton_BUTTON_B:
		return controller_button_icon::Playstation_Circle;
	case devilution::ControllerButton_BUTTON_X:
		return controller_button_icon::Playstation_Square;
	case devilution::ControllerButton_BUTTON_Y:
		return controller_button_icon::Playstation_Triangle;
	case devilution::ControllerButton_BUTTON_START:
		return controller_button_icon::Playstation_Options;
	case devilution::ControllerButton_BUTTON_BACK:
		return controller_button_icon::Playstation_Share;
	case devilution::ControllerButton_AXIS_TRIGGERLEFT:
		return controller_button_icon::Playstation_L2;
	case devilution::ControllerButton_AXIS_TRIGGERRIGHT:
		return controller_button_icon::Playstation_R2;
	case devilution::ControllerButton_BUTTON_LEFTSHOULDER:
		return controller_button_icon::Playstation_L1;
	case devilution::ControllerButton_BUTTON_RIGHTSHOULDER:
		return controller_button_icon::Playstation_R1;
	case devilution::ControllerButton_BUTTON_LEFTSTICK:
		return controller_button_icon::Playstation_L3;
	case devilution::ControllerButton_BUTTON_RIGHTSTICK:
		return controller_button_icon::Playstation_R3;
	case devilution::ControllerButton_BUTTON_DPAD_UP:
		return controller_button_icon::Playstation_DPad_Up;
	case devilution::ControllerButton_BUTTON_DPAD_DOWN:
		return controller_button_icon::Playstation_DPad_Down;
	case devilution::ControllerButton_BUTTON_DPAD_LEFT:
		return controller_button_icon::Playstation_DPad_Left;
	case devilution::ControllerButton_BUTTON_DPAD_RIGHT:
		return controller_button_icon::Playstation_DPad_Right;
	default:
		return ToGenericButtonText(button);
	}
}

std::string_view ToNintendoIcon(ControllerButton button)
{
	switch (button) {
	case devilution::ControllerButton_BUTTON_A:
		return controller_button_icon::Nintendo_B;
	case devilution::ControllerButton_BUTTON_B:
		return controller_button_icon::Nintendo_A;
	case devilution::ControllerButton_BUTTON_X:
		return controller_button_icon::Nintendo_Y;
	case devilution::ControllerButton_BUTTON_Y:
		return controller_button_icon::Nintendo_X;
	case devilution::ControllerButton_BUTTON_START:
		return controller_button_icon::Nintendo_Plus;
	case devilution::ControllerButton_BUTTON_BACK:
		return controller_button_icon::Nintendo_Minus;
	case devilution::ControllerButton_AXIS_TRIGGERLEFT:
		return controller_button_icon::Nintendo_ZL;
	case devilution::ControllerButton_AXIS_TRIGGERRIGHT:
		return controller_button_icon::Nintendo_ZR;
	case devilution::ControllerButton_BUTTON_LEFTSHOULDER:
		return controller_button_icon::Nintendo_L;
	case devilution::ControllerButton_BUTTON_RIGHTSHOULDER:
		return controller_button_icon::Nintendo_R;
	case devilution::ControllerButton_BUTTON_LEFTSTICK:
		return controller_button_icon::Nintendo_LStick_Click;
	case devilution::ControllerButton_BUTTON_RIGHTSTICK:
		return controller_button_icon::Nintendo_RStick_Click;
	case devilution::ControllerButton_BUTTON_DPAD_UP:
		return controller_button_icon::Nintendo_DPad_Up;
	case devilution::ControllerButton_BUTTON_DPAD_DOWN:
		return controller_button_icon::Nintendo_DPad_Down;
	case devilution::ControllerButton_BUTTON_DPAD_LEFT:
		return controller_button_icon::Nintendo_DPad_Left;
	case devilution::ControllerButton_BUTTON_DPAD_RIGHT:
		return controller_button_icon::Nintendo_DPad_Right;
	default:
		return ToGenericButtonText(button);
	}
}

std::string_view ToXboxIcon(ControllerButton button)
{
	switch (button) {
	case devilution::ControllerButton_BUTTON_A:
		return controller_button_icon::Xbox_A;
	case devilution::ControllerButton_BUTTON_B:
		return controller_button_icon::Xbox_B;
	case devilution::ControllerButton_BUTTON_X:
		return controller_button_icon::Xbox_X;
	case devilution::ControllerButton_BUTTON_Y:
		return controller_button_icon::Xbox_Y;
	case devilution::ControllerButton_BUTTON_START:
		return controller_button_icon::Xbox_Menu;
	case devilution::ControllerButton_BUTTON_BACK:
		return controller_button_icon::Xbox_View;
	case devilution::ControllerButton_AXIS_TRIGGERLEFT:
		return controller_button_icon::Xbox_LT;
	case devilution::ControllerButton_AXIS_TRIGGERRIGHT:
		return controller_button_icon::Xbox_RT;
	case devilution::ControllerButton_BUTTON_LEFTSHOULDER:
		return controller_button_icon::Xbox_LB;
	case devilution::ControllerButton_BUTTON_RIGHTSHOULDER:
		return controller_button_icon::Xbox_RB;
	case devilution::ControllerButton_BUTTON_LEFTSTICK:
		return controller_button_icon::Xbox_LStick_Click;
	case devilution::ControllerButton_BUTTON_RIGHTSTICK:
		return controller_button_icon::Xbox_RStick_Click;
	case devilution::ControllerButton_BUTTON_DPAD_UP:
		return controller_button_icon::Xbox_DPad_Up;
	case devilution::ControllerButton_BUTTON_DPAD_DOWN:
		return controller_button_icon::Xbox_DPad_Down;
	case devilution::ControllerButton_BUTTON_DPAD_LEFT:
		return controller_button_icon::Xbox_DPad_Left;
	case devilution::ControllerButton_BUTTON_DPAD_RIGHT:
		return controller_button_icon::Xbox_DPad_Right;
	default:
		return ToGenericButtonText(button);
	}
}

} // namespace

std::string_view ToString(GamepadLayout gamepadType, ControllerButton button)
{
	switch (gamepadType) {
	case GamepadLayout::PlayStation:
		return ToPlayStationIcon(button);
	case GamepadLayout::Nintendo:
		return ToNintendoIcon(button);
	case GamepadLayout::Xbox:
		return ToXboxIcon(button);
	default:
	case GamepadLayout::Generic:
		return ToGenericButtonText(button);
	}
}

} // namespace devilution

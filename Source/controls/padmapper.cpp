#include "controls/padmapper.hpp"

#include <array>

#include "controller.h"
#include "game_controls.h"
#include "options.h"

namespace devilution {

namespace {
std::array<const PadmapperOptions::Action *, enum_size<ControllerButton>::value> ButtonToReleaseAction;
} // namespace

void PadmapperPress(ControllerButton button, const PadmapperOptions::Action &action)
{
	if (action.actionPressed) action.actionPressed();
	SuppressedButton = action.boundInput.modifier;
	ButtonToReleaseAction[static_cast<size_t>(button)] = &action;
}

void PadmapperRelease(ControllerButton button, bool invokeAction)
{
	if (invokeAction) {
		const PadmapperOptions::Action *action = ButtonToReleaseAction[static_cast<size_t>(button)];
		if (action == nullptr)
			return; // Ignore unmapped buttons.

		// Check that the action can be triggered.
		if (action->actionReleased && action->isEnabled())
			action->actionReleased();
	}
	ButtonToReleaseAction[static_cast<size_t>(button)] = nullptr;
}

bool PadmapperIsActionActive(std::string_view actionName)
{
	for (const PadmapperOptions::Action &action : GetOptions().Padmapper.actions) {
		if (action.key != actionName)
			continue;
		const PadmapperOptions::Action *releaseAction = ButtonToReleaseAction[static_cast<size_t>(action.boundInput.button)];
		return releaseAction != nullptr && releaseAction->key == actionName;
	}
	return false;
}

void PadmapperReleaseAllActiveButtons()
{
	for (const PadmapperOptions::Action *action : ButtonToReleaseAction) {
		if (action != nullptr) {
			PadmapperRelease(action->boundInput.button, /*invokeAction=*/true);
		}
	}
}

std::string_view PadmapperActionNameTriggeredByButtonEvent(ControllerButtonEvent ctrlEvent)
{
	if (!ctrlEvent.up) {
		const PadmapperOptions::Action *pressAction = GetOptions().Padmapper.findAction(ctrlEvent.button, IsControllerButtonPressed);
		if (pressAction == nullptr) return {};
		return pressAction->key;
	}
	const PadmapperOptions::Action *releaseAction = ButtonToReleaseAction[static_cast<size_t>(ctrlEvent.button)];
	if (releaseAction == nullptr) return {};
	return releaseAction->key;
}

} // namespace devilution

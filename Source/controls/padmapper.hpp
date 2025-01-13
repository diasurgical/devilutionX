#pragma once

#include <string_view>

#include "controls/controller_buttons.h"
#include "options.h"

namespace devilution {

void PadmapperPress(ControllerButton button, const PadmapperOptions::Action &action);
void PadmapperRelease(ControllerButton button, bool invokeAction);
void PadmapperReleaseAllActiveButtons();
[[nodiscard]] bool PadmapperIsActionActive(std::string_view actionName);
[[nodiscard]] std::string_view PadmapperActionNameTriggeredByButtonEvent(ControllerButtonEvent ctrlEvent);

} // namespace devilution

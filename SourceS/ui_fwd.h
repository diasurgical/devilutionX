#pragma once

namespace dvl {

extern Sint16 screenWidth;
extern Sint16 screenHeight;
extern Sint16 viewportHeight;
extern Sint16 borderRight;

bool SpawnWindow(const char *lpWindowName);
void UiErrorOkDialog(const char *text, const char *caption, bool error = true);

} // namespace dvl

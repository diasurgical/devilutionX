#pragma once

namespace dvl {

extern int screenWidth;
extern int screenHeight;
extern int viewportHeight;
extern int borderRight;

bool SpawnWindow(const char *lpWindowName);
void UiErrorOkDialog(const char *text, const char *caption, bool error = true);

} // namespace dvl

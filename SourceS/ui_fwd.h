#pragma once

namespace dvl {

extern Uint16 gnScreenWidth;
extern Uint16 gnScreenHeight;
extern Uint16 gnViewportHeight;
extern Uint16 borderRight;

bool SpawnWindow(const char *lpWindowName);
void UiErrorOkDialog(const char *text, const char *caption, bool error = true);

} // namespace dvl

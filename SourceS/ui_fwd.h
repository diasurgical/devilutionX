#pragma once

namespace dvl {

bool SpawnWindow(const char *lpWindowName, int nWidth, int nHeight);
void UiErrorOkDialog(const char *text, const char *caption, bool error = true);

} // namespace dvl

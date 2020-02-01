#pragma once

namespace dvl {

bool SpawnWindow(LPCSTR lpWindowName, int nWidth, int nHeight);
void UiErrorOkDialog(const char *text, const char *caption, bool error = true);

} // namespace dvl

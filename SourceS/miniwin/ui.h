#pragma once

namespace dvl {

BOOL EndDialog(HWND hDlg, INT_PTR nResult);
void UiErrorOkDialog(const char *text, const char *caption, bool error = true);

} // namespace dvl

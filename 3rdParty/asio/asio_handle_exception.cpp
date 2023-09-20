#include <string_view>

#define ErrAsio(message) devilution::ErrDlg("ASIO Error", message, __FILE__, __LINE__)

namespace devilution {

extern void ErrDlg(const char* title, std::string_view error, std::string_view logFilePath, int logLineNr);

} // namespace devilution

namespace asio::detail {

void fatal_exception(const char* message)
{
	ErrAsio(message);
}

} // namespace asio::detail

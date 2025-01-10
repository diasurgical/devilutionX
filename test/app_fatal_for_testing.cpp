#include <iostream>
#include <string_view>

namespace devilution {

[[noreturn]] void DisplayFatalErrorAndExit(std::string_view title, std::string_view body)
{
	std::cerr << "fatal error: " << title << "\n"
	          << body << std::endl;
	std::abort();
}

[[noreturn]] void app_fatal(std::string_view str)
{
	std::cerr << "app_fatal: " << str << std::endl;
	std::abort();
}

[[noreturn]] void ErrDlg(const char *title, std::string_view error, std::string_view logFilePath, int logLineNr)
{
	std::cerr << "ErrDlg: " << title << "\n"
	          << error << "\n"
	          << logFilePath << ":" << logLineNr << std::endl;
	std::abort();
}

[[noreturn]] void assert_fail(int nLineNo, const char *pszFile, const char *pszFail)
{
	std::cerr << "assert_fail: " << pszFile << ":" << nLineNo << "\n"
	          << pszFail << std::endl;
	std::abort();
}

[[noreturn]] void InsertCDDlg(std::string_view archiveName)
{
	std::cerr << "InsertCDDlg error: " << archiveName << std::endl;
	std::abort();
}

} // namespace devilution

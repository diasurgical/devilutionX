#pragma once

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <optional>
#include <string>

#include "utils/file_util.h"
#include "utils/log.hpp"
namespace devilution {

// A wrapper around `FILE *` that logs errors.
struct LoggedFStream {
public:
	bool Open(const char *path, const char *mode)
	{
		s_ = OpenFile(path, mode);
		return CheckError(s_ != nullptr, "fopen(\"{}\", \"{}\")", path, mode);
	}

	void Close()
	{
		if (s_ != nullptr) {
			std::fclose(s_);
			s_ = nullptr;
		}
	}

	[[nodiscard]] bool IsOpen() const
	{
		return s_ != nullptr;
	}

	bool Seekp(long pos, int dir = SEEK_SET)
	{
		return CheckError(std::fseek(s_, pos, dir) == 0,
		    "fseek({}, {})", pos, DirToString(dir));
	}

	bool Tellp(long *result)
	{
		*result = std::ftell(s_);
		return CheckError(*result != -1L,
		    "ftell() = {}", *result);
	}

	bool Write(const char *data, size_t size)
	{
		return CheckError(std::fwrite(data, size, 1, s_) == 1,
		    "fwrite(data, {})", size);
	}

	bool Read(char *out, size_t size)
	{
		return CheckError(std::fread(out, size, 1, s_) == 1,
		    "fread(out, {})", size);
	}

private:
	static const char *DirToString(int dir);

	template <typename... PrintFArgs>
	bool CheckError(bool ok, const char *fmt, PrintFArgs... args)
	{
		if (!ok) {
			std::string fmtWithError = fmt;
			fmtWithError.append(": failed with \"{}\"");
			const char *errorMessage = std::strerror(errno);
			if (errorMessage == nullptr)
				errorMessage = "";
			LogError(LogCategory::System, fmtWithError.c_str(), args..., errorMessage);
		} else {
			LogVerbose(LogCategory::System, fmt, args...);
		}
		return ok;
	}

	FILE *s_ = nullptr;
};

} // namespace devilution

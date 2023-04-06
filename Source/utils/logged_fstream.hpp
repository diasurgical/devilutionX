#pragma once

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>

#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

// A wrapper around `FILE *` that logs errors.
struct LoggedFStream {
public:
	bool Open(const char *path, const char *mode)
	{
		s_ = OpenFile(path, mode);
		return CheckError("fopen(\"{}\", \"{}\")", path, mode);
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
		std::fseek(s_, pos, dir);
		return CheckError("fseek({}, {})", pos, DirToString(dir));
	}

	bool Tellp(long *result)
	{
		*result = std::ftell(s_);
		return CheckError("ftell() = {}", *result);
	}

	bool Write(const char *data, size_t size)
	{
		std::fwrite(data, size, 1, s_);
		return CheckError("fwrite(data, {})", size);
	}

	bool Read(char *out, size_t size)
	{
		std::fread(out, size, 1, s_);
		return CheckError("fread(out, {})", size);
	}

private:
	static const char *DirToString(int dir);

	template <typename... PrintFArgs>
	bool CheckError(const char *fmt, PrintFArgs... args)
	{
		const bool ok = s_ != nullptr && std::ferror(s_) == 0;
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

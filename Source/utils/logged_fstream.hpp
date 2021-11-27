#pragma once

#include <cerrno>
#include <cstring>
#include <fstream>
#include <string>

#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

// A wrapper around `std::fstream` that logs errors.
struct LoggedFStream {
public:
	bool Open(const char *path, std::ios::openmode mode)
	{
		s_ = CreateFileStream(path, mode);
		return CheckError("new std::fstream(\"{}\", {})", path, OpenModeToString(mode).c_str());
	}

	void Close()
	{
		s_ = std::nullopt;
	}

	[[nodiscard]] bool IsOpen() const
	{
		return s_ != std::nullopt;
	}

	bool Seekp(std::streampos pos)
	{
		s_->seekp(pos);
		return CheckError("seekp({})", pos);
	}

	bool Seekp(std::streamoff pos, std::ios::seekdir dir)
	{
		s_->seekp(pos, dir);
		return CheckError("seekp({}, {})", pos, DirToString(dir));
	}

	bool Tellp(std::streampos *result)
	{
		*result = s_->tellp();
		return CheckError("tellp() = {}", *result);
	}

	bool Write(const char *data, std::streamsize size)
	{
		s_->write(data, size);
		return CheckError("write(data, {})", size);
	}

	bool Read(char *out, std::streamsize size)
	{
		s_->read(out, size);
		return CheckError("read(out, {})", size);
	}

private:
	static const char *DirToString(std::ios::seekdir dir);
	static std::string OpenModeToString(std::ios::openmode mode);

	template <typename... PrintFArgs>
	bool CheckError(const char *fmt, PrintFArgs... args)
	{
		if (s_->fail()) {
			std::string fmtWithError = fmt;
			fmtWithError.append(": failed with \"{}\"");
			const char *errorMessage = std::strerror(errno);
			if (errorMessage == nullptr)
				errorMessage = "";
			LogError(LogCategory::System, fmtWithError.c_str(), args..., errorMessage);
		} else {
			LogVerbose(LogCategory::System, fmt, args...);
		}
		return !s_->fail();
	}

	std::optional<std::fstream> s_;
};

} // namespace devilution

#pragma once

#include <SDL.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "utils/stdcompat/string_view.hpp"
#include "utils/str_cat.hpp"

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

namespace devilution {

// Local definition to fix compilation issue due to header conflict.
[[noreturn]] void app_fatal(string_view);

enum class LogCategory {
	Application = SDL_LOG_CATEGORY_APPLICATION,
	Error = SDL_LOG_CATEGORY_ERROR,
	Assert = SDL_LOG_CATEGORY_ASSERT,
	System = SDL_LOG_CATEGORY_SYSTEM,
	Audio = SDL_LOG_CATEGORY_AUDIO,
	Video = SDL_LOG_CATEGORY_VIDEO,
	Render = SDL_LOG_CATEGORY_RENDER,
	Input = SDL_LOG_CATEGORY_INPUT,
	Test = SDL_LOG_CATEGORY_TEST,
};

constexpr auto defaultCategory = LogCategory::Application;

enum class LogPriority {
	Verbose = SDL_LOG_PRIORITY_VERBOSE,
	Debug = SDL_LOG_PRIORITY_DEBUG,
	Info = SDL_LOG_PRIORITY_INFO,
	Warn = SDL_LOG_PRIORITY_WARN,
	Error = SDL_LOG_PRIORITY_ERROR,
	Critical = SDL_LOG_PRIORITY_CRITICAL,
};

namespace detail {

template <typename... Args>
std::string format(string_view fmt, Args &&...args)
{
	FMT_TRY
	{
		return fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
	}
	FMT_CATCH(const fmt::format_error &e)
	{
#if FMT_EXCEPTIONS
		// e.what() is undefined if exceptions are disabled, so we wrap the whole block
		// with an `FMT_EXCEPTIONS` check.
		std::string error = StrCat("Format error, fmt: ", fmt, " error: ", e.what());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", error.c_str());
		app_fatal(error);
#endif
	}
}

} // namespace detail

template <typename... Args>
void Log(string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_Log("%s", str.c_str());
}

template <typename... Args>
void LogVerbose(LogCategory category, string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogVerbose(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogVerbose(string_view fmt, Args &&...args)
{
	LogVerbose(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogDebug(LogCategory category, string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogDebug(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogDebug(string_view fmt, Args &&...args)
{
	LogDebug(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogInfo(LogCategory category, string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogInfo(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogInfo(string_view fmt, Args &&...args)
{
	LogInfo(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogWarn(LogCategory category, string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogWarn(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogWarn(string_view fmt, Args &&...args)
{
	LogWarn(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogError(LogCategory category, string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogError(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogError(string_view fmt, Args &&...args)
{
	LogError(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogCritical(LogCategory category, string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogCritical(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogCritical(string_view fmt, Args &&...args)
{
	LogCritical(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogMessageV(LogCategory category, LogPriority priority, string_view fmt, Args &&...args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogMessageV(static_cast<int>(category), static_cast<SDL_LogPriority>(priority), "%s", str.c_str());
}

template <typename... Args>
void LogMessageV(string_view fmt, Args &&...args)
{
	LogMessageV(defaultCategory, fmt, std::forward<Args>(args)...);
}

} // namespace devilution

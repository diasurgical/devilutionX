#pragma once

#include <SDL.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

namespace devilution {

// Local definition to fix compilation issue due to header conflict.
[[noreturn]] void app_fatal(const char *pszFmt, ...);

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
std::string format(const char *fmt, Args &&... args)
{
	FMT_TRY
	{
		return fmt::format(fmt, std::forward<Args>(args)...);
	}
	FMT_CATCH(const fmt::format_error &e)
	{
		auto error = fmt::format("Format error, fmt: {}, error: {}", fmt ? fmt : "nullptr", e.what());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", error.c_str());
		app_fatal("%s", error.c_str());
	}
}

} // namespace detail

template <typename... Args>
void Log(const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_Log("%s", str.c_str());
}

template <typename... Args>
void LogVerbose(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogVerbose(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogVerbose(const char *fmt, Args &&... args)
{
	LogVerbose(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogDebug(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogDebug(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogDebug(const char *fmt, Args &&... args)
{
	LogDebug(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogInfo(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogInfo(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogInfo(const char *fmt, Args &&... args)
{
	LogInfo(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogWarn(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogWarn(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogWarn(const char *fmt, Args &&... args)
{
	LogWarn(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogError(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogError(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogError(const char *fmt, Args &&... args)
{
	LogError(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogCritical(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogCritical(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogCritical(const char *fmt, Args &&... args)
{
	LogCritical(defaultCategory, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void LogMessageV(LogCategory category, LogPriority priority, const char *fmt, Args &&... args)
{
	auto str = detail::format(fmt, std::forward<Args>(args)...);
	SDL_LogMessageV(static_cast<int>(category), static_cast<SDL_LogPriority>(priority), "%s", str.c_str());
}

template <typename... Args>
void LogMessageV(const char *fmt, Args &&... args)
{
	LogMessageV(defaultCategory, fmt, std::forward<Args>(args)...);
}

} // namespace devilution

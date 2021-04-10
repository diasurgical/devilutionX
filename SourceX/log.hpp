#pragma once

#ifdef _DEBUG
#ifdef TRACE_LOGGING
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include "spdlog/spdlog.h"

#include <vector>
#include <memory>
#include <cassert>

namespace devilution {

namespace log {

enum Category {
	Main,
	Storm,
	Drawing,
	Audio,
	Input,
	Network,
};

class Logger final {
public:
	Logger(const std::string &logFilepath);
	~Logger();

	template <typename... Args>
	static void trace(Category category, Args &&... args)
	{
		loggerFromCategory(category)->trace(std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void debug(Category category, Args &&... args)
	{
		loggerFromCategory(category)->debug(std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void info(Category category, Args &&... args)
	{
		loggerFromCategory(category)->info(std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void warn(Category category, Args &&... args)
	{
		loggerFromCategory(category)->warn(std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void error(Category category, Args &&... args)
	{
		loggerFromCategory(category)->error(std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void critical(Category category, Args &&... args)
	{
		loggerFromCategory(category)->critical(std::forward<Args>(args)...);
	}

	static std::shared_ptr<spdlog::logger> loggerFromCategory(Category category)
	{
		assert(category < gLogger->loggers.size());
		return gLogger->loggers[category];
	}

private:
	static Logger *gLogger;
	std::vector<std::shared_ptr<spdlog::logger>> loggers;
};

} // namespace log

} // namespace devilution

#define LOG_TRACE(Category, ...) SPDLOG_LOGGER_TRACE(devilution::log::Logger::loggerFromCategory(Category), __VA_ARGS__)
#define LOG_DEBUG(Category, ...) SPDLOG_LOGGER_DEBUG(devilution::log::Logger::loggerFromCategory(Category), __VA_ARGS__)
#define LOG_INFO(Category, ...) SPDLOG_LOGGER_INFO(devilution::log::Logger::loggerFromCategory(Category), __VA_ARGS__)
#define LOG_WARN(Category, ...) SPDLOG_LOGGER_WARN(devilution::log::Logger::loggerFromCategory(Category), __VA_ARGS__)
#define LOG_ERROR(Category, ...) SPDLOG_LOGGER_ERROR(devilution::log::Logger::loggerFromCategory(Category), __VA_ARGS__)
#define LOG_CRITICAL(Category, ...) SPDLOG_LOGGER_CRITICAL(devilution::log::Logger::loggerFromCategory(Category), __VA_ARGS__)

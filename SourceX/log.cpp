#include "log.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace devilution {

namespace log {

Logger *Logger::gLogger = nullptr;

Logger::Logger(const std::string &logFilepath)
{
	assert(gLogger == nullptr);
	gLogger = this;

	std::vector<spdlog::sink_ptr> sinks{
		std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
	};

	if (!logFilepath.empty()) {
		sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_st>(logFilepath, true));
	}

	loggers = {
		std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end()),
		std::make_shared<spdlog::logger>("storm", sinks.begin(), sinks.end()),
		std::make_shared<spdlog::logger>("drawing", sinks.begin(), sinks.end()),
		std::make_shared<spdlog::logger>("audio", sinks.begin(), sinks.end()),
		std::make_shared<spdlog::logger>("input", sinks.begin(), sinks.end()),
		std::make_shared<spdlog::logger>("network", sinks.begin(), sinks.end()),
	};
}

Logger::~Logger()
{
	spdlog::shutdown();
}

} // namespace log

} // namespace devilution

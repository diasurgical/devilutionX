#include "./sdl2_to_1_2_backports.h"

#include <cstddef>

#include "./console.h"

#define DEFAULT_PRIORITY SDL_LOG_PRIORITY_CRITICAL
#define DEFAULT_ASSERT_PRIORITY SDL_LOG_PRIORITY_WARN
#define DEFAULT_APPLICATION_PRIORITY SDL_LOG_PRIORITY_INFO
#define DEFAULT_TEST_PRIORITY SDL_LOG_PRIORITY_VERBOSE

namespace {

// We use the same names of these structs as the SDL2 implementation:
// NOLINTNEXTLINE(readability-identifier-naming)
struct SDL_LogLevel {
	int category;
	SDL_LogPriority priority;
	SDL_LogLevel *next;
};

SDL_LogLevel *SDL_loglevels;                                             // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_default_priority = DEFAULT_PRIORITY;                 // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_assert_priority = DEFAULT_ASSERT_PRIORITY;           // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_application_priority = DEFAULT_APPLICATION_PRIORITY; // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_test_priority = DEFAULT_TEST_PRIORITY;               // NOLINT(readability-identifier-naming)

// NOLINTNEXTLINE(readability-identifier-naming)
const char *const SDL_priority_prefixes[SDL_NUM_LOG_PRIORITIES] = {
	nullptr,
	"VERBOSE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"CRITICAL"
};

} // namespace

void SDL_LogSetAllPriority(SDL_LogPriority priority)
{
	for (SDL_LogLevel *entry = SDL_loglevels; entry != nullptr; entry = entry->next) {
		entry->priority = priority;
	}
	SDL_default_priority = priority;
	SDL_assert_priority = priority;
	SDL_application_priority = priority;
}

void SDL_LogSetPriority(int category, SDL_LogPriority priority)
{
	SDL_LogLevel *entry;
	for (entry = SDL_loglevels; entry != nullptr; entry = entry->next) {
		if (entry->category == category) {
			entry->priority = priority;
			return;
		}
	}

	entry = static_cast<SDL_LogLevel *>(SDL_malloc(sizeof(*entry)));
	if (entry != nullptr) {
		entry->category = category;
		entry->priority = priority;
		entry->next = SDL_loglevels;
		SDL_loglevels = entry;
	}
}

SDL_LogPriority SDL_LogGetPriority(int category)
{
	for (SDL_LogLevel *entry = SDL_loglevels; entry != nullptr; entry = entry->next) {
		if (entry->category == category) {
			return entry->priority;
		}
	}

	switch (category) {
	case SDL_LOG_CATEGORY_TEST:
		return SDL_test_priority;
	case SDL_LOG_CATEGORY_APPLICATION:
		return SDL_application_priority;
	case SDL_LOG_CATEGORY_ASSERT:
		return SDL_assert_priority;
	default:
		return SDL_default_priority;
	}
}

void SDL_Log(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, ap);
	va_end(ap);
}

void SDL_LogVerbose(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_VERBOSE, fmt, ap);
	va_end(ap);
}

void SDL_LogDebug(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_DEBUG, fmt, ap);
	va_end(ap);
}

void SDL_LogInfo(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_INFO, fmt, ap);
	va_end(ap);
}

void SDL_LogWarn(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_WARN, fmt, ap);
	va_end(ap);
}

void SDL_LogError(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_ERROR, fmt, ap);
	va_end(ap);
}

void SDL_LogCritical(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_CRITICAL, fmt, ap);
	va_end(ap);
}

void SDL_LogMessage(int category, SDL_LogPriority priority, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, priority, fmt, ap);
	va_end(ap);
}

void SDL_LogMessageV(int category, SDL_LogPriority priority, const char *fmt, va_list ap)
{
	if (static_cast<int>(priority) < 0 || priority >= SDL_NUM_LOG_PRIORITIES || priority < SDL_LogGetPriority(category))
		return;

	::devilution::printInConsole("%s: ", SDL_priority_prefixes[priority]);
	::devilution::printInConsoleV(fmt, ap);
	::devilution::printInConsole("\n");
}

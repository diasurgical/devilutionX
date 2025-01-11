#pragma once

#include <array>

namespace devilution {

struct QuickMessage {
	/** Config variable names for quick message */
	const char *const key;
	/** Default quick message */
	const char *const message;
};

extern std::array<QuickMessage, 10> QuickMessages;

} // namespace devilution

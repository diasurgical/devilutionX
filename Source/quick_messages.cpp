#include "quick_messages.hpp"

#include <array>

#include "utils/language.h"

namespace devilution {

std::array<QuickMessage, 10> QuickMessages = {
	QuickMessage { "QuickMessage1", N_("I need help! Come here!") },
	QuickMessage { "QuickMessage2", N_("Follow me.") },
	QuickMessage { "QuickMessage3", N_("Here's something for you.") },
	QuickMessage { "QuickMessage4", N_("Now you DIE!") },
	QuickMessage { "QuickMessage5", N_("Heal yourself!") },
	QuickMessage { "QuickMessage6", N_("Watch out!") },
	QuickMessage { "QuickMessage7", N_("Thanks.") },
	QuickMessage { "QuickMessage8", N_("Retreat!") },
	QuickMessage { "QuickMessage9", N_("Sorry.") },
	QuickMessage { "QuickMessage10", N_("I'm waiting.") },
};

} // namespace devilution

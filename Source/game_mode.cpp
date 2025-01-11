#include "game_mode.hpp"

#include <function_ref.hpp>

#include "options.h"

namespace devilution {
namespace {
std::vector<tl::function_ref<void()>> IsHellfireChangeHandlers;

void OptionGameModeChanged()
{
	gbIsHellfire = *GetOptions().GameMode.gameMode == StartUpGameMode::Hellfire;
	for (tl::function_ref<void()> handler : IsHellfireChangeHandlers) {
		handler();
	}
}
const auto OptionChangeHandlerGameMode = (GetOptions().GameMode.gameMode.SetValueChangedCallback(OptionGameModeChanged), true);

void OptionSharewareChanged()
{
	gbIsSpawn = *GetOptions().GameMode.shareware;
}
const auto OptionChangeHandlerShareware = (GetOptions().GameMode.shareware.SetValueChangedCallback(OptionSharewareChanged), true);
} // namespace

bool gbIsSpawn;
bool gbIsHellfire;
bool gbVanilla;
bool forceHellfire;

void AddIsHellfireChangeHandler(tl::function_ref<void()> callback)
{
	IsHellfireChangeHandlers.push_back(callback);
}

} // namespace devilution

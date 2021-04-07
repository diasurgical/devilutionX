#include "keymapper.hpp"
#include "miniwin/misc.h"

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

#include <cassert>

namespace devilution {

Keymapper::Keymapper(SetConfigKeyFunction setKeyFunction, GetConfigKeyFunction getKeyFunction)
    : setKeyFunction(setKeyFunction)
    , getKeyFunction(getKeyFunction)
{
	keyIDToKeyName.reserve('Z' - 'A' + '9' - '0' + 12);
	for (char c = 'A'; c <= 'Z'; ++c) {
		keyIDToKeyName.emplace(c, std::string(1, c));
	}
	for (char c = '0'; c <= '9'; ++c) {
		keyIDToKeyName.emplace(c, std::string(1, c));
	}
	for (int i = 0; i < 12; ++i) {
		keyIDToKeyName.emplace(DVL_VK_F1 + i, std::string(1, 'F') + std::to_string(i + 1));
	}

	keyNameToKeyID.reserve(keyIDToKeyName.size());
	for (const auto &kv : keyIDToKeyName) {
		keyNameToKeyID.emplace(kv.second, kv.first);
	}
}

Keymapper::ActionIndex Keymapper::addAction(const Action &action)
{
	actions.emplace_back(action);

	return actions.size() - 1;
}

void Keymapper::keyPressed(int key, bool isPlayerDead) const
{
	auto it = keyIDToAction.find(key);
	if (it == keyIDToAction.end())
		return; // Ignore unmapped keys.

	auto &action = it->second;

	// Some actions cannot be triggered while the player is dead.
	if (isPlayerDead && action.get().ifDead == Action::IfDead::Ignore)
		return;

	action();
}

std::string Keymapper::keyNameForAction(ActionIndex actionIndex) const
{
	assert(actionIndex < actions.size());
	auto key = actions[actionIndex].key;
	auto it = keyIDToKeyName.find(key);
	assert(it != keyIDToKeyName.end());
	return it->second;
}

void Keymapper::save() const
{
	// Use the action vector to go though the actions to keep the same order.
	for (const auto &action : actions) {
		auto keyNameIt = keyIDToKeyName.find(action.key);
		if (keyNameIt == keyIDToKeyName.end()) {
			SDL_Log("Keymapper: no name found for key '%d'", action.key);
			continue;
		}
		setKeyFunction(action.name, keyNameIt->second);
	}
}

void Keymapper::load()
{
	keyIDToAction.clear();

	for (auto &action : actions) {
		auto key = getActionKey(action);
		action.key = key; // Store the key here and in the map so we can save() the actions while keeping the same order as they have been added.
		keyIDToAction.emplace(key, action);
	}
}

int Keymapper::getActionKey(const Keymapper::Action &action)
{
	auto key = getKeyFunction(action.name);
	if (key.empty())
		return action.defaultKey; // Return the default key if no key has been set.

	auto keyIt = keyNameToKeyID.find(key);
	if (keyIt == keyNameToKeyID.end()) {
		// Return the default key if the key is unknown.
		SDL_Log("Keymapper: unknown key '%s'", key.c_str());
		return action.defaultKey;
	}

	auto it = keyIDToAction.find(keyIt->second);
	if (it != keyIDToAction.end()) {
		// Warn about overwriting keys.
		SDL_Log("Keymapper: key '%s' is already bound to action '%s', overwriting", key.c_str(), it->second.get().name.c_str());
	}

	return keyIt->second;
}

} // namespace devilution

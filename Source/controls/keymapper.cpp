#include "keymapper.hpp"

#include <cassert>
#include <fmt/format.h>
#include <utility>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "../control.h"
#include "../miniwin/miniwin.h"
#include "utils/log.hpp"

namespace devilution {

Keymapper::Keymapper(SetConfigKeyFunction setKeyFunction, GetConfigKeyFunction getKeyFunction)
    : setKeyFunction(std::move(setKeyFunction))
    , getKeyFunction(std::move(getKeyFunction))
{
	// Insert all supported keys: a-z, 0-9 and F1-F12.
	keyIDToKeyName.reserve(('Z' - 'A' + 1) + ('9' - '0' + 1) + 12);
	for (char c = 'A'; c <= 'Z'; ++c) {
		keyIDToKeyName.emplace(c, std::string(1, c));
	}
	for (char c = '0'; c <= '9'; ++c) {
		keyIDToKeyName.emplace(c, std::string(1, c));
	}
	for (int i = 0; i < 12; ++i) {
		keyIDToKeyName.emplace(DVL_VK_F1 + i, fmt::format("F{}", i + 1));
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

void Keymapper::keyPressed(int key) const
{
	auto it = keyIDToAction.find(key);
	if (it == keyIDToAction.end())
		return; // Ignore unmapped keys.

	const auto &action = it->second;

	// Check that the action can be triggered and that the chat textbox is not
	// open.
	if (!action.get().enable() || talkflag)
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
		if (action.key == DVL_VK_INVALID) {
			// Just add an empty config entry if the action is unbound.
			setKeyFunction(action.name, "");
			continue;
		}

		auto keyNameIt = keyIDToKeyName.find(action.key);
		if (keyNameIt == keyIDToKeyName.end()) {
			Log("Keymapper: no name found for key '{}'", action.key);
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
		action.key = key;
		if (key == DVL_VK_INVALID) {
			// Skip if the action has no key bound to it.
			continue;
		}
		// Store the key in action.key and in the map so we can save() the
		// actions while keeping the same order as they have been added.
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
		Log("Keymapper: unknown key '{}'", key);
		return action.defaultKey;
	}

	auto it = keyIDToAction.find(keyIt->second);
	if (it != keyIDToAction.end()) {
		// Warn about overwriting keys.
		Log("Keymapper: key '{}' is already bound to action '{}', overwriting", key, it->second.get().name);
	}

	return keyIt->second;
}

} // namespace devilution

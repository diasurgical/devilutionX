#include "keymapper.hpp"

#include <cassert>
#include <fmt/format.h>
#include <utility>

#include <SDL.h>

#include "control.h"
#include "options.h"
#include "utils/log.hpp"

namespace devilution {

Keymapper::Keymapper()
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

Keymapper::ActionIndex Keymapper::AddAction(const Action &action)
{
	actions.emplace_back(action);

	return actions.size() - 1;
}

void Keymapper::KeyPressed(int key) const
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

std::string Keymapper::KeyNameForAction(ActionIndex actionIndex) const
{
	assert(actionIndex < actions.size());
	auto key = actions[actionIndex].key;
	auto it = keyIDToKeyName.find(key);
	assert(it != keyIDToKeyName.end());
	return it->second;
}

void Keymapper::Save() const
{
	// Use the action vector to go though the actions to keep the same order.
	for (const auto &action : actions) {
		if (action.key == DVL_VK_INVALID) {
			// Just add an empty config entry if the action is unbound.
			SetIniValue("Keymapping", action.name.c_str(), "");
			continue;
		}

		auto keyNameIt = keyIDToKeyName.find(action.key);
		if (keyNameIt == keyIDToKeyName.end()) {
			Log("Keymapper: no name found for key '{}'", action.key);
			continue;
		}
		SetIniValue("Keymapping", action.name.c_str(), keyNameIt->second.c_str());
	}
}

void Keymapper::Load()
{
	keyIDToAction.clear();

	for (auto &action : actions) {
		auto key = GetActionKey(action);
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

int Keymapper::GetActionKey(const Keymapper::Action &action)
{
	std::array<char, 64> result;
	if (!GetIniValue("Keymapping", action.name.c_str(), result.data(), result.size()))
		return action.defaultKey; // Return the default key if no key has been set.

	std::string key = result.data();
	if (key.empty())
		return DVL_VK_INVALID;

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

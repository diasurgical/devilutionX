#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

namespace dvl {

// The Keymapper maps keys to actions.
class Keymapper final
{
public:
	using SetConfigKeyFunction = std::function<void(const std::string &key, const std::string &value)>;
	using GetConfigKeyFunction = std::function<std::string(const std::string &key)>;

	// Action represents an action that can be triggered using a keyboard shortcut.
	class Action final
	{
	public:
		// Can this action be triggered while the player is dead?
		enum class IfDead
		{
			Allow,
			Ignore,
		};

		Action(const std::string &name, int defaultKey, std::function<void()> action):
			name{name}, defaultKey{defaultKey}, action{action}, ifDead{IfDead::Ignore} {}
		Action(const std::string &name, int defaultKey, std::function<void()> action, IfDead ifDead):
			name{name}, defaultKey{defaultKey}, action{action}, ifDead{ifDead} {}

		void operator()() const { action(); }

	private:
		std::string name;
		int defaultKey;
		std::function<void()> action;
		IfDead ifDead;
		int key{};

		friend class Keymapper;
	};

	// Keymapper for now uses to function pointers to interact with the configuration.
	// This is mostly a workaround and should be replaced later when another INI library will be used.
	Keymapper(SetConfigKeyFunction setKeyFunction, GetConfigKeyFunction getKeyFunction);

	void addAction(const Action &action);
	void keyPressed(int key, bool isPlayerDead) const;
	void save() const;
	void load();

private:
	int getActionKey(const Action &action);

	std::vector<Action> actions;
	std::unordered_map<int, std::reference_wrapper<Action>> keyIDToAction;
	std::unordered_map<int, std::string> keyIDToKeyName;
	std::unordered_map<std::string, int> keyNameToKeyID;
	SetConfigKeyFunction setKeyFunction;
	GetConfigKeyFunction getKeyFunction;
};

} // namespace dvl
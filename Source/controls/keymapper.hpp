#pragma once

#include <cstddef>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

namespace devilution {

/** The Keymapper maps keys to actions. */
class Keymapper final {
public:
	using SetConfigKeyFunction = std::function<void(const std::string &key, const std::string &value)>;
	using GetConfigKeyFunction = std::function<std::string(const std::string &key)>;
	using ActionIndex = std::size_t;

	/**
	 * Action represents an action that can be triggered using a keyboard
	 * shortcut.
	 */
	class Action final {
	public:
		Action(const std::string &name, int defaultKey, std::function<void()> action)
		    : name(name)
		    , defaultKey(defaultKey)
		    , action(action)
		    , enable([] { return true; })
		{
		}
		Action(const std::string &name, int defaultKey, std::function<void()> action, std::function<bool()> enable)
		    : name(name)
		    , defaultKey(defaultKey)
		    , action(action)
		    , enable(enable)
		{
		}

		void operator()() const
		{
			action();
		}

	private:
		std::string name;
		int defaultKey;
		std::function<void()> action;
		std::function<bool()> enable;
		int key {};

		friend class Keymapper;
	};

	/**
	 * Keymapper, for now, uses two function pointers to interact with the
	 * configuration.
	 * This is mostly a workaround and should be replaced later when another INI
	 * library will be used.
	 */
	Keymapper(SetConfigKeyFunction setKeyFunction, GetConfigKeyFunction getKeyFunction);

	ActionIndex addAction(const Action &action);
	void keyPressed(int key) const;
	std::string keyNameForAction(ActionIndex actionIndex) const;
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

} // namespace devilution

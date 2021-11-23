#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace devilution {

/** The Keymapper maps keys to actions. */
class Keymapper final {
public:
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

	Keymapper();

	ActionIndex AddAction(const Action &action);
	void KeyPressed(int key) const;
	std::string KeyNameForAction(ActionIndex actionIndex) const;
	void Save() const;
	void Load();

private:
	int GetActionKey(const Action &action);

	std::vector<Action> actions;
	std::unordered_map<int, std::reference_wrapper<Action>> keyIDToAction;
	std::unordered_map<int, std::string> keyIDToKeyName;
	std::unordered_map<std::string, int> keyNameToKeyID;
};

} // namespace devilution

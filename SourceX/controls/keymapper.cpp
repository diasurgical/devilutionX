#include "keymapper.hpp"
#include "miniwin/misc_msg.h"

#include <SDL.h>

namespace dvl {

Keymapper::Keymapper(SetKeyFunction setKeyFunction, GetKeyFunction getKeyFunction):
    setKeyFunction{setKeyFunction},
    getKeyFunction{getKeyFunction}
{
}

void Keymapper::addAction(const Action &action)
{
    actions.emplace_back(std::make_shared<Action>(action));
}

void Keymapper::keyPressed(int key, bool isPlayerDead) const
{
    auto it = keyToAction.find(key);
    if(it == keyToAction.end())
        return; // ignore unmapped keys

    auto &action = *it->second;

    // Some actions cannot be triggered while the player is dead.
    if(isPlayerDead && action.ifDead == Action::IfDead::Ignore)
        return;

    action();
}

void Keymapper::save() const
{
    // Use the action vector to go though the actions to keep the same order.
    for(const auto &action: actions) {
        // Translate the key to SDL so we can query for its name.
        auto sdlKey = translate_dvl_key(action->key);
        if(sdlKey == -1) {
            SDL_Log("Keymapper: failed to translate key '%d' to SDL", action->key);
            continue;
        }
        setKeyFunction(action->name, SDL_GetKeyName(sdlKey));
    }
}

void Keymapper::load()
{
    for(const auto &action: actions) {
        auto key = getActionKey(*action);
        action->key = key; // Store the key here and in the map so we can save() the actions while keeping the same order as they have been added.
        keyToAction.emplace(key, action);
    }
}

int Keymapper::getActionKey(const Keymapper::Action &action)
{
    auto key = getKeyFunction(action.name);
    if(key.empty())
        return action.defaultKey; // return the default key if no key has been set

    auto sdlKey = SDL_GetKeyFromName(key.c_str());
    if(sdlKey == SDLK_UNKNOWN) {
        // return the default key if the key is unknown
        SDL_Log("Keymapper: unknown key '%s'", key.c_str());
        return action.defaultKey;
    }

    auto dvlKey = translate_sdl_key(SDL_Keysym{{}, sdlKey, {}, {}});
    if(dvlKey == -1) {
        // return the default key if we have failed translating it to a DVL key
        SDL_Log("Keymapper: unknown key '%s'", key.c_str());
        return action.defaultKey;
    }

    auto it = keyToAction.find(dvlKey);
    if(it != keyToAction.end()) {
        // warn about overwriting keys
        SDL_Log("Keymapper: key '%s' is already bound to action '%s', overwriting", key.c_str(), it->second->name.c_str());
    }

    return dvlKey;
}

}
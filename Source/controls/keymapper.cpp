#include "controls/keymapper.hpp"

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "control.h"
#include "options.h"
#include "utils/is_of.hpp"

namespace devilution {
namespace {

bool IsTextEntryKey(SDL_Keycode vkey)
{
	return IsAnyOf(vkey, SDLK_ESCAPE, SDLK_RETURN, SDLK_KP_ENTER, SDLK_BACKSPACE, SDLK_DOWN, SDLK_UP) || (vkey >= SDLK_SPACE && vkey <= SDLK_z);
}

bool IsNumberEntryKey(SDL_Keycode vkey)
{
	return ((vkey >= SDLK_0 && vkey <= SDLK_9) || vkey == SDLK_BACKSPACE);
}

SDL_Keycode ToAsciiUpper(SDL_Keycode key)
{
	if (key >= SDLK_a && key <= SDLK_z) {
		return static_cast<SDL_Keycode>(static_cast<Sint32>(key) - ('a' - 'A'));
	}
	return key;
}

} // namespace

void KeymapperPress(SDL_Keycode key)
{
	key = ToAsciiUpper(key);
	const KeymapperOptions::Action *action = GetOptions().Keymapper.findAction(static_cast<uint32_t>(key));
	if (action == nullptr || !action->actionPressed || !action->isEnabled()) return;

	// TODO: This should be handled outside of the keymapper.
	if (ChatFlag) return;

	action->actionPressed();
}

void KeymapperRelease(SDL_Keycode key)
{
	key = ToAsciiUpper(key);
	const KeymapperOptions::Action *action = GetOptions().Keymapper.findAction(static_cast<uint32_t>(key));
	if (action == nullptr || !action->actionReleased || !action->isEnabled()) return;

	// TODO: This should be handled outside of the keymapper.
	if ((ChatFlag && IsTextEntryKey(key)) || (DropGoldFlag && IsNumberEntryKey(key))) return;

	action->actionReleased();
}

} // namespace devilution

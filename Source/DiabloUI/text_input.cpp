#include "DiabloUI/text_input.hpp"

#include <memory>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "utils/log.hpp"
#include "utils/sdl_ptrs.h"
#include "utils/utf8.hpp"

namespace devilution {

bool HandleTextInputEvent(const SDL_Event &event, TextInputState &state)
{
	switch (event.type) {
	case SDL_KEYDOWN: {
		switch (event.key.keysym.sym) {
#ifndef USE_SDL1
		case SDLK_v:
			if ((SDL_GetModState() & KMOD_CTRL) != 0) {
				if (SDL_HasClipboardText() == SDL_TRUE) {
					std::unique_ptr<char, SDLFreeDeleter<char>> clipboard { SDL_GetClipboardText() };
					if (clipboard == nullptr || *clipboard == '\0') {
						Log("{}", SDL_GetError());
					} else {
						state.type(clipboard.get());
					}
				}
			}
			return true;
#endif
		case SDLK_BACKSPACE:
			state.backspace();
			return true;
		case SDLK_DELETE:
			state.del();
			return true;
		case SDLK_LEFT:
			state.moveCursorLeft();
			return true;
		case SDLK_RIGHT:
			state.moveCursorRight();
			return true;
		case SDLK_HOME:
			state.setCursorToStart();
			return true;
		case SDLK_END:
			state.setCursorToEnd();
			return true;
		default:
			break;
		}
#ifdef USE_SDL1
		if ((event.key.keysym.mod & KMOD_CTRL) == 0 && event.key.keysym.unicode >= ' ') {
			std::string utf8;
			AppendUtf8(event.key.keysym.unicode, utf8);
			state.type(utf8);
			return true;
		}
#endif
		return false;
	}
#ifndef USE_SDL1
	case SDL_TEXTINPUT:
#ifdef __vita__
		state.assign(event.text.text);
#else
		state.type(event.text.text);
#endif
		return true;
#endif
	default:
		return false;
	}
}

} // namespace devilution

#include "platform/switch/keyboard.h"

#include <cstring>

#include <SDL.h>
#include <switch.h>

#include "utils/utf8.hpp"

static void switch_keyboard_get(devilution::string_view guide_text, devilution::string_view initial_text, char *buf, unsigned buf_len)
{
	Result rc = 0;

	SwkbdConfig kbd;

	rc = swkbdCreate(&kbd, 0);

	if (R_SUCCEEDED(rc)) {
		swkbdConfigMakePresetDefault(&kbd);

		// swkbConfigSetGuide/InitialText both copy the input string. They expect a null terminated c-string but we're
		//  getting a string view which may be a substring of a larger std::string/c-string, so we copy to a temporary
		//  null-terminated string first for safety
		if (!guide_text.empty()) {
			std::string textCopy { guide_text };
			swkbdConfigSetGuideText(&kbd, textCopy.c_str());
		}
		if (!initial_text.empty()) {
			std::string textCopy { initial_text };
			swkbdConfigSetInitialText(&kbd, textCopy.c_str());
		}

		swkbdConfigSetStringLenMax(&kbd, buf_len);
		rc = swkbdShow(&kbd, buf, buf_len);
		swkbdClose(&kbd);
	}
}

static int get_utf8_character_bytes(const uint8_t *uc)
{
	if (uc[0] < 0x80) {
		return 1;
	} else if ((uc[0] & 0xe0) == 0xc0 && (uc[1] & 0xc0) == 0x80) {
		return 2;
	} else if ((uc[0] & 0xf0) == 0xe0 && (uc[1] & 0xc0) == 0x80 && (uc[2] & 0xc0) == 0x80) {
		return 3;
	} else if ((uc[0] & 0xf8) == 0xf0 && (uc[1] & 0xc0) == 0x80 && (uc[2] & 0xc0) == 0x80 && (uc[3] & 0xc0) == 0x80) {
		return 4;
	} else {
		return 1;
	}
}

static void switch_create_and_push_sdlkey_event(uint32_t event_type, SDL_Scancode scan, SDL_Keycode key)
{
	SDL_Event event;
	event.type = event_type;
	event.key.keysym.scancode = scan;
	event.key.keysym.sym = key;
	event.key.keysym.mod = 0;
	SDL_PushEvent(&event);
}

void switch_start_text_input(devilution::string_view guide_text, devilution::string_view initial_text, unsigned max_length)
{
	char text[max_length] = { '\0' };
	switch_keyboard_get(guide_text, initial_text, text, sizeof(text));
	for (int i = 0; i < 600; i++) {
		switch_create_and_push_sdlkey_event(SDL_KEYDOWN, SDL_SCANCODE_BACKSPACE, SDLK_BACKSPACE);
		switch_create_and_push_sdlkey_event(SDL_KEYUP, SDL_SCANCODE_BACKSPACE, SDLK_BACKSPACE);
	}
	for (int i = 0; i < 600; i++) {
		switch_create_and_push_sdlkey_event(SDL_KEYDOWN, SDL_SCANCODE_DELETE, SDLK_DELETE);
		switch_create_and_push_sdlkey_event(SDL_KEYUP, SDL_SCANCODE_DELETE, SDLK_DELETE);
	}
	if (text[0] == '\0') {
		devilution::CopyUtf8(text, initial_text, sizeof(text));
	}
	const uint8_t *utf8_text = (uint8_t *)text;
	for (int i = 0; i < sizeof(text) && utf8_text[i];) {
		int bytes_in_char = get_utf8_character_bytes(&utf8_text[i]);
		SDL_Event textinput_event;
		textinput_event.type = SDL_TEXTINPUT;
		for (int n = 0; n < bytes_in_char; n++) {
			textinput_event.text.text[n] = text[i + n];
		}
		textinput_event.text.text[bytes_in_char] = 0;
		SDL_PushEvent(&textinput_event);
		i += bytes_in_char;
	}
}

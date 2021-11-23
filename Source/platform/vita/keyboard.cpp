#include "platform/vita/keyboard.h"

#include <cstring>

#include <SDL.h>
#include <psp2/ime_dialog.h>
#include <psp2/types.h>

static void utf16_to_utf8(const uint16_t *src, uint8_t *dst)
{
	for (int i = 0; src[i]; i++) {
		if ((src[i] & 0xFF80) == 0) {
			*(dst++) = src[i] & 0xFF;
		} else if ((src[i] & 0xF800) == 0) {
			*(dst++) = ((src[i] >> 6) & 0xFF) | 0xC0;
			*(dst++) = (src[i] & 0x3F) | 0x80;
		} else if ((src[i] & 0xFC00) == 0xD800 && (src[i + 1] & 0xFC00) == 0xDC00) {
			*(dst++) = (((src[i] + 64) >> 8) & 0x3) | 0xF0;
			*(dst++) = (((src[i] >> 2) + 16) & 0x3F) | 0x80;
			*(dst++) = ((src[i] >> 4) & 0x30) | 0x80 | ((src[i + 1] << 2) & 0xF);
			*(dst++) = (src[i + 1] & 0x3F) | 0x80;
			i += 1;
		} else {
			*(dst++) = ((src[i] >> 12) & 0xF) | 0xE0;
			*(dst++) = ((src[i] >> 6) & 0x3F) | 0x80;
			*(dst++) = (src[i] & 0x3F) | 0x80;
		}
	}

	*dst = '\0';
}

static void utf8_to_utf16(const uint8_t *src, uint16_t *dst)
{
	for (int i = 0; src[i];) {
		if ((src[i] & 0xE0) == 0xE0) {
			*(dst++) = ((src[i] & 0x0F) << 12) | ((src[i + 1] & 0x3F) << 6) | (src[i + 2] & 0x3F);
			i += 3;
		} else if ((src[i] & 0xC0) == 0xC0) {
			*(dst++) = ((src[i] & 0x1F) << 6) | (src[i + 1] & 0x3F);
			i += 2;
		} else {
			*(dst++) = src[i];
			i += 1;
		}
	}

	*dst = '\0';
}

static int vita_input_thread(void *ime_buffer)
{
	while (1) {
		// update IME status. Terminate, if finished
		SceCommonDialogStatus dialogStatus = sceImeDialogGetStatus();
		if (dialogStatus == SCE_COMMON_DIALOG_STATUS_FINISHED) {
			uint8_t utf8_buffer[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
			SceImeDialogResult result;

			SDL_memset(&result, 0, sizeof(SceImeDialogResult));
			sceImeDialogGetResult(&result);

			// Convert UTF16 to UTF8
			utf16_to_utf8((SceWChar16 *)ime_buffer, utf8_buffer);

			// send sdl event
			SDL_Event event;
			event.text.type = SDL_TEXTINPUT;
			SDL_utf8strlcpy(event.text.text, (const char *)utf8_buffer, SDL_arraysize(event.text.text));
			SDL_PushEvent(&event);

			sceImeDialogTerm();
			break;
		}
	}
	return 0;
}

static int vita_keyboard_get(const char *guide_text, const char *initial_text, int max_len, SceWChar16 *buf)
{
	SceWChar16 title[SCE_IME_DIALOG_MAX_TITLE_LENGTH];
	SceWChar16 text[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
	SceInt32 res;

	SDL_memset(&title, 0, sizeof(title));
	SDL_memset(&text, 0, sizeof(text));
	utf8_to_utf16((const uint8_t *)guide_text, title);
	utf8_to_utf16((const uint8_t *)initial_text, text);

	SceImeDialogParam param;
	sceImeDialogParamInit(&param);

	param.supportedLanguages = 0;
	param.languagesForced = SCE_FALSE;
	param.type = SCE_IME_TYPE_DEFAULT;
	param.option = 0;
	param.textBoxMode = SCE_IME_DIALOG_TEXTBOX_MODE_WITH_CLEAR;
	param.maxTextLength = max_len;

	param.title = title;
	param.initialText = text;
	param.inputTextBuffer = buf;

	res = sceImeDialogInit(&param);
	if (res < 0) {
		return 0;
	}

	return 1;
}

void vita_start_text_input(const char *guide_text, const char *initial_text, int max_length)
{
	SceWChar16 ime_buffer[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
	if (vita_keyboard_get(guide_text, initial_text, max_length, ime_buffer)) {
		SDL_CreateThread(vita_input_thread, "vita_input_thread", (void *)ime_buffer);
	}
}

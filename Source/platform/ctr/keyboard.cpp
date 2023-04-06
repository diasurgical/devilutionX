#include <cstdlib>
#include <cstring>

#include "platform/ctr/keyboard.h"
#include "utils/utf8.hpp"

constexpr size_t MAX_TEXT_LENGTH = 255;

struct vkbdEvent {
	devilution::string_view hintText;
	devilution::string_view inText;
	char *outText;
	size_t maxLength;
};

static vkbdEvent events[16];
static int eventCount = 0;

void ctr_vkbdInput(devilution::string_view hintText, devilution::string_view inText, char *outText, size_t maxLength)
{
	if (eventCount >= sizeof(events))
		return;

	vkbdEvent &event = events[eventCount];
	event.hintText = hintText;
	event.inText = inText;
	event.outText = outText;
	event.maxLength = maxLength;
	eventCount++;
}

void ctr_vkbdFlush()
{
	for (int i = 0; i < eventCount; i++) {
		vkbdEvent &event = events[i];
		SwkbdState swkbd;

		swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, MAX_TEXT_LENGTH);
		swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);

		// swkbdSetInitialText stores the pointer to the c-string, only copying it when swkbdInputText is called. Need to
		//  ensure it has a valid null-terminated string until that point.
		std::string initialText { event.inText };
		swkbdSetInitialText(&swkbd, initialText.c_str());

		// swkbdSetHintText copies from the c-string immediately so we can use the output buffer to save a malloc
		char mybuf[MAX_TEXT_LENGTH + 1];
		devilution::CopyUtf8(mybuf, event.hintText, sizeof(mybuf));
		swkbdSetHintText(&swkbd, mybuf);

		memset(mybuf, 0, sizeof(mybuf));
		SwkbdButton button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));

		if (button == SWKBD_BUTTON_CONFIRM) {
			devilution::CopyUtf8(event.outText, mybuf, event.maxLength);
		}
	}

	eventCount = 0;
}

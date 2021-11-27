#include <cstdlib>
#include <cstring>

#include "platform/ctr/keyboard.h"
#include "utils/utf8.hpp"

constexpr size_t MAX_TEXT_LENGTH = 255;

struct vkbdEvent {
	const char *hintText;
	const char *inText;
	char *outText;
	int maxLength;
};

static vkbdEvent events[16];
static int eventCount = 0;

void ctr_vkbdInput(const char *hintText, const char *inText, char *outText, int maxLength)
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

		char mybuf[MAX_TEXT_LENGTH + 1];

		swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, MAX_TEXT_LENGTH);
		swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetInitialText(&swkbd, event.inText);
		swkbdSetHintText(&swkbd, event.hintText);

		SwkbdButton button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));

		if (button == SWKBD_BUTTON_CONFIRM) {
			devilution::CopyUtf8(event.outText, mybuf, event.maxLength);
			continue;
		}

		devilution::CopyUtf8(event.outText, event.inText, event.maxLength);
	}

	eventCount = 0;
}

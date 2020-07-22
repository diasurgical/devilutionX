#include <stdlib.h>
#include <string.h>

#include "platform/ctr/keyboard.h"

static uint16_t maxLength = 16;

static SwkbdCallbackResult MyCallback(void *user, const char **ppMessage, const char *inputText, size_t textLength)
{
	if (textLength > maxLength)
	{
		*ppMessage = "max 16 characters";
		return SWKBD_CALLBACK_CONTINUE;
	}

	return SWKBD_CALLBACK_OK;
}

const char* ctr_vkbdInput(const char *title, char *outText)
{
	SwkbdState swkbd;
	char mybuf[60];

	swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
	swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
	swkbdSetInitialText(&swkbd, mybuf);
	swkbdSetHintText(&swkbd, title);
	swkbdSetFilterCallback(&swkbd, MyCallback, NULL);

	SwkbdButton button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));

	if (button == SWKBD_BUTTON_CONFIRM)
	{
		strncpy(outText, mybuf, sizeof(maxLength));

		return outText;
	}

	return 0;
}

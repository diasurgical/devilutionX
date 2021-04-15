#include <stdlib.h>
#include <string.h>

#include "platform/ctr/keyboard.h"

const char *ctr_vkbdInput(const char *hintText, const char *inText, char *outText)
{
	SwkbdState swkbd;

	char mybuf[16];

	swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, 15);
	swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
	swkbdSetInitialText(&swkbd, inText);
	swkbdSetHintText(&swkbd, hintText);

	SwkbdButton button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));

	if (button == SWKBD_BUTTON_CONFIRM) {
		strcpy(outText, mybuf);
		return 0;
	}

	strcpy(outText, inText);
	return 0;
}

#include "devilution.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

Art dialogArt; // "ui_art\\spopup.pcx"
char dialogMessage[256];

Art progressArt; // "ui_art\\prog_bg.pcx"

void DialogActionOK()
{
}

void DialogActionCancel()
{
}

UiItem OKCANCEL_DIALOG[] = {
	UiImage(&dialogArt, { 180, 168, 280, 144 }),
	UiText(dialogMessage, { 200, 180, 240, 80 }, UIS_CENTER),
	UiButton("OK", &DialogActionOK, { 200, 265, 110, 28 }, UIS_SML1),
	UiButton("Cancel", &DialogActionCancel, { 330, 265, 110, 28 }, UIS_SML2),
};

UiItem OK_DIALOG[] = {
	UiImage(&dialogArt, { 180, 168, 280, 144 }),
	UiText(dialogMessage, { 200, 180, 240, 80 }, UIS_CENTER),
	UiButton("OK", &DialogActionOK, { 200, 265, 110, 28 }, UIS_SML1),
};

UiItem PROGRESS_DIALOG[] = {
	UiImage(&dialogArt, { 180, 168, 280, 144 }),
	UiText(dialogMessage, { 180, 177, 280, 43 }, UIS_CENTER),
	UiImage(&progressArt, { 205, 220, 228, 38 }),
	UiButton("Cancel", &DialogActionCancel, { 265, 267, 110, 28 }, UIS_SML1),
};

UiListItem SELOK_DIALOG_ITEMS[] = {
	{ "OK", 0 }
};
UiItem SELOK_DIALOG[] = {
	UiText(dialogMessage, { 140, 210, 400, 168 }, UIS_CENTER),
	UiList(SELOK_DIALOG_ITEMS, 230, 390, 180, 35, UIS_CENTER),
};

UiItem SPAWNERR_DIALOG[] = {
	UiText("The Rogue and Sorcerer are only available in the full retail version of Diablo. For ordering information call (800) 953-SNOW.", { 140, 199, 400, 177 }),
	UiButton("OK", &DialogActionOK, { 230, 407, 180, 43 }),
};

}

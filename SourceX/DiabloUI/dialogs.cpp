#include "devilution.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

UI_Item OKCANCEL_DIALOG[] = {
	{ { 180, 168, 280, 144 }, UI_IMAGE, 0, 0, "ui_art\\spopup.pcx" },
	{ { 200, 180, 240, 80 }, UI_TEXT, UIS_CENTER }, // message
	{ { 200, 265, 110, 28 }, UI_BUTTON, UIS_SML1, 0, "OK" },
	{ { 330, 265, 110, 28 }, UI_BUTTON, UIS_SML2, 0, "Cancel" },
};

UI_Item OK_DIALOG[] = {
	{ { 180, 168, 280, 144 }, UI_IMAGE, 0, 0, "ui_art\\spopup.pcx" },
	{ { 200, 180, 240, 80 }, UI_TEXT, UIS_CENTER }, // message
	{ { 265, 265, 110, 28 }, UI_BUTTON, UIS_SML1, 0, "OK" },
};

UI_Item PROGRESS_DIALOG[] = {
	{ { 180, 168, 280, 144 }, UI_IMAGE, 0, 0, "ui_art\\spopup.pcx" },
	{ { 180, 177, 280, 43 }, UI_TEXT, UIS_CENTER }, // Message
	{ { 205, 220, 228, 38 }, UI_IMAGE, 0, 0, "ui_art\\prog_bg.pcx" },
	{ { 265, 267, 110, 28 }, UI_BUTTON, UIS_SML1, 0, "Cancel" },
};

UI_Item SELOK_DIALOG[] = {
	{ { 140, 210, 400, 168 } }, // Message
	{ { 230, 390, 180, 35 }, UI_LIST, UIS_CENTER, 0, "OK" },
};

UI_Item SPAWNERR_DIALOG[] = {
	{ { 140, 199, 400, 177 }, UI_TEXT, 0, 0, "The Rogue and Sorcerer are only available in the full retail version of Diablo. For ordering information call (800) 953-SNOW." },
	{ { 230, 407, 180, 43 }, UI_BUTTON, 0, 0, "OK" },
};

}

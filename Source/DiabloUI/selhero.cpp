#include "selhero.h"

#include <algorithm>
#include <chrono>
#include <random>

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/scrollbar.h"
#include "DiabloUI/selgame.h"
#include "DiabloUI/selok.h"
#include "DiabloUI/selyesno.h"
#include "control.h"
#include "options.h"
#include "pfile.h"
#include "utils/language.h"

namespace devilution {

bool selhero_endMenu;
bool selhero_isMultiPlayer;

bool (*gfnHeroInfo)(bool (*fninfofunc)(_uiheroinfo *));
bool (*gfnHeroCreate)(_uiheroinfo *);
void (*gfnHeroStats)(unsigned int, _uidefaultstats *);

namespace {

std::size_t selhero_SaveCount = 0;
_uiheroinfo selhero_heros[MAX_CHARACTERS];
_uiheroinfo selhero_heroInfo;
const size_t MaxViewportItems = 6;
char textStats[5][4];
char title[32];
_selhero_selections selhero_result;
bool selhero_navigateYesNo;
bool selhero_deleteEnabled;

std::vector<UiItemBase *> vecSelHeroDialog;
std::vector<UiListItem *> vecSelHeroDlgItems;
std::vector<UiItemBase *> vecSelDlgItems;

UiImage *SELHERO_DIALOG_HERO_IMG;

bool bUIElementsLoaded = false;

void SelheroListFocus(int value);
void SelheroListSelect(int value);
void SelheroListEsc();
void SelheroLoadFocus(int value);
void SelheroLoadSelect(int value);
void SelheroNameSelect(int value);
void SelheroNameEsc();
void SelheroClassSelectorFocus(int value);
void SelheroClassSelectorSelect(int value);
void SelheroClassSelectorEsc();
const char *SelheroGenerateName(HeroClass heroClass);

void SelheroUiFocusNavigationYesNo()
{
	if (selhero_deleteEnabled)
		UiFocusNavigationYesNo();
}

void SelheroFreeListItems()
{
	for (auto pUIItem : vecSelHeroDlgItems) {
		delete pUIItem;
	}
	vecSelHeroDlgItems.clear();
}

void SelheroFreeDlgItems()
{
	for (auto pUIItem : vecSelDlgItems) {
		delete pUIItem;
	}
	vecSelDlgItems.clear();
}

void SelheroFree()
{
	ArtBackground.Unload();

	for (auto pUIItem : vecSelHeroDialog) {
		delete pUIItem;
	}
	vecSelHeroDialog.clear();

	SelheroFreeDlgItems();
	SelheroFreeListItems();
	UnloadScrollBar();

	bUIElementsLoaded = false;
}

void SelheroSetStats()
{
	SELHERO_DIALOG_HERO_IMG->m_frame = static_cast<int>(selhero_heroInfo.heroclass);
	snprintf(textStats[0], sizeof(textStats[0]), "%i", selhero_heroInfo.level);
	snprintf(textStats[1], sizeof(textStats[1]), "%i", selhero_heroInfo.strength);
	snprintf(textStats[2], sizeof(textStats[2]), "%i", selhero_heroInfo.magic);
	snprintf(textStats[3], sizeof(textStats[3]), "%i", selhero_heroInfo.dexterity);
	snprintf(textStats[4], sizeof(textStats[4]), "%i", selhero_heroInfo.vitality);
}

std::size_t listOffset = 0;
UiArtTextButton *SELLIST_DIALOG_DELETE_BUTTON;

void SelheroUpdateViewportItems()
{
	const std::size_t numViewportHeroes = std::min(selhero_SaveCount - listOffset, MaxViewportItems);
	for (std::size_t i = 0; i < numViewportHeroes; i++) {
		const std::size_t index = i + listOffset;
		vecSelHeroDlgItems[i]->m_text = selhero_heros[index].name;
		vecSelHeroDlgItems[i]->m_value = static_cast<int>(index);
	}
	if (numViewportHeroes < MaxViewportItems) {
		vecSelHeroDlgItems[numViewportHeroes]->m_text = _("New Hero");
		vecSelHeroDlgItems[numViewportHeroes]->m_value = static_cast<int>(selhero_SaveCount);
	}
}

void SelheroScrollIntoView(std::size_t index)
{
	std::size_t newOffset = listOffset;
	if (index >= listOffset + MaxViewportItems)
		newOffset = index - (MaxViewportItems - 1);
	if (index < listOffset)
		newOffset = index;
	if (newOffset != listOffset) {
		listOffset = newOffset;
		SelheroUpdateViewportItems();
	}
}

bool SelHeroGetHeroInfo(_uiheroinfo *pInfo)
{
	selhero_heros[selhero_SaveCount] = *pInfo;
	selhero_SaveCount++;

	return true;
}

void SelheroListFocus(int value)
{
	const auto index = static_cast<std::size_t>(value);
	SelheroScrollIntoView(index);
	int baseFlags = UIS_CENTER | UIS_BIG;
	if (selhero_SaveCount != 0 && index < selhero_SaveCount) {
		memcpy(&selhero_heroInfo, &selhero_heros[index], sizeof(selhero_heroInfo));
		SelheroSetStats();
		SELLIST_DIALOG_DELETE_BUTTON->m_iFlags = baseFlags | UIS_GOLD;
		selhero_deleteEnabled = true;
		return;
	}

	SELHERO_DIALOG_HERO_IMG->m_frame = static_cast<int>(enum_size<HeroClass>::value);
	strncpy(textStats[0], "--", sizeof(textStats[0]) - 1);
	strncpy(textStats[1], "--", sizeof(textStats[1]) - 1);
	strncpy(textStats[2], "--", sizeof(textStats[2]) - 1);
	strncpy(textStats[3], "--", sizeof(textStats[3]) - 1);
	strncpy(textStats[4], "--", sizeof(textStats[4]) - 1);
	SELLIST_DIALOG_DELETE_BUTTON->m_iFlags = baseFlags | UIS_DISABLED;
	selhero_deleteEnabled = false;
}

bool SelheroListDeleteYesNo()
{
	selhero_navigateYesNo = selhero_deleteEnabled;

	return selhero_navigateYesNo;
}

void SelheroListSelect(int value)
{
	if (static_cast<std::size_t>(value) == selhero_SaveCount) {
		SelheroFreeDlgItems();

		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
		vecSelDlgItems.push_back(new UiArtText(_("Choose Class"), rect1, UIS_CENTER | UIS_BIG));

		SelheroFreeListItems();
		int itemH = 33;
		vecSelHeroDlgItems.push_back(new UiListItem(_("Warrior"), static_cast<int>(HeroClass::Warrior)));
		vecSelHeroDlgItems.push_back(new UiListItem(_("Rogue"), static_cast<int>(HeroClass::Rogue)));
		vecSelHeroDlgItems.push_back(new UiListItem(_("Sorcerer"), static_cast<int>(HeroClass::Sorcerer)));
		if (gbIsHellfire) {
			vecSelHeroDlgItems.push_back(new UiListItem(_("Monk"), static_cast<int>(HeroClass::Monk)));
		}
		if (gbBard || sgOptions.Gameplay.bTestBard) {
			vecSelHeroDlgItems.push_back(new UiListItem(_("Bard"), static_cast<int>(HeroClass::Bard)));
		}
		if (gbBarbarian || sgOptions.Gameplay.bTestBarbarian) {
			vecSelHeroDlgItems.push_back(new UiListItem(_("Barbarian"), static_cast<int>(HeroClass::Barbarian)));
		}
		if (vecSelHeroDlgItems.size() > 4)
			itemH = 26;
		int itemY = 246 + (176 - vecSelHeroDlgItems.size() * itemH) / 2;
		vecSelDlgItems.push_back(new UiList(vecSelHeroDlgItems, PANEL_LEFT + 264, (UI_OFFSET_Y + itemY), 320, itemH, UIS_CENTER | UIS_MED | UIS_GOLD));

		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 279), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton(_("OK"), &UiFocusNavigationSelect, rect2, UIS_CENTER | UIS_BIG | UIS_GOLD));

		SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 429), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton(_("Cancel"), &UiFocusNavigationEsc, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));

		UiInitList(vecSelHeroDlgItems.size(), SelheroClassSelectorFocus, SelheroClassSelectorSelect, SelheroClassSelectorEsc, vecSelDlgItems);
		memset(&selhero_heroInfo.name, 0, sizeof(selhero_heroInfo.name));
		strncpy(title, _("New Single Player Hero"), sizeof(title) - 1);
		if (selhero_isMultiPlayer) {
			strncpy(title, _("New Multi Player Hero"), sizeof(title) - 1);
		}
		return;
	}

	if (selhero_heroInfo.hassaved) {
		SelheroFreeDlgItems();

		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
		vecSelDlgItems.push_back(new UiArtText(_("Save File Exists"), rect1, UIS_CENTER | UIS_BIG));

		SelheroFreeListItems();
		vecSelHeroDlgItems.push_back(new UiListItem(_("Load Game"), 0));
		vecSelHeroDlgItems.push_back(new UiListItem(_("New Game"), 1));
		vecSelDlgItems.push_back(new UiList(vecSelHeroDlgItems, PANEL_LEFT + 265, (UI_OFFSET_Y + 285), 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD));

		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 279), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton(_("OK"), &UiFocusNavigationSelect, rect2, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 429), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton(_("Cancel"), &UiFocusNavigationEsc, rect3, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		UiInitList(vecSelHeroDlgItems.size(), SelheroLoadFocus, SelheroLoadSelect, selhero_List_Init, vecSelDlgItems, true);
		strncpy(title, _("Single Player Characters"), sizeof(title) - 1);
		return;
	}

	SelheroLoadSelect(1);
}

void SelheroListEsc()
{
	UiInitList_clear();

	selhero_endMenu = true;
	selhero_result = SELHERO_PREVIOUS;
}

void SelheroClassSelectorFocus(int value)
{
	const auto heroClass = static_cast<HeroClass>(vecSelHeroDlgItems[value]->m_value);

	_uidefaultstats defaults;
	gfnHeroStats(static_cast<unsigned int>(heroClass), &defaults);

	selhero_heroInfo.level = 1;
	selhero_heroInfo.heroclass = heroClass;
	selhero_heroInfo.strength = defaults.strength;
	selhero_heroInfo.magic = defaults.magic;
	selhero_heroInfo.dexterity = defaults.dexterity;
	selhero_heroInfo.vitality = defaults.vitality;

	SelheroSetStats();
}

bool ShouldPrefillHeroName()
{
#if defined(PREFILL_PLAYER_NAME)
	return true;
#else
	return sgbControllerActive;
#endif
}

void SelheroClassSelectorSelect(int value)
{
	auto hClass = static_cast<HeroClass>(vecSelHeroDlgItems[value]->m_value);
	if (gbSpawned && (hClass == HeroClass::Rogue || hClass == HeroClass::Sorcerer || (hClass == HeroClass::Bard && hfbard_mpq == nullptr))) {
		ArtBackground.Unload();
		UiSelOkDialog(nullptr, _("The Rogue and Sorcerer are only available in the full retail version of Diablo. Visit https://www.gog.com/game/diablo to purchase."), false);
		LoadBackgroundArt("ui_art\\selhero.pcx");
		SelheroListSelect(selhero_SaveCount);
		return;
	}

	strncpy(title, _("New Single Player Hero"), sizeof(title) - 1);
	if (selhero_isMultiPlayer) {
		strncpy(title, _("New Multi Player Hero"), sizeof(title) - 1);
	}
	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
	if (ShouldPrefillHeroName())
		strncpy(selhero_heroInfo.name, SelheroGenerateName(selhero_heroInfo.heroclass), sizeof(selhero_heroInfo.name) - 1);
	SelheroFreeDlgItems();
	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
	vecSelDlgItems.push_back(new UiArtText(_("Enter Name"), rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 265), (Sint16)(UI_OFFSET_Y + 317), 320, 33 };
	vecSelDlgItems.push_back(new UiEdit(_("Enter Name"), selhero_heroInfo.name, 15, rect2, UIS_MED | UIS_GOLD));

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 279), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton(_("OK"), &UiFocusNavigationSelect, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 429), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton(_("Cancel"), &UiFocusNavigationEsc, rect4, UIS_CENTER | UIS_BIG | UIS_GOLD));

	UiInitList(0, nullptr, SelheroNameSelect, SelheroNameEsc, vecSelDlgItems);
}

void SelheroClassSelectorEsc()
{
	SelheroFreeDlgItems();
	SelheroFreeListItems();

	if (selhero_SaveCount != 0) {
		selhero_List_Init();
		return;
	}

	SelheroListEsc();
}

void SelheroNameSelect(int value)
{

	if (!UiValidPlayerName(selhero_heroInfo.name)) {
		ArtBackground.Unload();
		UiSelOkDialog(title, _("Invalid name. A name cannot contain spaces, reserved characters, or reserved words.\n"), false);
		LoadBackgroundArt("ui_art\\selhero.pcx");
	} else {
		bool overwrite = true;
		for (std::size_t i = 0; i < selhero_SaveCount; i++) {
			if (strcasecmp(selhero_heros[i].name, selhero_heroInfo.name) == 0) {
				ArtBackground.Unload();
				char dialogText[256];
				strncpy(dialogText, fmt::format(_(/* Error message when User tries to create multiple heros with the same name */ "Character already exists. Do you want to overwrite \"{:s}\"?"), selhero_heroInfo.name).c_str(), sizeof(dialogText));

				overwrite = UiSelHeroYesNoDialog(title, dialogText);
				LoadBackgroundArt("ui_art\\selhero.pcx");
				break;
			}
		}

		if (overwrite) {
			if (gfnHeroCreate(&selhero_heroInfo)) {
				SelheroLoadSelect(1);
				return;
			}
			UiErrorOkDialog(_(/* TRANSLATORS: Error Message */ "Unable to create character."), vecSelDlgItems);
		}
	}

	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
	SelheroClassSelectorSelect(0);
}

void SelheroNameEsc()
{
	SelheroListSelect(selhero_SaveCount);
}

void SelheroLoadFocus(int value)
{
}

void SelheroLoadSelect(int value)
{
	UiInitList_clear();
	selhero_endMenu = true;
	if (vecSelHeroDlgItems[value]->m_value == 0) {
		selhero_result = SELHERO_CONTINUE;
		return;
	}

	if (!selhero_isMultiPlayer) {
		// This is part of a dangerous hack to enable difficulty selection in single-player.
		// FIXME: Dialogs should not refer to each other's variables.

		// We disable `selhero_endMenu` and replace the background and art
		// and the item list with the difficulty selection ones.
		//
		// This means selhero's render loop will render selgame's items,
		// which happens to work because the render loops are similar.
		selhero_endMenu = false;
		SelheroFree();
		LoadBackgroundArt("ui_art\\selgame.pcx");
		selgame_GameSelection_Select(0);
	}

	selhero_result = SELHERO_NEW_DUNGEON;
}

const char *SelheroGenerateName(HeroClass heroClass)
{
	static const char *const names[6][10] = {
		{
		    // Warrior
		    "Aidan",
		    "Qarak",
		    "Born",
		    "Cathan",
		    "Halbu",
		    "Lenalas",
		    "Maximus",
		    "Vane",
		    "Myrdgar",
		    "Rothat",
		},
		{
		    // Rogue
		    "Moreina",
		    "Akara",
		    "Kashya",
		    "Flavie",
		    "Divo",
		    "Oriana",
		    "Iantha",
		    "Shikha",
		    "Basanti",
		    "Elexa",
		},
		{
		    // Sorcerer
		    "Jazreth",
		    "Drognan",
		    "Armin",
		    "Fauztin",
		    "Jere",
		    "Kazzulk",
		    "Ranslor",
		    "Sarnakyle",
		    "Valthek",
		    "Horazon",
		},
		{
		    // Monk
		    "Akyev",
		    "Dvorak",
		    "Kekegi",
		    "Kharazim",
		    "Mikulov",
		    "Shenlong",
		    "Vedenin",
		    "Vhalit",
		    "Vylnas",
		    "Zhota",
		},
		{
		    // Bard (uses Rogue names)
		    "Moreina",
		    "Akara",
		    "Kashya",
		    "Flavie",
		    "Divo",
		    "Oriana",
		    "Iantha",
		    "Shikha",
		    "Basanti",
		    "Elexa",
		},
		{
		    // Barbarian
		    "Alaric",
		    "Barloc",
		    "Egtheow",
		    "Guthlaf",
		    "Heorogar",
		    "Hrothgar",
		    "Oslaf",
		    "Qual-Kehk",
		    "Ragnar",
		    "Ulf",
		},
	};

	int iRand = rand() % 10;

	return names[static_cast<std::size_t>(heroClass) % 6][iRand];
}

} // namespace

void selhero_Init()
{
	LoadBackgroundArt("ui_art\\selhero.pcx");
	UiAddBackground(&vecSelHeroDialog);
	UiAddLogo(&vecSelHeroDialog);
	LoadScrollBar();

	selhero_SaveCount = 0;
	gfnHeroInfo(SelHeroGetHeroInfo);
	std::reverse(selhero_heros, selhero_heros + selhero_SaveCount);

	SelheroFreeDlgItems();
	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 24), (Sint16)(UI_OFFSET_Y + 161), 590, 35 };
	vecSelHeroDialog.push_back(new UiArtText(title, rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 30), (Sint16)(UI_OFFSET_Y + 211), 180, 76 };
	SELHERO_DIALOG_HERO_IMG = new UiImage(&ArtHero, static_cast<int>(enum_size<HeroClass>::value), rect2);
	vecSelHeroDialog.push_back(SELHERO_DIALOG_HERO_IMG);

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 323), 110, 21 };
	vecSelHeroDialog.push_back(new UiArtText(_("Level:"), rect3, UIS_RIGHT));

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 323), 110, 21 };
	vecSelHeroDialog.push_back(new UiArtText(_("Level:"), rect4, UIS_RIGHT));
	SDL_Rect rect5 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 323), 40, 21 };
	vecSelHeroDialog.push_back(new UiArtText(textStats[0], rect5, UIS_CENTER));

	SDL_Rect rect6 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 358), 110, 21 };
	vecSelHeroDialog.push_back(new UiArtText(_("Strength:"), rect6, UIS_RIGHT));
	SDL_Rect rect7 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 358), 40, 21 };
	vecSelHeroDialog.push_back(new UiArtText(textStats[1], rect7, UIS_CENTER));

	SDL_Rect rect8 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 380), 110, 21 };
	vecSelHeroDialog.push_back(new UiArtText(_("Magic:"), rect8, UIS_RIGHT));
	SDL_Rect rect9 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 380), 40, 21 };
	vecSelHeroDialog.push_back(new UiArtText(textStats[2], rect9, UIS_CENTER));

	SDL_Rect rect10 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 401), 110, 21 };
	vecSelHeroDialog.push_back(new UiArtText(_("Dexterity:"), rect10, UIS_RIGHT));
	SDL_Rect rect11 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 401), 40, 21 };
	vecSelHeroDialog.push_back(new UiArtText(textStats[3], rect11, UIS_CENTER));

	SDL_Rect rect12 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 422), 110, 21 };
	vecSelHeroDialog.push_back(new UiArtText(_("Vitality:"), rect12, UIS_RIGHT));
	SDL_Rect rect13 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 422), 40, 21 };
	vecSelHeroDialog.push_back(new UiArtText(textStats[4], rect13, UIS_CENTER));
}

void selhero_List_Init()
{
	listOffset = 0;
	SelheroFreeDlgItems();

	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
	vecSelDlgItems.push_back(new UiArtText(_("Select Hero"), rect1, UIS_CENTER | UIS_BIG));

	SelheroFreeListItems();
	const size_t numViewportHeroes = std::min(selhero_SaveCount + 1, MaxViewportItems);
	for (std::size_t i = 0; i < numViewportHeroes; i++) {
		vecSelHeroDlgItems.push_back(new UiListItem("", -1));
	}
	SelheroUpdateViewportItems();

	vecSelDlgItems.push_back(new UiList(vecSelHeroDlgItems, PANEL_LEFT + 265, (UI_OFFSET_Y + 256), 320, 26, UIS_CENTER | UIS_MED | UIS_GOLD));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 585), (Sint16)(UI_OFFSET_Y + 244), 25, 178 };
	auto *scrollBar = new UiScrollBar(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, rect2);
	vecSelDlgItems.push_back(scrollBar);

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 239), (Sint16)(UI_OFFSET_Y + 429), 120, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton(_("OK"), &UiFocusNavigationSelect, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 364), (Sint16)(UI_OFFSET_Y + 429), 120, 35 };
	SELLIST_DIALOG_DELETE_BUTTON = new UiArtTextButton(_("Delete"), &SelheroUiFocusNavigationYesNo, rect4, UIS_CENTER | UIS_BIG | UIS_DISABLED);
	vecSelDlgItems.push_back(SELLIST_DIALOG_DELETE_BUTTON);

	SDL_Rect rect5 = { (Sint16)(PANEL_LEFT + 489), (Sint16)(UI_OFFSET_Y + 429), 120, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton(_("Cancel"), &UiFocusNavigationEsc, rect5, UIS_CENTER | UIS_BIG | UIS_GOLD));

	UiInitList(selhero_SaveCount + 1, SelheroListFocus, SelheroListSelect, SelheroListEsc, vecSelDlgItems, false, SelheroListDeleteYesNo);
	UiInitScrollBar(scrollBar, MaxViewportItems, &listOffset);
	if (selhero_isMultiPlayer) {
		strcpy(title, _("Multi Player Characters"));
	} else {
		strcpy(title, _("Single Player Characters"));
	}
}

static void UiSelHeroDialog(
    bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)),
    bool (*fncreate)(_uiheroinfo *),
    void (*fnstats)(unsigned int, _uidefaultstats *),
    bool (*fnremove)(_uiheroinfo *),
    _selhero_selections *dlgresult,
    char (*name)[16])
{
	bUIElementsLoaded = true;

	do {
		gfnHeroInfo = fninfo;
		gfnHeroCreate = fncreate;
		gfnHeroStats = fnstats;
		selhero_result = *dlgresult;

		selhero_navigateYesNo = false;

		selhero_Init();

		if (selhero_SaveCount != 0) {
			selhero_List_Init();
		} else {
			SelheroListSelect(selhero_SaveCount);
		}

		selhero_endMenu = false;
		while (!selhero_endMenu && !selhero_navigateYesNo) {
			UiClearScreen();
			UiRenderItems(vecSelHeroDialog);
			UiPollAndRender();
		}
		SelheroFree();

		if (selhero_navigateYesNo) {
			char dialogTitle[32];
			char dialogText[256];
			if (selhero_isMultiPlayer) {
				strncpy(dialogTitle, _("Delete Multi Player Hero"), sizeof(dialogTitle) - 1);
			} else {
				strncpy(dialogTitle, _("Delete Single Player Hero"), sizeof(dialogTitle) - 1);
			}
			strncpy(dialogText, fmt::format(_("Are you sure you want to delete the character \"{:s}\"?"), selhero_heroInfo.name).c_str(), sizeof(dialogText));

			if (UiSelHeroYesNoDialog(dialogTitle, dialogText))
				fnremove(&selhero_heroInfo);
		}
	} while (selhero_navigateYesNo);

	*dlgresult = selhero_result;
	strncpy(*name, selhero_heroInfo.name, sizeof(*name));
}

void UiSelHeroSingDialog(
    bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)),
    bool (*fncreate)(_uiheroinfo *),
    bool (*fnremove)(_uiheroinfo *),
    void (*fnstats)(unsigned int, _uidefaultstats *),
    _selhero_selections *dlgresult,
    char (*name)[16],
    _difficulty *difficulty)
{
	selhero_isMultiPlayer = false;
	UiSelHeroDialog(fninfo, fncreate, fnstats, fnremove, dlgresult, name);
	*difficulty = nDifficulty;
}

void UiSelHeroMultDialog(
    bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)),
    bool (*fncreate)(_uiheroinfo *),
    bool (*fnremove)(_uiheroinfo *),
    void (*fnstats)(unsigned int, _uidefaultstats *),
    _selhero_selections *dlgresult,
    char (*name)[16])
{
	selhero_isMultiPlayer = true;
	UiSelHeroDialog(fninfo, fncreate, fnstats, fnremove, dlgresult, name);
}

} // namespace devilution

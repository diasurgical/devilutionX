#include "DiabloUI/hub/hub.h"

#include <fmt/format.h>

#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/hero/selhero.h"
#include "DiabloUI/hub/chat.h"
#include "DiabloUI/hub/create.h"
#include "DiabloUI/hub/friends.h"
#include "DiabloUI/hub/join.h"
#include "DiabloUI/scrollbar.h"
#include "DiabloUI/selok.h"
#include "config.h"
#include "control.h"
#include "engine/assets.hpp"
#include "engine/load_clx.hpp"
#include "menu.h"
#include "options.h"
#include "storm/storm_net.hpp"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

bool hubmain_endMenu;
std::vector<std::unique_ptr<UiItemBase>> vecHubMainDialog;
OptionalOwnedClxSpriteList Layout;

namespace {

enum class HubPanels : uint8_t {
	Chat,
	Friends,
	Create,
	Join,
};

HubPanels HubPanel = HubPanels::Join;

OptionalOwnedClxSpriteList PlayerIcons;
OptionalOwnedClxSpriteList PlayerSpawnIcon;
OptionalOwnedClxSpriteList PlayerLevelFont;
OptionalOwnedClxSpriteList LagGreen;
OptionalOwnedClxSpriteList LagYellow;
OptionalOwnedClxSpriteList LagRed;

void LoadHubPlayerGraphics()
{
	PlayerIcons = LoadPcxSpriteList("ui_art\\heroport", -14);
	PlayerSpawnIcon = LoadPcx("ui_art\\spwnport");
	PlayerLevelFont = LoadPcxSpriteList("ui_art\\heronum", 10);
	LagGreen = LoadPcx("ui_art\\greenlag");
	LagYellow = LoadPcx("ui_art\\yellolag");
	LagRed = LoadPcx("ui_art\\redlag");
}

std::vector<std::unique_ptr<UiItemBase>> vecHubBackground;

void hub_Init()
{
	LoadDialogButtonGraphics();
	LoadHubScrollBar();

	LoadBackgroundArt("ui_art\\bnconnbg");
	ArtBackgroundWidescreen = LoadOptionalClx("ui_art\\bnconnbgw.clx");

	uint8_t transparentColor = 250;
	AssetRef ref = FindAsset("ui_art\\xsmlogo");
	if (ref.ok() && ref.size() == 167723)
		transparentColor = 32;
	ArtLogo = LoadPcxSpriteList("ui_art\\xsmlogo", /*numFrames=*/15, transparentColor);

	switch (HubPanel) {
	case HubPanels::Chat:
		hubmain_Init();
		break;
	case HubPanels::Friends:
		break;
	case HubPanels::Create:
		HubLoadCreate();
		break;
	case HubPanels::Join:
		HubLoadJoin();
		break;
	}

	LoadHubPlayerGraphics();
}

void hubmain_Free()
{
	FreeDialogButtonGraphics();
	ArtBackground = std::nullopt;
	ArtBackgroundWidescreen = std::nullopt;
	Layout = std::nullopt;
	vecHubMainDialog.clear();
}

bool IsKnownHeroType(uint32_t gameMode, HeroClass heroClass)
{
	if (gameMode != GameIdDiabloFull && gameMode != GameIdHellfireFull)
		return false;

	return heroClass == HeroClass::Warrior
	    || heroClass == HeroClass::Rogue
	    || heroClass == HeroClass::Sorcerer;
}

} // namespace

void LoadHubScrollBar()
{
	ScrollBarWidth = 17;
	ScrollBarArrowFrame_UP_ACTIVE = 2;
	ScrollBarArrowFrame_UP = 0;
	ScrollBarArrowFrame_DOWN_ACTIVE = 3;
	ScrollBarArrowFrame_DOWN = 1;
	ArtScrollBarArrow = LoadPcxSpriteList("ui_art\\scrlarrw", 4);
	ArtScrollBarBackground = LoadPcx("ui_art\\scrlbar");
	ArtScrollBarThumb = LoadPcx("ui_art\\scrlthmb");
}

void UiHubPlacePlayerIcon(Point position, uint32_t gameMode, const PlayerInfo &player)
{
	ClxSprite sprite = (*PlayerSpawnIcon)[0];
	if (IsKnownHeroType(gameMode, player.heroClass)) {
		int level = player.diabloKillLevel * 3;
		sprite = (*PlayerIcons)[level + static_cast<uint8_t>(player.heroClass)];
	}
	const SDL_Rect rect0 = MakeSdlRect(position.x, position.y, 0, 0);
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>(sprite, rect0));

	const SDL_Rect rect1 = MakeSdlRect(position.x + 20, position.y + 5, 0, 0);
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*PlayerLevelFont)[player.level % 10], rect1));
	if (player.level > 9) {
		const SDL_Rect rect2 = MakeSdlRect(position.x + 14, position.y + 5, 0, 0);
		vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*PlayerLevelFont)[player.level / 10], rect2));
	}
}

void UiHubPlaceLatencyMeter(int latency, Point position)
{
	int bars = latency / 50 + 1;
	bars = std::min(bars, 6);

	ClxSprite lagSprite = (*LagRed)[0];
	if (bars <= 2)
		lagSprite = (*LagGreen)[0];
	else if (bars <= 4)
		lagSprite = (*LagYellow)[0];

	const SDL_Rect rect4 = MakeSdlRect(position.x, position.y, 3 * bars, 11);
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>(lagSprite, rect4));
}

void hubmain_GameSelection_Focus(int value)
{
}

void hubmain_Diff_Select(int value)
{
}

void hubmain_GameSelection_Esc()
{
	UiInitList_clear();
	hubmain_endMenu = true;
}

void hub_GameSelection_Init()
{
	const Point uiPosition = GetUIRectangle().position;

	const SDL_Rect rect0 = MakeSdlRect(0, uiPosition.y, 0, 0);
	vecHubBackground.push_back(std::make_unique<UiImageClx>((*ArtBackground)[0], rect0, UiFlags::AlignCenter));
	vecHubBackground.push_back(std::make_unique<UiImageClx>((*ArtBackgroundWidescreen)[0], rect0, UiFlags::AlignCenter));

	vecHubBackground.push_back(std::make_unique<UiImageAnimatedClx>(*ArtLogo, MakeSdlRect(uiPosition.x, uiPosition.y, 0, 0)));

	const SDL_Rect rect1 = MakeSdlRect(uiPosition.x, uiPosition.y + (*ArtBackground)[0].height() - (*Layout)[0].height(), (*Layout)[0].width(), (*Layout)[0].height());
	vecHubBackground.push_back(std::make_unique<UiImageClx>((*Layout)[0], rect1, UiFlags::AlignCenter));

	switch (HubPanel) {
	case HubPanels::Chat:
		hubmain_GameSelection_Init();
		break;
	case HubPanels::Friends:
		break;
	case HubPanels::Create:
		HubInitCreate();
		break;
	case HubPanels::Join:
		HubInitJoin();
		break;
	}

	UiInitList(hubmain_GameSelection_Focus, hubmain_Diff_Select, hubmain_GameSelection_Esc, vecHubMainDialog, true);
}

bool UiHubMain()
{
	hub_Init();
	hub_GameSelection_Init();

	hubmain_endMenu = false;

	while (!hubmain_endMenu) {
		UiClearScreen();
		UiRenderItems(vecHubBackground);
		if (HubPanel == HubPanels::Chat)
			DrawChat();
		UiPollAndRender();
	}
	hubmain_Free();

	return true;
}
} // namespace devilution

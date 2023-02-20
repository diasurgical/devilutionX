#include "DiabloUI/hub/chat.h"

#include "DiabloUI/hub/hub.h"
#include "DiabloUI/scrollbar.h"
#include "engine/load_pcx.hpp"

namespace devilution {

namespace {

char ChatMessage[129];

OptionalOwnedClxSpriteList MainButton;
std::vector<PlayerInfo> PlayerList;

struct ColoredText {
	std::string text;
	UiFlags color;
};

struct MultiColoredText {
	std::string text;
	std::vector<ColoredText> colors;
	int offset = 0;
};

std::vector<MultiColoredText> ChatLogLines;

void BuildFixtures()
{
	PlayerList.emplace_back(PlayerInfo { "KPhoenix", HeroClass::Warrior, 35, 2, GameIdDiabloFull, 50 });
	PlayerList.emplace_back(PlayerInfo { "AJenbo", HeroClass::Rogue, 23, 1, GameIdHellfireFull, 320 });
	PlayerList.emplace_back(PlayerInfo { "FireIceTalon", HeroClass::Sorcerer, 50, 3, GameIdDiabloFull, 150 });
	PlayerList.emplace_back(PlayerInfo { "glebm", HeroClass::Monk, 12, 0, GameIdHellfireFull, 1400 });
	PlayerList.emplace_back(PlayerInfo { "qndel", HeroClass::Warrior, 1, 0, GameIdDiabloSpawn, 450 });

	ChatLogLines.emplace_back(MultiColoredText { "{0}", { { "KPhoenix has appeared on the network", UiFlags::ColorUiGreen } } });
	ChatLogLines.emplace_back(MultiColoredText { "{0} {1}", { { "<KPhoenix>", UiFlags::ColorYellow }, { "Hello", UiFlags::ColorDialogWhite } } });
	ChatLogLines.emplace_back(MultiColoredText { "{0} {1}", { { "<AJenbo>", UiFlags::ColorBlue }, { "Ready?", UiFlags::ColorUiSilverDark } } });
	ChatLogLines.emplace_back(MultiColoredText { "{0}", { { "FireIceTalon started a new game", UiFlags::ColorRed } } });
}

void hubmain_Free()
{
	PlayerList.clear();
	ChatLogLines.clear();
}

void DialogActionOK()
{
}

} // namespace

void hubmain_Init()
{
	Layout = LoadPcx("ui_art\\chat_bkg", /*transparentColor=*/0);
	MainButton = LoadPcxSpriteList("ui_art\\bnbuttns", -71);

	BuildFixtures();
}

void UiHubInitPlayerList()
{
	const Point uiPosition = GetUIRectangle().position + Displacement { 460, 200 };
	int yOffset = 0;

	for (const PlayerInfo &player : PlayerList) {
		UiHubPlacePlayerIcon({ uiPosition.x, uiPosition.y + yOffset }, player.gameMode, player);

		const SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 30, uiPosition.y + yOffset - 6, 89, 20);
		vecHubMainDialog.push_back(std::make_unique<UiArtText>(player.name.data(), rect3, UiFlags::FontSizeDialog | UiFlags::ColorYellow, -1));

		UiHubPlaceLatencyMeter(player.latency, { uiPosition.x + 121, uiPosition.y + yOffset + 2 });

		yOffset += 16;
	}
}

void hubmain_GameSelection_Init()
{
	const Point uiPosition = GetUIRectangle().position;

	const SDL_Rect rect2 = MakeSdlRect(uiPosition.x + 445, uiPosition.y + 148, 179, 28);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Global Chat").data(), rect2, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter | UiFlags::VerticalCenter, -1));

	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*MainButton)[0], MakeSdlRect(uiPosition.x + 10, uiPosition.y + 140, 85, 71)));
	const SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 10, uiPosition.y + 140 + 43, 85, 71);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Friends").data(), rect3, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -1));
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*MainButton)[5], MakeSdlRect(uiPosition.x + 10, uiPosition.y + 227, 85, 71)));
	const SDL_Rect rect4 = MakeSdlRect(uiPosition.x + 10, uiPosition.y + 227 + 43, 85, 71);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Create").data(), rect4, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -1));
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*MainButton)[10], MakeSdlRect(uiPosition.x + 10, uiPosition.y + 315, 85, 71)));
	const SDL_Rect rect5 = MakeSdlRect(uiPosition.x + 10, uiPosition.y + 315 + 43, 85, 71);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Join").data(), rect5, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -1));
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*MainButton)[15], MakeSdlRect(uiPosition.x + 10, uiPosition.y + 402, 85, 71)));
	const SDL_Rect rect6 = MakeSdlRect(uiPosition.x + 10, uiPosition.y + 402 + 43, 85, 71);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Quit").data(), rect6, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -1));

	UiHubInitPlayerList();

	const SDL_Rect rectScrollbar1 = MakeSdlRect(uiPosition.x + 402, uiPosition.y + 158, 17, 251);
	vecHubMainDialog.push_back(std::make_unique<UiScrollbar>((*ArtScrollBarBackground)[0], (*ArtScrollBarThumb)[0], *ArtScrollBarArrow, rectScrollbar1));

	const SDL_Rect rectScrollbar2 = MakeSdlRect(uiPosition.x + 602, uiPosition.y + 199, 17, 209);
	vecHubMainDialog.push_back(std::make_unique<UiScrollbar>((*ArtScrollBarBackground)[0], (*ArtScrollBarThumb)[0], *ArtScrollBarArrow, rectScrollbar2));

	const SDL_Rect rect7 = MakeSdlRect(uiPosition.x + 264 + 188, uiPosition.y + 335 + 103, 85, 35);
	vecHubMainDialog.push_back(std::make_unique<UiButton>(_("Send"), &DialogActionOK, rect7));

	const SDL_Rect rect8 = MakeSdlRect(uiPosition.x + 264 + 283, uiPosition.y + 335 + 103, 85, 35);
	vecHubMainDialog.push_back(std::make_unique<UiButton>(_("Whisper"), &DialogActionOK, rect8));

	const SDL_Rect rect9 = MakeSdlRect(uiPosition.x + 117, uiPosition.y + 433, 306, 30);
	vecHubMainDialog.push_back(std::make_unique<UiEdit>(_("Enter chat message"), ChatMessage, 128, false, rect9, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite));
}

void DrawChat()
{
	const Point uiPosition = GetUIRectangle().position + Displacement { 116, 161 };

	const Surface &out = Surface(DiabloUiSurface());
	int displayLines = ChatLogLines.size();
	displayLines = std::min(100, displayLines);
	int SkipLines = 0;
	int lineHeight = 22;
	for (int i = 0; i < displayLines; i++) {
		if (i + SkipLines >= ChatLogLines.size())
			break;
		MultiColoredText &text = ChatLogLines[ChatLogLines.size() - (i + SkipLines + 1)];
		const string_view line = text.text;

		std::vector<DrawStringFormatArg> args;
		for (auto &x : text.colors) {
			args.emplace_back(DrawStringFormatArg { x.text, x.color });
		}
		DrawStringWithColors(out, line, args, { { (uiPosition.x + text.offset), uiPosition.y + i * lineHeight }, { 280 - text.offset * 2, lineHeight } }, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, /*spacing=*/1, lineHeight);
	}
}

} // namespace devilution

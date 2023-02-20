#include "DiabloUI/hub/join.h"

#include "DiabloUI/hub/hub.h"
#include "DiabloUI/scrollbar.h"
#include "engine/load_pcx.hpp"

namespace devilution {

namespace {

void DialogActionOK()
{
}

std::vector<PlayerInfo> PlayerList;

std::string relativeTime;
std::string difficultyString;
std::string GetDifficultyString(int difficulty)
{
	constexpr std::array<const char *, 3> DifficultyStrs = { N_("Normal"), N_("Nightmare"), N_("Hell") };
	const string_view difficultyStr = _(DifficultyStrs[difficulty]);
	return fmt::format(fmt::runtime(_(/* TRANSLATORS: "Nightmare Difficulty" */ "{:s} Difficulty")), difficultyStr);
}

char GameNameInput[32];
char GamePasswordInput[32];

} // namespace

void HubLoadJoin()
{
	Layout = LoadPcx("ui_art\\bnjoinbg", /*transparentColor=*/0);
}

void HubInitJoin()
{
	const Point uiPosition = GetUIRectangle().position;

	const SDL_Rect rect0 = MakeSdlRect(uiPosition.x + 17, uiPosition.y + 150, 274, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Matching Public Games").data(), rect0, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -1));

	const SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 312, uiPosition.y + 150, 311, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Join Game").data(), rect1, UiFlags::FontSizeDialog | UiFlags::ColorYellow | UiFlags::AlignCenter, -1));

	const SDL_Rect rect2 = MakeSdlRect(uiPosition.x + 326, uiPosition.y + 185, 274, 26 * 2);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("To Join a game, enter the game\ninfomration below.").data(), rect2, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));

	const SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 326, uiPosition.y + 249, 86, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Name:").data(), rect3, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignRight, -1));
	const SDL_Rect rect4 = MakeSdlRect(uiPosition.x + 421, uiPosition.y + 250, 192, 29);
	vecHubMainDialog.push_back(std::make_unique<UiEdit>(_("Enter game name"), GameNameInput, 32, false, rect4, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite));

	const SDL_Rect rect5 = MakeSdlRect(uiPosition.x + 326, uiPosition.y + 294, 86, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Password:").data(), rect5, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignRight, -1));
	const SDL_Rect rect6 = MakeSdlRect(uiPosition.x + 421, uiPosition.y + 295, 192, 29);
	vecHubMainDialog.push_back(std::make_unique<UiEdit>(_("Enter game password"), GamePasswordInput, 32, false, rect6, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite));

	const SDL_Rect rectScrollbar = MakeSdlRect(uiPosition.x + 269, uiPosition.y + 189, 17, 274);
	vecHubMainDialog.push_back(std::make_unique<UiScrollbar>((*ArtScrollBarBackground)[0], (*ArtScrollBarThumb)[0], *ArtScrollBarArrow, rectScrollbar));

	UiHubPlaceLatencyMeter(75, { uiPosition.x + 245, uiPosition.y + 195 });

	const SDL_Rect rect7 = MakeSdlRect(uiPosition.x + 27, uiPosition.y + 186, 218, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>("CWEJZ", rect7, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));

	const Point detailsPosition = uiPosition + Displacement { 326, 342 };
	int yOffset = 0;

	difficultyString = GetDifficultyString(1);
	const SDL_Rect rect8 = MakeSdlRect(detailsPosition.x, detailsPosition.y + yOffset - 6, 149, 18);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(difficultyString.data(), rect8, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));
	yOffset += 16;

	int seconds = 76 * 60;
	int hours = seconds / 3600;
	int minutes = (seconds % 3600) / 60;
	relativeTime = fmt::format(fmt::runtime(ngettext("{:d} minute", "{:d} minutes", minutes)), minutes);
	if (hours > 0) {
		if (minutes > 0) {
			relativeTime = fmt::format(fmt::runtime(ngettext("{:d} minute", "{:d} minutes", minutes)), minutes);
			relativeTime = fmt::format(fmt::runtime(ngettext(
			                               /* TRANSLATORS: {:s} the translated minuts (3 minuts).*/
			                               "Time: {:d} hour and {:s}",
			                               "Time: {:d} hours and {:s}",
			                               hours)),
			    hours, relativeTime);
		} else {
			relativeTime = fmt::format(fmt::runtime(ngettext("Time: {:d} hour", "Time: {:d} hours", hours)), hours);
		}
	} else {
		relativeTime = fmt::format(fmt::runtime(_("Time: {:s}")), relativeTime);
	}
	const SDL_Rect rect9 = MakeSdlRect(detailsPosition.x, detailsPosition.y + yOffset - 6, 149, 18);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(relativeTime.data(), rect9, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));
	yOffset += 16;

	const SDL_Rect rect10 = MakeSdlRect(detailsPosition.x, detailsPosition.y + yOffset - 6, 51, 18);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Players:").data(), rect10, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));

	PlayerList.emplace_back(PlayerInfo { "KPhoenix", HeroClass::Warrior, 35, 2, GameIdDiabloFull, 0 });
	PlayerList.emplace_back(PlayerInfo { "AJenbo", HeroClass::Rogue, 23, 1, GameIdDiabloFull, 0 });
	PlayerList.emplace_back(PlayerInfo { "glebm", HeroClass::Sorcerer, 19, 0, GameIdDiabloFull, 0 });

	for (auto &player : PlayerList) {
		UiHubPlacePlayerIcon({ detailsPosition.x + 52, detailsPosition.y + yOffset }, GameIdDiabloFull, player);
		const SDL_Rect rect = MakeSdlRect(detailsPosition.x + 52 + 30, detailsPosition.y + yOffset - 6, 149 - 30, 18);
		vecHubMainDialog.push_back(std::make_unique<UiArtText>(player.name.data(), rect, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));

		yOffset += 16;
	}

	const Point detailsPosition2 = detailsPosition + Displacement { 184, 0 };
	yOffset = 0;

	const SDL_Rect rect11 = MakeSdlRect(detailsPosition2.x, detailsPosition2.y + yOffset - 6, 149, 18);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Run in Town").data(), rect11, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));
	yOffset += 16;

	const SDL_Rect rect12 = MakeSdlRect(detailsPosition2.x, detailsPosition2.y + yOffset - 6, 149, 18);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Theo Quest").data(), rect12, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));
	yOffset += 16;

	const SDL_Rect rect13 = MakeSdlRect(detailsPosition2.x, detailsPosition2.y + yOffset - 6, 149, 18);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Friendly Fire").data(), rect13, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1));
	yOffset += 16;

	const SDL_Rect rect14 = MakeSdlRect(uiPosition.x + 264 + 188, uiPosition.y + 335 + 103, 85, 35);
	vecHubMainDialog.push_back(std::make_unique<UiButton>(_("OK"), &DialogActionOK, rect14));

	const SDL_Rect rect15 = MakeSdlRect(uiPosition.x + 264 + 283, uiPosition.y + 335 + 103, 85, 35);
	vecHubMainDialog.push_back(std::make_unique<UiButton>(_("Cancel"), &DialogActionOK, rect15));
}

} // namespace devilution

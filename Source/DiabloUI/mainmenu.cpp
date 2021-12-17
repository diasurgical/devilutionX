
#include "DiabloUI/diabloui.h"
#include "DiabloUI/selok.h"
#include "control.h"
#include "main_loop.hpp"
#include "utils/language.h"

namespace devilution {
namespace {
int mainmenu_attract_time_out; // seconds
uint32_t dwAttractTicks;

class MainMenuDialog : public MainLoopHandler {
public:
	MainMenuDialog(const char *name, _mainmenu_selections *result, void (*fnSound)(const char *file), int attractTimeOut)
	    : result_(result)
	{
		*result_ = MAINMENU_NONE;
		mainmenu_attract_time_out = attractTimeOut;
		mainmenu_restart_repintro(); // for automatic starts
		gfnSoundFunction = fnSound;

		listItems_.push_back(std::make_unique<UiListItem>(_("Single Player"), MAINMENU_SINGLE_PLAYER));
		listItems_.push_back(std::make_unique<UiListItem>(_("Multi Player"), MAINMENU_MULTIPLAYER));
		listItems_.push_back(std::make_unique<UiListItem>(_("Settings"), MAINMENU_SETTINGS));
		listItems_.push_back(std::make_unique<UiListItem>(_("Support"), MAINMENU_SHOW_SUPPORT));
		listItems_.push_back(std::make_unique<UiListItem>(_("Show Credits"), MAINMENU_SHOW_CREDITS));
		listItems_.push_back(std::make_unique<UiListItem>(gbIsHellfire ? _("Exit Hellfire") : _("Exit Diablo"), MAINMENU_EXIT_DIABLO));

		if (!gbIsSpawn || gbIsHellfire) {
			if (gbIsHellfire)
				LoadArt("ui_art\\mainmenuw.pcx", &ArtBackgroundWidescreen);
			LoadBackgroundArt("ui_art\\mainmenu.pcx");
		} else {
			LoadBackgroundArt("ui_art\\swmmenu.pcx");
		}

		UiAddBackground(&items_);
		UiAddLogo(&items_);

		if (gbIsSpawn && gbIsHellfire) {
			SDL_Rect rect1 = { (Sint16)(PANEL_LEFT), (Sint16)(UI_OFFSET_Y + 145), 640, 30 };
			items_.push_back(std::make_unique<UiArtText>(_("Shareware"), rect1, UiFlags::FontSize30 | UiFlags::ColorUiSilver | UiFlags::AlignCenter, 8));
		}

		items_.push_back(std::make_unique<UiList>(listItems_, listItems_.size(), PANEL_LEFT + 64, (UI_OFFSET_Y + 192), 510, 43, UiFlags::FontSize42 | UiFlags::ColorUiGold | UiFlags::AlignCenter, 5));

		SDL_Rect rect2 = { 17, (Sint16)(gnScreenHeight - 36), 605, 21 };
		items_.push_back(std::make_unique<UiArtText>(name, rect2, UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

		instance_ = this;
		UiInitList(nullptr, UiMainMenuSelect, MainmenuEsc, items_, true);
	}

	~MainMenuDialog() override
	{
		ArtBackgroundWidescreen.Unload();
		ArtBackground.Unload();
		instance_ = nullptr;
	}

	void HandleEvent(SDL_Event &event) override
	{
		UiFocusNavigation(&event);
		UiHandleEvents(&event);
	}

	void Render() override
	{
		UiClearScreen();
		UiRender();
		if (SDL_GetTicks() >= dwAttractTicks && (diabdat_mpq || hellfire_mpq)) {
			*result_ = MAINMENU_ATTRACT_MODE;
			NextMainLoopHandler();
		}
	}

	void Select(int value)
	{
		*result_ = (_mainmenu_selections)listItems_[value]->m_value;
		if (*result_ != MAINMENU_NONE) {
			NextMainLoopHandler();
		}
	}

	void Esc()
	{
		const size_t last = listItems_.size() - 1;
		if (SelectedItem == last) {
			Select(static_cast<int>(last));
		} else {
			SelectedItem = last;
		}
	}

private:
	static void UiMainMenuSelect(int value)
	{
		instance_->Select(value);
	}

	static void MainmenuEsc()
	{
		instance_->Esc();
	}

	static MainMenuDialog *instance_;

	std::vector<std::unique_ptr<UiItemBase>> items_;
	std::vector<std::unique_ptr<UiListItem>> listItems_;
	_mainmenu_selections *result_;
};

MainMenuDialog *MainMenuDialog::instance_ = nullptr;

} // namespace

void mainmenu_restart_repintro()
{
	dwAttractTicks = SDL_GetTicks() + mainmenu_attract_time_out * 1000;
}

bool UiMainMenuDialog(const char *name, _mainmenu_selections *pdwResult, void (*fnSound)(const char *file), int attractTimeOut)
{
	SetMainLoopHandler(std::make_unique<MainMenuDialog>(name, pdwResult, fnSound, attractTimeOut));
	return true;
}

} // namespace devilution

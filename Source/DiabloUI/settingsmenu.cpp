#include "selstart.h"

#include "DiabloUI/diabloui.h"
#include "DiabloUI/scrollbar.h"
#include "control.h"
#include "engine/render/text_render.hpp"
#include "hwcursor.hpp"
#include "options.h"
#include "utils/language.h"

namespace devilution {
namespace {

bool endMenu = false;
bool backToMain = false;

std::vector<std::unique_ptr<UiListItem>> vecDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecDialog;
std::vector<OptionEntryBase *> vecOptions;
OptionEntryBase *selectedOption = nullptr;

enum class ShownMenuType {
	Settings,
	ListOption,
};

ShownMenuType shownMenu;

char optionDescription[512];

Rectangle rectDescription;

enum class SpecialMenuEntry {
	None = -1,
	PreviousMenu = -2,
};

bool IsValidEntry(OptionEntryBase *pOptionEntry)
{
	auto flags = pOptionEntry->GetFlags();
	if (HasAnyOf(flags, OptionEntryFlags::NeedDiabloMpq) && !diabdat_mpq)
		return false;
	if (HasAnyOf(flags, OptionEntryFlags::NeedHellfireMpq) && !hellfire_mpq)
		return false;
	return HasNoneOf(flags, OptionEntryFlags::Invisible | (gbIsHellfire ? OptionEntryFlags::OnlyDiablo : OptionEntryFlags::OnlyHellfire));
}

std::vector<DrawStringFormatArg> CreateDrawStringFormatArgForEntry(OptionEntryBase *pEntry)
{
	return std::vector<DrawStringFormatArg> {
		{ pEntry->GetName().data(), UiFlags::ColorUiGold },
		{ pEntry->GetValueDescription().data(), UiFlags::ColorUiSilver }
	};
}

void CleanUpSettingsUI()
{
	UiInitList_clear();

	vecDialogItems.clear();
	vecDialog.clear();
	vecOptions.clear();

	ArtBackground.Unload();
	ArtBackgroundWidescreen.Unload();
	UnloadScrollBar();
}

void GoBackOneMenuLevel()
{
	endMenu = true;
	backToMain = shownMenu == ShownMenuType::Settings;
	shownMenu = ShownMenuType::Settings;
}

void ItemFocused(int value)
{
	auto &vecItem = vecDialogItems[value];
	optionDescription[0] = '\0';
	if (vecItem->m_value < 0 || shownMenu != ShownMenuType::Settings) {
		return;
	}
	auto *pOption = vecOptions[vecItem->m_value];
	auto paragraphs = WordWrapString(pOption->GetDescription(), rectDescription.size.width, GameFont12, 1);
	strncpy(optionDescription, paragraphs.c_str(), sizeof(optionDescription));
}

bool ChangeOptionValue(OptionEntryBase *pOption, size_t listIndex)
{
	if (HasAnyOf(pOption->GetFlags(), OptionEntryFlags::RecreateUI)) {
		endMenu = true;
		// Clean up all UI related Data
		CleanUpSettingsUI();
		UnloadUiGFX();
		FreeItemGFX();
		selectedOption = pOption;
	}

	switch (pOption->GetType()) {
	case OptionEntryType::Boolean: {
		auto *pOptionBoolean = static_cast<OptionEntryBoolean *>(pOption);
		pOptionBoolean->SetValue(!**pOptionBoolean);
	} break;
	case OptionEntryType::List: {
		auto *pOptionList = static_cast<OptionEntryListBase *>(pOption);
		pOptionList->SetActiveListIndex(listIndex);
	} break;
	}

	if (HasAnyOf(pOption->GetFlags(), OptionEntryFlags::RecreateUI)) {
		// Reinitalize UI with changed settings (for example game mode, language or resolution)
		UiInitialize();
		InitItemGFX();
		if (IsHardwareCursor())
			SetHardwareCursor(CursorInfo::UnknownCursor());
		return false;
	}

	return true;
}

void ItemSelected(int value)
{
	auto &vecItem = vecDialogItems[value];
	int vecItemValue = vecItem->m_value;
	if (vecItemValue < 0) {
		auto specialMenuEntry = static_cast<SpecialMenuEntry>(vecItemValue);
		switch (specialMenuEntry) {
		case SpecialMenuEntry::None:
			break;
		case SpecialMenuEntry::PreviousMenu:
			GoBackOneMenuLevel();
			break;
		}
		return;
	}

	switch (shownMenu) {
	case ShownMenuType::Settings: {
		auto *pOption = vecOptions[vecItemValue];
		bool updateValueDescription = false;
		if (pOption->GetType() == OptionEntryType::List) {
			auto *pOptionList = static_cast<OptionEntryListBase *>(pOption);
			if (pOptionList->GetListSize() > 2) {
				selectedOption = pOption;
				endMenu = true;
				shownMenu = ShownMenuType::ListOption;
			} else {
				// If the list contains only two items, we don't show a submenu and instead change the option value instantly
				size_t nextIndex = pOptionList->GetActiveListIndex() + 1;
				if (nextIndex >= pOptionList->GetListSize())
					nextIndex = 0;
				updateValueDescription = ChangeOptionValue(pOption, nextIndex);
			}
		} else {
			updateValueDescription = ChangeOptionValue(pOption, 0);
		}
		if (updateValueDescription) {
			vecItem->args.clear();
			for (auto &arg : CreateDrawStringFormatArgForEntry(pOption))
				vecItem->args.push_back(arg);
		}
	} break;
	case ShownMenuType::ListOption: {
		ChangeOptionValue(selectedOption, vecItemValue);
		GoBackOneMenuLevel();
	} break;
	}
}

void EscPressed()
{
	GoBackOneMenuLevel();
}

} // namespace

void UiSettingsMenu()
{
	backToMain = false;
	shownMenu = ShownMenuType::Settings;
	selectedOption = nullptr;

	do {
		endMenu = false;

		LoadBackgroundArt("ui_art\\black.pcx");
		LoadScrollBar();
		UiAddBackground(&vecDialog);
		UiAddLogo(&vecDialog);

		Rectangle rectList = { { PANEL_LEFT + 50, (UI_OFFSET_Y + 204) }, { 540, 208 } };
		rectDescription = { { PANEL_LEFT + 24, rectList.position.y + rectList.size.height + 16 }, { 590, 35 } };

		optionDescription[0] = '\0';

		const char *titleText = shownMenu == ShownMenuType::Settings ? _("Settings") : selectedOption->GetName().data();
		vecDialog.push_back(std::make_unique<UiArtText>(titleText, MakeSdlRect(PANEL_LEFT, UI_OFFSET_Y + 161, PANEL_WIDTH, 35), UiFlags::FontSize30 | UiFlags::ColorUiSilver | UiFlags::AlignCenter, 8));
		vecDialog.push_back(std::make_unique<UiScrollbar>(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, MakeSdlRect(rectList.position.x + rectList.size.width + 5, rectList.position.y, 25, rectList.size.height)));
		vecDialog.push_back(std::make_unique<UiArtText>(optionDescription, MakeSdlRect(rectDescription), UiFlags::FontSize12 | UiFlags::ColorUiSilverDark | UiFlags::AlignCenter, 1, IsSmallFontTall() ? 22 : 18));

		size_t itemToSelect = 1;

		switch (shownMenu) {
		case ShownMenuType::Settings: {
			size_t catCount = 0;
			for (auto *pCategory : sgOptions.GetCategories()) {
				bool categoryCreated = false;
				for (auto *pEntry : pCategory->GetEntries()) {
					if (!IsValidEntry(pEntry))
						continue;
					if (!categoryCreated) {
						if (catCount > 0)
							vecDialogItems.push_back(std::make_unique<UiListItem>("", -1, UiFlags::ElementDisabled));
						catCount += 1;
						vecDialogItems.push_back(std::make_unique<UiListItem>(pCategory->GetName().data(), static_cast<int>(SpecialMenuEntry::None), UiFlags::ColorWhitegold | UiFlags::ElementDisabled));
						categoryCreated = true;
					}
					auto formatArgs = CreateDrawStringFormatArgForEntry(pEntry);
					if (selectedOption == pEntry)
						itemToSelect = vecDialogItems.size();
					vecDialogItems.push_back(std::make_unique<UiListItem>("{}: {}", formatArgs, vecOptions.size(), UiFlags::ColorUiGold));
					vecOptions.push_back(pEntry);
				}
			}
		} break;
		case ShownMenuType::ListOption: {
			auto *pOptionList = static_cast<OptionEntryListBase *>(selectedOption);
			for (size_t i = 0; i < pOptionList->GetListSize(); i++) {
				vecDialogItems.push_back(std::make_unique<UiListItem>(pOptionList->GetListDescription(i).data(), i, UiFlags::ColorUiGold));
			}
			itemToSelect = pOptionList->GetActiveListIndex();
		} break;
		}

		vecDialogItems.push_back(std::make_unique<UiListItem>("", static_cast<int>(SpecialMenuEntry::None), UiFlags::ElementDisabled));
		vecDialogItems.push_back(std::make_unique<UiListItem>(_("Previous Menu"), static_cast<int>(SpecialMenuEntry::PreviousMenu), UiFlags::ColorUiGold));

		vecDialog.push_back(std::make_unique<UiList>(vecDialogItems, rectList.size.height / 26, rectList.position.x, rectList.position.y, rectList.size.width, 26, UiFlags::FontSize24 | UiFlags::AlignCenter));

		UiInitList(ItemFocused, ItemSelected, EscPressed, vecDialog, true, nullptr, itemToSelect);

		while (!endMenu) {
			UiClearScreen();
			UiRenderItems(vecDialog);
			UiPollAndRender();
		}

		CleanUpSettingsUI();
	} while (!backToMain);
}

} // namespace devilution

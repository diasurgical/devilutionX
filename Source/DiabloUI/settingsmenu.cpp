#include "selstart.h"

#include "DiabloUI/diabloui.h"
#include "DiabloUI/scrollbar.h"
#include "control.h"
#include "engine/render/text_render.hpp"
#include "hwcursor.hpp"
#include "options.h"
#include "utils/language.h"
#include "utils/utf8.hpp"

namespace devilution {
namespace {

constexpr size_t IndexKeyInput = 1;

bool endMenu = false;
bool backToMain = false;

std::vector<std::unique_ptr<UiListItem>> vecDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecDialog;
std::vector<OptionEntryBase *> vecOptions;
OptionEntryBase *selectedOption = nullptr;

enum class ShownMenuType {
	Settings,
	ListOption,
	KeyInput,
};

ShownMenuType shownMenu;

char optionDescription[512];

Rectangle rectList;
Rectangle rectDescription;

enum class SpecialMenuEntry {
	None = -1,
	PreviousMenu = -2,
	UnbindKey = -3,
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

/** @brief Check if the option text can't fit in one list line (list width minus drawn selector) */
bool NeedsTwoLinesToDisplayOption(std::vector<DrawStringFormatArg> &formatArgs)
{
	return GetLineWidth("{}: {}", formatArgs.data(), formatArgs.size(), GameFontTables::GameFont24, 1) >= (rectList.size.width - 90);
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

void UpdateDescription(const OptionEntryBase &option)
{
	auto paragraphs = WordWrapString(option.GetDescription(), rectDescription.size.width, GameFont12, 1);
	CopyUtf8(optionDescription, paragraphs, sizeof(optionDescription));
}

void ItemFocused(int value)
{
	if (shownMenu != ShownMenuType::Settings)
		return;

	auto &vecItem = vecDialogItems[value];
	optionDescription[0] = '\0';
	if (vecItem->m_value < 0 || shownMenu != ShownMenuType::Settings) {
		return;
	}
	auto *pOption = vecOptions[vecItem->m_value];
	UpdateDescription(*pOption);
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
		case SpecialMenuEntry::UnbindKey:
			auto *pOptionKey = static_cast<KeymapperOptions::Action *>(selectedOption);
			pOptionKey->SetValue(DVL_VK_INVALID);
			vecDialogItems[IndexKeyInput]->m_text = selectedOption->GetValueDescription().data();
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
		} else if (pOption->GetType() == OptionEntryType::Key) {
			selectedOption = pOption;
			endMenu = true;
			shownMenu = ShownMenuType::KeyInput;
		} else {
			updateValueDescription = ChangeOptionValue(pOption, 0);
		}
		if (updateValueDescription) {
			auto args = CreateDrawStringFormatArgForEntry(pOption);
			bool optionUsesTwoLines = ((value + 1) < vecDialogItems.size() && vecDialogItems[value]->m_value == vecDialogItems[value + 1]->m_value);
			if (NeedsTwoLinesToDisplayOption(args) != optionUsesTwoLines) {
				selectedOption = pOption;
				endMenu = true;
			} else {
				vecItem->args.clear();
				for (auto &arg : args)
					vecItem->args.push_back(arg);
				if (optionUsesTwoLines) {
					vecDialogItems[value + 1]->m_text = pOption->GetValueDescription().data();
				}
			}
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

void FullscreenChanged()
{
	auto *fullscreenOption = &sgOptions.Graphics.fullscreen;

	for (auto &vecItem : vecDialogItems) {
		int vecItemValue = vecItem->m_value;
		if (vecItemValue < 0)
			continue;

		auto *pOption = vecOptions[vecItemValue];
		if (pOption != fullscreenOption)
			continue;

		vecItem->args.clear();
		for (auto &arg : CreateDrawStringFormatArgForEntry(pOption))
			vecItem->args.push_back(arg);
		break;
	}
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

		rectList = { { PANEL_LEFT + 50, (UI_OFFSET_Y + 204) }, { 540, 208 } };
		rectDescription = { { PANEL_LEFT + 24, rectList.position.y + rectList.size.height + 16 }, { 590, 35 } };

		optionDescription[0] = '\0';

		const char *titleText = shownMenu == ShownMenuType::Settings ? _("Settings") : selectedOption->GetName().data();
		vecDialog.push_back(std::make_unique<UiArtText>(titleText, MakeSdlRect(PANEL_LEFT, UI_OFFSET_Y + 161, PANEL_WIDTH, 35), UiFlags::FontSize30 | UiFlags::ColorUiSilver | UiFlags::AlignCenter, 8));
		vecDialog.push_back(std::make_unique<UiScrollbar>(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, MakeSdlRect(rectList.position.x + rectList.size.width + 5, rectList.position.y, 25, rectList.size.height)));
		vecDialog.push_back(std::make_unique<UiArtText>(optionDescription, MakeSdlRect(rectDescription), UiFlags::FontSize12 | UiFlags::ColorUiSilverDark | UiFlags::AlignCenter, 1, IsSmallFontTall() ? 22 : 18));

		size_t itemToSelect = 1;
		std::function<bool(SDL_Event &)> eventHandler;

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
							vecDialogItems.push_back(std::make_unique<UiListItem>("", static_cast<int>(SpecialMenuEntry::None), UiFlags::ElementDisabled));
						catCount += 1;
						vecDialogItems.push_back(std::make_unique<UiListItem>(pCategory->GetName().data(), static_cast<int>(SpecialMenuEntry::None), UiFlags::ColorWhitegold | UiFlags::ElementDisabled));
						categoryCreated = true;
					}
					if (selectedOption == pEntry)
						itemToSelect = vecDialogItems.size();
					auto formatArgs = CreateDrawStringFormatArgForEntry(pEntry);
					if (NeedsTwoLinesToDisplayOption(formatArgs)) {
						vecDialogItems.push_back(std::make_unique<UiListItem>("{}:", formatArgs, vecOptions.size(), UiFlags::ColorUiGold | UiFlags::NeedsNextElement));
						vecDialogItems.push_back(std::make_unique<UiListItem>(pEntry->GetValueDescription().data(), vecOptions.size(), UiFlags::ColorUiSilver | UiFlags::ElementDisabled));
					} else {
						vecDialogItems.push_back(std::make_unique<UiListItem>("{}: {}", formatArgs, vecOptions.size(), UiFlags::ColorUiGold));
					}
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
			UpdateDescription(*pOptionList);
		} break;
		case ShownMenuType::KeyInput: {
			vecDialogItems.push_back(std::make_unique<UiListItem>(_("Bound key:"), static_cast<int>(SpecialMenuEntry::None), UiFlags::ColorWhitegold | UiFlags::ElementDisabled));
			vecDialogItems.push_back(std::make_unique<UiListItem>(selectedOption->GetValueDescription().data(), static_cast<int>(SpecialMenuEntry::None), UiFlags::ColorUiGold));
			assert(IndexKeyInput == vecDialogItems.size() - 1);
			itemToSelect = IndexKeyInput;
			eventHandler = [](SDL_Event &event) {
				if (SelectedItem != IndexKeyInput)
					return false;
				if (event.type != SDL_KEYDOWN)
					return false;
				int key = TranslateSdlKey(event.key.keysym);
				// Ignore unknown keys
				if (key == DVL_VK_INVALID || key == -1)
					return false;
				auto *pOptionKey = static_cast<KeymapperOptions::Action *>(selectedOption);
				if (!pOptionKey->SetValue(key))
					return false;
				vecDialogItems[IndexKeyInput]->m_text = selectedOption->GetValueDescription().data();
				return true;
			};
			vecDialogItems.push_back(std::make_unique<UiListItem>(_("Press any key to change."), static_cast<int>(SpecialMenuEntry::None), UiFlags::ColorUiSilver | UiFlags::ElementDisabled));
			vecDialogItems.push_back(std::make_unique<UiListItem>("", static_cast<int>(SpecialMenuEntry::None), UiFlags::ElementDisabled));
			vecDialogItems.push_back(std::make_unique<UiListItem>(_("Unbind key"), static_cast<int>(SpecialMenuEntry::UnbindKey), UiFlags::ColorUiGold));
			UpdateDescription(*selectedOption);
		} break;
		}

		vecDialogItems.push_back(std::make_unique<UiListItem>("", static_cast<int>(SpecialMenuEntry::None), UiFlags::ElementDisabled));
		vecDialogItems.push_back(std::make_unique<UiListItem>(_("Previous Menu"), static_cast<int>(SpecialMenuEntry::PreviousMenu), UiFlags::ColorUiGold));

		vecDialog.push_back(std::make_unique<UiList>(vecDialogItems, rectList.size.height / 26, rectList.position.x, rectList.position.y, rectList.size.width, 26, UiFlags::FontSize24 | UiFlags::AlignCenter));

		UiInitList(ItemFocused, ItemSelected, EscPressed, vecDialog, true, FullscreenChanged, nullptr, itemToSelect);

		while (!endMenu) {
			UiClearScreen();
			UiRenderItems(vecDialog);
			UiPollAndRender(eventHandler);
		}

		CleanUpSettingsUI();
	} while (!backToMain);
}

} // namespace devilution

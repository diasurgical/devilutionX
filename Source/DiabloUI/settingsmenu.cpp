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
bool recreateUI = false;

std::vector<std::unique_ptr<UiListItem>> vecDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecDialog;
std::vector<OptionEntryBase *> vecOptions;
OptionEntryBase *optionToPreselect = nullptr;

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

void ItemFocused(int value)
{
	auto &vecItem = vecDialogItems[value];
	optionDescription[0] = '\0';
	if (vecItem->m_value < 0) {
		return;
	}
	auto *pOption = vecOptions[vecItem->m_value];
	auto paragraphs = WordWrapString(pOption->GetDescription(), rectDescription.size.width, GameFont12, 1);
	strncpy(optionDescription, paragraphs.c_str(), sizeof(optionDescription));
}

void ItemSelected(int value)
{
	auto &vecItem = vecDialogItems[value];
	if (vecItem->m_value < 0) {
		auto specialMenuEntry = static_cast<SpecialMenuEntry>(vecItem->m_value);
		switch (specialMenuEntry) {
		case SpecialMenuEntry::None:
			break;
		case SpecialMenuEntry::PreviousMenu:
			endMenu = true;
			break;
		}
		return;
	}

	auto *pOption = vecOptions[vecItem->m_value];
	if (HasAnyOf(pOption->GetFlags(), OptionEntryFlags::RecreateUI)) {
		endMenu = true;
		recreateUI = true;
		optionToPreselect = pOption;
	}
	switch (pOption->GetType()) {
	case OptionEntryType::Boolean: {
		auto *pOptionBoolean = static_cast<OptionEntryBoolean *>(pOption);
		pOptionBoolean->SetValue(!**pOptionBoolean);
	} break;
	case OptionEntryType::List: {
		auto *pOptionList = static_cast<OptionEntryListBase *>(pOption);
		size_t nextIndex = pOptionList->GetActiveListIndex() + 1;
		if (nextIndex >= pOptionList->GetListSize())
			nextIndex = 0;
		pOptionList->SetActiveListIndex(nextIndex);
	} break;
	}
	vecDialogItems[value]->args.clear();
	for (auto &arg : CreateDrawStringFormatArgForEntry(pOption))
		vecDialogItems[value]->args.push_back(arg);
}

void EscPressed()
{
	endMenu = true;
}

} // namespace

void UiSettingsMenu()
{
	endMenu = false;

	do {
		LoadBackgroundArt("ui_art\\black.pcx");
		LoadScrollBar();
		UiAddBackground(&vecDialog);
		UiAddLogo(&vecDialog);

		Rectangle rectList = { { PANEL_LEFT + 50, (UI_OFFSET_Y + 204) }, { 540, 208 } };
		rectDescription = { { PANEL_LEFT + 24, rectList.position.y + rectList.size.height + 16 }, { 590, 35 } };

		optionDescription[0] = '\0';

		const char *titleText = _("Settings");
		vecDialog.push_back(std::make_unique<UiArtText>(titleText, MakeSdlRect(PANEL_LEFT, UI_OFFSET_Y + 161, PANEL_WIDTH, 35), UiFlags::FontSize30 | UiFlags::ColorUiSilver | UiFlags::AlignCenter, 8));
		vecDialog.push_back(std::make_unique<UiScrollbar>(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, MakeSdlRect(rectList.position.x + rectList.size.width + 5, rectList.position.y, 25, rectList.size.height)));
		vecDialog.push_back(std::make_unique<UiArtText>(optionDescription, MakeSdlRect(rectDescription), UiFlags::FontSize12 | UiFlags::ColorUiSilverDark | UiFlags::AlignCenter, 1, IsSmallFontTall() ? 22 : 18));

		size_t catCount = 0;
		size_t itemToSelect = 1;

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
				if (optionToPreselect == pEntry)
					itemToSelect = vecDialogItems.size();
				vecDialogItems.push_back(std::make_unique<UiListItem>("{}: {}", formatArgs, vecOptions.size(), UiFlags::ColorUiGold));
				vecOptions.push_back(pEntry);
			}
		}

		vecDialogItems.push_back(std::make_unique<UiListItem>("", -1, UiFlags::ElementDisabled));
		vecDialogItems.push_back(std::make_unique<UiListItem>(_("Previous Menu"), static_cast<int>(SpecialMenuEntry::PreviousMenu), UiFlags::ColorUiGold));

		vecDialog.push_back(std::make_unique<UiList>(vecDialogItems, rectList.size.height / 26, rectList.position.x, rectList.position.y, rectList.size.width, 26, UiFlags::FontSize24 | UiFlags::AlignCenter));

		UiInitList(ItemFocused, ItemSelected, EscPressed, vecDialog, true, nullptr, itemToSelect);
		optionToPreselect = nullptr;

		while (!endMenu) {
			UiClearScreen();
			UiRenderItems(vecDialog);
			UiPollAndRender();
		}

		vecDialogItems.clear();
		vecDialog.clear();
		vecOptions.clear();

		ArtBackground.Unload();
		ArtBackgroundWidescreen.Unload();
		UnloadScrollBar();

		if (recreateUI) {
			UiInitialize();
			FreeItemGFX();
			InitItemGFX();
			if (IsHardwareCursor())
				SetHardwareCursor(CursorInfo::UnknownCursor());
			recreateUI = false;
			endMenu = false;
		}
	} while (!endMenu);
}

} // namespace devilution

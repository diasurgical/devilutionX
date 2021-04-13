
#include "all.h"
#include "paths.h"

#include "document.h"
#include "filereadstream.h"
#include "filewritestream.h"
#include "prettywriter.h"

#include "DiabloUI/art_draw.h"

#include <cstdio>

#include <fstream>
#include <string>
#include <vector>

namespace devilution {
namespace {

constexpr size_t kSlotsWidth = 10; // number of slots horizontally
constexpr size_t kSlotsHeight = 9; // number of slots vertically
constexpr int kSlotsLeft = 15; // left pixel edge of slots
constexpr int kSlotsTop = 77; // top pixel edge of slots

constexpr int kStashWidth = 320;
constexpr int kStashHeight = 352;

constexpr int kInvalidSlot = -1;
constexpr int kInvalidIdx = -1;

constexpr int kStashSlotBase = -2;
#define STASH_SLOT_XFORM(slot) (kStashSlotBase - slot)

constexpr ItemStruct kNoItem{ 0, 0, item_type::ITYPE_NONE };

typedef long long gold_t;

enum StashUiId : unsigned char
{
	FIRST,
	PREVIOUS,
	DELETE,
	NAME,
	ADD,
	NEXT,
	LAST,
	GOLD,
	GOLD_TEXT,

	COUNT,
	MIN = FIRST,
	MAX = LAST
};

constexpr RECT32 kStashUiControls[] = {
	{15, 16, 20, 18}, // FIRST
	{35, 16, 20, 18}, // PREVIOUS
	{66, 16, 20, 18}, // DELETE
	{91, 16, 138, 18}, // NAME
	{234, 16, 20, 18}, // ADD
	{265, 16, 20, 18}, // NEXT
	{285, 16, 20, 18}, // LAST
	{58, 43, 171, 18}, // GOLD
	{91, 43, 138, 18}, // GOLD_TEXT
};

int GetTextWidth(const char* s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[(BYTE)*s++]]] + 1;
	}
	return l;
}

char* PrintWithSeparator(char* out, long long n)
{
	if (n < 1000)
	{
		sprintf(out, "%d", n);
		return out + strlen(out);
	}

	char* append = PrintWithSeparator(out, n / 1000);
	sprintf(append, ",%03d", n % 1000);
	return append + 4;
}

void JsonGet(const rapidjson::Value& value, const char* name, std::string& t)
{
	auto it = value.FindMember(name);
	if (it == value.MemberEnd())
		return;

	t = it->value.GetString();
}

void JsonGet(const rapidjson::Value& value, const char* name, bool& t)
{
	auto it = value.FindMember(name);
	if (it == value.MemberEnd())
		return;

	t = it->value.GetBool();
}

template<typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
void JsonGet(const rapidjson::Value& value, const char* name, T& t)
{
	auto it = value.FindMember(name);
	if (it == value.MemberEnd())
		return;

	t = static_cast<T>(it->value.GetInt());
}

template<typename T, typename std::enable_if_t<std::is_enum_v<T>, bool> = true>
void JsonGet(const rapidjson::Value& value, const char* name, T& t)
{
	auto it = value.FindMember(name);
	if (it == value.MemberEnd())
		return;

	t = static_cast<T>(it->value.GetInt());
}

int Clamp(int x, int min, int max)
{
	return std::min(std::max(x, min), max);
}

bool IsHoldingItem(void)
{
	return pcurs >= CURSOR_FIRSTITEM;
}

bool IsPlayerAttacking(const PlayerStruct& player)
{
	return (player._pmode >= PM_ATTACK);
}

void ExitGoldDrop(void)
{
	dropGoldFlag = false;
	dropGoldValue = 0;
}

bool IsInSlot(int px, int py, int slotx, int sloty, bool* leftOfCenter, bool* aboveCenter)
{
	int left = kSlotsLeft + slotx * INV_SLOT_SIZE_PX + slotx;
	int top = kSlotsTop + sloty * INV_SLOT_SIZE_PX + sloty;

	if (px >= left && px <= left + INV_SLOT_SIZE_PX &&
		py >= top && py <= top + INV_SLOT_SIZE_PX)
	{
		if (leftOfCenter)
			*leftOfCenter = (px - left - INV_SLOT_SIZE_PX / 2 < 0);

		if (aboveCenter)
			*aboveCenter = (py - top - INV_SLOT_SIZE_PX / 2 < 0);

		return true;
	}

	return false;
}

int GetSlotForPixel(int px, int py, bool* leftOfCenter = nullptr, bool* aboveCenter = nullptr)
{
	for (int x = 0; x < kSlotsWidth; ++x)
	{
		for (int y = 0; y < kSlotsHeight; ++y)
		{
			if (IsInSlot(px, py, x, y, leftOfCenter, aboveCenter))
				return y * kSlotsWidth + x;
		}
	}

	return kInvalidSlot;
}

int GetItemWidth(const ItemStruct& item)
{
	return InvItemWidth[item._iCurs + CURSOR_FIRSTITEM];
}

int GetItemSlotWidth(const ItemStruct& item)
{
	return GetItemWidth(item) / INV_SLOT_SIZE_PX;
}

int GetItemHeight(const ItemStruct& item)
{
	return InvItemHeight[item._iCurs + CURSOR_FIRSTITEM];
}

int GetItemSlotHeight(const ItemStruct& item)
{
	return GetItemHeight(item) / INV_SLOT_SIZE_PX;
}

bool ItemStatsMet(const PlayerStruct& player, const ItemStruct& item)
{
	return (player._pStrength >= item._iMinStr &&
			player._pMagic >= item._iMinMag &&
			player._pDexterity >= item._iMinDex);
}

bool ItemStatsMet(const ItemStruct& item)
{
	const PlayerStruct& player = plr[myplr];

	return ItemStatsMet(player, item);
}

void UpdateSpellbookRequirements(const PlayerStruct& player, ItemStruct& item)
{
	if (item._iMiscId != IMISC_BOOK)
		return;

	int magNeeded = spelldata[item._iSpell].sMinInt;
	int spellLevel = player._pSplLvl[item._iSpell];

	while (spellLevel-- > 0 && magNeeded <= 255)
	{
		magNeeded += 20 * magNeeded / 100;
	}

	item._iMinMag = std::min(magNeeded, 255);
}

bool SlotIntersectsItem(const ItemStruct& item, int slotx, int sloty)
{
	int left = item._ix;
	int top = item._iy;

	int isw = GetItemSlotWidth(item);
	int ish = GetItemSlotHeight(item);

	return (slotx >= left && slotx < left + isw &&
			sloty >= top && sloty < top + ish);
}

bool CheckItemOverlap(const ItemStruct& item, int left, int top, int width, int height)
{
	const int right = left + (width - 1);
	const int bottom = top + (height - 1);

	const int il = item._ix;
	const int it = item._iy;
	const int ir = il + GetItemSlotWidth(item) - 1;
	const int ib = it + GetItemSlotHeight(item) - 1;

	if (right < il || left > ir || bottom < it || top > ib)
		return false;

	return true;
}

void InvDrawSlot(CelOutputBuffer out, int X, int Y, int W, int H)
{
	BYTE* dst = out.at(X, Y);

	for (int y = 0; y < H; ++y)
	{
		*(dst + out.pitch() * y) = PAL8_BLUE;
	}

	for (int x = 0; x < W; ++x)
	{
		*(dst + out.pitch() * H) = PAL8_BLUE;
		*dst++ = PAL8_BLUE;
	}

	for (int y = 0; y < H; ++y)
	{
		*(dst + out.pitch() * y) = PAL8_BLUE;
	}
}

void InvDrawSlotBack(CelOutputBuffer out, int X, int Y, int W, int H)
{
	BYTE* dst;

	dst = out.at(X, Y);

	for (int y = Y; y < Y + H; ++y)
	{
		for (int x = X; x < X + W; ++x)
		{
			BYTE pix = *dst;
			if (pix >= PAL16_BLUE) {
				if (pix <= PAL16_BLUE + 15)
					pix -= PAL16_BLUE - PAL16_BEIGE;
				else if (pix >= PAL16_GRAY)
					pix -= PAL16_GRAY - PAL16_BEIGE;
			}
			*dst++ = pix;
		}
		dst += out.pitch() - W;
	}
}

void DrawItem(const ItemStruct& item, CelOutputBuffer& out)
{
	int frame = item._iCurs + CURSOR_FIRSTITEM;
	const int pxwidth = GetItemWidth(item);
	const int pxheight = GetItemHeight(item);

	BYTE* celData = pCursCels;
	if (frame > 179)
	{
		celData = pCursCels2;
		frame -= 179;
	}

	const int px = kSlotsLeft + item._ix * INV_SLOT_SIZE_PX + item._ix;
	const int py = kSlotsTop + item._iy * INV_SLOT_SIZE_PX + item._iy;

	const int sw = GetItemSlotWidth(item);
	const int sh = GetItemSlotHeight(item);

	const bool mouseHover = (MouseX >= px && MouseX < px + pxwidth + sw &&
							 MouseY >= py && MouseY < py + pxheight + sh);

	for (int sloty = 0; sloty < sh; ++sloty)
	{
		for (int slotx = 0; slotx < sw; ++slotx)
		{
			InvDrawSlotBack(out, px + slotx * INV_SLOT_SIZE_PX + slotx, py + sloty * INV_SLOT_SIZE_PX + sloty, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);
		}
	}

	const bool statsMet = ItemStatsMet(item);

	// Cels draw bottom to top, so add pxheight
	const int cely = py + pxheight;

	// Draw outline first
	if (mouseHover)
	{
		int color = ICOL_WHITE;

		if (item._iMagical != ITEM_QUALITY_NORMAL)
			color = ICOL_BLUE;

		if (!statsMet)
			color = ICOL_RED;

		CelBlitOutlineTo(out, color, px, cely, celData, frame, pxwidth, false);
	}

	if (statsMet)
		CelClippedDrawTo(out, px, cely, celData, frame, pxwidth);
	else
		CelDrawLightRedTo(out, px, cely, celData, frame, pxwidth, 1);
}

void PlayItemPickupSound(void)
{
	PlaySFX(IS_IGRAB);
}

void PlayItemDropSound(const ItemStruct& item)
{
	PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

using StashTabItems = std::vector<ItemStruct>;

class StashTab
{
public:

	void Draw(CelOutputBuffer& out);

	ItemStruct Drop(ItemStruct what, int slot, bool leftOfCenter, bool aboveCenter);
	ItemStruct Pick(int slot);

	ItemStruct* Highlight(int slot);
	ItemStruct* GetItemInSlot(int slot);	

	void UpdateSpellbookRequirements(const PlayerStruct& player);

	void Load(const rapidjson::Value& jtab);
	void Save(rapidjson::Value& jtab, rapidjson::Document& jdoc) const;

	bool IsEmpty(void) const { return items.empty(); }
	const std::string& GetName(void) const { return name; }
private:

	int GetIdxInSlot(int slot);

	std::string name;
	StashTabItems items;
};
using StashTabs = std::vector<StashTab>;

void StashTab::Draw(CelOutputBuffer& out)
{
	for (const auto& item : items)
	{
		DrawItem(item, out);
	}
}

ItemStruct StashTab::Drop(ItemStruct what, int slot, bool leftOfCenter, bool aboveCenter)
{
	int slotx = slot % kSlotsWidth;
	int sloty = slot / kSlotsWidth;

	int isw = GetItemSlotWidth(what);
	int ish = GetItemSlotHeight(what);

	// Adjust for item size
	slotx -= (isw - 1) / 2;
	sloty -= (ish - 1) / 2;

	// Snap to correct slot if off center
	slotx -= (leftOfCenter && isw % 2 == 0 ? 1 : 0);
	sloty -= (aboveCenter && ish % 2 == 0 ? 1 : 0);

	// Clamp to valid area
	slotx = Clamp(slotx, 0, kSlotsWidth - isw);
	sloty = Clamp(sloty, 0, kSlotsHeight - ish);

	// Find intersecting existing item(s).
	int foundIdx = -1;

	for (int i = 0; i < items.size(); ++i)
	{
		if (CheckItemOverlap(items[i], slotx, sloty, isw, ish))
		{
			if (foundIdx >= 0)
			{
				// We have already found an overlap. We can't pick up two items, so stop the drop.
				return what;
			}
			else
			{
				foundIdx = i;
			}
		}
	}

	ItemStruct swappedItem = kNoItem;
	if (foundIdx >= 0)
	{
		swappedItem = items[foundIdx];
		items.erase(items.begin() + foundIdx);
	}

	what._ix = slotx;
	what._iy = sloty;

	items.push_back(what);
	PlayItemDropSound(what);

	return swappedItem;
}

ItemStruct StashTab::Pick(int slot)
{
	int idx = GetIdxInSlot(slot);

	if (idx >= 0)
	{
		ItemStruct picked = items[idx];
		items.erase(items.begin() + idx);
		PlayItemPickupSound();

		return picked;
	}

	return kNoItem;
}

ItemStruct* StashTab::Highlight(int slot)
{
	return GetItemInSlot(slot);
}

ItemStruct* StashTab::GetItemInSlot(int slot)
{
	int idx = GetIdxInSlot(slot);

	if (idx >= 0)
		return &items[idx];

	return nullptr;
}

int StashTab::GetIdxInSlot(int slot)
{
	int slotx = slot % kSlotsWidth;
	int sloty = slot / kSlotsWidth;

	for (int i = 0; i < items.size(); ++i)
	{
		if (SlotIntersectsItem(items[i], slotx, sloty))
		{
			return i;
		}
	}

	return kInvalidIdx;
}

void StashTab::UpdateSpellbookRequirements(const PlayerStruct& player)
{
	for (auto& item : items)
	{
		devilution::UpdateSpellbookRequirements(player, item);
	}
}

void StashTab::Load(const rapidjson::Value& jtab)
{
	JsonGet(jtab, "name", name);

	auto& jitems = jtab["items"].GetArray();

	for (const auto& jitem : jitems)
	{
		items.push_back({});

		auto& item = items.back();

#define JSONREAD(field) JsonGet(jitem, #field, item. ##field)
		JSONREAD(_iSeed);
		JSONREAD(_iCreateInfo);
		JSONREAD(_itype);
		JSONREAD(_ix);
		JSONREAD(_iy);
		JSONREAD(_iAnimFlag);
		item._iAnimData = nullptr;
		JSONREAD(_iAnimLen);
		JSONREAD(_iAnimFrame);
		JSONREAD(_iAnimWidth);
		JSONREAD(_iAnimWidth2);
		JSONREAD(_iDelFlag);
		JSONREAD(_iSelFlag);
		JSONREAD(_iPostDraw);
		JSONREAD(_iIdentified);
		JSONREAD(_iMagical);
		std::strncpy(item._iName, jitem["_iName"].GetString(), sizeof(item._iName));
		std::strncpy(item._iIName, jitem["_iIName"].GetString(), sizeof(item._iIName));
		JSONREAD(_iLoc);
		JSONREAD(_iClass);
		JSONREAD(_iCurs);
		JSONREAD(_ivalue);
		JSONREAD(_iIvalue);
		JSONREAD(_iMinDam);
		JSONREAD(_iMaxDam);
		JSONREAD(_iAC);
		JSONREAD(_iFlags);
		JSONREAD(_iMiscId);
		JSONREAD(_iSpell);
		JSONREAD(_iCharges);
		JSONREAD(_iMaxCharges);
		JSONREAD(_iDurability);
		JSONREAD(_iMaxDur);
		JSONREAD(_iPLDam);
		JSONREAD(_iPLToHit);
		JSONREAD(_iPLAC);
		JSONREAD(_iPLStr);
		JSONREAD(_iPLMag);
		JSONREAD(_iPLDex);
		JSONREAD(_iPLVit);
		JSONREAD(_iPLFR);
		JSONREAD(_iPLLR);
		JSONREAD(_iPLMR);
		JSONREAD(_iPLMana);
		JSONREAD(_iPLHP);
		JSONREAD(_iPLDamMod);
		JSONREAD(_iPLGetHit);
		JSONREAD(_iPLLight);
		JSONREAD(_iSplLvlAdd);
		JSONREAD(_iRequest);
		JSONREAD(_iUid);
		JSONREAD(_iFMinDam);
		JSONREAD(_iFMaxDam);
		JSONREAD(_iLMinDam);
		JSONREAD(_iLMaxDam);
		JSONREAD(_iPLEnAc);
		JSONREAD(_iPrePower);
		JSONREAD(_iSufPower);
		JSONREAD(_iVAdd1);
		JSONREAD(_iVMult1);
		JSONREAD(_iVAdd2);
		JSONREAD(_iVMult2);
		JSONREAD(_iMinStr);
		JSONREAD(_iMinMag);
		JSONREAD(_iMinDex);
		JSONREAD(_iStatFlag);
		JSONREAD(IDidx);
		JSONREAD(dwBuff);
		JSONREAD(_iDamAcFlags);
#undef JSONREAD
	}
}

void StashTab::Save(rapidjson::Value& jtab, rapidjson::Document& jdoc) const
{
	jtab.AddMember("name", rapidjson::GenericStringRef<char>(name.c_str()), jdoc.GetAllocator());

	rapidjson::Value jitems(rapidjson::kArrayType);

	for (const auto& item : items)
	{
		rapidjson::Value jitem(rapidjson::kObjectType);
		
		// Serialize item.
#define JSONWRITE(field) \
	if (item. ##field != kNoItem. ##field) \
		jitem.AddMember(#field, item. ##field, jdoc.GetAllocator())

		JSONWRITE(_iSeed);
		JSONWRITE(_iCreateInfo);
		JSONWRITE(_itype);
		JSONWRITE(_ix);
		JSONWRITE(_iy);
		JSONWRITE(_iAnimFlag);
		// JSONWRITE(_iAnimData); This is skipped
		JSONWRITE(_iAnimLen);
		JSONWRITE(_iAnimFrame);
		JSONWRITE(_iAnimWidth);
		JSONWRITE(_iAnimWidth2);
		JSONWRITE(_iDelFlag);
		JSONWRITE(_iSelFlag);
		JSONWRITE(_iPostDraw);
		JSONWRITE(_iIdentified);
		JSONWRITE(_iMagical);
		jitem.AddMember("_iName", rapidjson::Value(item._iName, std::strlen(item._iName)), jdoc.GetAllocator());
		jitem.AddMember("_iIName", rapidjson::Value(item._iIName, std::strlen(item._iIName)), jdoc.GetAllocator());
		JSONWRITE(_iLoc);
		JSONWRITE(_iClass);
		JSONWRITE(_iCurs);
		JSONWRITE(_ivalue);
		JSONWRITE(_iIvalue);
		JSONWRITE(_iMinDam);
		JSONWRITE(_iMaxDam);
		JSONWRITE(_iAC);
		JSONWRITE(_iFlags);
		JSONWRITE(_iMiscId);
		JSONWRITE(_iSpell);
		JSONWRITE(_iCharges);
		JSONWRITE(_iMaxCharges);
		JSONWRITE(_iDurability);
		JSONWRITE(_iMaxDur);
		JSONWRITE(_iPLDam);
		JSONWRITE(_iPLToHit);
		JSONWRITE(_iPLAC);
		JSONWRITE(_iPLStr);
		JSONWRITE(_iPLMag);
		JSONWRITE(_iPLDex);
		JSONWRITE(_iPLVit);
		JSONWRITE(_iPLFR);
		JSONWRITE(_iPLLR);
		JSONWRITE(_iPLMR);
		JSONWRITE(_iPLMana);
		JSONWRITE(_iPLHP);
		JSONWRITE(_iPLDamMod);
		JSONWRITE(_iPLGetHit);
		JSONWRITE(_iPLLight);
		JSONWRITE(_iSplLvlAdd);
		JSONWRITE(_iRequest);
		JSONWRITE(_iUid);
		JSONWRITE(_iFMinDam);
		JSONWRITE(_iFMaxDam);
		JSONWRITE(_iLMinDam);
		JSONWRITE(_iLMaxDam);
		JSONWRITE(_iPLEnAc);
		JSONWRITE(_iPrePower);
		JSONWRITE(_iSufPower);
		JSONWRITE(_iVAdd1);
		JSONWRITE(_iVMult1);
		JSONWRITE(_iVAdd2);
		JSONWRITE(_iVMult2);
		JSONWRITE(_iMinStr);
		JSONWRITE(_iMinMag);
		JSONWRITE(_iMinDex);
		JSONWRITE(_iStatFlag);
		JSONWRITE(IDidx);
		JSONWRITE(dwBuff);
		JSONWRITE(_iDamAcFlags);
#undef JSONWRITE

		jitems.PushBack(jitem, jdoc.GetAllocator());
	}

	jtab.AddMember("items", jitems, jdoc.GetAllocator());
}

///////////////////////////////////////////////////////////////////////////////

class Stash
{
public:

	Stash(void)
		: tabs(1)
	{
	}

	void Show(bool show);
	bool IsVisible(void) const;

	void Draw(CelOutputBuffer& out);

	void Click(PlayerStruct& player, int mouseX, int mouseY, bool shiftDown);
	void Pick(PlayerStruct& player, int mouseX, int mouseY, bool shiftDown);
	void Drop(PlayerStruct& player, int mouseX, int mouseY, bool shiftDown);

	void PickupGold(PlayerStruct& player, gold_t amount);
	void DropGold(PlayerStruct& player, gold_t amount);

	int Highlight(int mouseX, int mouseY);

	ItemStruct* GetItemInSlot(int slot);

	StashTab& ActiveTab(void) { return tabs[activeTab]; }

	void UiClick(StashUiId clicked, PlayerStruct& player);
	void UiType(char vkey);
	bool IsRenameVisible(void) const { return isRenameVisible; }

	void UpdateSpellbookRequirements(const PlayerStruct& player);
	
	void Load(void);
	void Save(void);

private:
	StashTabs tabs;
	size_t activeTab = 0;

	gold_t gold = 0;

	bool isVisible = false;
	bool isRenameVisible = false;

	std::unique_ptr<Art> stashback;
} stash;

void Stash::Show(bool show)
{
	isVisible = show;
}

bool Stash::IsVisible(void) const
{
	// return isVisible;
	return invflag;
}

void Stash::Draw(CelOutputBuffer& out)
{
	DrawArt(out, 0, 0, stashback.get());

	{ // Draw stash name
		const auto& nameBox = kStashUiControls[StashUiId::NAME];

		std::string tabName = ActiveTab().GetName() + " (" + std::to_string(activeTab + 1) + "/" + std::to_string(tabs.size()) + ")";
		int textWidth = GetTextWidth(tabName.c_str());

		PrintGameStr(out, nameBox.x + (nameBox.w - textWidth) / 2, nameBox.y + nameBox.h - 4, tabName.c_str(), text_color::COL_WHITE);
	}

	{ // Draw gold amount
		const auto& goldTextBox = kStashUiControls[StashUiId::GOLD_TEXT];

		char buffer[64];
		PrintWithSeparator(buffer, gold);
		int textWidth = GetTextWidth(buffer);

		PrintGameStr(out, goldTextBox.x + (goldTextBox.w - textWidth) / 2, goldTextBox.y + goldTextBox.h - 4, buffer, text_color::COL_GOLD);
	}

#if DRAW_DEBUG_GRID
	for (int y = 0; y < kSlotsHeight; ++y)
	{
		for (int x = 0; x < kSlotsWidth; ++x)
		{
			int left = kSlotsLeft + INV_SLOT_SIZE_PX * x + x;
			int top = kSlotsTop + INV_SLOT_SIZE_PX * y + y;

			InvDrawSlot(out, left, top, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);
		}
	}
#endif

	ActiveTab().Draw(out);
}

void Stash::Click(PlayerStruct& player, int mouseX, int mouseY, bool shiftDown)
{
	// If we're holding gold, clicking anywhere in the stash drops it.
	if (IsHoldingItem() && player.HoldItem._itype == ITYPE_GOLD)
	{
		DropGold(player, player.HoldItem._ivalue);
		return;
	}

	// Check stash controls first.
	for (int idx = StashUiId::MIN; idx < StashUiId::COUNT; ++idx)
	{
		const auto& rect = kStashUiControls[idx];
		if (mouseX >= rect.x && mouseX < rect.x + rect.w &&
			mouseY >= rect.y && mouseY < rect.y + rect.h)
		{
			UiClick(static_cast<StashUiId>(idx), player);
			return;
		}
	}

	if (IsHoldingItem())
	{
		Drop(player, mouseX, mouseY, shiftDown);
	}
	else
	{
		Pick(player, mouseX, mouseY, shiftDown);
	}
}

void Stash::UiClick(StashUiId clicked, PlayerStruct& player)
{
	switch (clicked)
	{
		case StashUiId::FIRST:
			activeTab = 0;
			break;
		case StashUiId::PREVIOUS:
			if (activeTab == 0)
				activeTab = tabs.size() - 1;
			else
				--activeTab;
			break;
		case StashUiId::DELETE:
			if (tabs.size() > 1 && ActiveTab().IsEmpty())
			{
				tabs.erase(tabs.begin() + activeTab);
				if (activeTab >= tabs.size())
					activeTab = tabs.size() - 1;
			}
			break;
		case StashUiId::ADD:
			tabs.insert(tabs.begin() + activeTab + 1, StashTab{});
			++activeTab;
			break;
		case StashUiId::NEXT:
			++activeTab;
			if (activeTab >= tabs.size())
				activeTab = 0;
			break;
		case StashUiId::LAST:
			activeTab = tabs.size() - 1;
			break;
		case StashUiId::GOLD:
			PickupGold(player, 5000);
			break;

	}
}

void Stash::Pick(PlayerStruct& player, int mouseX, int mouseY, bool shiftDown)
{
	if (IsPlayerAttacking(player))
		return;

	ExitGoldDrop();

	int slot = GetSlotForPixel(mouseX, mouseY);
	if (slot == kInvalidSlot)
		return;

	const ItemStruct& picked = ActiveTab().Pick(slot);
	if (picked.isEmpty())
	{
		NewCursor(CURSOR_HAND);
		player.HoldItem = kNoItem;
	}
	else
	{
		NewCursor(picked._iCurs + CURSOR_FIRSTITEM);
		SetCursorPos(mouseX - GetItemWidth(picked) / 2, mouseY - GetItemHeight(picked) / 2);
		player.HoldItem = picked;
		player.HoldItem._iStatFlag = ItemStatsMet(player, picked);
	}
}

void Stash::Drop(PlayerStruct& player, int mouseX, int mouseY, bool shiftDown)
{
	const ItemStruct& item = player.HoldItem;

	const int iw = GetItemWidth(item);
	const int ih = GetItemHeight(item);

	int itemCursorX = mouseX + iw / 2;
	int itemCursorY = mouseY + ih / 2;

	bool leftOfCenter = false;
	bool aboveCenter = false;
	int slot = GetSlotForPixel(itemCursorX, itemCursorY, &leftOfCenter, &aboveCenter);
	if (slot == kInvalidSlot)
		return;

	const ItemStruct& swapped = ActiveTab().Drop(item, slot, leftOfCenter, aboveCenter);
	if (swapped.isEmpty())
	{
		NewCursor(CURSOR_HAND);
		SetCursorPos(mouseX + iw / 2, mouseY + ih / 2);
		player.HoldItem = kNoItem;
	}
	else
	{
		NewCursor(swapped._iCurs + CURSOR_FIRSTITEM);
		player.HoldItem = swapped;
		player.HoldItem._iStatFlag = ItemStatsMet(player, swapped);
	}
}

void Stash::PickupGold(PlayerStruct& player, gold_t amount)
{
	gold_t pickedGold = std::min(amount, gold);

	auto& hi = player.HoldItem;
	hi = kNoItem;

	SetPlrHandItem(&hi, IDI_GOLD);
	GetGoldSeed(myplr, &hi);
	hi._ivalue = pickedGold;
	hi._iStatFlag = true;
	SetPlrHandGoldCurs(&hi);
	NewCursor(hi._iCurs + CURSOR_FIRSTITEM);

	PlaySFX(IS_IGRAB);

	gold -= pickedGold;
}

void Stash::DropGold(PlayerStruct& player, gold_t amount)
{
	gold += amount;

	player.HoldItem = kNoItem;
	NewCursor(CURSOR_HAND);

	PlaySFX(IS_GOLD);
}

int Stash::Highlight(int mouseX, int mouseY)
{
	const int slot = GetSlotForPixel(mouseX, mouseY);
	if (slot == kInvalidSlot)
		return kInvalidSlot;

	ItemStruct* item = ActiveTab().Highlight(slot);

	if (!item)
		return kInvalidSlot;

	ClearPanel();

	if (item->_itype == ITYPE_GOLD)
	{
		sprintf(infostr, "%i gold %s", item->_ivalue, get_pieces_str(item->_ivalue));
	}
	else
	{
		if (item->_iMagical == ITEM_QUALITY_MAGIC)
		{
			infoclr = COL_BLUE;
		}
		else if (item->_iMagical == ITEM_QUALITY_UNIQUE)
		{
			infoclr = COL_GOLD;
		}
		strcpy(infostr, item->_iName);
		if (item->_iIdentified)
		{
			strcpy(infostr, item->_iIName);
			PrintItemDetails(item);
		}
		else
		{
			PrintItemDur(item);
		}
	}

	return STASH_SLOT_XFORM(slot);
}

ItemStruct* Stash::GetItemInSlot(int slot)
{
	if (slot == kInvalidSlot)
		return nullptr;

	return ActiveTab().GetItemInSlot(slot);
}

void Stash::UpdateSpellbookRequirements(const PlayerStruct& player)
{
	for (auto& tab : tabs)
	{
		tab.UpdateSpellbookRequirements(player);
	}
}

void Stash::Load(void)
{
	// Also load art here
	stashback = std::make_unique<Art>();
	LoadArt("data\\stashback.pcx", stashback.get());

	std::string stashPath = GetPrefPath() + "stash.json";

	std::FILE* stashFile = std::fopen(stashPath.c_str(), "r");

	constexpr size_t kBufferSize = 1024;
	char buffer[kBufferSize];
	rapidjson::FileReadStream istream(stashFile, buffer, kBufferSize);

	rapidjson::Document jdoc;

	jdoc.ParseStream(istream);

	auto& jroot = jdoc.GetObject();

	// Get global stash values.
	JsonGet(jroot, "gold", gold);

	// Read tabs.
	const auto& jtabs = jroot["tabs"].GetArray();

	tabs.clear();
	tabs.reserve(jtabs.Size());

	for (const auto& jtab : jtabs)
	{
		tabs.push_back({});

		tabs.back().Load(jtab);
	}

	std::fclose(stashFile);

	if (tabs.empty())
		tabs.push_back({});
}

void Stash::Save(void)
{
	std::string stashPath = GetPrefPath() + "stash.json";

	std::FILE* stashFile = std::fopen(stashPath.c_str(), "w");

	constexpr size_t kBufferSize = 1024;
	char buffer[kBufferSize];
	rapidjson::FileWriteStream ostream(stashFile, buffer, kBufferSize);

	rapidjson::Document jdoc;

	auto& jroot = jdoc.SetObject();

	// Write all stash global info.
	jroot.AddMember("gold", gold, jdoc.GetAllocator());

	// Write tabs.
	rapidjson::Value jtabs(rapidjson::kArrayType);

	for (int i = 0; i < tabs.size(); ++i)
	{
		rapidjson::Value jtab(rapidjson::kObjectType);

		tabs[i].Save(jtab, jdoc);

		jtabs.PushBack(jtab, jdoc.GetAllocator());
	}

	jroot.AddMember("tabs", jtabs, jdoc.GetAllocator());

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> jwriter(ostream);
	jdoc.Accept(jwriter);
	
	std::fclose(stashFile);
}

///////////////////////////////////////////////////////////////////////////////

void ShowStash(bool show)
{
	if (show)
		stash.UpdateSpellbookRequirements(plr[myplr]);

	stash.Show(show);
}

bool IsStashVisible(void)
{
	return stash.IsVisible();
}

bool CheckStashHit(int mouseX, int mouseY)
{
	return stash.IsVisible() && (mouseX < kStashWidth) && (mouseY < kStashHeight);
}

void StashClick(int mouseX, int mouseY, bool shiftDown)
{
	stash.Click(plr[myplr], mouseX, mouseY, shiftDown);
}

int CheckStashHighlight(int mouseX, int mouseY)
{
	return stash.Highlight(mouseX, mouseY);
}

void DrawStash(CelOutputBuffer out)
{
	if (!stash.IsVisible())
		return;

	stash.Draw(out);
}

ItemStruct* StashCheckItem(int pnum, int slot)
{
	if (!stash.IsVisible())
		return nullptr;

	if (slot > kStashSlotBase)
		return nullptr;

	return stash.GetItemInSlot(STASH_SLOT_XFORM(slot));
}

bool IsStashTakingKeyInput(void)
{
	return IsStashVisible() && stash.IsRenameVisible();
}

void StashHandleKeyInput(char vkey)
{

}

void StashUpdateSpellbookRequirements(int pnum)
{
	stash.UpdateSpellbookRequirements(plr[pnum]);
}

void LoadStash(void)
{
	stash.Load();
}

void SaveStash(void)
{
	stash.Save();
}

} // namespace devilution

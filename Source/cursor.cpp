/**
 * @file cursor.cpp
 *
 * Implementation of cursor tracking functionality.
 */
#include "cursor.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/plrctrls.h"
#include "doom.h"
#include "engine.h"
#include "engine/backbuffer_state.hpp"
#include "engine/demomode.h"
#include "engine/point.hpp"
#include "engine/points_in_rectangle_range.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/trn.hpp"
#include "hwcursor.hpp"
#include "inv.h"
#include "levels/trigs.h"
#include "missiles.h"
#include "options.h"
#include "qol/itemlabels.h"
#include "qol/stash.h"
#include "towners.h"
#include "track.h"
#include "utils/attributes.h"
#include "utils/language.h"
#include "utils/sdl_bilinear_scale.hpp"
#include "utils/surface_to_clx.hpp"
#include "utils/utf8.hpp"

#ifdef UNPACKED_MPQS
#include "engine/load_clx.hpp"
#else
#include "engine/load_cel.hpp"
#include "engine/load_file.hpp"
#include "utils/parse_int.hpp"
#endif

namespace devilution {
namespace {
/** Cursor images CEL */
OptionalOwnedClxSpriteList pCursCels;
OptionalOwnedClxSpriteList pCursCels2;

OptionalOwnedClxSpriteList *HalfSizeItemSprites;
OptionalOwnedClxSpriteList *HalfSizeItemSpritesRed;

bool IsValidMonsterForSelection(const Monster &monster)
{
	if (monster.hitPoints >> 6 <= 0)
		return false;
	if ((monster.flags & MFLAG_HIDDEN) != 0)
		return false;
	if (monster.isPlayerMinion())
		return false;
	return true;
}

bool TrySelectMonster(bool flipflag, Point tile, tl::function_ref<bool(const Monster &)> isValidMonster)
{
	auto checkPosition = [&](int8_t selectionType, Displacement displacement) {
		Point posToCheck = tile + displacement;
		if (!InDungeonBounds(posToCheck) || dMonster[posToCheck.x][posToCheck.y] == 0)
			return;
		const uint16_t monsterId = std::abs(dMonster[posToCheck.x][posToCheck.y]) - 1;
		const Monster &monster = Monsters[monsterId];
		if (IsTileLit(posToCheck) && (monster.data().selectionType & selectionType) != 0 && isValidMonster(monster)) {
			cursPosition = posToCheck;
			pcursmonst = monsterId;
		}
	};

	if (!flipflag)
		checkPosition(4, { 2, 1 });
	if (flipflag)
		checkPosition(4, { 1, 2 });
	checkPosition(4, { 2, 2 });
	if (!flipflag)
		checkPosition(2, { 1, 0 });
	if (flipflag)
		checkPosition(2, { 0, 1 });
	checkPosition(1, { 0, 0 });
	checkPosition(2, { 1, 1 });
	return pcursmonst != -1;
}

bool TrySelectTowner(bool flipflag, Point tile)
{
	auto checkPosition = [&](Displacement displacement) {
		Point posToCheck = tile + displacement;
		if (!InDungeonBounds(posToCheck) || dMonster[posToCheck.x][posToCheck.y] == 0)
			return;
		const uint16_t monsterId = std::abs(dMonster[posToCheck.x][posToCheck.y]) - 1;
		cursPosition = posToCheck;
		pcursmonst = monsterId;
	};
	if (!flipflag)
		checkPosition({ 1, 0 });
	if (flipflag)
		checkPosition({ 0, 1 });
	checkPosition({ 0, 0 });
	checkPosition({ 1, 1 });
	return pcursmonst != -1;
}

bool TrySelectPlayer(bool flipflag, int mx, int my)
{
	if (!flipflag && mx + 1 < MAXDUNX && dPlayer[mx + 1][my] != 0) {
		const uint8_t playerId = std::abs(dPlayer[mx + 1][my]) - 1;
		Player &player = Players[playerId];
		if (&player != MyPlayer && player._pHitPoints != 0) {
			cursPosition = Point { mx, my } + Displacement { 1, 0 };
			PlayerUnderCursor = &player;
		}
	}
	if (flipflag && my + 1 < MAXDUNY && dPlayer[mx][my + 1] != 0) {
		const uint8_t playerId = std::abs(dPlayer[mx][my + 1]) - 1;
		Player &player = Players[playerId];
		if (&player != MyPlayer && player._pHitPoints != 0) {
			cursPosition = Point { mx, my } + Displacement { 0, 1 };
			PlayerUnderCursor = &player;
		}
	}
	if (dPlayer[mx][my] != 0) {
		const uint8_t playerId = std::abs(dPlayer[mx][my]) - 1;
		Player &player = Players[playerId];
		if (&player != MyPlayer) {
			cursPosition = { mx, my };
			PlayerUnderCursor = &player;
		}
	}
	if (TileContainsDeadPlayer({ mx, my })) {
		for (const Player &player : Players) {
			if (player.position.tile == Point { mx, my } && &player != MyPlayer) {
				cursPosition = { mx, my };
				PlayerUnderCursor = &player;
			}
		}
	}
	if (pcurs == CURSOR_RESURRECT) {
		for (int xx = -1; xx < 2; xx++) {
			for (int yy = -1; yy < 2; yy++) {
				if (TileContainsDeadPlayer({ mx + xx, my + yy })) {
					for (const Player &player : Players) {
						if (player.position.tile.x == mx + xx && player.position.tile.y == my + yy && &player != MyPlayer) {
							cursPosition = Point { mx, my } + Displacement { xx, yy };
							PlayerUnderCursor = &player;
						}
					}
				}
			}
		}
	}
	if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dPlayer[mx + 1][my + 1] != 0) {
		const uint8_t playerId = std::abs(dPlayer[mx + 1][my + 1]) - 1;
		const Player &player = Players[playerId];
		if (&player != MyPlayer && player._pHitPoints != 0) {
			cursPosition = Point { mx, my } + Displacement { 1, 1 };
			PlayerUnderCursor = &player;
		}
	}

	return PlayerUnderCursor != nullptr;
}

bool TrySelectObject(bool flipflag, Point tile)
{
	// No monsters or players under the cursor, try find an object starting with the tile below the current tile (tall
	//  objects like doors)
	Point testPosition = tile + Direction::South;
	Object *object = FindObjectAtPosition(testPosition);

	if (object == nullptr || object->_oSelFlag < 2) {
		// Either no object or can't interact from the test position, try the current tile
		testPosition = tile;
		object = FindObjectAtPosition(testPosition);

		if (object == nullptr || IsNoneOf(object->_oSelFlag, 1, 3)) {
			// Still no object (that could be activated from this position), try the tile to the bottom left or right
			//  (whichever is closest to the cursor as determined when we set flipflag earlier)
			testPosition = tile + (flipflag ? Direction::SouthWest : Direction::SouthEast);
			object = FindObjectAtPosition(testPosition);

			if (object != nullptr && object->_oSelFlag < 2) {
				// Found an object but it's not in range, clear the pointer
				object = nullptr;
			}
		}
	}
	if (object == nullptr)
		return false;

	// found object that can be activated with the given cursor position
	cursPosition = testPosition;
	ObjectUnderCursor = object;
	return true;
}

bool TrySelectItem(bool flipflag, int mx, int my)
{
	if (!flipflag && mx + 1 < MAXDUNX && dItem[mx + 1][my] > 0) {
		const uint8_t itemId = dItem[mx + 1][my] - 1;
		if (Items[itemId]._iSelFlag >= 2) {
			cursPosition = Point { mx, my } + Displacement { 1, 0 };
			pcursitem = static_cast<int8_t>(itemId);
		}
	}
	if (flipflag && my + 1 < MAXDUNY && dItem[mx][my + 1] > 0) {
		const uint8_t itemId = dItem[mx][my + 1] - 1;
		if (Items[itemId]._iSelFlag >= 2) {
			cursPosition = Point { mx, my } + Displacement { 0, 1 };
			pcursitem = static_cast<int8_t>(itemId);
		}
	}
	if (dItem[mx][my] > 0) {
		const uint8_t itemId = dItem[mx][my] - 1;
		if (Items[itemId]._iSelFlag == 1 || Items[itemId]._iSelFlag == 3) {
			cursPosition = { mx, my };
			pcursitem = static_cast<int8_t>(itemId);
		}
	}
	if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dItem[mx + 1][my + 1] > 0) {
		const uint8_t itemId = dItem[mx + 1][my + 1] - 1;
		if (Items[itemId]._iSelFlag >= 2) {
			cursPosition = Point { mx, my } + Displacement { 1, 1 };
			pcursitem = static_cast<int8_t>(itemId);
		}
	}
	return pcursitem != -1;
}

bool TrySelectPixelBased(Point tile)
{
	if (demo::IsRunning() || demo::IsRecording() || HeadlessMode) {
		// Recorded demos can run headless, but headless mode doesn't support loading sprites that are needed for pixel perfect selection
		// => Ensure demos are always compatible
		// => Never use sprites for selection when handling demos
		return false;
	}

	auto checkSprite = [](Point renderingTile, const ClxSprite sprite, Displacement renderingOffset) {
		const Point renderPosition = GetScreenPosition(renderingTile) + renderingOffset;
		Point spriteTopLeft = renderPosition - Displacement { 0, sprite.height() };
		Size spriteSize = { sprite.width(), sprite.height() };
		if (*sgOptions.Graphics.zoom) {
			spriteSize *= 2;
			spriteTopLeft *= 2;
		}
		const Rectangle spriteCoords = Rectangle(spriteTopLeft, spriteSize);
		if (!spriteCoords.contains(MousePosition))
			return false;
		Point pointInSprite = Point { 0, 0 } + (MousePosition - spriteCoords.position);
		if (*sgOptions.Graphics.zoom)
			pointInSprite /= 2;
		return IsPointWithinClx(pointInSprite, sprite);
	};

	auto convertFromRenderingToWorldTile = [](Point renderingPoint) {
		// Columns
		Displacement ret = Displacement(Direction::East) * renderingPoint.x;
		// Rows
		ret += Displacement(Direction::South) * renderingPoint.y / 2;
		if (renderingPoint.y & 1)
			ret.deltaY += 1;
		return ret;
	};

	// Try to find the selected entity from rendered pixels.
	// We search the rendered rows/columns backwards, because the last rendered tile overrides previous rendered pixels.
	auto searchArea = PointsInRectangle(Rectangle { { -1, -1 }, { 3, 8 } });
	for (auto it = searchArea.rbegin(); it != searchArea.rend(); ++it) {
		Point renderingColumnRaw = *it;
		Point adjacentTile = tile + convertFromRenderingToWorldTile(renderingColumnRaw);
		if (!InDungeonBounds(adjacentTile))
			continue;

		int monsterId = dMonster[adjacentTile.x][adjacentTile.y];
		// Never select a monster if a target-player-only spell is selected
		if (monsterId != 0 && IsNoneOf(pcurs, CURSOR_HEALOTHER, CURSOR_RESURRECT)) {
			monsterId = std::abs(monsterId) - 1;
			if (leveltype == DTYPE_TOWN) {
				const Towner &towner = Towners[monsterId];
				const ClxSprite sprite = towner.currentSprite();
				Displacement renderingOffset = towner.getRenderingOffset();
				if (checkSprite(adjacentTile, sprite, renderingOffset)) {
					cursPosition = adjacentTile;
					pcursmonst = monsterId;
					return true;
				}
			} else {
				const Monster &monster = Monsters[monsterId];
				if (IsTileLit(adjacentTile) && IsValidMonsterForSelection(monster)) {
					const ClxSprite sprite = monster.animInfo.currentSprite();
					Displacement renderingOffset = monster.getRenderingOffset(sprite);
					if (checkSprite(adjacentTile, sprite, renderingOffset)) {
						cursPosition = adjacentTile;
						pcursmonst = monsterId;
						return true;
					}
				}
			}
		}

		const int8_t dPlayerValue = dPlayer[adjacentTile.x][adjacentTile.y];
		if (dPlayerValue != 0) {
			const uint8_t playerId = std::abs(dPlayerValue) - 1;
			if (playerId != MyPlayerId) {
				const Player &player = Players[playerId];
				const ClxSprite sprite = player.currentSprite();
				Displacement renderingOffset = player.getRenderingOffset(sprite);
				if (checkSprite(adjacentTile, sprite, renderingOffset)) {
					cursPosition = adjacentTile;
					PlayerUnderCursor = &player;
					return true;
				}
			}
		}
		if (TileContainsDeadPlayer(adjacentTile)) {
			for (const Player &player : Players) {
				if (player.position.tile == adjacentTile && &player != MyPlayer) {
					const ClxSprite sprite = player.currentSprite();
					Displacement renderingOffset = player.getRenderingOffset(sprite);
					if (checkSprite(adjacentTile, sprite, renderingOffset)) {
						cursPosition = adjacentTile;
						PlayerUnderCursor = &player;
						return true;
					}
				}
			}
		}

		Object *object = FindObjectAtPosition(adjacentTile);
		if (object != nullptr && object->_oSelFlag != 0) {
			const ClxSprite sprite = object->currentSprite();
			Displacement renderingOffset = object->getRenderingOffset(sprite, adjacentTile);
			if (checkSprite(adjacentTile, sprite, renderingOffset)) {
				cursPosition = adjacentTile;
				ObjectUnderCursor = object;
				return true;
			}
		}

		uint8_t itemId = dItem[adjacentTile.x][adjacentTile.y];
		if (itemId != 0) {
			itemId = itemId - 1;
			const Item &item = Items[itemId];
			const ClxSprite sprite = item.AnimInfo.currentSprite();
			Displacement renderingOffset = item.getRenderingOffset(sprite);
			if (checkSprite(adjacentTile, sprite, renderingOffset)) {
				cursPosition = adjacentTile;
				pcursitem = static_cast<int8_t>(itemId);
				return true;
			}
		}
	}

	return false;
}

#ifndef UNPACKED_MPQS
std::vector<uint16_t> ReadWidths(const char *path)
{
	size_t len;
	const std::unique_ptr<char[]> data = LoadFileInMem<char>(path, &len);
	std::string_view str { data.get(), len };
	std::vector<uint16_t> result;
	while (!str.empty()) {
		const char *end;
		const ParseIntResult<uint16_t> parseResult = ParseInt<uint16_t>(str, std::numeric_limits<uint16_t>::min(),
		    std::numeric_limits<uint16_t>::max(), &end);
		if (!parseResult.has_value()) {
			app_fatal(StrCat("Failed to parse ", path, ": [", str, "]"));
		}
		result.push_back(parseResult.value());
		str.remove_prefix(end - str.data());
		while (!str.empty() && (str[0] == '\r' || str[0] == '\n')) {
			str.remove_prefix(1);
		}
	}
	return result;
}
#endif

} // namespace

/** Current highlighted monster */
int pcursmonst = -1;

/** inv_item value */
int8_t pcursinvitem;
/** StashItem value */
uint16_t pcursstashitem;
/** Current highlighted item */
int8_t pcursitem;
/** Current highlighted object */
Object *ObjectUnderCursor;
/** Current highlighted player */
const Player *PlayerUnderCursor;
/** Current highlighted tile position */
Point cursPosition;
/** Previously highlighted monster */
int pcurstemp;
/** Index of current cursor image */
int pcurs;

void InitCursor()
{
	assert(!pCursCels);
#ifdef UNPACKED_MPQS
	pCursCels = LoadClx("data\\inv\\objcurs.clx");
	if (gbIsHellfire) {
		pCursCels2 = LoadClx("data\\inv\\objcurs2.clx");
	}
#else
	pCursCels = LoadCel("data\\inv\\objcurs", ReadWidths("data\\inv\\objcurs-widths.txt").data());
	if (gbIsHellfire) {
		pCursCels2 = LoadCel("data\\inv\\objcurs2", ReadWidths("data\\inv\\objcurs2-widths.txt").data());
	}
#endif
	ClearCursor();
}

void FreeCursor()
{
	pCursCels = std::nullopt;
	pCursCels2 = std::nullopt;
	ClearCursor();
}

ClxSprite GetInvItemSprite(int cursId)
{
	assert(cursId > 0);
	const size_t numSprites = pCursCels->numSprites();
	if (static_cast<size_t>(cursId) <= numSprites) {
		return (*pCursCels)[cursId - 1];
	}
	assert(gbIsHellfire);
	assert(cursId - numSprites <= pCursCels2->numSprites());
	return (*pCursCels2)[cursId - numSprites - 1];
}

Size GetInvItemSize(int cursId)
{
	const ClxSprite sprite = GetInvItemSprite(cursId);
	return { sprite.width(), sprite.height() };
}

ClxSprite GetHalfSizeItemSprite(int cursId)
{
	return (*HalfSizeItemSprites[cursId])[0];
}

ClxSprite GetHalfSizeItemSpriteRed(int cursId)
{
	return (*HalfSizeItemSpritesRed[cursId])[0];
}

void CreateHalfSizeItemSprites()
{
	if (HalfSizeItemSprites != nullptr)
		return;
	const int numInvItems = pCursCels->numSprites() - (static_cast<size_t>(CURSOR_FIRSTITEM) - 1)
	    + (gbIsHellfire ? pCursCels2->numSprites() : 0);
	HalfSizeItemSprites = new OptionalOwnedClxSpriteList[numInvItems];
	HalfSizeItemSpritesRed = new OptionalOwnedClxSpriteList[numInvItems];
	const uint8_t *redTrn = GetInfravisionTRN();

	constexpr int MaxWidth = 28 * 3;
	constexpr int MaxHeight = 28 * 3;
	OwnedSurface ownedItemSurface { MaxWidth, MaxHeight };
	OwnedSurface ownedHalfSurface { MaxWidth / 2, MaxHeight / 2 };

	const auto createHalfSize = [&, redTrn](const ClxSprite itemSprite, size_t outputIndex) {
		if (itemSprite.width() <= 28 && itemSprite.height() <= 28) {
			// Skip creating half-size sprites for 1x1 items because we always render them at full size anyway.
			return;
		}
		const Surface itemSurface = ownedItemSurface.subregion(0, 0, itemSprite.width(), itemSprite.height());
		SDL_Rect itemSurfaceRect = MakeSdlRect(0, 0, itemSurface.w(), itemSurface.h());
		SDL_SetClipRect(itemSurface.surface, &itemSurfaceRect);
		SDL_FillRect(itemSurface.surface, nullptr, 1);
		ClxDraw(itemSurface, { 0, itemSurface.h() }, itemSprite);

		const Surface halfSurface = ownedHalfSurface.subregion(0, 0, itemSurface.w() / 2, itemSurface.h() / 2);
		SDL_Rect halfSurfaceRect = MakeSdlRect(0, 0, halfSurface.w(), halfSurface.h());
		SDL_SetClipRect(halfSurface.surface, &halfSurfaceRect);
		BilinearDownscaleByHalf8(itemSurface.surface, paletteTransparencyLookup, halfSurface.surface, 1);
		HalfSizeItemSprites[outputIndex].emplace(SurfaceToClx(halfSurface, 1, 1));

		SDL_FillRect(itemSurface.surface, nullptr, 1);
		ClxDrawTRN(itemSurface, { 0, itemSurface.h() }, itemSprite, redTrn);
		BilinearDownscaleByHalf8(itemSurface.surface, paletteTransparencyLookup, halfSurface.surface, 1);
		HalfSizeItemSpritesRed[outputIndex].emplace(SurfaceToClx(halfSurface, 1, 1));
	};

	size_t outputIndex = 0;
	for (size_t i = static_cast<int>(CURSOR_FIRSTITEM) - 1, n = pCursCels->numSprites(); i < n; ++i, ++outputIndex) {
		createHalfSize((*pCursCels)[i], outputIndex);
	}
	if (gbIsHellfire) {
		for (size_t i = 0, n = pCursCels2->numSprites(); i < n; ++i, ++outputIndex) {
			createHalfSize((*pCursCels2)[i], outputIndex);
		}
	}
}

void FreeHalfSizeItemSprites()
{
	if (HalfSizeItemSprites != nullptr) {
		delete[] HalfSizeItemSprites;
		HalfSizeItemSprites = nullptr;
		delete[] HalfSizeItemSpritesRed;
		HalfSizeItemSpritesRed = nullptr;
	}
}

void DrawItem(const Item &item, const Surface &out, Point position, ClxSprite clx)
{
	const bool usable = !IsInspectingPlayer() ? item._iStatFlag : InspectPlayer->CanUseItem(item);
	if (usable) {
		ClxDraw(out, position, clx);
	} else {
		ClxDrawTRN(out, position, clx, GetInfravisionTRN());
	}
}

void ResetCursor()
{
	NewCursor(pcurs);
}

void NewCursor(const Item &item)
{
	if (item.isEmpty()) {
		NewCursor(CURSOR_HAND);
	} else {
		NewCursor(item._iCurs + CURSOR_FIRSTITEM);
	}
}

void NewCursor(int cursId)
{
	if (pcurs >= CURSOR_FIRSTITEM && cursId > CURSOR_HAND && cursId < CURSOR_HOURGLASS) {
		if (!TryDropItem()) {
			return;
		}
	}

	if (cursId < CURSOR_HOURGLASS && MyPlayer != nullptr) {
		MyPlayer->HoldItem.clear();
	}
	pcurs = cursId;

	if (IsHardwareCursorEnabled() && ControlDevice == ControlTypes::KeyboardAndMouse) {
		if (!ArtCursor && cursId == CURSOR_NONE)
			return;

		const CursorInfo newCursor = ArtCursor
		    ? CursorInfo::UserInterfaceCursor()
		    : CursorInfo::GameCursor(cursId);
		if (newCursor != GetCurrentCursorInfo())
			SetHardwareCursor(newCursor);
	}
}

void DrawSoftwareCursor(const Surface &out, Point position, int cursId)
{
	const ClxSprite sprite = GetInvItemSprite(cursId);
	if (!MyPlayer->HoldItem.isEmpty()) {
		const auto &heldItem = MyPlayer->HoldItem;
		ClxDrawOutline(out, GetOutlineColor(heldItem, true), position, sprite);
		DrawItem(heldItem, out, position, sprite);
	} else {
		ClxDraw(out, position, sprite);
	}
}

void InitLevelCursor()
{
	NewCursor(CURSOR_HAND);
	cursPosition = ViewPosition;
	pcurstemp = -1;
	pcursmonst = -1;
	ObjectUnderCursor = nullptr;
	pcursitem = -1;
	pcursstashitem = StashStruct::EmptyCell;
	PlayerUnderCursor = nullptr;
	ClearCursor();
}

void CheckTown()
{
	for (auto &missile : Missiles) {
		if (missile._mitype == MissileID::TownPortal) {
			if (EntranceBoundaryContains(missile.position.tile, cursPosition)) {
				trigflag = true;
				InfoString = _("Town Portal");
				AddPanelString(fmt::format(fmt::runtime(_("from {:s}")), Players[missile._misource]._pName));
				cursPosition = missile.position.tile;
			}
		}
	}
}

void CheckRportal()
{
	for (auto &missile : Missiles) {
		if (missile._mitype == MissileID::RedPortal) {
			if (EntranceBoundaryContains(missile.position.tile, cursPosition)) {
				trigflag = true;
				InfoString = _("Portal to");
				AddPanelString(!setlevel ? _("The Unholy Altar") : _("level 15"));
				cursPosition = missile.position.tile;
			}
		}
	}
}

void CheckCursMove()
{
	if (IsItemLabelHighlighted())
		return;

	int sx = MousePosition.x;
	int sy = MousePosition.y;

	if (CanPanelsCoverView()) {
		if (IsLeftPanelOpen()) {
			sx -= GetScreenWidth() / 4;
		} else if (IsRightPanelOpen()) {
			sx += GetScreenWidth() / 4;
		}
	}
	const Rectangle &mainPanel = GetMainPanel();
	if (mainPanel.contains(MousePosition) && track_isscrolling()) {
		sy = mainPanel.position.y - 1;
	}

	if (*sgOptions.Graphics.zoom) {
		sx /= 2;
		sy /= 2;
	}

	// Adjust by player offset and tile grid alignment
	int xo = 0;
	int yo = 0;
	CalcTileOffset(&xo, &yo);
	sx += xo;
	sy += yo;

	const Player &myPlayer = *MyPlayer;

	if (myPlayer.isWalking()) {
		Displacement offset = GetOffsetForWalking(myPlayer.AnimInfo, myPlayer._pdir, true);
		sx -= offset.deltaX;
		sy -= offset.deltaY;

		// Predict the next frame when walking to avoid input jitter
		DisplacementOf<int16_t> offset2 = myPlayer.position.CalculateWalkingOffsetShifted8(myPlayer._pdir, myPlayer.AnimInfo);
		DisplacementOf<int16_t> velocity = myPlayer.position.GetWalkingVelocityShifted8(myPlayer._pdir, myPlayer.AnimInfo);
		int fx = offset2.deltaX / 256;
		int fy = offset2.deltaY / 256;
		fx -= (offset2.deltaX + velocity.deltaX) / 256;
		fy -= (offset2.deltaY + velocity.deltaY) / 256;

		sx -= fx;
		sy -= fy;
	}

	// Convert to tile grid
	int mx = ViewPosition.x;
	int my = ViewPosition.y;

	int columns = 0;
	int rows = 0;
	TilesInView(&columns, &rows);
	int lrow = rows - RowsCoveredByPanel();

	// Center player tile on screen
	ShiftGrid(&mx, &my, -columns / 2, -lrow / 2);

	// Align grid
	if ((columns % 2) == 0 && (lrow % 2) == 0) {
		sy += TILE_HEIGHT / 2;
	} else if ((columns % 2) != 0 && (lrow % 2) != 0) {
		sx -= TILE_WIDTH / 2;
	} else if ((columns % 2) != 0 && (lrow % 2) == 0) {
		my++;
	}

	if (*sgOptions.Graphics.zoom) {
		sy -= TILE_HEIGHT / 4;
	}

	int tx = sx / TILE_WIDTH;
	int ty = sy / TILE_HEIGHT;
	ShiftGrid(&mx, &my, tx, ty);

	// Shift position to match diamond grid aligment
	int px = sx % TILE_WIDTH;
	int py = sy % TILE_HEIGHT;

	// Shift position to match diamond grid aligment
	bool flipy = py < (px / 2);
	if (flipy) {
		my--;
	}
	bool flipx = py >= TILE_HEIGHT - (px / 2);
	if (flipx) {
		mx++;
	}

	mx = std::clamp(mx, 0, MAXDUNX - 1);
	my = std::clamp(my, 0, MAXDUNY - 1);

	const Point currentTile { mx, my };

	// While holding the button down we should retain target (but potentially lose it if it dies, goes out of view, etc)
	if ((sgbMouseDown != CLICK_NONE || ControllerActionHeld != GameActionType_NONE) && IsNoneOf(LastMouseButtonAction, MouseActionType::None, MouseActionType::Attack, MouseActionType::Spell)) {
		InvalidateTargets();

		if (pcursmonst == -1 && ObjectUnderCursor == nullptr && pcursitem == -1 && pcursinvitem == -1 && pcursstashitem == StashStruct::EmptyCell && PlayerUnderCursor == nullptr) {
			cursPosition = { mx, my };
			CheckTrigForce();
			CheckTown();
			CheckRportal();
		}
		return;
	}

	bool flipflag = (flipy && flipx) || ((flipy || flipx) && px < TILE_WIDTH / 2);

	pcurstemp = pcursmonst;
	pcursmonst = -1;
	ObjectUnderCursor = nullptr;
	pcursitem = -1;
	if (pcursinvitem != -1) {
		RedrawComponent(PanelDrawComponent::Belt);
	}
	pcursinvitem = -1;
	pcursstashitem = StashStruct::EmptyCell;
	PlayerUnderCursor = nullptr;
	ShowUniqueItemInfoBox = false;
	panelflag = false;
	trigflag = false;

	if (myPlayer._pInvincible) {
		return;
	}
	if (!myPlayer.HoldItem.isEmpty() || spselflag) {
		cursPosition = { mx, my };
		return;
	}
	if (mainPanel.contains(MousePosition)) {
		CheckPanelInfo();
		return;
	}
	if (DoomFlag) {
		return;
	}
	if (invflag && GetRightPanel().contains(MousePosition)) {
		pcursinvitem = CheckInvHLight();
		return;
	}
	if (IsStashOpen && GetLeftPanel().contains(MousePosition)) {
		pcursstashitem = CheckStashHLight(MousePosition);
	}
	if (sbookflag && GetRightPanel().contains(MousePosition)) {
		return;
	}
	if (IsLeftPanelOpen() && GetLeftPanel().contains(MousePosition)) {
		return;
	}

	if (pcurs == CURSOR_IDENTIFY) {
		ObjectUnderCursor = nullptr;
		pcursmonst = -1;
		pcursitem = -1;
		cursPosition = { mx, my };
		return;
	}

	if (TrySelectPixelBased(currentTile))
		return;

	if (leveltype != DTYPE_TOWN) {
		// Never select a monster if a target-player-only spell is selected
		if (IsNoneOf(pcurs, CURSOR_HEALOTHER, CURSOR_RESURRECT)) {
			if (pcurstemp != -1 && TrySelectMonster(flipflag, currentTile, [](const Monster &monster) {
				    if (!IsValidMonsterForSelection(monster))
					    return false;
				    if (monster.getId() != static_cast<unsigned>(pcurstemp))
					    return false;
				    return true;
			    })) {
				// found a valid previous selected monster
				return;
			}
			if (TrySelectMonster(flipflag, currentTile, IsValidMonsterForSelection)) {
				// found a valid monster
				return;
			}
		}
	} else {
		if (TrySelectTowner(flipflag, currentTile)) {
			// found a towner
			return;
		}
	}

	if (TrySelectPlayer(flipflag, mx, my)) {
		// found a player
		return;
	}

	if (TrySelectObject(flipflag, currentTile)) {
		// found an object
		return;
	}

	if (TrySelectItem(flipflag, mx, my)) {
		// found an item
		return;
	}

	cursPosition = currentTile;
	CheckTrigForce();
	CheckTown();
	CheckRportal();
}

} // namespace devilution

/**
 * @file scrollrt.cpp
 *
 * Implementation of functionality for rendering the dungeons, monsters and calling other render routines.
 */
#include "engine/render/scrollrt.h"

#include "DiabloUI/ui_flags.hpp"
#include "automap.h"
#include "controls/plrctrls.h"
#include "cursor.h"
#include "dead.h"
#include "doom.h"
#include "engine/dx.h"
#include "engine/render/clx_render.hpp"
#include "engine/render/dun_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/trn.hpp"
#include "error.h"
#include "gmenu.h"
#include "help.h"
#include "hwcursor.hpp"
#include "init.h"
#include "inv.h"
#include "lighting.h"
#include "minitext.h"
#ifdef _DEBUG
#include "miniwin/misc_msg.h"
#endif
#include "missiles.h"
#include "nthread.h"
#include "options.h"
#include "panels/charpanel.hpp"
#include "plrmsg.h"
#include "qol/chatlog.h"
#include "qol/itemlabels.h"
#include "qol/monhealthbar.h"
#include "qol/stash.h"
#include "qol/xpbar.h"
#include "stores.h"
#include "towners.h"
#include "utils/bitset2d.hpp"
#include "utils/display.h"
#include "utils/endian.hpp"
#include "utils/log.hpp"
#include "utils/str_cat.hpp"

#ifndef USE_SDL1
#include "controls/touch/renderers.h"
#endif

#ifdef _DEBUG
#include "debug.h"
#endif

namespace devilution {

/**
 * Specifies the current light entry.
 */
int LightTableIndex;

/**
 * Specifies the current MIN block of the level CEL file, as used during rendering of the level tiles.
 *
 * frameNum  := block & 0x0FFF
 * frameType := block & 0x7000 >> 12
 */
uint32_t level_cel_block;
bool AutoMapShowItems;
/**
 * Specifies the type of arches to render.
 */
char arch_draw_type;
/**
 * Specifies whether transparency is active for the current CEL file being decoded.
 */
bool cel_transparency_active;
/**
 * Specifies whether foliage (tile has extra content that overlaps previous tile) being rendered.
 */
bool cel_foliage_active = false;
/**
 * Specifies the current dungeon piece ID of the level, as used during rendering of the level tiles.
 */
int level_piece_id;

// DevilutionX extension.
extern void DrawControllerModifierHints(const Surface &out);

bool frameflag;

namespace {
/**
 * @brief Hash algorithm for point
 */
struct PointHash {
	std::size_t operator()(Point const &s) const noexcept
	{
		return s.x ^ (s.y << 1);
	}
};

/**
 * @brief Contains all Missile at rendering position
 */
std::unordered_multimap<Point, Missile *, PointHash> MissilesAtRenderingTile;

/**
 * @brief Could the missile (at the next game tick) collide? This method is a simplified version of CheckMissileCol (for example without random).
 */
bool CouldMissileCollide(Point tile, bool checkPlayerAndMonster)
{
	if (!InDungeonBounds(tile))
		return true;
	if (checkPlayerAndMonster) {
		if (dMonster[tile.x][tile.y] > 0)
			return true;
		if (dPlayer[tile.x][tile.y] > 0)
			return true;
	}

	return IsMissileBlockedByTile(tile);
}

void UpdateMissileRendererData(Missile &m)
{
	m.position.tileForRendering = m.position.tile;
	m.position.offsetForRendering = m.position.offset;

	const MissileMovementDistribution missileMovement = MissilesData[m._mitype].MovementDistribution;
	// don't calculate missile position if they don't move
	if (missileMovement == MissileMovementDistribution::Disabled || m.position.velocity == Displacement {})
		return;

	float fProgress = gfProgressToNextGameTick;
	Displacement velocity = m.position.velocity * fProgress;
	Displacement pixelsTravelled = (m.position.traveled + velocity) >> 16;
	Displacement tileOffset = pixelsTravelled.screenToMissile();

	// calculcate the future missile position
	m.position.tileForRendering = m.position.start + tileOffset;
	m.position.offsetForRendering = pixelsTravelled + tileOffset.worldToScreen();

	// In some cases this calculcated position is invalid.
	// For example a missile shouldn't move inside a wall.
	// In this case the game logic don't advance the missile position and removes the missile or shows an explosion animation at the old position.
	// For the animation distribution logic this means we are not allowed to move to a tile where the missile could collide, cause this could be a invalid position.

	// If we are still at the current tile, this tile was already checked and is a valid tile
	if (m.position.tileForRendering == m.position.tile)
		return;

	// If no collision can happen at the new tile we can advance
	if (!CouldMissileCollide(m.position.tileForRendering, missileMovement == MissileMovementDistribution::Blockable))
		return;

	// The new tile could be invalid, so don't advance to it.
	// We search the last offset that is in the old (valid) tile.
	// Implementation note: If someone knows the correct math to calculate this without the loop, I would really appreciate it.
	while (m.position.tile != m.position.tileForRendering) {
		fProgress -= 0.01F;

		if (fProgress <= 0.0F) {
			m.position.tileForRendering = m.position.tile;
			m.position.offsetForRendering = m.position.offset;
			return;
		}

		velocity = m.position.velocity * fProgress;
		pixelsTravelled = (m.position.traveled + velocity) >> 16;
		tileOffset = pixelsTravelled.screenToMissile();

		m.position.tileForRendering = m.position.start + tileOffset;
		m.position.offsetForRendering = pixelsTravelled + tileOffset.worldToScreen();
	}
}

void UpdateMissilesRendererData()
{
	MissilesAtRenderingTile.clear();

	for (auto &m : Missiles) {
		UpdateMissileRendererData(m);
		MissilesAtRenderingTile.insert(std::make_pair(m.position.tileForRendering, &m));
	}
}

uint32_t sgdwCursWdtOld;
int sgdwCursX;
int sgdwCursY;
/**
 * Lower bound of back buffer.
 */
uint32_t sgdwCursHgt;

int sgdwCursXOld;
int sgdwCursYOld;

uint32_t sgdwCursWdt;
uint8_t sgSaveBack[8192];
uint32_t sgdwCursHgtOld;

/**
 * @brief Keeps track of which tiles have been rendered already.
 */
Bitset2d<MAXDUNX, MAXDUNY> dRendered;

int lastFpsUpdateInMs;

const char *const PlayerModeNames[] = {
	"standing",
	"walking (1)",
	"walking (2)",
	"walking (3)",
	"attacking (melee)",
	"attacking (ranged)",
	"blocking",
	"getting hit",
	"dying",
	"casting a spell",
	"changing levels",
	"quitting"
};

void BlitCursor(uint8_t *dst, std::uint32_t dstPitch, uint8_t *src, std::uint32_t srcPitch)
{
	for (std::uint32_t i = 0; i < sgdwCursHgt; ++i, src += srcPitch, dst += dstPitch) {
		memcpy(dst, src, sgdwCursWdt);
	}
}

/**
 * @brief Remove the cursor from the buffer
 */
void UndrawCursor(const Surface &out)
{
	if (sgdwCursWdt == 0) {
		return;
	}

	BlitCursor(out.at(sgdwCursX, sgdwCursY), out.pitch(), sgSaveBack, sgdwCursWdt);

	sgdwCursXOld = sgdwCursX;
	sgdwCursYOld = sgdwCursY;
	sgdwCursWdtOld = sgdwCursWdt;
	sgdwCursHgtOld = sgdwCursHgt;
	sgdwCursWdt = 0;
}

bool ShouldShowCursor()
{
	if (ControlMode == ControlTypes::KeyboardAndMouse)
		return true;
	if (pcurs == CURSOR_TELEPORT)
		return true;
	if (invflag)
		return true;
	if (chrflag && MyPlayer->_pStatPts > 0)
		return true;

	return false;
}

/**
 * @brief Save the content behind the cursor to a temporary buffer, then draw the cursor.
 */
void DrawCursor(const Surface &out)
{
	if (pcurs <= CURSOR_NONE || !ShouldShowCursor()) {
		return;
	}

	Size cursSize = GetInvItemSize(pcurs);
	if (cursSize.width == 0 || cursSize.height == 0) {
		return;
	}

	// Copy the buffer before the item cursor and its 1px outline are drawn to a temporary buffer.
	const int outlineWidth = !MyPlayer->HoldItem.isEmpty() ? 1 : 0;

	if (MousePosition.x < -cursSize.width - outlineWidth || MousePosition.x - outlineWidth >= out.w() || MousePosition.y < -cursSize.height - outlineWidth || MousePosition.y - outlineWidth >= out.h())
		return;

	constexpr auto Clip = [](int &pos, std::uint32_t &length, std::uint32_t posEnd) {
		if (pos < 0) {
			length += pos;
			pos = 0;
		} else if (pos + length > posEnd) {
			length = posEnd - pos;
		}
	};

	sgdwCursX = MousePosition.x - outlineWidth;
	sgdwCursWdt = cursSize.width + 2 * outlineWidth;
	Clip(sgdwCursX, sgdwCursWdt, out.w());

	sgdwCursY = MousePosition.y - outlineWidth;
	sgdwCursHgt = cursSize.height + 2 * outlineWidth;
	Clip(sgdwCursY, sgdwCursHgt, out.h());

	BlitCursor(sgSaveBack, sgdwCursWdt, out.at(sgdwCursX, sgdwCursY), out.pitch());
	DrawSoftwareCursor(out, MousePosition + Displacement { 0, cursSize.height - 1 }, pcurs);
}

/**
 * @brief Render a missile sprite
 * @param out Output buffer
 * @param missile Pointer to Missile struct
 * @param targetBufferPosition Output buffer coordinate
 * @param pre Is the sprite in the background
 */
void DrawMissilePrivate(const Surface &out, const Missile &missile, Point targetBufferPosition, bool pre)
{
	if (missile._miPreFlag != pre || !missile._miDrawFlag)
		return;

	const Point missileRenderPosition { targetBufferPosition + missile.position.offsetForRendering - Displacement { missile._miAnimWidth2, 0 } };
	const ClxSprite sprite = (*missile._miAnimData)[missile._miAnimFrame - 1];
	if (missile._miUniqTrans != 0)
		ClxDrawTRN(out, missileRenderPosition, sprite, Monsters[missile._misource].uniqueMonsterTRN.get());
	else if (missile._miLightFlag)
		ClxDrawLight(out, missileRenderPosition, sprite);
	else
		ClxDraw(out, missileRenderPosition, sprite);
}

/**
 * @brief Render a missile sprites for a given tile
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 * @param pre Is the sprite in the background
 */
void DrawMissile(const Surface &out, Point tilePosition, Point targetBufferPosition, bool pre)
{
	const auto range = MissilesAtRenderingTile.equal_range(tilePosition);
	for (auto it = range.first; it != range.second; it++) {
		DrawMissilePrivate(out, *it->second, targetBufferPosition, pre);
	}
}

/**
 * @brief Render a monster sprite
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 * @param monster Monster reference
 */
void DrawMonster(const Surface &out, Point tilePosition, Point targetBufferPosition, const Monster &monster)
{
	if (!monster.animInfo.sprites) {
		Log("Draw Monster \"{}\": NULL Cel Buffer", monster.name());
		return;
	}

	constexpr auto getMonsterModeDisplayName = [](MonsterMode monsterMode) {
		switch (monsterMode) {
		case MonsterMode::Stand:
			return "standing";

		case MonsterMode::MoveNorthwards:
			return "moving (northwards)";

		case MonsterMode::MoveSouthwards:
			return "moving (southwards)";

		case MonsterMode::MoveSideways:
			return "moving (sideways)";

		case MonsterMode::MeleeAttack:
			return "attacking (melee)";

		case MonsterMode::HitRecovery:
			return "getting hit";

		case MonsterMode::Death:
			return "dying";

		case MonsterMode::SpecialMeleeAttack:
			return "attacking (special melee)";

		case MonsterMode::FadeIn:
			return "fading in";

		case MonsterMode::FadeOut:
			return "fading out";

		case MonsterMode::RangedAttack:
			return "attacking (ranged)";

		case MonsterMode::SpecialStand:
			return "standing (special)";

		case MonsterMode::SpecialRangedAttack:
			return "attacking (special ranged)";

		case MonsterMode::Delay:
			return "delaying";

		case MonsterMode::Charge:
			return "charging";

		case MonsterMode::Petrified:
			return "petrified";

		case MonsterMode::Heal:
			return "healing";

		case MonsterMode::Talk:
			return "talking";

		default:
			app_fatal("Invalid monster mode.");
		}
	};

	const ClxSprite sprite = monster.animInfo.currentSprite();

	if (!IsTileLit(tilePosition)) {
		ClxDrawTRN(out, targetBufferPosition, sprite, GetInfravisionTRN());
		return;
	}
	uint8_t *trn = nullptr;
	if (monster.isUnique())
		trn = monster.uniqueMonsterTRN.get();
	if (monster.mode == MonsterMode::Petrified)
		trn = GetStoneTRN();
	if (MyPlayer->_pInfraFlag && LightTableIndex > 8)
		trn = GetInfravisionTRN();
	if (trn != nullptr)
		ClxDrawTRN(out, targetBufferPosition, sprite, trn);
	else
		ClxDrawLight(out, targetBufferPosition, sprite);
}

/**
 * @brief Helper for rendering a specific player icon (Mana Shield or Reflect)
 */
void DrawPlayerIconHelper(const Surface &out, missile_graphic_id missileGraphicId, Point position, bool lighting, bool infraVision)
{
	position.x -= MissileSpriteData[missileGraphicId].animWidth2;

	const ClxSprite sprite = (*MissileSpriteData[missileGraphicId].sprites).list()[0];

	if (!lighting) {
		ClxDraw(out, position, sprite);
		return;
	}

	if (infraVision) {
		ClxDrawTRN(out, position, sprite, GetInfravisionTRN());
		return;
	}

	ClxDrawLight(out, position, sprite);
}

/**
 * @brief Helper for rendering player icons (Mana Shield and Reflect)
 * @param out Output buffer
 * @param player Player reference
 * @param position Output buffer coordinates
 * @param infraVision Should infravision be applied
 */
void DrawPlayerIcons(const Surface &out, const Player &player, Point position, bool infraVision)
{
	if (player.pManaShield)
		DrawPlayerIconHelper(out, MFILE_MANASHLD, position, &player != MyPlayer, infraVision);
	if (player.wReflections > 0)
		DrawPlayerIconHelper(out, MFILE_REFLECT, position + Displacement { 0, 16 }, &player != MyPlayer, infraVision);
}

/**
 * @brief Render a player sprite
 * @param out Output buffer
 * @param player Player reference
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 */
void DrawPlayer(const Surface &out, const Player &player, Point tilePosition, Point targetBufferPosition)
{
	if (!IsTileLit(tilePosition) && !MyPlayer->_pInfraFlag && leveltype != DTYPE_TOWN) {
		return;
	}

	const ClxSprite sprite = player.previewCelSprite ? *player.previewCelSprite : player.AnimInfo.currentSprite();

	Point spriteBufferPosition = targetBufferPosition - Displacement { CalculateWidth2(sprite.width()), 0 };

	if (static_cast<size_t>(pcursplr) < Players.size() && &player == &Players[pcursplr])
		ClxDrawOutlineSkipColorZero(out, 165, spriteBufferPosition, sprite);

	if (&player == MyPlayer) {
		ClxDraw(out, spriteBufferPosition, sprite);
		DrawPlayerIcons(out, player, targetBufferPosition, false);
		return;
	}

	if (!IsTileLit(tilePosition) || (MyPlayer->_pInfraFlag && LightTableIndex > 8)) {
		ClxDrawTRN(out, spriteBufferPosition, sprite, GetInfravisionTRN());
		DrawPlayerIcons(out, player, targetBufferPosition, true);
		return;
	}

	int l = LightTableIndex;
	if (LightTableIndex < 5)
		LightTableIndex = 0;
	else
		LightTableIndex -= 5;

	ClxDrawLight(out, spriteBufferPosition, sprite);
	DrawPlayerIcons(out, player, targetBufferPosition, false);

	LightTableIndex = l;
}

/**
 * @brief Render a player sprite
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 */
void DrawDeadPlayer(const Surface &out, Point tilePosition, Point targetBufferPosition)
{
	dFlags[tilePosition.x][tilePosition.y] &= ~DungeonFlag::DeadPlayer;

	for (Player &player : Players) {
		if (player.plractive && player._pHitPoints == 0 && player.isOnActiveLevel() && player.position.tile == tilePosition) {
			dFlags[tilePosition.x][tilePosition.y] |= DungeonFlag::DeadPlayer;
			const Point playerRenderPosition { targetBufferPosition };
			DrawPlayer(out, player, tilePosition, playerRenderPosition);
		}
	}
}

/**
 * @brief Render an object sprite
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 * @param pre Is the sprite in the background
 */
void DrawObject(const Surface &out, Point tilePosition, Point targetBufferPosition, bool pre)
{
	if (LightTableIndex >= LightsMax) {
		return;
	}

	Object *object = FindObjectAtPosition(tilePosition);
	if (object == nullptr) {
		return;
	}

	const Object &objectToDraw = *object;
	if (objectToDraw._oPreFlag != pre) {
		return;
	}

	const ClxSprite sprite = (*objectToDraw._oAnimData)[objectToDraw._oAnimFrame - 1];

	Point screenPosition = targetBufferPosition - Displacement { CalculateWidth2(sprite.width()), 0 };
	if (objectToDraw.position != tilePosition) {
		// drawing a large or offset object, calculate the correct position for the center of the sprite
		Displacement worldOffset = objectToDraw.position - tilePosition;
		screenPosition -= worldOffset.worldToScreen();
	}

	if (&objectToDraw == ObjectUnderCursor) {
		ClxDrawOutlineSkipColorZero(out, 194, screenPosition, sprite);
	}
	if (objectToDraw._oLight) {
		ClxDrawLight(out, screenPosition, sprite);
	} else {
		ClxDraw(out, screenPosition, sprite);
	}
}

static void DrawDungeon(const Surface & /*out*/, Point /*tilePosition*/, Point /*targetBufferPosition*/);

/**
 * @brief Render a cell
 * @param out Target buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Target buffer coordinates
 */
void DrawCell(const Surface &out, Point tilePosition, Point targetBufferPosition)
{
	level_piece_id = dPiece[tilePosition.x][tilePosition.y];
	MICROS *pMap = &DPieceMicros[level_piece_id];
	cel_transparency_active = TileHasAny(level_piece_id, TileProperties::Transparent) && TransList[dTransVal[tilePosition.x][tilePosition.y]];
	cel_foliage_active = !TileHasAny(level_piece_id, TileProperties::Solid);
	for (int i = 0; i < (MicroTileLen / 2); i++) {
		level_cel_block = pMap->mt[2 * i];
		if (level_cel_block != 0) {
			arch_draw_type = i == 0 ? 1 : 0;
			RenderTile(out, targetBufferPosition);
		}
		level_cel_block = pMap->mt[2 * i + 1];
		if (level_cel_block != 0) {
			arch_draw_type = i == 0 ? 2 : 0;
			RenderTile(out, targetBufferPosition + Displacement { TILE_WIDTH / 2, 0 });
		}
		targetBufferPosition.y -= TILE_HEIGHT;
	}
	cel_foliage_active = false;
}

/**
 * @brief Render a floor tiles
 * @param out Target buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Target buffer coordinate
 */
void DrawFloor(const Surface &out, Point tilePosition, Point targetBufferPosition)
{
	cel_transparency_active = false;
	LightTableIndex = dLight[tilePosition.x][tilePosition.y];

	arch_draw_type = 1; // Left
	int pn = dPiece[tilePosition.x][tilePosition.y];
	level_cel_block = DPieceMicros[pn].mt[0];
	if (level_cel_block != 0) {
		RenderTile(out, targetBufferPosition);
	}
	arch_draw_type = 2; // Right
	level_cel_block = DPieceMicros[pn].mt[1];
	if (level_cel_block != 0) {
		RenderTile(out, targetBufferPosition + Displacement { TILE_WIDTH / 2, 0 });
	}
}

/**
 * @brief Draw item for a given tile
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 * @param pre Is the sprite in the background
 */
void DrawItem(const Surface &out, Point tilePosition, Point targetBufferPosition, bool pre)
{
	int8_t bItem = dItem[tilePosition.x][tilePosition.y];

	if (bItem <= 0)
		return;

	auto &item = Items[bItem - 1];
	if (item._iPostDraw == pre)
		return;

	const ClxSprite sprite = item.AnimInfo.currentSprite();
	int px = targetBufferPosition.x - CalculateWidth2(sprite.width());
	const Point position { px, targetBufferPosition.y };
	if (bItem - 1 == pcursitem || AutoMapShowItems) {
		ClxDrawOutlineSkipColorZero(out, GetOutlineColor(item, false), position, sprite);
	}
	ClxDrawLight(out, position, sprite);
	if (item.AnimInfo.isLastFrame() || item._iCurs == ICURS_MAGIC_ROCK)
		AddItemToLabelQueue(bItem - 1, px, targetBufferPosition.y);
}

/**
 * @brief Check if and how a monster should be rendered
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 */
void DrawMonsterHelper(const Surface &out, Point tilePosition, Point targetBufferPosition)
{
	int mi = dMonster[tilePosition.x][tilePosition.y];
	bool isNegativeMonster = mi < 0;
	mi = abs(mi) - 1;

	if (leveltype == DTYPE_TOWN) {
		if (isNegativeMonster)
			return;
		auto &towner = Towners[mi];
		int px = targetBufferPosition.x - CalculateWidth2(towner._tAnimWidth);
		const Point position { px, targetBufferPosition.y };
		const ClxSprite sprite = towner.currentSprite();
		if (mi == pcursmonst) {
			ClxDrawOutlineSkipColorZero(out, 166, position, sprite);
		}
		ClxDraw(out, position, sprite);
		return;
	}

	if (!IsTileLit(tilePosition) && !MyPlayer->_pInfraFlag)
		return;

	if (static_cast<size_t>(mi) >= MaxMonsters) {
		Log("Draw Monster: tried to draw illegal monster {}", mi);
		return;
	}

	const auto &monster = Monsters[mi];
	if ((monster.flags & MFLAG_HIDDEN) != 0) {
		return;
	}

	const ClxSprite sprite = monster.animInfo.currentSprite();

	Displacement offset = {};
	if (monster.isWalking()) {
		bool isSideWalkingToLeft = monster.mode == MonsterMode::MoveSideways && monster.direction == Direction::West;
		if (isNegativeMonster && !isSideWalkingToLeft)
			return;
		if (!isNegativeMonster && isSideWalkingToLeft)
			return;
		offset = GetOffsetForWalking(monster.animInfo, monster.direction);
		if (isSideWalkingToLeft)
			offset -= Displacement { 64, 0 };
	} else if (isNegativeMonster) {
		return;
	}

	const Point monsterRenderPosition { targetBufferPosition + offset - Displacement { CalculateWidth2(sprite.width()), 0 } };
	if (mi == pcursmonst) {
		ClxDrawOutlineSkipColorZero(out, 233, monsterRenderPosition, sprite);
	}
	DrawMonster(out, tilePosition, monsterRenderPosition, monster);
}

/**
 * @brief Check if and how a player should be rendered
 * @param out Output buffer
 * @param player Player reference
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 */
void DrawPlayerHelper(const Surface &out, const Player &player, Point tilePosition, Point targetBufferPosition)
{
	Displacement offset = {};
	if (player.IsWalking()) {
		offset = GetOffsetForWalking(player.AnimInfo, player._pdir);
	}

	const Point playerRenderPosition { targetBufferPosition + offset };

	DrawPlayer(out, player, tilePosition, playerRenderPosition);
}

/**
 * @brief Render object sprites
 * @param out Target buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Target buffer coordinates
 */
void DrawDungeon(const Surface &out, Point tilePosition, Point targetBufferPosition)
{
	assert(InDungeonBounds(tilePosition));

	if (dRendered.test(tilePosition.x, tilePosition.y))
		return;
	dRendered.set(tilePosition.x, tilePosition.y);

	LightTableIndex = dLight[tilePosition.x][tilePosition.y];

	DrawCell(out, tilePosition, targetBufferPosition);

	int8_t bDead = dCorpse[tilePosition.x][tilePosition.y];
	int8_t bMap = dTransVal[tilePosition.x][tilePosition.y];

#ifdef _DEBUG
	if (DebugVision && IsTileLit(tilePosition)) {
		ClxDraw(out, targetBufferPosition, (*pSquareCel)[0]);
	}
#endif

	if (MissilePreFlag) {
		DrawMissile(out, tilePosition, targetBufferPosition, true);
	}

	if (LightTableIndex < LightsMax && bDead != 0) {
		do {
			Corpse &corpse = Corpses[(bDead & 0x1F) - 1];
			const Point position { targetBufferPosition.x - CalculateWidth2(corpse.width), targetBufferPosition.y };
			const ClxSprite sprite = corpse.spritesForDirection(static_cast<Direction>((bDead >> 5) & 7))[corpse.frame];
			if (corpse.translationPaletteIndex != 0) {
				const uint8_t *trn = Monsters[corpse.translationPaletteIndex - 1].uniqueMonsterTRN.get();
				ClxDrawTRN(out, position, sprite, trn);
			} else {
				ClxDrawLight(out, position, sprite);
			}
		} while (false);
	}
	DrawObject(out, tilePosition, targetBufferPosition, true);
	DrawItem(out, tilePosition, targetBufferPosition, true);

	if (TileContainsDeadPlayer(tilePosition)) {
		DrawDeadPlayer(out, tilePosition, targetBufferPosition);
	}
	int8_t playerId = dPlayer[tilePosition.x][tilePosition.y];
	if (static_cast<size_t>(playerId - 1) < Players.size()) {
		DrawPlayerHelper(out, Players[playerId - 1], tilePosition, targetBufferPosition);
	}
	if (dMonster[tilePosition.x][tilePosition.y] != 0) {
		DrawMonsterHelper(out, tilePosition, targetBufferPosition);
	}
	DrawMissile(out, tilePosition, targetBufferPosition, false);
	DrawObject(out, tilePosition, targetBufferPosition, false);
	DrawItem(out, tilePosition, targetBufferPosition, false);

	if (leveltype != DTYPE_TOWN) {
		char bArch = dSpecial[tilePosition.x][tilePosition.y];
		if (bArch != 0) {
			cel_transparency_active = TransList[bMap];
#ifdef _DEBUG
			if ((SDL_GetModState() & KMOD_ALT) != 0) {
				cel_transparency_active = false; // Turn transparency off here for debugging
			}
#endif
			if (cel_transparency_active) {
				ClxDrawLightBlended(out, targetBufferPosition, (*pSpecialCels)[bArch - 1]);
			} else {
				ClxDrawLight(out, targetBufferPosition, (*pSpecialCels)[bArch - 1]);
			}
#ifdef _DEBUG
			if ((SDL_GetModState() & KMOD_ALT) != 0) {
				cel_transparency_active = TransList[bMap]; // Turn transparency back to its normal state
			}
#endif
		}
	} else {
		// Tree leaves should always cover player when entering or leaving the tile,
		// So delay the rendering until after the next row is being drawn.
		// This could probably have been better solved by sprites in screen space.
		if (tilePosition.x > 0 && tilePosition.y > 0 && targetBufferPosition.y > TILE_HEIGHT) {
			char bArch = dSpecial[tilePosition.x - 1][tilePosition.y - 1];
			if (bArch != 0) {
				ClxDraw(out, targetBufferPosition + Displacement { 0, -TILE_HEIGHT }, (*pSpecialCels)[bArch - 1]);
			}
		}
	}
}

/**
 * @brief Render a row of tiles
 * @param out Buffer to render to
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Target buffer coordinates
 * @param rows Number of rows
 * @param columns Tile in a row
 */
void DrawFloor(const Surface &out, Point tilePosition, Point targetBufferPosition, int rows, int columns)
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (InDungeonBounds(tilePosition)) {
				if (!TileHasAny(dPiece[tilePosition.x][tilePosition.y], TileProperties::Solid))
					DrawFloor(out, tilePosition, targetBufferPosition);
			} else {
				world_draw_black_tile(out, targetBufferPosition.x, targetBufferPosition.y);
			}
			tilePosition += Direction::East;
			targetBufferPosition.x += TILE_WIDTH;
		}
		// Return to start of row
		tilePosition += Displacement(Direction::West) * columns;
		targetBufferPosition.x -= columns * TILE_WIDTH;

		// Jump to next row
		targetBufferPosition.y += TILE_HEIGHT / 2;
		if ((i & 1) != 0) {
			tilePosition.x++;
			columns--;
			targetBufferPosition.x += TILE_WIDTH / 2;
		} else {
			tilePosition.y++;
			columns++;
			targetBufferPosition.x -= TILE_WIDTH / 2;
		}
	}
}

bool IsWall(Point position)
{
	return TileHasAny(dPiece[position.x][position.y], TileProperties::Solid) || dSpecial[position.x][position.y] != 0;
}

/**
 * @brief Render a row of tile
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Buffer coordinates
 * @param rows Number of rows
 * @param columns Tile in a row
 */
void DrawTileContent(const Surface &out, Point tilePosition, Point targetBufferPosition, int rows, int columns)
{
	// Keep evaluating until MicroTiles can't affect screen
	rows += MicroTileLen;
	dRendered.reset();

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (InDungeonBounds(tilePosition)) {
#ifdef _DEBUG
				DebugCoordsMap[tilePosition.x + tilePosition.y * MAXDUNX] = targetBufferPosition;
#endif
				if (tilePosition.x + 1 < MAXDUNX && tilePosition.y - 1 >= 0 && targetBufferPosition.x + TILE_WIDTH <= gnScreenWidth) {
					// Render objects behind walls first to prevent sprites, that are moving
					// between tiles, from poking through the walls as they exceed the tile bounds.
					// A proper fix for this would probably be to layout the sceen and render by
					// sprite screen position rather than tile position.
					if (IsWall(tilePosition) && (IsWall(tilePosition + Displacement { 1, 0 }) || (tilePosition.x > 0 && IsWall(tilePosition + Displacement { -1, 0 })))) { // Part of a wall aligned on the x-axis
						if (IsTileNotSolid(tilePosition + Displacement { 1, -1 }) && IsTileNotSolid(tilePosition + Displacement { 0, -1 })) {                              // Has walkable area behind it
							DrawDungeon(out, tilePosition + Direction::East, { targetBufferPosition.x + TILE_WIDTH, targetBufferPosition.y });
						}
					}
				}
				DrawDungeon(out, tilePosition, targetBufferPosition);
			}
			tilePosition += Direction::East;
			targetBufferPosition.x += TILE_WIDTH;
		}
		// Return to start of row
		tilePosition += Displacement(Direction::West) * columns;
		targetBufferPosition.x -= columns * TILE_WIDTH;

		// Jump to next row
		targetBufferPosition.y += TILE_HEIGHT / 2;
		if ((i & 1) != 0) {
			tilePosition.x++;
			columns--;
			targetBufferPosition.x += TILE_WIDTH / 2;
		} else {
			tilePosition.y++;
			columns++;
			targetBufferPosition.x -= TILE_WIDTH / 2;
		}
	}
}

/**
 * @brief Scale up the top left part of the buffer 2x.
 */
void Zoom(const Surface &out)
{
	int viewportWidth = out.w();
	int viewportOffsetX = 0;
	if (CanPanelsCoverView()) {
		if (IsLeftPanelOpen()) {
			viewportWidth -= SidePanelSize.width;
			viewportOffsetX = SidePanelSize.width;
		} else if (IsRightPanelOpen()) {
			viewportWidth -= SidePanelSize.width;
		}
	}

	// We round to even for the source width and height.
	// If the width / height was odd, we copy just one extra pixel / row later on.
	const int srcWidth = (viewportWidth + 1) / 2;
	const int doubleableWidth = viewportWidth / 2;
	const int srcHeight = (out.h() + 1) / 2;
	const int doubleableHeight = out.h() / 2;

	uint8_t *src = out.at(srcWidth - 1, srcHeight - 1);
	uint8_t *dst = out.at(viewportOffsetX + viewportWidth - 1, out.h() - 1);
	const bool oddViewportWidth = (viewportWidth % 2) == 1;

	for (int hgt = 0; hgt < doubleableHeight; hgt++) {
		// Double the pixels in the line.
		for (int i = 0; i < doubleableWidth; i++) {
			*dst-- = *src;
			*dst-- = *src;
			--src;
		}

		// Copy a single extra pixel if the output width is odd.
		if (oddViewportWidth) {
			*dst-- = *src;
			--src;
		}

		// Skip the rest of the source line.
		src -= (out.pitch() - srcWidth);

		// Double the line.
		memcpy(dst - out.pitch() + 1, dst + 1, viewportWidth);

		// Skip the rest of the destination line.
		dst -= 2 * out.pitch() - viewportWidth;
	}
	if ((out.h() % 2) == 1) {
		memcpy(dst - out.pitch() + 1, dst + 1, viewportWidth);
	}
}

Displacement tileOffset;
Displacement tileShift;
int tileColums;
int tileRows;

/**
 * @brief Configure render and process screen rows
 * @param fullOut Buffer to render to
 * @param position Center of view in dPiece coordinate
 */
void DrawGame(const Surface &fullOut, Point position)
{
	// Limit rendering to the view area
	const Surface &out = !*sgOptions.Graphics.zoom
	    ? fullOut.subregionY(0, gnViewportHeight)
	    : fullOut.subregionY(0, (gnViewportHeight + 1) / 2);

	// Adjust by player offset and tile grid alignment
	Player &myPlayer = *MyPlayer;
	Displacement offset = {};
	if (myPlayer.IsWalking())
		offset = GetOffsetForWalking(myPlayer.AnimInfo, myPlayer._pdir, true);
	int sx = offset.deltaX + tileOffset.deltaX;
	int sy = offset.deltaY + tileOffset.deltaY;

	int columns = tileColums;
	int rows = tileRows;

	position += tileShift;

	// Skip rendering parts covered by the panels
	if (CanPanelsCoverView()) {
		if (!*sgOptions.Graphics.zoom) {
			if (IsLeftPanelOpen()) {
				position += Displacement(Direction::East) * 2;
				columns -= 4;
				sx += SidePanelSize.width - TILE_WIDTH / 2;
			}
			if (IsRightPanelOpen()) {
				position += Displacement(Direction::East) * 2;
				columns -= 4;
				sx += -TILE_WIDTH / 2;
			}
		} else {
			if (IsLeftPanelOpen()) {
				position += Direction::East;
				columns -= 2;
				sx += -TILE_WIDTH / 2 / 2; // SPANEL_WIDTH accounted for in Zoom()
			}
			if (IsRightPanelOpen()) {
				position += Direction::East;
				columns -= 2;
				sx += -TILE_WIDTH / 2 / 2;
			}
		}
	}

	UpdateMissilesRendererData();

	// Draw areas moving in and out of the screen
	if (myPlayer.IsWalking()) {
		switch (myPlayer._pdir) {
		case Direction::North:
			sy -= TILE_HEIGHT;
			position += Direction::North;
			rows += 2;
			break;
		case Direction::NorthEast:
			sy -= TILE_HEIGHT;
			position += Direction::North;
			columns++;
			rows += 2;
			break;
		case Direction::East:
			columns++;
			break;
		case Direction::SouthEast:
			columns++;
			rows++;
			break;
		case Direction::South:
			rows += 2;
			break;
		case Direction::SouthWest:
			sx -= TILE_WIDTH;
			position += Direction::West;
			columns++;
			rows++;
			break;
		case Direction::West:
			sx -= TILE_WIDTH;
			position += Direction::West;
			columns++;
			break;
		case Direction::NorthWest:
			sx -= TILE_WIDTH / 2;
			sy -= TILE_HEIGHT / 2;
			position += Direction::NorthWest;
			columns++;
			rows++;
			break;
		}
	}

	DrawFloor(out, position, { sx, sy }, rows, columns);
	DrawTileContent(out, position, { sx, sy }, rows, columns);

	if (*sgOptions.Graphics.zoom) {
		Zoom(fullOut.subregionY(0, gnViewportHeight));
	}
}

/**
 * @brief Start rendering of screen, town variation
 * @param out Buffer to render to
 * @param startPosition Center of view in dPiece coordinates
 */
void DrawView(const Surface &out, Point startPosition)
{
#ifdef _DEBUG
	DebugCoordsMap.clear();
#endif
	DrawGame(out, startPosition);
	if (AutomapActive) {
		DrawAutomap(out.subregionY(0, gnViewportHeight));
	}
#ifdef _DEBUG
	bool debugGridTextNeeded = IsDebugGridTextNeeded();
	if (debugGridTextNeeded || DebugGrid) {
		// force redrawing or debug stuff stays on panel on 640x480 resolution
		force_redraw = 255;
		char debugGridTextBuffer[10];
		bool megaTiles = IsDebugGridInMegatiles();

		for (auto m : DebugCoordsMap) {
			Point dunCoords = { m.first % MAXDUNX, m.first / MAXDUNX };
			if (megaTiles && (dunCoords.x % 2 == 1 || dunCoords.y % 2 == 1))
				continue;
			Point pixelCoords = m.second;
			if (megaTiles)
				pixelCoords += Displacement { 0, TILE_HEIGHT / 2 };
			if (*sgOptions.Graphics.zoom)
				pixelCoords *= 2;
			if (debugGridTextNeeded && GetDebugGridText(dunCoords, debugGridTextBuffer)) {
				Size tileSize = { TILE_WIDTH, TILE_HEIGHT };
				if (*sgOptions.Graphics.zoom)
					tileSize *= 2;
				DrawString(out, debugGridTextBuffer, { pixelCoords - Displacement { 0, tileSize.height }, tileSize }, UiFlags::ColorRed | UiFlags::AlignCenter | UiFlags::VerticalCenter);
			}
			if (DebugGrid) {
				auto DrawDebugSquare = [&out](Point center, Displacement hor, Displacement ver, uint8_t col) {
					auto DrawLine = [&out](Point from, Point to, uint8_t col) {
						int dx = to.x - from.x;
						int dy = to.y - from.y;
						int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
						float ix = dx / (float)steps;
						float iy = dy / (float)steps;
						float sx = from.x;
						float sy = from.y;

						for (int i = 0; i <= steps; i++, sx += ix, sy += iy)
							out.SetPixel({ (int)sx, (int)sy }, col);
					};
					DrawLine(center - hor, center + ver, col);
					DrawLine(center + hor, center + ver, col);
					DrawLine(center - hor, center - ver, col);
					DrawLine(center + hor, center - ver, col);
				};

				Displacement hor = { TILE_WIDTH / 2, 0 };
				Displacement ver = { 0, TILE_HEIGHT / 2 };
				if (*sgOptions.Graphics.zoom) {
					hor *= 2;
					ver *= 2;
				}
				Point center = pixelCoords + hor - ver;

				if (megaTiles) {
					hor *= 2;
					ver *= 2;
				}

				uint8_t col = PAL16_BEIGE;

				DrawDebugSquare(center, hor, ver, col);
			}
		}
	}
#endif
	DrawMonsterHealthBar(out);
	DrawItemNameLabels(out);

	if (stextflag != STORE_NONE && !qtextflag)
		DrawSText(out);
	if (invflag) {
		DrawInv(out);
	} else if (sbookflag) {
		DrawSpellBook(out);
	}

	DrawDurIcon(out);

	if (chrflag) {
		DrawChr(out);
	} else if (QuestLogIsOpen) {
		DrawQuestLog(out);
	} else if (IsStashOpen) {
		DrawStash(out);
	}
	DrawLevelUpIcon(out);
	if (ShowUniqueItemInfoBox) {
		DrawUniqueInfo(out);
	}
	if (qtextflag) {
		DrawQText(out);
	}
	if (spselflag) {
		DrawSpellList(out);
	}
	if (dropGoldFlag) {
		DrawGoldSplit(out, dropGoldValue);
	}
	DrawGoldWithdraw(out, WithdrawGoldValue);
	if (HelpFlag) {
		DrawHelp(out);
	}
	if (ChatLogFlag) {
		DrawChatLog(out);
	}
	if (IsDiabloMsgAvailable()) {
		DrawDiabloMsg(out);
	}
	if (MyPlayerIsDead) {
		RedBack(out);
	} else if (PauseMode != 0) {
		gmenu_draw_pause(out);
	}

	DrawControllerModifierHints(out);
	DrawPlrMsg(out);
	gmenu_draw(out);
	doom_draw(out);
	DrawInfoBox(out);
	control_update_life_mana(); // Update life/mana totals before rendering any portion of the flask.
	DrawLifeFlaskUpper(out);
	DrawManaFlaskUpper(out);
}

/**
 * @brief Display the current average FPS over 1 sec
 */
void DrawFPS(const Surface &out)
{
	static int framesSinceLastUpdate = 0;
	static string_view formatted {};

	if (!frameflag || !gbActive) {
		return;
	}

	framesSinceLastUpdate++;
	uint32_t runtimeInMs = SDL_GetTicks();
	uint32_t msSinceLastUpdate = runtimeInMs - lastFpsUpdateInMs;
	if (msSinceLastUpdate >= 1000) {
		lastFpsUpdateInMs = runtimeInMs;
		int framerate = 1000 * framesSinceLastUpdate / msSinceLastUpdate;
		framesSinceLastUpdate = 0;

		static char buf[12] {};
		const char *end = BufCopy(buf, framerate, " FPS");
		formatted = { buf, static_cast<string_view::size_type>(end - buf) };
	};
	DrawString(out, formatted, Point { 8, 68 }, UiFlags::ColorRed);
}

/**
 * @brief Update part of the screen from the back buffer
 * @param dwX Back buffer coordinate
 * @param dwY Back buffer coordinate
 * @param dwWdt Back buffer coordinate
 * @param dwHgt Back buffer coordinate
 */
void DoBlitScreen(Sint16 dwX, Sint16 dwY, Uint16 dwWdt, Uint16 dwHgt)
{
	// In SDL1 SDL_Rect x and y are Sint16. Cast explicitly to avoid a compiler warning.
	using CoordType = decltype(SDL_Rect {}.x);
	SDL_Rect srcRect {
		static_cast<CoordType>(dwX),
		static_cast<CoordType>(dwY),
		dwWdt, dwHgt
	};
	SDL_Rect dstRect { dwX, dwY, dwWdt, dwHgt };

	BltFast(&srcRect, &dstRect);
}

/**
 * @brief Check render pipeline and blit individual screen parts
 * @param dwHgt Section of screen to update from top to bottom
 * @param drawDesc Render info box
 * @param drawHp Render health bar
 * @param drawMana Render mana bar
 * @param drawSbar Render belt
 * @param drawBtn Render panel buttons
 */
void DrawMain(int dwHgt, bool drawDesc, bool drawHp, bool drawMana, bool drawSbar, bool drawBtn)
{
	if (!gbActive || RenderDirectlyToOutputSurface) {
		return;
	}

	assert(dwHgt >= 0 && dwHgt <= gnScreenHeight);

	if (dwHgt > 0) {
		DoBlitScreen(0, 0, gnScreenWidth, dwHgt);
	}
	if (dwHgt < gnScreenHeight) {
		const Point mainPanelPosition = GetMainPanel().position;
		if (drawSbar) {
			DoBlitScreen(mainPanelPosition.x + 204, mainPanelPosition.y + 5, 232, 28);
		}
		if (drawDesc) {
			DoBlitScreen(mainPanelPosition.x + 176, mainPanelPosition.y + 46, 288, 63);
		}
		if (drawMana) {
			DoBlitScreen(mainPanelPosition.x + 460, mainPanelPosition.y, 88, 72);
			DoBlitScreen(mainPanelPosition.x + 564, mainPanelPosition.y + 64, 56, 56);
		}
		if (drawHp) {
			DoBlitScreen(mainPanelPosition.x + 96, mainPanelPosition.y, 88, 72);
		}
		if (drawBtn) {
			DoBlitScreen(mainPanelPosition.x + 8, mainPanelPosition.y + 5, 72, 119);
			DoBlitScreen(mainPanelPosition.x + 556, mainPanelPosition.y + 5, 72, 48);
			if (gbIsMultiplayer) {
				DoBlitScreen(mainPanelPosition.x + 84, mainPanelPosition.y + 91, 36, 32);
				DoBlitScreen(mainPanelPosition.x + 524, mainPanelPosition.y + 91, 36, 32);
			}
		}
		if (sgdwCursWdtOld != 0) {
			DoBlitScreen(sgdwCursXOld, sgdwCursYOld, sgdwCursWdtOld, sgdwCursHgtOld);
		}
		if (sgdwCursWdt != 0) {
			DoBlitScreen(sgdwCursX, sgdwCursY, sgdwCursWdt, sgdwCursHgt);
		}
	}
}

} // namespace

Displacement GetOffsetForWalking(const AnimationInfo &animationInfo, const Direction dir, bool cameraMode /*= false*/)
{
	// clang-format off
	//                                           South,        SouthWest,    West,         NorthWest,    North,        NorthEast,     East,         SouthEast,
	constexpr Displacement StartOffset[8]    = { {   0, -32 }, {  32, -16 }, {  64,   0 }, {   0,   0 }, {   0,   0 }, {  0,    0 },  { -64,   0 }, { -32, -16 } };
	constexpr Displacement MovingOffset[8]   = { {   0,  32 }, { -32,  16 }, { -64,   0 }, { -32, -16 }, {   0, -32 }, {  32, -16 },  {  64,   0 }, {  32,  16 } };
	// clang-format on

	float fAnimationProgress = animationInfo.getAnimationProgress();
	Displacement offset = MovingOffset[static_cast<size_t>(dir)];
	offset *= fAnimationProgress;

	if (cameraMode) {
		offset = -offset;
	} else {
		offset += StartOffset[static_cast<size_t>(dir)];
	}

	return offset;
}

void ClearCursor() // CODE_FIX: this was supposed to be in cursor.cpp
{
	sgdwCursWdt = 0;
	sgdwCursWdtOld = 0;
}

void ShiftGrid(int *x, int *y, int horizontal, int vertical)
{
	*x += vertical + horizontal;
	*y += vertical - horizontal;
}

int RowsCoveredByPanel()
{
	auto &mainPanelSize = GetMainPanel().size;
	if (GetScreenWidth() <= mainPanelSize.width) {
		return 0;
	}

	int rows = mainPanelSize.height / TILE_HEIGHT;
	if (*sgOptions.Graphics.zoom) {
		rows /= 2;
	}

	return rows;
}

void CalcTileOffset(int *offsetX, int *offsetY)
{
	uint16_t screenWidth = GetScreenWidth();
	uint16_t viewportHeight = GetViewportHeight();

	int x;
	int y;

	if (!*sgOptions.Graphics.zoom) {
		x = screenWidth % TILE_WIDTH;
		y = viewportHeight % TILE_HEIGHT;
	} else {
		x = (screenWidth / 2) % TILE_WIDTH;
		y = (viewportHeight / 2) % TILE_HEIGHT;
	}

	if (x != 0)
		x = (TILE_WIDTH - x) / 2;
	if (y != 0)
		y = (TILE_HEIGHT - y) / 2;

	*offsetX = x;
	*offsetY = y;
}

void TilesInView(int *rcolumns, int *rrows)
{
	uint16_t screenWidth = GetScreenWidth();
	uint16_t viewportHeight = GetViewportHeight();

	int columns = screenWidth / TILE_WIDTH;
	if ((screenWidth % TILE_WIDTH) != 0) {
		columns++;
	}
	int rows = viewportHeight / TILE_HEIGHT;
	if ((viewportHeight % TILE_HEIGHT) != 0) {
		rows++;
	}

	if (*sgOptions.Graphics.zoom) {
		// Half the number of tiles, rounded up
		if ((columns & 1) != 0) {
			columns++;
		}
		columns /= 2;
		if ((rows & 1) != 0) {
			rows++;
		}
		rows /= 2;
	}

	*rcolumns = columns;
	*rrows = rows;
}

void CalcViewportGeometry()
{
	tileShift = { 0, 0 };

	// Adjust by player offset and tile grid alignment
	int xo = 0;
	int yo = 0;
	CalcTileOffset(&xo, &yo);
	tileOffset = { -xo, -yo - 1 + TILE_HEIGHT / 2 };

	TilesInView(&tileColums, &tileRows);
	int lrow = tileRows - RowsCoveredByPanel();

	// Center player tile on screen
	tileShift += Displacement(Direction::West) * (tileColums / 2);
	tileShift += Displacement(Direction::North) * (lrow / 2);

	tileRows *= 2;

	// Align grid
	if ((tileColums & 1) == 0) {
		tileShift.deltaY--; // Shift player row to one that can be centered with out pixel offset
		if ((lrow & 1) == 0) {
			// Offset tile to vertically align the player when both rows and colums are even
			tileRows++;
			tileOffset.deltaY -= TILE_HEIGHT / 2;
		}
	} else if ((tileColums & 1) != 0 && (lrow & 1) != 0) {
		// Offset tile to vertically align the player when both rows and colums are odd
		tileShift += Displacement(Direction::North);
		tileRows++;
		tileOffset.deltaY -= TILE_HEIGHT / 2;
	}

	// Slightly lower the zoomed view
	if (*sgOptions.Graphics.zoom) {
		tileOffset.deltaY += TILE_HEIGHT / 4;
		if (yo < TILE_HEIGHT / 4)
			tileRows++;
	}

	tileRows++; // Cover lower edge saw tooth, right edge accounted for in scrollrt_draw()
}

extern SDL_Surface *PalSurface;

void ClearScreenBuffer()
{
	if (HeadlessMode)
		return;

	assert(PalSurface != nullptr);
	SDL_FillRect(PalSurface, nullptr, 0);
}

#ifdef _DEBUG
void ScrollView()
{
	if (!MyPlayer->HoldItem.isEmpty())
		return;

	if (MousePosition.x < 20) {
		if (dmaxPosition.y - 1 <= ViewPosition.y || dminPosition.x >= ViewPosition.x) {
			if (dmaxPosition.y - 1 > ViewPosition.y) {
				ViewPosition.y++;
			}
			if (dminPosition.x < ViewPosition.x) {
				ViewPosition.x--;
			}
		} else {
			ViewPosition.y++;
			ViewPosition.x--;
		}
	}
	if (MousePosition.x > gnScreenWidth - 20) {
		if (dmaxPosition.x - 1 <= ViewPosition.x || dminPosition.y >= ViewPosition.y) {
			if (dmaxPosition.x - 1 > ViewPosition.x) {
				ViewPosition.x++;
			}
			if (dminPosition.y < ViewPosition.y) {
				ViewPosition.y--;
			}
		} else {
			ViewPosition.y--;
			ViewPosition.x++;
		}
	}
	if (MousePosition.y < 20) {
		if (dminPosition.y >= ViewPosition.y || dminPosition.x >= ViewPosition.x) {
			if (dminPosition.y < ViewPosition.y) {
				ViewPosition.y--;
			}
			if (dminPosition.x < ViewPosition.x) {
				ViewPosition.x--;
			}
		} else {
			ViewPosition.x--;
			ViewPosition.y--;
		}
	}
	if (MousePosition.y > gnScreenHeight - 20) {
		if (dmaxPosition.y - 1 <= ViewPosition.y || dmaxPosition.x - 1 <= ViewPosition.x) {
			if (dmaxPosition.y - 1 > ViewPosition.y) {
				ViewPosition.y++;
			}
			if (dmaxPosition.x - 1 > ViewPosition.x) {
				ViewPosition.x++;
			}
		} else {
			ViewPosition.x++;
			ViewPosition.y++;
		}
	}
}
#endif

void EnableFrameCount()
{
	frameflag = true;
	lastFpsUpdateInMs = SDL_GetTicks();
}

void scrollrt_draw_game_screen()
{
	if (HeadlessMode)
		return;

	int hgt = 0;

	if (force_redraw == 255) {
		force_redraw = 0;
		hgt = gnScreenHeight;
	}

	if (IsHardwareCursor()) {
		SetHardwareCursorVisible(ShouldShowCursor());
	} else {
		DrawCursor(GlobalBackBuffer());
	}

	DrawMain(hgt, false, false, false, false, false);

	RenderPresent();

	if (!IsHardwareCursor()) {
		UndrawCursor(GlobalBackBuffer());
	}
}

void DrawAndBlit()
{
	if (!gbRunGame || HeadlessMode) {
		return;
	}

	int hgt = 0;
	bool ddsdesc = false;
	bool ctrlPan = false;

	const Rectangle &mainPanel = GetMainPanel();

	if (gnScreenWidth > mainPanel.size.width || force_redraw == 255 || IsHighlightingLabelsEnabled()) {
		drawhpflag = true;
		drawmanaflag = true;
		drawbtnflag = true;
		drawsbarflag = true;
		ddsdesc = false;
		ctrlPan = true;
		hgt = gnScreenHeight;
	} else if (force_redraw == 1) {
		ddsdesc = true;
		ctrlPan = false;
		hgt = gnViewportHeight;
	}

	force_redraw = 0;

	const Surface &out = GlobalBackBuffer();
	UndrawCursor(out);

	nthread_UpdateProgressToNextGameTick();

	DrawView(out, ViewPosition);
	if (ctrlPan) {
		DrawCtrlPan(out);
	}
	if (drawhpflag) {
		DrawLifeFlaskLower(out);
	}
	if (drawmanaflag) {
		DrawManaFlaskLower(out);

		DrawSpell(out);
	}
	if (drawbtnflag) {
		DrawCtrlBtns(out);
	}
	if (drawsbarflag) {
		DrawInvBelt(out);
	}
	if (talkflag) {
		DrawTalkPan(out);
		hgt = gnScreenHeight;
	}
	DrawXPBar(out);
	if (*sgOptions.Graphics.showHealthValues)
		DrawFlaskValues(out, { mainPanel.position.x + 134, mainPanel.position.y + 28 }, MyPlayer->_pHitPoints >> 6, MyPlayer->_pMaxHP >> 6);
	if (*sgOptions.Graphics.showManaValues)
		DrawFlaskValues(out, { mainPanel.position.x + mainPanel.size.width - 138, mainPanel.position.y + 28 }, MyPlayer->_pMana >> 6, MyPlayer->_pMaxMana >> 6);

	if (IsHardwareCursor()) {
		SetHardwareCursorVisible(ShouldShowCursor());
	} else {
		DrawCursor(out);
	}

	DrawFPS(out);

	DrawMain(hgt, ddsdesc, drawhpflag, drawmanaflag, drawsbarflag, drawbtnflag);

	RenderPresent();

	drawhpflag = false;
	drawmanaflag = false;
	drawbtnflag = false;
	drawsbarflag = false;
}

} // namespace devilution

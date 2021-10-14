/**
 * @file scrollrt.cpp
 *
 * Implementation of functionality for rendering the dungeons, monsters and calling other render routines.
 */

#include "automap.h"
#include "controls/touch/renderers.h"
#include "cursor.h"
#include "dead.h"
#include "doom.h"
#include "dx.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/cl2_render.hpp"
#include "engine/render/dun_render.hpp"
#include "engine/render/text_render.hpp"
#include "panels/charpanel.hpp"
#include "error.h"
#include "gmenu.h"
#include "help.h"
#include "hwcursor.hpp"
#include "init.h"
#include "inv.h"
#include "lighting.h"
#include "minitext.h"
#include "missiles.h"
#include "nthread.h"
#include "plrmsg.h"
#include "qol/itemlabels.h"
#include "qol/monhealthbar.h"
#include "qol/xpbar.h"
#include "stores.h"
#include "towners.h"
#include "utils/display.h"
#include "utils/endian.hpp"
#include "utils/log.hpp"

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
	int oid = dObject[tile.x][tile.y];
	if (oid != 0) {
		oid = oid > 0 ? oid - 1 : -(oid + 1);
		if (!Objects[oid]._oMissFlag)
			return true;
	}
	return nMissileTable[dPiece[tile.x][tile.y]];
}

void UpdateMissileRendererData(Missile &m)
{
	m.position.tileForRendering = m.position.tile;
	m.position.offsetForRendering = m.position.offset;

	const MissileMovementDistrubution missileMovement = MissilesData[m._mitype].MovementDistribution;
	// don't calculate missile position if they don't move
	if (missileMovement == MissileMovementDistrubution::Disabled || m.position.velocity == Displacement {})
		return;

	float fProgress = gfProgressToNextGameTick;
	Displacement velocity = m.position.velocity * fProgress;
	Displacement traveled = m.position.traveled + velocity;

	int mx = traveled.deltaX >> 16;
	int my = traveled.deltaY >> 16;
	int dx = (mx + 2 * my) / 64;
	int dy = (2 * my - mx) / 64;

	// calculcate the future missile position
	m.position.tileForRendering = m.position.start + Displacement { dx, dy };
	m.position.offsetForRendering = { mx + (dy * 32) - (dx * 32), my - (dx * 16) - (dy * 16) };

	// In some cases this calculcated position is invalid.
	// For example a missile shouldn't move inside a wall.
	// In this case the game logic don't advance the missile position and removes the missile or shows an explosion animation at the old position.
	// For the animation distribution logic this means we are not allowed to move to a tile where the missile could collide, cause this could be a invalid position.

	// If we are still at the current tile, this tile was already checked and is a valid tile
	if (m.position.tileForRendering == m.position.tile)
		return;

	// If no collision can happen at the new tile we can advance
	if (!CouldMissileCollide(m.position.tileForRendering, missileMovement == MissileMovementDistrubution::Blockable))
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
		traveled = m.position.traveled + velocity;

		mx = traveled.deltaX >> 16;
		my = traveled.deltaY >> 16;
		dx = (mx + 2 * my) / 64;
		dy = (2 * my - mx) / 64;

		m.position.tileForRendering = m.position.start + Displacement { dx, dy };
		m.position.offsetForRendering = { mx + (dy * 32) - (dx * 32), my - (dx * 16) - (dy * 16) };
	}
}

void UpdateMissilesRendererData()
{
	MissilesAtRenderingTile.clear();

	for (int i = 0; i < ActiveMissileCount; i++) {
		assert(ActiveMissiles[i] < MAXMISSILES);
		Missile &m = Missiles[ActiveMissiles[i]];
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
BYTE sgSaveBack[8192];
uint32_t sgdwCursHgtOld;

bool dRendered[MAXDUNX][MAXDUNY];

bool frameflag;
int frameend;
int framerate;
int framestart;

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

void BlitCursor(BYTE *dst, std::uint32_t dstPitch, BYTE *src, std::uint32_t srcPitch)
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
	if (!sgbControllerActive && !sgbTouchActive)
		return true;
	if (IsMovingMouseCursorWithController())
		return true;
	if (pcurs == CURSOR_TELEPORT)
		return true;
	if (invflag)
		return true;
	if (chrflag && Players[MyPlayerId]._pStatPts > 0)
		return true;

	return false;
}

/**
 * @brief Save the content behind the cursor to a temporary buffer, then draw the cursor.
 */
void DrawCursor(const Surface &out)
{
	if (pcurs <= CURSOR_NONE || cursSize.width == 0 || cursSize.height == 0 || !ShouldShowCursor()) {
		return;
	}

	// Copy the buffer before the item cursor and its 1px outline are drawn to a temporary buffer.
	const int outlineWidth = IsItemSprite(pcurs) ? 1 : 0;

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
	CelDrawCursor(out, MousePosition + Displacement { 0, cursSize.height - 1 }, pcurs);
}

/**
 * @brief Render a missile sprite
 * @param out Output buffer
 * @param m Pointer to Missile struct
 * @param targetBufferPosition Output buffer coordinate
 * @param pre Is the sprite in the background
 */
void DrawMissilePrivate(const Surface &out, const Missile &missile, Point targetBufferPosition, bool pre)
{
	if (missile._miPreFlag != pre || !missile._miDrawFlag)
		return;

	if (missile._miAnimData == nullptr) {
		Log("Draw Missile 2 type {}: NULL Cel Buffer", missile._mitype);
		return;
	}
	int nCel = missile._miAnimFrame;
	const auto *frameTable = reinterpret_cast<const uint32_t *>(missile._miAnimData);
	int frames = SDL_SwapLE32(frameTable[0]);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		Log("Draw Missile 2: frame {} of {}, missile type=={}", nCel, frames, missile._mitype);
		return;
	}

	const Point missileRenderPosition { targetBufferPosition + missile.position.offsetForRendering - Displacement { missile._miAnimWidth2, 0 } };
	CelSprite cel { missile._miAnimData, missile._miAnimWidth };
	if (missile._miUniqTrans != 0)
		Cl2DrawLightTbl(out, missileRenderPosition.x, missileRenderPosition.y, cel, missile._miAnimFrame, missile._miUniqTrans + 3);
	else if (missile._miLightFlag)
		Cl2DrawLight(out, missileRenderPosition.x, missileRenderPosition.y, cel, missile._miAnimFrame);
	else
		Cl2Draw(out, missileRenderPosition.x, missileRenderPosition.y, cel, missile._miAnimFrame);
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
 * @param m Id of monster
 */
void DrawMonster(const Surface &out, Point tilePosition, Point targetBufferPosition, const Monster &monster)
{
	if (monster.AnimInfo.pCelSprite == nullptr) {
		Log("Draw Monster \"{}\": NULL Cel Buffer", monster.mName);
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

	int nCel = monster.AnimInfo.GetFrameToUseForRendering();
	const auto *frameTable = reinterpret_cast<const uint32_t *>(monster.AnimInfo.pCelSprite->Data());
	int frames = SDL_SwapLE32(frameTable[0]);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		Log(
		    "Draw Monster \"{}\" {}: facing {}, frame {} of {}",
		    monster.mName,
		    getMonsterModeDisplayName(monster._mmode),
		    monster._mdir,
		    nCel,
		    frames);
		return;
	}

	const auto &cel = *monster.AnimInfo.pCelSprite;

	if (HasNoneOf(dFlags[tilePosition.x][tilePosition.y], DungeonFlag::Lit)) {
		Cl2DrawLightTbl(out, targetBufferPosition.x, targetBufferPosition.y, cel, nCel, 1);
		return;
	}
	int trans = 0;
	if (monster._uniqtype != 0)
		trans = monster._uniqtrans + 4;
	if (monster._mmode == MonsterMode::Petrified)
		trans = 2;
	if (Players[MyPlayerId]._pInfraFlag && LightTableIndex > 8)
		trans = 1;
	if (trans != 0)
		Cl2DrawLightTbl(out, targetBufferPosition.x, targetBufferPosition.y, cel, nCel, trans);
	else
		Cl2DrawLight(out, targetBufferPosition.x, targetBufferPosition.y, cel, nCel);
}

/**
 * @brief Helper for rendering a specific player icon (Mana Shield or Reflect)
 */
void DrawPlayerIconHelper(const Surface &out, int pnum, missile_graphic_id missileGraphicId, Point position, bool lighting)
{
	position.x += CalculateWidth2(Players[pnum].AnimInfo.pCelSprite->Width()) - MissileSpriteData[missileGraphicId].animWidth2;

	int width = MissileSpriteData[missileGraphicId].animWidth;
	byte *pCelBuff = MissileSpriteData[missileGraphicId].animData[0].get();

	CelSprite cel { pCelBuff, width };

	if (pnum == MyPlayerId) {
		Cl2Draw(out, position.x, position.y, cel, 1);
		return;
	}

	if (lighting) {
		Cl2DrawLightTbl(out, position.x, position.y, cel, 1, 1);
		return;
	}

	Cl2DrawLight(out, position.x, position.y, cel, 1);
}

/**
 * @brief Helper for rendering player icons (Mana Shield and Reflect)
 * @param out Output buffer
 * @param pnum Player id
 * @param position Output buffer coordinates
 * @param lighting Should lighting be applied
 */
void DrawPlayerIcons(const Surface &out, int pnum, Point position, bool lighting)
{
	auto &player = Players[pnum];
	if (player.pManaShield)
		DrawPlayerIconHelper(out, pnum, MFILE_MANASHLD, position, lighting);
	if (player.wReflections > 0)
		DrawPlayerIconHelper(out, pnum, MFILE_REFLECT, position + Displacement { 0, 16 }, lighting);
}

/**
 * @brief Render a player sprite
 * @param out Output buffer
 * @param pnum Player id
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 * @param pCelBuff sprite buffer
 * @param nCel frame
 * @param nWidth width
 */
void DrawPlayer(const Surface &out, int pnum, Point tilePosition, Point targetBufferPosition)
{
	if (HasNoneOf(dFlags[tilePosition.x][tilePosition.y], DungeonFlag::Lit) && !Players[MyPlayerId]._pInfraFlag && leveltype != DTYPE_TOWN) {
		return;
	}

	auto &player = Players[pnum];

	const auto *pCelSprite = player.AnimInfo.pCelSprite;
	int nCel = player.AnimInfo.GetFrameToUseForRendering();

	if (pCelSprite == nullptr) {
		Log("Drawing player {} \"{}\": NULL CelSprite", pnum, player._pName);
		return;
	}

	int frames = SDL_SwapLE32(*reinterpret_cast<const DWORD *>(pCelSprite->Data()));
	if (nCel < 1 || frames > 50 || nCel > frames) {
		const char *szMode = "unknown action";
		if (player._pmode <= PM_QUIT)
			szMode = PlayerModeNames[player._pmode];
		Log(
		    "Drawing player {} \"{}\" {}: facing {}, frame {} of {}",
		    pnum,
		    player._pName,
		    szMode,
		    player._pdir,
		    nCel,
		    frames);
		return;
	}

	if (pnum == pcursplr)
		Cl2DrawOutline(out, 165, targetBufferPosition.x, targetBufferPosition.y, *pCelSprite, nCel);

	if (pnum == MyPlayerId) {
		Cl2Draw(out, targetBufferPosition.x, targetBufferPosition.y, *pCelSprite, nCel);
		DrawPlayerIcons(out, pnum, targetBufferPosition, true);
		return;
	}

	if (HasNoneOf(dFlags[tilePosition.x][tilePosition.y], DungeonFlag::Lit) || (Players[MyPlayerId]._pInfraFlag && LightTableIndex > 8)) {
		Cl2DrawLightTbl(out, targetBufferPosition.x, targetBufferPosition.y, *pCelSprite, nCel, 1);
		DrawPlayerIcons(out, pnum, targetBufferPosition, true);
		return;
	}

	int l = LightTableIndex;
	if (LightTableIndex < 5)
		LightTableIndex = 0;
	else
		LightTableIndex -= 5;

	Cl2DrawLight(out, targetBufferPosition.x, targetBufferPosition.y, *pCelSprite, nCel);
	DrawPlayerIcons(out, pnum, targetBufferPosition, false);

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

	for (int i = 0; i < MAX_PLRS; i++) {
		auto &player = Players[i];
		if (player.plractive && player._pHitPoints == 0 && player.plrlevel == (BYTE)currlevel && player.position.tile == tilePosition) {
			dFlags[tilePosition.x][tilePosition.y] |= DungeonFlag::DeadPlayer;
			const Displacement center { CalculateWidth2(player.AnimInfo.pCelSprite == nullptr ? 96 : player.AnimInfo.pCelSprite->Width()), 0 };
			const Point playerRenderPosition { targetBufferPosition + player.position.offset - center };
			DrawPlayer(out, i, tilePosition, playerRenderPosition);
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

	auto objectId = abs(dObject[tilePosition.x][tilePosition.y]) - 1;
	if (objectId < 0) {
		return;
	}

	Object &objectToDraw = Objects[objectId];
	if (objectToDraw._oPreFlag != pre) {
		return;
	}

	Point screenPosition = targetBufferPosition - Displacement { CalculateWidth2(objectToDraw._oAnimWidth), 0 };
	if (objectToDraw.position != tilePosition) {
		// drawing a large or offset object, calculate the correct position for the center of the sprite
		Displacement screenOffset = objectToDraw.position - tilePosition;
		screenPosition -= screenOffset.WorldToScreen();
	}

	byte *pCelBuff = objectToDraw._oAnimData;
	if (pCelBuff == nullptr) {
		Log("Draw Object type {}: NULL Cel Buffer", objectToDraw._otype);
		return;
	}

	uint32_t nCel = objectToDraw._oAnimFrame;
	uint32_t frames = LoadLE32(pCelBuff);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		Log("Draw Object: frame {} of {}, object type=={}", nCel, frames, objectToDraw._otype);
		return;
	}

	CelSprite cel { objectToDraw._oAnimData, objectToDraw._oAnimWidth };
	if (pcursobj != -1 && &objectToDraw == &Objects[pcursobj]) {
		CelBlitOutlineTo(out, 194, screenPosition, cel, objectToDraw._oAnimFrame);
	}
	if (objectToDraw._oLight) {
		CelClippedDrawLightTo(out, screenPosition, cel, objectToDraw._oAnimFrame);
	} else {
		CelClippedDrawTo(out, screenPosition, cel, objectToDraw._oAnimFrame);
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
	MICROS *pMap = &dpiece_defs_map_2[tilePosition.x][tilePosition.y];
	level_piece_id = dPiece[tilePosition.x][tilePosition.y];
	cel_transparency_active = nTransTable[level_piece_id] && TransList[dTransVal[tilePosition.x][tilePosition.y]];
	cel_foliage_active = !nSolidTable[level_piece_id];
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
	level_cel_block = dpiece_defs_map_2[tilePosition.x][tilePosition.y].mt[0];
	if (level_cel_block != 0) {
		RenderTile(out, targetBufferPosition);
	}
	arch_draw_type = 2; // Right
	level_cel_block = dpiece_defs_map_2[tilePosition.x][tilePosition.y].mt[1];
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

	const auto *cel = item.AnimInfo.pCelSprite;
	if (cel == nullptr) {
		Log("Draw Item \"{}\" 1: NULL CelSprite", item._iIName);
		return;
	}

	int nCel = item.AnimInfo.GetFrameToUseForRendering();
	int frames = SDL_SwapLE32(*(DWORD *)cel->Data());
	if (nCel < 1 || frames > 50 || nCel > frames) {
		Log("Draw \"{}\" Item 1: frame {} of {}, item type=={}", item._iIName, nCel, frames, item._itype);
		return;
	}

	int px = targetBufferPosition.x - CalculateWidth2(cel->Width());
	const Point position { px, targetBufferPosition.y };
	if (bItem - 1 == pcursitem || AutoMapShowItems) {
		CelBlitOutlineTo(out, GetOutlineColor(item, false), position, *cel, nCel);
	}
	CelClippedDrawLightTo(out, position, *cel, nCel);
	if (item.AnimInfo.CurrentFrame == item.AnimInfo.NumberOfFrames || item._iCurs == ICURS_MAGIC_ROCK)
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
	mi = mi > 0 ? mi - 1 : -(mi + 1);

	if (leveltype == DTYPE_TOWN) {
		auto &towner = Towners[mi];
		int px = targetBufferPosition.x - CalculateWidth2(towner._tAnimWidth);
		const Point position { px, targetBufferPosition.y };
		if (mi == pcursmonst) {
			CelBlitOutlineTo(out, 166, position, CelSprite(towner._tAnimData, towner._tAnimWidth), towner._tAnimFrame);
		}
		assert(towner._tAnimData);
		CelClippedDrawTo(out, position, CelSprite(towner._tAnimData, towner._tAnimWidth), towner._tAnimFrame);
		return;
	}

	if (HasNoneOf(dFlags[tilePosition.x][tilePosition.y], DungeonFlag::Lit) && !Players[MyPlayerId]._pInfraFlag)
		return;

	if (mi < 0 || mi >= MAXMONSTERS) {
		Log("Draw Monster: tried to draw illegal monster {}", mi);
		return;
	}

	const auto &monster = Monsters[mi];
	if ((monster._mFlags & MFLAG_HIDDEN) != 0) {
		return;
	}

	if (monster.MType == nullptr) {
		Log("Draw Monster \"{}\": uninitialized monster", monster.mName);
		return;
	}

	const CelSprite &cel = *monster.AnimInfo.pCelSprite;

	Displacement offset = monster.position.offset;
	if (monster.IsWalking()) {
		offset = GetOffsetForWalking(monster.AnimInfo, monster._mdir);
	}

	const Point monsterRenderPosition { targetBufferPosition + offset - Displacement { CalculateWidth2(cel.Width()), 0 } };
	if (mi == pcursmonst) {
		Cl2DrawOutline(out, 233, monsterRenderPosition.x, monsterRenderPosition.y, cel, monster.AnimInfo.GetFrameToUseForRendering());
	}
	DrawMonster(out, tilePosition, monsterRenderPosition, monster);
}

/**
 * @brief Check if and how a player should be rendered
 * @param out Output buffer
 * @param tilePosition dPiece coordinates
 * @param targetBufferPosition Output buffer coordinates
 */
void DrawPlayerHelper(const Surface &out, Point tilePosition, Point targetBufferPosition)
{
	int8_t p = dPlayer[tilePosition.x][tilePosition.y];
	p = p > 0 ? p - 1 : -(p + 1);

	if (p < 0 || p >= MAX_PLRS) {
		Log("draw player: tried to draw illegal player {}", p);
		return;
	}
	auto &player = Players[p];

	Displacement offset = player.position.offset;
	if (player.IsWalking()) {
		offset = GetOffsetForWalking(player.AnimInfo, player._pdir);
	}

	const Displacement center { CalculateWidth2(player.AnimInfo.pCelSprite == nullptr ? 96 : player.AnimInfo.pCelSprite->Width()), 0 };
	const Point playerRenderPosition { targetBufferPosition + offset - center };

	DrawPlayer(out, p, tilePosition, playerRenderPosition);
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

	if (dRendered[tilePosition.x][tilePosition.y])
		return;
	dRendered[tilePosition.x][tilePosition.y] = true;

	LightTableIndex = dLight[tilePosition.x][tilePosition.y];

	DrawCell(out, tilePosition, targetBufferPosition);

	DungeonFlag bFlag = dFlags[tilePosition.x][tilePosition.y];
	int8_t bDead = dCorpse[tilePosition.x][tilePosition.y];
	int8_t bMap = dTransVal[tilePosition.x][tilePosition.y];

#ifdef _DEBUG
	if (DebugVision && HasAnyOf(bFlag, DungeonFlag::Lit)) {
		CelClippedDrawTo(out, targetBufferPosition, *pSquareCel, 1);
	}
	DebugCoordsMap[tilePosition.x + tilePosition.y * MAXDUNX] = targetBufferPosition;
#endif

	if (MissilePreFlag) {
		DrawMissile(out, tilePosition, targetBufferPosition, true);
	}

	if (LightTableIndex < LightsMax && bDead != 0) {
		do {
			Corpse *pDeadGuy = &Corpses[(bDead & 0x1F) - 1];
			int px = targetBufferPosition.x - CalculateWidth2(pDeadGuy->width);
			const byte *pCelBuff = pDeadGuy->data[(bDead >> 5) & 7];
			assert(pCelBuff != nullptr);
			const auto *frameTable = reinterpret_cast<const uint32_t *>(pCelBuff);
			int frames = SDL_SwapLE32(frameTable[0]);
			int nCel = pDeadGuy->frame;
			if (nCel < 1 || frames > 50 || nCel > frames) {
				Log("Unclipped dead: frame {} of {}, deadnum=={}", nCel, frames, (bDead & 0x1F) - 1);
				break;
			}
			if (pDeadGuy->translationPaletteIndex != 0) {
				Cl2DrawLightTbl(out, px, targetBufferPosition.y, CelSprite(pCelBuff, pDeadGuy->width), nCel, pDeadGuy->translationPaletteIndex);
			} else {
				Cl2DrawLight(out, px, targetBufferPosition.y, CelSprite(pCelBuff, pDeadGuy->width), nCel);
			}
		} while (false);
	}
	DrawObject(out, tilePosition, targetBufferPosition, true);
	DrawItem(out, tilePosition, targetBufferPosition, true);

	if (HasAnyOf(bFlag, DungeonFlag::DeadPlayer)) {
		DrawDeadPlayer(out, tilePosition, targetBufferPosition);
	}
	if (dPlayer[tilePosition.x][tilePosition.y] > 0) {
		DrawPlayerHelper(out, tilePosition, targetBufferPosition);
	}
	if (dMonster[tilePosition.x][tilePosition.y] > 0) {
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
			if (GetAsyncKeyState(DVL_VK_MENU)) {
				cel_transparency_active = false; // Turn transparency off here for debugging
			}
#endif
			CelClippedBlitLightTransTo(out, targetBufferPosition, *pSpecialCels, bArch);
#ifdef _DEBUG
			if (GetAsyncKeyState(DVL_VK_MENU)) {
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
				CelDrawTo(out, targetBufferPosition + Displacement { 0, -TILE_HEIGHT }, *pSpecialCels, bArch);
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
				level_piece_id = dPiece[tilePosition.x][tilePosition.y];
				if (level_piece_id != 0) {
					if (!nSolidTable[level_piece_id])
						DrawFloor(out, tilePosition, targetBufferPosition);
				} else {
					world_draw_black_tile(out, targetBufferPosition.x, targetBufferPosition.y);
				}
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

#define IsWall(x, y) (dPiece[x][y] == 0 || nSolidTable[dPiece[x][y]] || dSpecial[x][y] != 0)
#define IsWalkable(x, y) (dPiece[x][y] != 0 && IsTileNotSolid({ x, y }))

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
	memset(dRendered, 0, sizeof(dRendered));

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (InDungeonBounds(tilePosition)) {
				if (tilePosition.x + 1 < MAXDUNX && tilePosition.y - 1 >= 0 && targetBufferPosition.x + TILE_WIDTH <= gnScreenWidth) {
					// Render objects behind walls first to prevent sprites, that are moving
					// between tiles, from poking through the walls as they exceed the tile bounds.
					// A proper fix for this would probably be to layout the sceen and render by
					// sprite screen position rather than tile position.
					if (IsWall(tilePosition.x, tilePosition.y) && (IsWall(tilePosition.x + 1, tilePosition.y) || (tilePosition.x > 0 && IsWall(tilePosition.x - 1, tilePosition.y)))) { // Part of a wall aligned on the x-axis
						if (IsWalkable(tilePosition.x + 1, tilePosition.y - 1) && IsWalkable(tilePosition.x, tilePosition.y - 1)) {                                                     // Has walkable area behind it
							DrawDungeon(out, tilePosition + Direction::East, { targetBufferPosition.x + TILE_WIDTH, targetBufferPosition.y });
						}
					}
				}
				if (dPiece[tilePosition.x][tilePosition.y] != 0) {
					DrawDungeon(out, tilePosition, targetBufferPosition);
				}
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
		if (chrflag || QuestLogIsOpen) {
			viewportWidth -= SPANEL_WIDTH;
			viewportOffsetX = SPANEL_WIDTH;
		} else if (invflag || sbookflag) {
			viewportWidth -= SPANEL_WIDTH;
		}
	}

	// We round to even for the source width and height.
	// If the width / height was odd, we copy just one extra pixel / row later on.
	const int srcWidth = (viewportWidth + 1) / 2;
	const int doubleableWidth = viewportWidth / 2;
	const int srcHeight = (out.h() + 1) / 2;
	const int doubleableHeight = out.h() / 2;

	BYTE *src = out.at(srcWidth - 1, srcHeight - 1);
	BYTE *dst = out.at(viewportOffsetX + viewportWidth - 1, out.h() - 1);
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
 * @param full_out Buffer to render to
 * @param position Center of view in dPiece coordinate
 */
void DrawGame(const Surface &fullOut, Point position)
{
	// Limit rendering to the view area
	const Surface &out = zoomflag
	    ? fullOut.subregionY(0, gnViewportHeight)
	    : fullOut.subregionY(0, (gnViewportHeight + 1) / 2);

	// Adjust by player offset and tile grid alignment
	auto &myPlayer = Players[MyPlayerId];
	Displacement offset = ScrollInfo.offset;
	if (myPlayer.IsWalking())
		offset = GetOffsetForWalking(myPlayer.AnimInfo, myPlayer._pdir, true);
	int sx = offset.deltaX + tileOffset.deltaX;
	int sy = offset.deltaY + tileOffset.deltaY;

	int columns = tileColums;
	int rows = tileRows;

	position += tileShift;

	// Skip rendering parts covered by the panels
	if (CanPanelsCoverView()) {
		if (zoomflag) {
			if (chrflag || QuestLogIsOpen) {
				position += Displacement(Direction::East) * 2;
				columns -= 4;
				sx += SPANEL_WIDTH - TILE_WIDTH / 2;
			}
			if (invflag || sbookflag) {
				position += Displacement(Direction::East) * 2;
				columns -= 4;
				sx += -TILE_WIDTH / 2;
			}
		} else {
			if (chrflag || QuestLogIsOpen) {
				position += Direction::East;
				columns -= 2;
				sx += -TILE_WIDTH / 2 / 2; // SPANEL_WIDTH accounted for in Zoom()
			}
			if (invflag || sbookflag) {
				position += Direction::East;
				columns -= 2;
				sx += -TILE_WIDTH / 2 / 2;
			}
		}
	}

	UpdateMissilesRendererData();

	// Draw areas moving in and out of the screen
	switch (ScrollInfo._sdir) {
	case ScrollDirection::North:
		sy -= TILE_HEIGHT;
		position += Direction::North;
		rows += 2;
		break;
	case ScrollDirection::NorthEast:
		sy -= TILE_HEIGHT;
		position += Direction::North;
		columns++;
		rows += 2;
		break;
	case ScrollDirection::East:
		columns++;
		break;
	case ScrollDirection::SouthEast:
		columns++;
		rows++;
		break;
	case ScrollDirection::South:
		rows += 2;
		break;
	case ScrollDirection::SouthWest:
		sx -= TILE_WIDTH;
		position += Direction::West;
		columns++;
		rows++;
		break;
	case ScrollDirection::West:
		sx -= TILE_WIDTH;
		position += Direction::West;
		columns++;
		break;
	case ScrollDirection::NorthWest:
		sx -= TILE_WIDTH / 2;
		sy -= TILE_HEIGHT / 2;
		position += Direction::NorthWest;
		columns++;
		rows++;
		break;
	case ScrollDirection::None:
		break;
	}

	DrawFloor(out, position, { sx, sy }, rows, columns);
	DrawTileContent(out, position, { sx, sy }, rows, columns);

	if (!zoomflag) {
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
			if (!zoomflag)
				pixelCoords *= 2;
			if (debugGridTextNeeded && GetDebugGridText(dunCoords, debugGridTextBuffer)) {
				Size tileSize = { TILE_WIDTH, TILE_HEIGHT };
				if (!zoomflag)
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
				if (!zoomflag) {
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
	}
#ifndef VIRTUAL_GAMEPAD
	if (!chrflag && Players[MyPlayerId]._pStatPts != 0 && !spselflag
	    && (!QuestLogIsOpen || !LeftPanel.Contains(MainPanel.position + Displacement { 0, -74 }))) {
		DrawLevelUpIcon(out);
	}
#endif
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
	if (HelpFlag) {
		DrawHelp(out);
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
	char string[12];

	if (!frameflag || !gbActive) {
		return;
	}

	frameend++;
	uint32_t tc = SDL_GetTicks();
	uint32_t frames = tc - framestart;
	if (tc - framestart >= 1000) {
		framestart = tc;
		framerate = 1000 * frameend / frames;
		frameend = 0;
	}
	snprintf(string, 12, "%i FPS", framerate);
	DrawString(out, string, Point { 8, 53 }, UiFlags::ColorRed);
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
 * @param draw_desc Render info box
 * @param draw_hp Render health bar
 * @param draw_mana Render mana bar
 * @param draw_sbar Render belt
 * @param draw_btn Render panel buttons
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
		if (drawSbar) {
			DoBlitScreen(PANEL_LEFT + 204, PANEL_TOP + 5, 232, 28);
		}
		if (drawDesc) {
			DoBlitScreen(PANEL_LEFT + 176, PANEL_TOP + 46, 288, 60);
		}
		if (drawMana) {
			DoBlitScreen(PANEL_LEFT + 460, PANEL_TOP, 88, 72);
			DoBlitScreen(PANEL_LEFT + 564, PANEL_TOP + 64, 56, 56);
		}
		if (drawHp) {
			DoBlitScreen(PANEL_LEFT + 96, PANEL_TOP, 88, 72);
		}
		if (drawBtn) {
			DoBlitScreen(PANEL_LEFT + 8, PANEL_TOP + 5, 72, 119);
			DoBlitScreen(PANEL_LEFT + 556, PANEL_TOP + 5, 72, 48);
			if (gbIsMultiplayer) {
				DoBlitScreen(PANEL_LEFT + 84, PANEL_TOP + 91, 36, 32);
				DoBlitScreen(PANEL_LEFT + 524, PANEL_TOP + 91, 36, 32);
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

	float fAnimationProgress = animationInfo.GetAnimationProgress();
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
	if (gnScreenWidth <= PANEL_WIDTH) {
		return 0;
	}

	int rows = PANEL_HEIGHT / TILE_HEIGHT;
	if (!zoomflag) {
		rows /= 2;
	}

	return rows;
}

void CalcTileOffset(int *offsetX, int *offsetY)
{
	int x;
	int y;

	if (zoomflag) {
		x = gnScreenWidth % TILE_WIDTH;
		y = gnViewportHeight % TILE_HEIGHT;
	} else {
		x = (gnScreenWidth / 2) % TILE_WIDTH;
		y = (gnViewportHeight / 2) % TILE_HEIGHT;
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
	int columns = gnScreenWidth / TILE_WIDTH;
	if ((gnScreenWidth % TILE_WIDTH) != 0) {
		columns++;
	}
	int rows = gnViewportHeight / TILE_HEIGHT;
	if ((gnViewportHeight % TILE_HEIGHT) != 0) {
		rows++;
	}

	if (!zoomflag) {
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
	if (!zoomflag) {
		tileOffset.deltaY += TILE_HEIGHT / 4;
		if (yo < TILE_HEIGHT / 4)
			tileRows++;
	}

	tileRows++; // Cover lower edge saw tooth, right edge accounted for in scrollrt_draw()
}

extern SDL_Surface *PalSurface;

void ClearScreenBuffer()
{
	lock_buf(3);

	assert(PalSurface != nullptr);
	SDL_FillRect(PalSurface, nullptr, 0);

	unlock_buf(3);
}

#ifdef _DEBUG
void ScrollView()
{
	bool scroll;

	if (pcurs >= CURSOR_FIRSTITEM)
		return;

	scroll = false;

	if (MousePosition.x < 20) {
		if (dmaxPosition.y - 1 <= ViewPosition.y || dminPosition.x >= ViewPosition.x) {
			if (dmaxPosition.y - 1 > ViewPosition.y) {
				ViewPosition.y++;
				scroll = true;
			}
			if (dminPosition.x < ViewPosition.x) {
				ViewPosition.x--;
				scroll = true;
			}
		} else {
			ViewPosition.y++;
			ViewPosition.x--;
			scroll = true;
		}
	}
	if (MousePosition.x > gnScreenWidth - 20) {
		if (dmaxPosition.x - 1 <= ViewPosition.x || dminPosition.y >= ViewPosition.y) {
			if (dmaxPosition.x - 1 > ViewPosition.x) {
				ViewPosition.x++;
				scroll = true;
			}
			if (dminPosition.y < ViewPosition.y) {
				ViewPosition.y--;
				scroll = true;
			}
		} else {
			ViewPosition.y--;
			ViewPosition.x++;
			scroll = true;
		}
	}
	if (MousePosition.y < 20) {
		if (dminPosition.y >= ViewPosition.y || dminPosition.x >= ViewPosition.x) {
			if (dminPosition.y < ViewPosition.y) {
				ViewPosition.y--;
				scroll = true;
			}
			if (dminPosition.x < ViewPosition.x) {
				ViewPosition.x--;
				scroll = true;
			}
		} else {
			ViewPosition.x--;
			ViewPosition.y--;
			scroll = true;
		}
	}
	if (MousePosition.y > gnScreenHeight - 20) {
		if (dmaxPosition.y - 1 <= ViewPosition.y || dmaxPosition.x - 1 <= ViewPosition.x) {
			if (dmaxPosition.y - 1 > ViewPosition.y) {
				ViewPosition.y++;
				scroll = true;
			}
			if (dmaxPosition.x - 1 > ViewPosition.x) {
				ViewPosition.x++;
				scroll = true;
			}
		} else {
			ViewPosition.x++;
			ViewPosition.y++;
			scroll = true;
		}
	}

	if (scroll)
		ScrollInfo._sdir = ScrollDirection::None;
}
#endif

void EnableFrameCount()
{
	frameflag = true;
	framestart = SDL_GetTicks();
}

void scrollrt_draw_game_screen()
{
	int hgt = 0;

	if (force_redraw == 255) {
		force_redraw = 0;
		hgt = gnScreenHeight;
	}

	if (IsHardwareCursor()) {
		SetHardwareCursorVisible(ShouldShowCursor());
	} else {
		lock_buf(0);
		DrawCursor(GlobalBackBuffer());
		unlock_buf(0);
	}

	DrawMain(hgt, false, false, false, false, false);

	RenderPresent();

	if (!IsHardwareCursor()) {
		lock_buf(0);
		UndrawCursor(GlobalBackBuffer());
		unlock_buf(0);
	}
}

void DrawAndBlit()
{
	if (!gbRunGame) {
		return;
	}

	int hgt = 0;
	bool ddsdesc = false;
	bool ctrlPan = false;

	if (gnScreenWidth > PANEL_WIDTH || force_redraw == 255 || IsHighlightingLabelsEnabled()) {
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

	lock_buf(0);
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

	if (IsHardwareCursor()) {
		SetHardwareCursorVisible(ShouldShowCursor());
	} else {
		DrawCursor(out);
	}

	DrawFPS(out);

	unlock_buf(0);

	DrawMain(hgt, ddsdesc, drawhpflag, drawmanaflag, drawsbarflag, drawbtnflag);

	RenderPresent();

	drawhpflag = false;
	drawmanaflag = false;
	drawbtnflag = false;
	drawsbarflag = false;
}

} // namespace devilution

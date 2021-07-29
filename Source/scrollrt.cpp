/**
 * @file scrollrt.cpp
 *
 * Implementation of functionality for rendering the dungeons, monsters and calling other render routines.
 */

#include "automap.h"
#include "cursor.h"
#include "dead.h"
#include "doom.h"
#include "dx.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/cl2_render.hpp"
#include "engine/render/dun_render.hpp"
#include "engine/render/text_render.hpp"
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
std::unordered_multimap<Point, MissileStruct *, PointHash> MissilesAtRenderingTile;

/**
 * @brief Could the missile (at the next game tick) collide? This method is a simplified version of CheckMissileCol (for example without random).
 */
bool CouldMissileCollide(Point tile, bool checkPlayerAndMonster)
{
	if (tile.x >= MAXDUNX || tile.x < 0)
		return true;
	if (tile.y >= MAXDUNY || tile.y < 0)
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

void UpdateMissileRendererData(MissileStruct &m)
{
	m.position.tileForRendering = m.position.tile;
	m.position.offsetForRendering = m.position.offset;

	const MissileMovementDistrubution missileMovement = MissileData[m._mitype].MovementDistribution;
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
		MissileStruct &m = Missiles[ActiveMissiles[i]];
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

const char *const MonsterModeNames[] = {
	"standing",
	"walking (1)",
	"walking (2)",
	"walking (3)",
	"attacking",
	"getting hit",
	"dying",
	"attacking (special)",
	"fading in",
	"fading out",
	"attacking (ranged)",
	"standing (special)",
	"attacking (special ranged)",
	"delaying",
	"charging",
	"stoned",
	"healing",
	"talking"
};

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
	return !(sgbControllerActive && !IsMovingMouseCursorWithController() && pcurs != CURSOR_TELEPORT && !invflag && (!chrflag || Players[MyPlayerId]._pStatPts <= 0));
}

/**
 * @brief Save the content behind the cursor to a temporary buffer, then draw the cursor.
 */
void DrawCursor(const Surface &out)
{
	if (pcurs <= CURSOR_NONE || cursW == 0 || cursH == 0 || !ShouldShowCursor()) {
		return;
	}

	// Copy the buffer before the item cursor and its 1px outline are drawn to a temporary buffer.
	const int outlineWidth = IsItemSprite(pcurs) ? 1 : 0;

	if (MousePosition.x < -cursW - outlineWidth || MousePosition.x - outlineWidth >= out.w() || MousePosition.y < -cursH - outlineWidth || MousePosition.y - outlineWidth >= out.h())
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
	sgdwCursWdt = cursW + 2 * outlineWidth;
	Clip(sgdwCursX, sgdwCursWdt, out.w());

	sgdwCursY = MousePosition.y - outlineWidth;
	sgdwCursHgt = cursH + 2 * outlineWidth;
	Clip(sgdwCursY, sgdwCursHgt, out.h());

	BlitCursor(sgSaveBack, sgdwCursWdt, out.at(sgdwCursX, sgdwCursY), out.pitch());
	CelDrawCursor(out, MousePosition + Displacement { 0, cursH - 1 }, pcurs);
}

/**
 * @brief Render a missile sprite
 * @param out Output buffer
 * @param m Pointer to MissileStruct struct
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pre Is the sprite in the background
 */
void DrawMissilePrivate(const Surface &out, const MissileStruct *m, int sx, int sy, bool pre)
{
	if (m->_miPreFlag != pre || !m->_miDrawFlag)
		return;

	if (m->_miAnimData == nullptr) {
		Log("Draw Missile 2 type {}: NULL Cel Buffer", m->_mitype);
		return;
	}
	int nCel = m->_miAnimFrame;
	const auto *frameTable = reinterpret_cast<const uint32_t *>(m->_miAnimData);
	int frames = SDL_SwapLE32(frameTable[0]);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		Log("Draw Missile 2: frame {} of {}, missile type=={}", nCel, frames, m->_mitype);
		return;
	}
	int mx = sx + m->position.offsetForRendering.deltaX - m->_miAnimWidth2;
	int my = sy + m->position.offsetForRendering.deltaY;
	CelSprite cel { m->_miAnimData, m->_miAnimWidth };
	if (m->_miUniqTrans != 0)
		Cl2DrawLightTbl(out, mx, my, cel, m->_miAnimFrame, m->_miUniqTrans + 3);
	else if (m->_miLightFlag)
		Cl2DrawLight(out, mx, my, cel, m->_miAnimFrame);
	else
		Cl2Draw(out, mx, my, cel, m->_miAnimFrame);
}

/**
 * @brief Render a missile sprites for a given tile
 * @param out Output buffer
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pre Is the sprite in the background
 */
void DrawMissile(const Surface &out, int x, int y, int sx, int sy, bool pre)
{
	const auto range = MissilesAtRenderingTile.equal_range(Point { x, y });
	for (auto it = range.first; it != range.second; it++) {
		DrawMissilePrivate(out, it->second, sx, sy, pre);
	}
}

/**
 * @brief Render a monster sprite
 * @param out Output buffer
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param mx Output buffer coordinate
 * @param my Output buffer coordinate
 * @param m Id of monster
 */
void DrawMonster(const Surface &out, int x, int y, int mx, int my, const MonsterStruct &monster)
{
	if (monster.AnimInfo.pCelSprite == nullptr) {
		Log("Draw Monster \"{}\": NULL Cel Buffer", monster.mName);
		return;
	}

	int nCel = monster.AnimInfo.GetFrameToUseForRendering();
	const auto *frameTable = reinterpret_cast<const uint32_t *>(monster.AnimInfo.pCelSprite->Data());
	int frames = SDL_SwapLE32(frameTable[0]);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		const char *szMode = "unknown action";
		if (monster._mmode <= 17)
			szMode = MonsterModeNames[monster._mmode];
		Log(
		    "Draw Monster \"{}\" {}: facing {}, frame {} of {}",
		    monster.mName,
		    szMode,
		    monster._mdir,
		    nCel,
		    frames);
		return;
	}

	const auto &cel = *monster.AnimInfo.pCelSprite;

	if ((dFlags[x][y] & BFLAG_LIT) == 0) {
		Cl2DrawLightTbl(out, mx, my, cel, nCel, 1);
		return;
	}
	int trans = 0;
	if (monster._uniqtype != 0)
		trans = monster._uniqtrans + 4;
	if (monster._mmode == MM_STONE)
		trans = 2;
	if (Players[MyPlayerId]._pInfraFlag && LightTableIndex > 8)
		trans = 1;
	if (trans != 0)
		Cl2DrawLightTbl(out, mx, my, cel, nCel, trans);
	else
		Cl2DrawLight(out, mx, my, cel, nCel);
}

/**
 * @brief Helper for rendering a specific player icon (Mana Shield or Reflect)
 */
void DrawPlayerIconHelper(const Surface &out, int pnum, missile_graphic_id missileGraphicId, int x, int y, bool lighting)
{
	x += CalculateWidth2(Players[pnum].AnimInfo.pCelSprite->Width()) - MissileSpriteData[missileGraphicId].mAnimWidth2[0];

	int width = MissileSpriteData[missileGraphicId].mAnimWidth[0];
	byte *pCelBuff = MissileSpriteData[missileGraphicId].mAnimData[0];

	CelSprite cel { pCelBuff, width };

	if (pnum == MyPlayerId) {
		Cl2Draw(out, x, y, cel, 1);
		return;
	}

	if (lighting) {
		Cl2DrawLightTbl(out, x, y, cel, 1, 1);
		return;
	}

	Cl2DrawLight(out, x, y, cel, 1);
}

/**
 * @brief Helper for rendering player icons (Mana Shield and Reflect)
 * @param out Output buffer
 * @param pnum Player id
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param lighting Should lighting be applied
 */
void DrawPlayerIcons(const Surface &out, int pnum, int x, int y, bool lighting)
{
	auto &player = Players[pnum];
	if (player.pManaShield)
		DrawPlayerIconHelper(out, pnum, MFILE_MANASHLD, x, y, lighting);
	if (player.wReflections > 0)
		DrawPlayerIconHelper(out, pnum, MFILE_REFLECT, x, y + 16, lighting);
}

/**
 * @brief Render a player sprite
 * @param out Output buffer
 * @param pnum Player id
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param px Output buffer coordinate
 * @param py Output buffer coordinate
 * @param pCelBuff sprite buffer
 * @param nCel frame
 * @param nWidth width
 */
void DrawPlayer(const Surface &out, int pnum, int x, int y, int px, int py)
{
	if ((dFlags[x][y] & BFLAG_LIT) == 0 && !Players[MyPlayerId]._pInfraFlag && leveltype != DTYPE_TOWN) {
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
		Cl2DrawOutline(out, 165, px, py, *pCelSprite, nCel);

	if (pnum == MyPlayerId) {
		Cl2Draw(out, px, py, *pCelSprite, nCel);
		DrawPlayerIcons(out, pnum, px, py, true);
		return;
	}

	if ((dFlags[x][y] & BFLAG_LIT) == 0 || (Players[MyPlayerId]._pInfraFlag && LightTableIndex > 8)) {
		Cl2DrawLightTbl(out, px, py, *pCelSprite, nCel, 1);
		DrawPlayerIcons(out, pnum, px, py, true);
		return;
	}

	int l = LightTableIndex;
	if (LightTableIndex < 5)
		LightTableIndex = 0;
	else
		LightTableIndex -= 5;

	Cl2DrawLight(out, px, py, *pCelSprite, nCel);
	DrawPlayerIcons(out, pnum, px, py, false);

	LightTableIndex = l;
}

/**
 * @brief Render a player sprite
 * @param out Output buffer
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 */
void DrawDeadPlayer(const Surface &out, int x, int y, int sx, int sy)
{
	dFlags[x][y] &= ~BFLAG_DEAD_PLAYER;

	for (int i = 0; i < MAX_PLRS; i++) {
		auto &player = Players[i];
		if (player.plractive && player._pHitPoints == 0 && player.plrlevel == (BYTE)currlevel && player.position.tile.x == x && player.position.tile.y == y) {
			dFlags[x][y] |= BFLAG_DEAD_PLAYER;
			int px = sx + player.position.offset.deltaX - CalculateWidth2(player.AnimInfo.pCelSprite == nullptr ? 96 : player.AnimInfo.pCelSprite->Width());
			int py = sy + player.position.offset.deltaY;
			DrawPlayer(out, i, x, y, px, py);
		}
	}
}

/**
 * @brief Render an object sprite
 * @param out Output buffer
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param ox Output buffer coordinate
 * @param oy Output buffer coordinate
 * @param pre Is the sprite in the background
 */
void DrawObject(const Surface &out, int x, int y, int ox, int oy, bool pre)
{
	int8_t bv = dObject[x][y];
	if (bv == 0 || LightTableIndex >= LightsMax)
		return;

	Point objectPosition {};

	if (bv > 0) {
		bv = bv - 1;
		if (Objects[bv]._oPreFlag != pre)
			return;
		objectPosition.x = ox - CalculateWidth2(Objects[bv]._oAnimWidth);
		objectPosition.y = oy;
	} else {
		bv = -(bv + 1);
		if (Objects[bv]._oPreFlag != pre)
			return;
		int xx = Objects[bv].position.x - x;
		int yy = Objects[bv].position.y - y;
		objectPosition.x = (xx * TILE_WIDTH / 2) + ox - CalculateWidth2(Objects[bv]._oAnimWidth) - (yy * TILE_WIDTH / 2);
		objectPosition.y = oy + (yy * TILE_HEIGHT / 2) + (xx * TILE_HEIGHT / 2);
	}

	assert(bv >= 0 && bv < MAXOBJECTS);

	byte *pCelBuff = Objects[bv]._oAnimData;
	if (pCelBuff == nullptr) {
		Log("Draw Object type {}: NULL Cel Buffer", Objects[bv]._otype);
		return;
	}

	uint32_t nCel = Objects[bv]._oAnimFrame;
	uint32_t frames = LoadLE32(pCelBuff);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		Log("Draw Object: frame {} of {}, object type=={}", nCel, frames, Objects[bv]._otype);
		return;
	}

	CelSprite cel { Objects[bv]._oAnimData, Objects[bv]._oAnimWidth };
	if (bv == pcursobj)
		CelBlitOutlineTo(out, 194, objectPosition, cel, Objects[bv]._oAnimFrame);
	if (Objects[bv]._oLight) {
		CelClippedDrawLightTo(out, objectPosition, cel, Objects[bv]._oAnimFrame);
	} else {
		CelClippedDrawTo(out, objectPosition, cel, Objects[bv]._oAnimFrame);
	}
}

static void DrawDungeon(const Surface & /*out*/, int /*sx*/, int /*sy*/, int /*dx*/, int /*dy*/);

/**
 * @brief Render a cell
 * @param out Target buffer
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 */
void DrawCell(const Surface &out, int x, int y, int sx, int sy)
{
	MICROS *pMap = &dpiece_defs_map_2[x][y];
	level_piece_id = dPiece[x][y];
	cel_transparency_active = nTransTable[level_piece_id] && TransList[dTransVal[x][y]];
	cel_foliage_active = !nSolidTable[level_piece_id];
	for (int i = 0; i < (MicroTileLen / 2); i++) {
		level_cel_block = pMap->mt[2 * i];
		if (level_cel_block != 0) {
			arch_draw_type = i == 0 ? 1 : 0;
			RenderTile(out, sx, sy);
		}
		level_cel_block = pMap->mt[2 * i + 1];
		if (level_cel_block != 0) {
			arch_draw_type = i == 0 ? 2 : 0;
			RenderTile(out, sx + TILE_WIDTH / 2, sy);
		}
		sy -= TILE_HEIGHT;
	}
	cel_foliage_active = false;
}

/**
 * @brief Render a floor tiles
 * @param out Target buffer
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 */
void DrawFloor(const Surface &out, int x, int y, int sx, int sy)
{
	cel_transparency_active = false;
	LightTableIndex = dLight[x][y];

	arch_draw_type = 1; // Left
	level_cel_block = dpiece_defs_map_2[x][y].mt[0];
	if (level_cel_block != 0) {
		RenderTile(out, sx, sy);
	}
	arch_draw_type = 2; // Right
	level_cel_block = dpiece_defs_map_2[x][y].mt[1];
	if (level_cel_block != 0) {
		RenderTile(out, sx + TILE_WIDTH / 2, sy);
	}
}

/**
 * @brief Draw item for a given tile
 * @param out Output buffer
 * @param y dPiece coordinate
 * @param x dPiece coordinate
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pre Is the sprite in the background
 */
void DrawItem(const Surface &out, int x, int y, int sx, int sy, bool pre)
{
	int8_t bItem = dItem[x][y];

	if (bItem <= 0)
		return;

	ItemStruct *pItem = &Items[bItem - 1];
	if (pItem->_iPostDraw == pre)
		return;

	const auto *cel = pItem->AnimInfo.pCelSprite;
	if (cel == nullptr) {
		Log("Draw Item \"{}\" 1: NULL CelSprite", pItem->_iIName);
		return;
	}

	int nCel = pItem->AnimInfo.GetFrameToUseForRendering();
	int frames = SDL_SwapLE32(*(DWORD *)cel->Data());
	if (nCel < 1 || frames > 50 || nCel > frames) {
		Log("Draw \"{}\" Item 1: frame {} of {}, item type=={}", pItem->_iIName, nCel, frames, pItem->_itype);
		return;
	}

	int px = sx - CalculateWidth2(cel->Width());
	const Point position { px, sy };
	if (bItem - 1 == pcursitem || AutoMapShowItems) {
		CelBlitOutlineTo(out, GetOutlineColor(*pItem, false), position, *cel, nCel);
	}
	CelClippedDrawLightTo(out, position, *cel, nCel);
	if (pItem->AnimInfo.CurrentFrame == pItem->AnimInfo.NumberOfFrames || pItem->_iCurs == ICURS_MAGIC_ROCK)
		AddItemToLabelQueue(bItem - 1, px, sy);
}

/**
 * @brief Check if and how a monster should be rendered
 * @param out Output buffer
 * @param y dPiece coordinate
 * @param x dPiece coordinate
 * @param oy dPiece Y offset
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 */
void DrawMonsterHelper(const Surface &out, int x, int y, int oy, int sx, int sy)
{
	int mi = dMonster[x][y + oy];
	mi = mi > 0 ? mi - 1 : -(mi + 1);

	if (leveltype == DTYPE_TOWN) {
		auto &towner = Towners[mi];
		int px = sx - CalculateWidth2(towner._tAnimWidth);
		const Point position { px, sy };
		if (mi == pcursmonst) {
			CelBlitOutlineTo(out, 166, position, CelSprite(towner._tAnimData, towner._tAnimWidth), towner._tAnimFrame);
		}
		assert(towner._tAnimData);
		CelClippedDrawTo(out, position, CelSprite(towner._tAnimData, towner._tAnimWidth), towner._tAnimFrame);
		return;
	}

	if ((dFlags[x][y] & BFLAG_LIT) == 0 && !Players[MyPlayerId]._pInfraFlag)
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

	int px = sx + offset.deltaX - CalculateWidth2(cel.Width());
	int py = sy + offset.deltaY;
	if (mi == pcursmonst) {
		Cl2DrawOutline(out, 233, px, py, cel, monster.AnimInfo.GetFrameToUseForRendering());
	}
	DrawMonster(out, x, y, px, py, monster);
}

/**
 * @brief Check if and how a player should be rendered
 * @param out Output buffer
 * @param y dPiece coordinate
 * @param x dPiece coordinate
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 */
void DrawPlayerHelper(const Surface &out, int x, int y, int sx, int sy)
{
	int8_t p = dPlayer[x][y];
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
	int px = sx + offset.deltaX - CalculateWidth2(player.AnimInfo.pCelSprite == nullptr ? 96 : player.AnimInfo.pCelSprite->Width());
	int py = sy + offset.deltaY;

	DrawPlayer(out, p, x, y, px, py);
}

/**
 * @brief Render object sprites
 * @param out Target buffer
 * @param sx dPiece coordinate
 * @param sy dPiece coordinate
 * @param dx Target buffer coordinate
 * @param dy Target buffer coordinate
 */
void DrawDungeon(const Surface &out, int sx, int sy, int dx, int dy)
{
	assert(sx >= 0 && sx < MAXDUNX);
	assert(sy >= 0 && sy < MAXDUNY);

	if (dRendered[sx][sy])
		return;
	dRendered[sx][sy] = true;

	LightTableIndex = dLight[sx][sy];

	DrawCell(out, sx, sy, dx, dy);

	int8_t bFlag = dFlags[sx][sy];
	int8_t bDead = dDead[sx][sy];
	int8_t bMap = dTransVal[sx][sy];

	int negMon = 0;
	if (sy > 0) // check for OOB
		negMon = dMonster[sx][sy - 1];

#ifdef _DEBUG
	if (visiondebug && (bFlag & BFLAG_LIT) != 0) {
		CelClippedDrawTo(out, { dx, dy }, *pSquareCel, 1);
	}
#endif

	if (MissilePreFlag) {
		DrawMissile(out, sx, sy, dx, dy, true);
	}

	if (LightTableIndex < LightsMax && bDead != 0) {
		do {
			DeadStruct *pDeadGuy = &Dead[(bDead & 0x1F) - 1];
			auto dd = static_cast<Direction>((bDead >> 5) & 7);
			int px = dx - CalculateWidth2(pDeadGuy->width);
			const byte *pCelBuff = pDeadGuy->data[dd];
			assert(pCelBuff != nullptr);
			const auto *frameTable = reinterpret_cast<const uint32_t *>(pCelBuff);
			int frames = SDL_SwapLE32(frameTable[0]);
			int nCel = pDeadGuy->frame;
			if (nCel < 1 || frames > 50 || nCel > frames) {
				Log("Unclipped dead: frame {} of {}, deadnum=={}", nCel, frames, (bDead & 0x1F) - 1);
				break;
			}
			if (pDeadGuy->translationPaletteIndex != 0) {
				Cl2DrawLightTbl(out, px, dy, CelSprite(pCelBuff, pDeadGuy->width), nCel, pDeadGuy->translationPaletteIndex);
			} else {
				Cl2DrawLight(out, px, dy, CelSprite(pCelBuff, pDeadGuy->width), nCel);
			}
		} while (false);
	}
	DrawObject(out, sx, sy, dx, dy, true);
	DrawItem(out, sx, sy, dx, dy, true);
	if ((bFlag & BFLAG_PLAYERLR) != 0) {
		int syy = sy - 1;
		assert(syy >= 0 && syy < MAXDUNY);
		DrawPlayerHelper(out, sx, syy, dx, dy);
	}
	if ((bFlag & BFLAG_MONSTLR) != 0 && negMon < 0) {
		DrawMonsterHelper(out, sx, sy, -1, dx, dy);
	}
	if ((bFlag & BFLAG_DEAD_PLAYER) != 0) {
		DrawDeadPlayer(out, sx, sy, dx, dy);
	}
	if (dPlayer[sx][sy] > 0) {
		DrawPlayerHelper(out, sx, sy, dx, dy);
	}
	if (dMonster[sx][sy] > 0) {
		DrawMonsterHelper(out, sx, sy, 0, dx, dy);
	}
	DrawMissile(out, sx, sy, dx, dy, false);
	DrawObject(out, sx, sy, dx, dy, false);
	DrawItem(out, sx, sy, dx, dy, false);

	if (leveltype != DTYPE_TOWN) {
		char bArch = dSpecial[sx][sy];
		if (bArch != 0) {
			cel_transparency_active = TransList[bMap];
#ifdef _DEBUG
			if (GetAsyncKeyState(DVL_VK_MENU)) {
				cel_transparency_active = false; // Turn transparency off here for debugging
			}
#endif
			CelClippedBlitLightTransTo(out, { dx, dy }, *pSpecialCels, bArch);
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
		if (sx > 0 && sy > 0 && dy > TILE_HEIGHT) {
			char bArch = dSpecial[sx - 1][sy - 1];
			if (bArch != 0) {
				CelDrawTo(out, { dx, dy - TILE_HEIGHT }, *pSpecialCels, bArch);
			}
		}
	}
}

/**
 * @brief Render a row of tiles
 * @param out Buffer to render to
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param rows Number of rows
 * @param columns Tile in a row
 */
void DrawFloor(const Surface &out, int x, int y, int sx, int sy, int rows, int columns)
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY) {
				level_piece_id = dPiece[x][y];
				if (level_piece_id != 0) {
					if (!nSolidTable[level_piece_id])
						DrawFloor(out, x, y, sx, sy);
				} else {
					world_draw_black_tile(out, sx, sy);
				}
			} else {
				world_draw_black_tile(out, sx, sy);
			}
			ShiftGrid(&x, &y, 1, 0);
			sx += TILE_WIDTH;
		}
		// Return to start of row
		ShiftGrid(&x, &y, -columns, 0);
		sx -= columns * TILE_WIDTH;

		// Jump to next row
		sy += TILE_HEIGHT / 2;
		if ((i & 1) != 0) {
			x++;
			columns--;
			sx += TILE_WIDTH / 2;
		} else {
			y++;
			columns++;
			sx -= TILE_WIDTH / 2;
		}
	}
}

#define IsWall(x, y) (dPiece[x][y] == 0 || nSolidTable[dPiece[x][y]] || dSpecial[x][y] != 0)
#define IsWalkable(x, y) (dPiece[x][y] != 0 && IsTileNotSolid({ x, y }))

/**
 * @brief Render a row of tile
 * @param out Output buffer
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Buffer coordinate
 * @param sy Buffer coordinate
 * @param rows Number of rows
 * @param columns Tile in a row
 */
void DrawTileContent(const Surface &out, int x, int y, int sx, int sy, int rows, int columns)
{
	// Keep evaluating until MicroTiles can't affect screen
	rows += MicroTileLen;
	memset(dRendered, 0, sizeof(dRendered));

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY) {
				if (x + 1 < MAXDUNX && y - 1 >= 0 && sx + TILE_WIDTH <= gnScreenWidth) {
					// Render objects behind walls first to prevent sprites, that are moving
					// between tiles, from poking through the walls as they exceed the tile bounds.
					// A proper fix for this would probably be to layout the sceen and render by
					// sprite screen position rather than tile position.
					if (IsWall(x, y) && (IsWall(x + 1, y) || (x > 0 && IsWall(x - 1, y)))) { // Part of a wall aligned on the x-axis
						if (IsWalkable(x + 1, y - 1) && IsWalkable(x, y - 1)) {              // Has walkable area behind it
							DrawDungeon(out, x + 1, y - 1, sx + TILE_WIDTH, sy);
						}
					}
				}
				if (dPiece[x][y] != 0) {
					DrawDungeon(out, x, y, sx, sy);
				}
			}
			ShiftGrid(&x, &y, 1, 0);
			sx += TILE_WIDTH;
		}
		// Return to start of row
		ShiftGrid(&x, &y, -columns, 0);
		sx -= columns * TILE_WIDTH;

		// Jump to next row
		sy += TILE_HEIGHT / 2;
		if ((i & 1) != 0) {
			x++;
			columns--;
			sx += TILE_WIDTH / 2;
		} else {
			y++;
			columns++;
			sx -= TILE_WIDTH / 2;
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

int tileOffsetX;
int tileOffsetY;
int tileShiftX;
int tileShiftY;
int tileColums;
int tileRows;

/**
 * @brief Configure render and process screen rows
 * @param full_out Buffer to render to
 * @param x Center of view in dPiece coordinate
 * @param y Center of view in dPiece coordinate
 */
void DrawGame(const Surface &fullOut, int x, int y)
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
	int sx = offset.deltaX + tileOffsetX;
	int sy = offset.deltaY + tileOffsetY;

	int columns = tileColums;
	int rows = tileRows;

	x += tileShiftX;
	y += tileShiftY;

	// Skip rendering parts covered by the panels
	if (CanPanelsCoverView()) {
		if (zoomflag) {
			if (chrflag || QuestLogIsOpen) {
				ShiftGrid(&x, &y, 2, 0);
				columns -= 4;
				sx += SPANEL_WIDTH - TILE_WIDTH / 2;
			}
			if (invflag || sbookflag) {
				ShiftGrid(&x, &y, 2, 0);
				columns -= 4;
				sx += -TILE_WIDTH / 2;
			}
		} else {
			if (chrflag || QuestLogIsOpen) {
				ShiftGrid(&x, &y, 1, 0);
				columns -= 2;
				sx += -TILE_WIDTH / 2 / 2; // SPANEL_WIDTH accounted for in Zoom()
			}
			if (invflag || sbookflag) {
				ShiftGrid(&x, &y, 1, 0);
				columns -= 2;
				sx += -TILE_WIDTH / 2 / 2;
			}
		}
	}

	UpdateMissilesRendererData();

	// Draw areas moving in and out of the screen
	switch (ScrollInfo._sdir) {
	case SDIR_N:
		sy -= TILE_HEIGHT;
		ShiftGrid(&x, &y, 0, -1);
		rows += 2;
		break;
	case SDIR_NE:
		sy -= TILE_HEIGHT;
		ShiftGrid(&x, &y, 0, -1);
		columns++;
		rows += 2;
		break;
	case SDIR_E:
		columns++;
		break;
	case SDIR_SE:
		columns++;
		rows++;
		break;
	case SDIR_S:
		rows += 2;
		break;
	case SDIR_SW:
		sx -= TILE_WIDTH;
		ShiftGrid(&x, &y, -1, 0);
		columns++;
		rows++;
		break;
	case SDIR_W:
		sx -= TILE_WIDTH;
		ShiftGrid(&x, &y, -1, 0);
		columns++;
		break;
	case SDIR_NW:
		sx -= TILE_WIDTH / 2;
		sy -= TILE_HEIGHT / 2;
		x--;
		columns++;
		rows++;
		break;
	case SDIR_NONE:
		break;
	}

	DrawFloor(out, x, y, sx, sy, rows, columns);
	DrawTileContent(out, x, y, sx, sy, rows, columns);

	if (!zoomflag) {
		Zoom(fullOut.subregionY(0, gnViewportHeight));
	}
}

/**
 * @brief Start rendering of screen, town variation
 * @param out Buffer to render to
 * @param StartX Center of view in dPiece coordinate
 * @param StartY Center of view in dPiece coordinate
 */
void DrawView(const Surface &out, int startX, int startY)
{
	DrawGame(out, startX, startY);
	if (AutomapActive) {
		DrawAutomap(out.subregionY(0, gnViewportHeight));
	}
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
	if (!chrflag && Players[MyPlayerId]._pStatPts != 0 && !spselflag
	    && (!QuestLogIsOpen || gnScreenHeight >= SPANEL_HEIGHT + PANEL_HEIGHT + 74 || gnScreenWidth >= 4 * SPANEL_WIDTH)) {
		DrawLevelUpIcon(out);
	}
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
	if (msgflag != EMSG_NONE) {
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
	DrawString(out, string, Point { 8, 65 }, UiFlags::UIS_RED);
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
	//                                           DIR_S,        DIR_SW,       DIR_W,	       DIR_NW,        DIR_N,        DIR_NE,        DIR_E,        DIR_SE,
	constexpr Displacement StartOffset[8]    = { {   0, -32 }, {  32, -16 }, {  32, -16 }, {   0,   0 }, {   0,   0 }, {  0,    0 },  { -32, -16 }, { -32, -16 } };
	constexpr Displacement MovingOffset[8]   = { {   0,  32 }, { -32,  16 }, { -64,   0 }, { -32, -16 }, {   0, -32 }, {  32, -16 },  {  64,   0 }, {  32,  16 } };
	constexpr bool IsDiagionalWalk[8]        = {        false,         true,        false,         true,        false,         true,         false,         true };
	// clang-format on

	float fAnimationProgress = animationInfo.GetAnimationProgress();
	Displacement offset = MovingOffset[dir];
	offset *= fAnimationProgress;

	if (cameraMode) {
		offset = -offset;
	} else {
		offset += StartOffset[dir];
	}

	return offset;
}

/**
 * @brief Clear cursor state
 */
void ClearCursor() // CODE_FIX: this was supposed to be in cursor.cpp
{
	sgdwCursWdt = 0;
	sgdwCursWdtOld = 0;
}

/**
 * @brief Shifting the view area along the logical grid
 *        Note: this won't allow you to shift between even and odd rows
 * @param horizontal Shift the screen left or right
 * @param vertical Shift the screen up or down
 */
void ShiftGrid(int *x, int *y, int horizontal, int vertical)
{
	*x += vertical + horizontal;
	*y += vertical - horizontal;
}

/**
 * @brief Gets the number of rows covered by the main panel
 */
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

/**
 * @brief Calculate the offset needed for centering tiles in view area
 * @param offsetX Offset in pixels
 * @param offsetY Offset in pixels
 */
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

/**
 * @brief Calculate the needed diamond tile to cover the view area
 * @param columns Tiles needed per row
 * @param rows Both even and odd rows
 */
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
	tileShiftX = 0;
	tileShiftY = 0;

	// Adjust by player offset and tile grid alignment
	int xo = 0;
	int yo = 0;
	CalcTileOffset(&xo, &yo);
	tileOffsetX = 0 - xo;
	tileOffsetY = 0 - yo - 1 + TILE_HEIGHT / 2;

	TilesInView(&tileColums, &tileRows);
	int lrow = tileRows - RowsCoveredByPanel();

	// Center player tile on screen
	ShiftGrid(&tileShiftX, &tileShiftY, -tileColums / 2, -lrow / 2);

	tileRows *= 2;

	// Align grid
	if ((tileColums & 1) == 0) {
		tileShiftY--; // Shift player row to one that can be centered with out pixel offset
		if ((lrow & 1) == 0) {
			// Offset tile to vertically align the player when both rows and colums are even
			tileRows++;
			tileOffsetY -= TILE_HEIGHT / 2;
		}
	} else if ((tileColums & 1) != 0 && (lrow & 1) != 0) {
		// Offset tile to vertically align the player when both rows and colums are odd
		ShiftGrid(&tileShiftX, &tileShiftY, 0, -1);
		tileRows++;
		tileOffsetY -= TILE_HEIGHT / 2;
	}

	// Slightly lower the zoomed view
	if (!zoomflag) {
		tileOffsetY += TILE_HEIGHT / 4;
		if (yo < TILE_HEIGHT / 4)
			tileRows++;
	}

	tileRows++; // Cover lower edge saw tooth, right edge accounted for in scrollrt_draw()
}

extern SDL_Surface *pal_surface;

/**
 * @brief Render the whole screen black
 */
void ClearScreenBuffer()
{
	lock_buf(3);

	assert(pal_surface != nullptr);
	SDL_FillRect(pal_surface, nullptr, 0);

	unlock_buf(3);
}

#ifdef _DEBUG
/**
 * @brief Scroll the screen when mouse is close to the edge
 */
void ScrollView()
{
	bool scroll;

	if (pcurs >= CURSOR_FIRSTITEM)
		return;

	scroll = false;

	if (MousePosition.x < 20) {
		if (dmaxy - 1 <= ViewY || dminx >= ViewX) {
			if (dmaxy - 1 > ViewY) {
				ViewY++;
				scroll = true;
			}
			if (dminx < ViewX) {
				ViewX--;
				scroll = true;
			}
		} else {
			ViewY++;
			ViewX--;
			scroll = true;
		}
	}
	if (MousePosition.x > gnScreenWidth - 20) {
		if (dmaxx - 1 <= ViewX || dminy >= ViewY) {
			if (dmaxx - 1 > ViewX) {
				ViewX++;
				scroll = true;
			}
			if (dminy < ViewY) {
				ViewY--;
				scroll = true;
			}
		} else {
			ViewY--;
			ViewX++;
			scroll = true;
		}
	}
	if (MousePosition.y < 20) {
		if (dminy >= ViewY || dminx >= ViewX) {
			if (dminy < ViewY) {
				ViewY--;
				scroll = true;
			}
			if (dminx < ViewX) {
				ViewX--;
				scroll = true;
			}
		} else {
			ViewX--;
			ViewY--;
			scroll = true;
		}
	}
	if (MousePosition.y > gnScreenHeight - 20) {
		if (dmaxy - 1 <= ViewY || dmaxx - 1 <= ViewX) {
			if (dmaxy - 1 > ViewY) {
				ViewY++;
				scroll = true;
			}
			if (dmaxx - 1 > ViewX) {
				ViewX++;
				scroll = true;
			}
		} else {
			ViewX++;
			ViewY++;
			scroll = true;
		}
	}

	if (scroll)
		ScrollInfo._sdir = SDIR_NONE;
}
#endif

/**
 * @brief Initialize the FPS meter
 */
void EnableFrameCount()
{
	frameflag = !frameflag;
	framestart = SDL_GetTicks();
}

/**
 * @brief Redraw screen
 */
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

/**
 * @brief Render the game
 */
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

	DrawView(out, ViewX, ViewY);
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

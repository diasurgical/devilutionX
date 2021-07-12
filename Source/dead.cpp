/**
 * @file dead.cpp
 *
 * Implementation of functions for placing dead monsters.
 */
#include "dead.h"

#include "gendung.h"
#include "lighting.h"
#include "misdat.h"
#include "monster.h"

namespace devilution {

DeadStruct Dead[MaxDead];
int8_t stonendx;

namespace {
void InitDeadAnimationFromMonster(DeadStruct &dead, const CMonster &mon)
{
	int i = 0;
	auto &animData = mon.GetAnimData(MonsterGraphic::Death);
	for (const auto &celSprite : animData.CelSpritesForDirections)
		dead.data[i++] = celSprite->Data();
	dead.frame = animData.Frames;
	dead.width = animData.CelSpritesForDirections[0]->Width();
}
} // namespace

void InitDead()
{
	int8_t mtypes[MAXMONSTERS] = {};

	int8_t nd = 0;

	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		if (mtypes[LevelMonsterTypes[i].mtype] != 0)
			continue;

		InitDeadAnimationFromMonster(Dead[nd], LevelMonsterTypes[i]);
		Dead[nd].translationPaletteIndex = 0;
		nd++;

		LevelMonsterTypes[i].mdeadval = nd;
		mtypes[LevelMonsterTypes[i].mtype] = nd;
	}

	for (auto &dead : Dead[nd].data)
		dead = MissileSpriteData[MFILE_BLODBUR].mAnimData[0];
	Dead[nd].frame = 8;
	Dead[nd].width = 128;
	Dead[nd].translationPaletteIndex = 0;
	nd++;

	for (auto &dead : Dead[nd].data)
		dead = MissileSpriteData[MFILE_SHATTER1].mAnimData[0];

	Dead[nd].frame = 12;
	Dead[nd].width = 128;
	Dead[nd].translationPaletteIndex = 0;
	nd++;

	stonendx = nd;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		int mi = ActiveMonsters[i];
		if (Monsters[mi]._uniqtype != 0) {
			InitDeadAnimationFromMonster(Dead[nd], *Monsters[mi].MType);
			Dead[nd].translationPaletteIndex = Monsters[mi]._uniqtrans + 4;
			nd++;

			Monsters[mi]._udeadval = nd;
		}
	}

	assert(static_cast<unsigned>(nd) <= MaxDead);
}

void AddDead(Point tilePosition, int8_t dv, Direction ddir)
{
	dDead[tilePosition.x][tilePosition.y] = (dv & 0x1F) + (ddir << 5);
}

void SetDead()
{
	for (int i = 0; i < ActiveMonsterCount; i++) {
		int mi = ActiveMonsters[i];
		if (Monsters[mi]._uniqtype == 0)
			continue;
		for (int dx = 0; dx < MAXDUNX; dx++) {
			for (int dy = 0; dy < MAXDUNY; dy++) {
				if ((dDead[dx][dy] & 0x1F) == Monsters[mi]._udeadval)
					ChangeLightXY(Monsters[mi].mlid, { dx, dy });
			}
		}
	}
}

} // namespace devilution

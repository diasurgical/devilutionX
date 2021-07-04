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
	for (const auto &celSprite : mon.Anims[MA_DEATH].CelSpritesForDirections)
		dead.data[i++] = celSprite->Data();
	dead.frame = mon.Anims[MA_DEATH].Frames;
	dead.width = mon.Anims[MA_DEATH].CelSpritesForDirections[0]->Width();
}
} // namespace

void InitDead()
{
	int8_t mtypes[MAXMONSTERS] = {};

	int8_t nd = 0;

	for (int i = 0; i < nummtypes; i++) {
		if (mtypes[Monsters[i].mtype] != 0)
			continue;

		InitDeadAnimationFromMonster(Dead[nd], Monsters[i]);
		Dead[nd].translationPaletteIndex = 0;
		nd++;

		Monsters[i].mdeadval = nd;
		mtypes[Monsters[i].mtype] = nd;
	}

	for (auto &dead : Dead[nd].data)
		dead = misfiledata[MFILE_BLODBUR].mAnimData[0];
	Dead[nd].frame = 8;
	Dead[nd].width = 128;
	Dead[nd].translationPaletteIndex = 0;
	nd++;

	for (auto &dead : Dead[nd].data)
		dead = misfiledata[MFILE_SHATTER1].mAnimData[0];

	Dead[nd].frame = 12;
	Dead[nd].width = 128;
	Dead[nd].translationPaletteIndex = 0;
	nd++;

	stonendx = nd;

	for (int i = 0; i < nummonsters; i++) {
		int mi = monstactive[i];
		if (monster[mi]._uniqtype != 0) {
			InitDeadAnimationFromMonster(Dead[nd], *monster[mi].MType);
			Dead[nd].translationPaletteIndex = monster[mi]._uniqtrans + 4;
			nd++;

			monster[mi]._udeadval = nd;
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
	for (int i = 0; i < nummonsters; i++) {
		int mi = monstactive[i];
		if (monster[mi]._uniqtype == 0)
			continue;
		for (int dx = 0; dx < MAXDUNX; dx++) {
			for (int dy = 0; dy < MAXDUNY; dy++) {
				if ((dDead[dx][dy] & 0x1F) == monster[mi]._udeadval)
					ChangeLightXY(monster[mi].mlid, { dx, dy });
			}
		}
	}
}

} // namespace devilution

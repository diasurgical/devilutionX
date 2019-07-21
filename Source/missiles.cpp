#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

int missileactive[MAXMISSILES];
int missileavail[MAXMISSILES];
MissileStruct missile[MAXMISSILES];
int nummissiles;
BOOL ManashieldFlag;
ChainStruct chain[MAXMISSILES];
BOOL MissilePreFlag;
int numchains;

int XDirAdd[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
int YDirAdd[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

void GetDamageAmt(int i, int *mind, int *maxd)
{
	int k, sl;

	/// ASSERT: assert((DWORD)myplr < MAX_PLRS);
	/// ASSERT: assert((DWORD)i < 64);
	sl = plr[myplr]._pSplLvl[i] + plr[myplr]._pISplLvlAdd;

	switch (i) {
	case SPL_FIREBOLT:
		*mind = (plr[myplr]._pMagic >> 3) + sl + 1;
		*maxd = (plr[myplr]._pMagic >> 3) + sl + 10;
		break;
	case SPL_HEAL: /// BUGFIX: healing calculation is unused
		*mind = plr[myplr]._pLevel + sl + 1;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			*mind *= 2;
		}
		if (plr[myplr]._pClass == PC_ROGUE) {
			*mind += *mind >> 1;
		}
		*maxd = 10;
		for (k = 0; k < plr[myplr]._pLevel; k++) {
			*maxd += 4;
		}
		for (k = 0; k < sl; k++) {
			*maxd += 6;
		}
		if (plr[myplr]._pClass == PC_WARRIOR) {
			*maxd *= 2;
		}
		if (plr[myplr]._pClass == PC_ROGUE) {
			*maxd += *maxd >> 1;
		}
		*mind = -1;
		*maxd = -1;
		break;
	case SPL_LIGHTNING:
		*mind = 2;
		*maxd = plr[myplr]._pLevel + 2;
		break;
	case SPL_FLASH:
		*mind = plr[myplr]._pLevel;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*mind += *mind >> 1;
		*maxd = *mind * 2;
		break;
	case SPL_IDENTIFY:
	case SPL_TOWN:
	case SPL_STONE:
	case SPL_INFRA:
	case SPL_RNDTELEPORT:
	case SPL_MANASHIELD:
	case SPL_DOOMSERP:
	case SPL_BLODRIT:
	case SPL_INVISIBIL:
	case SPL_BLODBOIL:
	case SPL_TELEPORT:
	case SPL_ETHEREALIZE:
	case SPL_REPAIR:
	case SPL_RECHARGE:
	case SPL_DISARM:
	case SPL_RESURRECT:
	case SPL_TELEKINESIS:
	case SPL_BONESPIRIT:
		*mind = -1;
		*maxd = -1;
		break;
	case SPL_FIREWALL:
		*mind = (4 * plr[myplr]._pLevel + 8) >> 1;
		*maxd = (4 * plr[myplr]._pLevel + 80) >> 1;
		break;
	case SPL_FIREBALL:
		*mind = 2 * plr[myplr]._pLevel + 4;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*maxd = 2 * plr[myplr]._pLevel + 40;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		break;
	case SPL_GUARDIAN:
		*mind = (plr[myplr]._pLevel >> 1) + 1;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*maxd = (plr[myplr]._pLevel >> 1) + 10;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		break;
	case SPL_CHAIN:
		*mind = 4;
		*maxd = 2 * plr[myplr]._pLevel + 4;
		break;
	case SPL_WAVE:
		*mind = 6 * (plr[myplr]._pLevel + 1);
		*maxd = 6 * (plr[myplr]._pLevel + 10);
		break;
	case SPL_NOVA:
		*mind = (plr[myplr]._pLevel + 5) >> 1;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*mind *= 5;
		*maxd = (plr[myplr]._pLevel + 30) >> 1;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		*maxd *= 5;
		break;
	case SPL_FLAME:
		*mind = 3;
		*maxd = plr[myplr]._pLevel + 4;
		*maxd += *maxd >> 1;
		break;
	case SPL_GOLEM:
		*mind = 11;
		*maxd = 17;
		break;
	case SPL_APOCA:
		*mind = 0;
		for (k = 0; k < plr[myplr]._pLevel; k++) {
			*mind += 1;
		}
		*maxd = 0;
		for (k = 0; k < plr[myplr]._pLevel; k++) {
			*maxd += 6;
		}
		break;
	case SPL_ELEMENT:
		*mind = 2 * plr[myplr]._pLevel + 4;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		/// BUGFIX: add here '*mind >>= 1;'
		*maxd = 2 * plr[myplr]._pLevel + 40;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		/// BUGFIX: add here '*maxd >>= 1;'
		break;
	case SPL_CBOLT:
		*mind = 1;
		*maxd = (plr[myplr]._pMagic >> 2) + 1;
		break;
	case SPL_HBOLT:
		*mind = plr[myplr]._pLevel + 9;
		*maxd = plr[myplr]._pLevel + 18;
		break;
	case SPL_HEALOTHER: /// BUGFIX: healing calculation is unused
		*mind = plr[myplr]._pLevel + sl + 1;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			*mind *= 2;
		}
		if (plr[myplr]._pClass == PC_ROGUE) {
			*mind += *mind >> 1;
		}
		*maxd = 10;
		for (k = 0; k < plr[myplr]._pLevel; k++) {
			*maxd += 4;
		}
		for (k = 0; k < sl; k++) {
			*maxd += 6;
		}
		if (plr[myplr]._pClass == PC_WARRIOR) {
			*maxd *= 2;
		}
		if (plr[myplr]._pClass == PC_ROGUE) {
			*maxd += *maxd >> 1;
		}
		*mind = -1;
		*maxd = -1;
		break;
	case SPL_FLARE:
		*mind = (plr[myplr]._pMagic >> 1) + 3 * sl - (plr[myplr]._pMagic >> 3);
		*maxd = *mind;
		break;
	}
}

BOOL CheckBlock(int fx, int fy, int tx, int ty)
{
	int pn;
	BOOL coll;

	coll = FALSE;
	while (fx != tx || fy != ty) {
		pn = GetDirection(fx, fy, tx, ty);
		fx += XDirAdd[pn];
		fy += YDirAdd[pn];
		if (nSolidTable[dPiece[fx][fy]])
			coll = TRUE;
	}

	return coll;
}

int FindClosest(int sx, int sy, int rad)
{
	int j, i, mid, tx, ty, cr;
	int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

	if (rad > 19)
		rad = 19;

	for (i = 1; i < rad; i++) {
		cr = CrawlNum[i] + 2;
		for (j = (BYTE)CrawlTable[CrawlNum[i]]; j > 0; j--) {
			tx = sx + CrawlTable[cr - 1];
			ty = sy + CrawlTable[cr];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = dMonster[tx][ty];
				if (mid > 0 && !CheckBlock(sx, sy, tx, ty))
					return mid - 1;
			}
			cr += 2;
		}
	}
	return -1;
}

int GetSpellLevel(int id, int sn)
{
	int result;

	if (id == myplr)
		result = plr[id]._pISplLvlAdd + plr[id]._pSplLvl[sn];
	else
		result = 1;

	if (result < 0)
		result = 0;

	return result;
}

int GetDirection8(int x1, int y1, int x2, int y2)
{
	BYTE Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	int mx, my, md;
	ALIGN_BY_1 BYTE urtoll[] = { 3, 4, 5 },
	                ultolr[] = { 3, 2, 1 },
	                lrtoul[] = { 7, 6, 5 },
	                lltour[] = { 7, 0, 1 };

	mx = abs(x2 - x1);
	if (mx > 15)
		mx = 15;
	my = abs(y2 - y1);
	if (my > 15)
		my = 15;
	md = Dirs[my][mx];
	if (x1 > x2) {
		if (y1 > y2)
			md = urtoll[md];
		else
			md = ultolr[md];
	} else if (y1 > y2)
		md = lrtoul[md];
	else
		md = lltour[md];
	return md;
}

int GetDirection16(int x1, int y1, int x2, int y2)
{
	BYTE Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 3, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2 }
	};
	BYTE urtoll[5] = { 6, 7, 8, 9, 10 };
	BYTE ultolr[5] = { 6, 5, 4, 3, 2 };
	BYTE lltour[5] = { 14, 13, 12, 11, 10 };
	BYTE lrtoul[5] = { 14, 15, 0, 1, 2 };
	int mx, my, md;

	mx = abs(x2 - x1);
	if (mx > 15)
		mx = 15;
	my = abs(y2 - y1);
	if (my > 15)
		my = 15;
	md = Dirs[my][mx];
	if (x1 > x2) {
		if (y1 > y2)
			md = urtoll[md];
		else
			md = ultolr[md];
	} else if (y1 > y2) {
		md = lltour[md];
	} else {
		md = lrtoul[md];
	}
	return md;
}

void DeleteMissile(int mi, int i)
{
	int src;

	if (missile[mi]._mitype == MIS_MANASHIELD) {
		src = missile[mi]._misource;
		if (src == myplr)
			NetSendCmd(TRUE, CMD_REMSHIELD);
		plr[src].pManaShield = FALSE;
	}
	missileavail[MAXMISSILES - nummissiles] = mi;
	nummissiles--;
	if (nummissiles > 0 && i != nummissiles)
		missileactive[i] = missileactive[nummissiles];
}

void GetMissileVel(int i, int sx, int sy, int dx, int dy, int v)
{
	double dxp, dyp, dr;

	if (dx != sx || dy != sy) {
		dxp = (dx + sy - sx - dy) << 21;
		dyp = (dy + dx - sx - sy) << 21;
		dr = sqrt(dxp * dxp + dyp * dyp);
		missile[i]._mixvel = (dxp * (v << 16)) / dr;
		missile[i]._miyvel = (dyp * (v << 15)) / dr;
	} else {
		missile[i]._mixvel = 0;
		missile[i]._miyvel = 0;
	}
}

void PutMissile(int i)
{
	int x, y;

	x = missile[i]._mix;
	y = missile[i]._miy;
	if (x <= 0 || y <= 0 || x >= MAXDUNX || y >= MAXDUNY)
		missile[i]._miDelFlag = TRUE;
	if (!missile[i]._miDelFlag) {
		dFlags[x][y] |= BFLAG_MISSILE;
		if (dMissile[x][y] == 0)
			dMissile[x][y] = i + 1;
		else
			dMissile[x][y] = -1;
		if (missile[i]._miPreFlag)
			MissilePreFlag = TRUE;
	}
}

void GetMissilePos(int i)
{
	int mx, my, dx, dy, lx, ly;

	mx = missile[i]._mitxoff >> 16;
	my = missile[i]._mityoff >> 16;
	dx = mx + 2 * my;
	dy = 2 * my - mx;
	if (dx < 0) {
		lx = -(-dx >> 3);
		dx = -(-dx >> 6);
	} else {
		lx = dx >> 3;
		dx = dx >> 6;
	}
	if (dy < 0) {
		ly = -(-dy >> 3);
		dy = -(-dy >> 6);
	} else {
		ly = dy >> 3;
		dy = dy >> 6;
	}
	missile[i]._mix = dx + missile[i]._misx;
	missile[i]._miy = dy + missile[i]._misy;
	missile[i]._mixoff = mx + (dy << 5) - (dx << 5);
	missile[i]._miyoff = my - (dx << 4) - (dy << 4);
	ChangeLightOff(missile[i]._mlid, lx - (dx << 3), ly - (dy << 3));
}

void MoveMissilePos(int i)
{
	int dx, dy, x, y;

	switch (missile[i]._mimfnum) {
	case DIR_S:
		dx = 1;
		dy = 1;
		break;
	case DIR_SW:
		dx = 1;
		dy = 1;
		break;
	case DIR_W:
		dx = 0;
		dy = 1;
		break;
	case DIR_NW:
		dx = 0;
		dy = 0;
		break;
	case DIR_N:
		dx = 0;
		dy = 0;
		break;
	case DIR_NE:
		dx = 0;
		dy = 0;
		break;
	case DIR_E:
		dx = 1;
		dy = 0;
		break;
	case DIR_SE:
		dx = 1;
		dy = 1;
		break;
	}
	x = missile[i]._mix + dx;
	y = missile[i]._miy + dy;
	if (PosOkMonst(missile[i]._misource, x, y)) {
		missile[i]._mix += dx;
		missile[i]._miy += dy;
		missile[i]._mixoff += (dy << 5) - (dx << 5);
		missile[i]._miyoff -= (dy << 4) + (dx << 4);
	}
}

BOOL MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, BOOLEAN shift)
{
	int hit, hper, dam, mor, mir;
	BOOL resist, ret, rv;

	resist = FALSE;
	if (monster[m].mtalkmsg) {
		ret = FALSE;
	} else if (monster[m]._mhitpoints >> 6 <= 0) {
		ret = FALSE;
	} else if (monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MGOAL_RETREAT)
		ret = FALSE;
	else if (monster[m]._mmode == MM_CHARGE)
		ret = FALSE;

	else {
		mir = missiledata[t].mResist;
		mor = monster[m].mMagicRes;
		if (mor & IMUNE_MAGIC && mir == MISR_MAGIC) {
			ret = FALSE;
		}

		else if (mor & IMUNE_FIRE && mir == MISR_FIRE) {
			ret = FALSE;
		}

		else if (mor & IMUNE_LIGHTNING && mir == MISR_LIGHTNING) {
			ret = FALSE;
		}

		//else if (mor & IMUNE_ACID && mir == MISR_ACID) {
		//ret = FALSE;
		//}
		else {

			if ((mor & RESIST_MAGIC && mir == MISR_MAGIC) || (mor & RESIST_FIRE && mir == MISR_FIRE) || (mor & RESIST_LIGHTNING && mir == MISR_LIGHTNING)) {
				resist = TRUE;
			}

			hit = random(68, 100);
			hper = 90 - (BYTE)monster[m].mArmorClass - dist;
			if (hper < 5)
				hper = 5;
			if (hper > 95)
				hper = 95;
			if (CheckMonsterHit(m, &rv)) {
				ret = rv;
			} else {
#ifdef _DEBUG
				if (hit >= hper && !debug_mode_dollar_sign && !debug_mode_key_inverted_v && monster[m]._mmode != MM_STONE) {
					ret = FALSE;
				}
#else
				if (hit >= hper && monster[m]._mmode != MM_STONE) {
					ret = FALSE;
				}
#endif
				else {
					dam = mindam + random(68, maxdam - mindam + 1);
					if (!shift)
						dam <<= 6;
					if (resist)
						monster[m]._mhitpoints -= dam >> 2;
					else
						monster[m]._mhitpoints -= dam;
#ifdef _DEBUG
					if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
						monster[m]._mhitpoints = 0;
#endif
					if (monster[m]._mhitpoints >> 6 <= 0) {
						if (monster[m]._mmode == MM_STONE) {
							M_StartKill(m, -1);
							monster[m]._mmode = MM_STONE;
						} else {
							M_StartKill(m, -1);
						}
					} else {
						if (resist) {
							PlayEffect(m, 1);
						} else if (monster[m]._mmode == MM_STONE) {
							if (m > 3)
								M_StartHit(m, -1, dam);
							monster[m]._mmode = MM_STONE;
						} else {
							if (m > 3)
								M_StartHit(m, -1, dam);
						}
					}
					ret = TRUE;
				}
			}
		}
	}
	return ret;
}

BOOL MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, BOOLEAN shift)
{
	int hit, hper, dam, mor, mir;
	BOOL resist, ret;

	resist = FALSE;
	if (monster[m].mtalkmsg
	    || monster[m]._mhitpoints >> 6 <= 0
	    || t == MIS_HBOLT && monster[m].MType->mtype != MT_DIABLO && monster[m].MData->mMonstClass) {
		return FALSE;
	}
	if (monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MGOAL_RETREAT)
		return FALSE;
	if (monster[m]._mmode == MM_CHARGE)
		return FALSE;

	mor = monster[m].mMagicRes;
	mir = missiledata[t].mResist;

	if (mor & IMUNE_MAGIC && mir == MISR_MAGIC
	    || mor & IMUNE_FIRE && mir == MISR_FIRE
	    || mor & IMUNE_LIGHTNING && mir == MISR_LIGHTNING
	    || (mor & IMUNE_ACID) && mir == MISR_ACID)
		return FALSE;

	if (mor & RESIST_MAGIC && mir == MISR_MAGIC
	    || mor & RESIST_FIRE && mir == MISR_FIRE
	    || mor & RESIST_LIGHTNING && mir == MISR_LIGHTNING)
		resist = TRUE;

	hit = random(69, 100);
	if (!missiledata[t].mType) {
		hper = plr[pnum]._pDexterity
		    + plr[pnum]._pIBonusToHit
		    + plr[pnum]._pLevel
		    - monster[m].mArmorClass
		    - (dist * dist >> 1)
		    + plr[pnum]._pIEnAc
		    + 50;
		if (plr[pnum]._pClass == PC_ROGUE)
			hper += 20;
		if (plr[pnum]._pClass == PC_WARRIOR)
			hper += 10;
	} else {
		hper = plr[pnum]._pMagic - (monster[m].mLevel << 1) - dist + 50;
		if (plr[pnum]._pClass == PC_SORCERER)
			hper += 20;
	}
	if (hper < 5)
		hper = 5;
	if (hper > 95)
		hper = 95;
	if (monster[m]._mmode == MM_STONE)
		hit = 0;
	if (CheckMonsterHit(m, &ret))
		return ret;
#ifdef _DEBUG
	if (hit >= hper && !debug_mode_key_inverted_v && !debug_mode_dollar_sign)
		return 0;
#else
	if (hit >= hper)
		return 0;
#endif
	if (t == MIS_BONESPIRIT) {
		dam = monster[m]._mhitpoints / 3 >> 6;
	} else {
		dam = mindam + random(70, maxdam - mindam + 1);
	}
	if (!missiledata[t].mType) {
		dam = plr[pnum]._pIBonusDamMod + dam * plr[pnum]._pIBonusDam / 100 + dam;
		if (plr[pnum]._pClass == PC_ROGUE)
			dam += plr[pnum]._pDamageMod;
		else
			dam += (plr[pnum]._pDamageMod >> 1);
	}
	if (!shift)
		dam <<= 6;
	if (resist)
		dam >>= 2;
	if (pnum == myplr)
		monster[m]._mhitpoints -= dam;
	if (plr[pnum]._pIFlags & ISPL_FIRE_ARROWS)
		monster[m]._mFlags |= MFLAG_NOHEAL;

	if (monster[m]._mhitpoints >> 6 <= 0) {
		if (monster[m]._mmode == MM_STONE) {
			M_StartKill(m, pnum);
			monster[m]._mmode = MM_STONE;
		} else {
			M_StartKill(m, pnum);
		}
	} else {
		if (resist) {
			PlayEffect(m, 1);
		} else if (monster[m]._mmode == MM_STONE) {
			if (m > 3)
				M_StartHit(m, pnum, dam);
			monster[m]._mmode = MM_STONE;
		} else {
			if (!missiledata[t].mType && plr[pnum]._pIFlags & ISPL_KNOCKBACK) {
				M_GetKnockback(m);
			}
			if (m > 3)
				M_StartHit(m, pnum, dam);
		}
	}

	if (!monster[m]._msquelch) {
		monster[m]._msquelch = UCHAR_MAX;
		monster[m]._lastx = plr[pnum].WorldX;
		monster[m]._lasty = plr[pnum].WorldY;
	}
	return TRUE;
}

BOOL PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, BOOLEAN shift, int earflag)
{
	int hit, hper, tac, dam, blk, blkper, resper;

	if (plr[pnum]._pHitPoints >> 6 <= 0) {
		return FALSE;
	}

	if (plr[pnum]._pInvincible) {
		return FALSE;
	}

	if (plr[pnum]._pSpellFlags & 1 && !missiledata[mtype].mType) {
		return FALSE;
	}

	hit = random(72, 100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hit = 1000;
#endif
	if (!missiledata[mtype].mType) {
		tac = plr[pnum]._pIAC + plr[pnum]._pIBonusAC + plr[pnum]._pDexterity / 5;
		if (m != -1) {
			hper = monster[m].mHit
			    + ((monster[m].mLevel - plr[pnum]._pLevel) << 1)
			    + 30
			    - (dist << 1) - tac;
		} else {
			hper = 100 - (tac >> 1) - (dist << 1);
		}
	} else {
		if (m != -1) {
			hper = +40 - (plr[pnum]._pLevel << 1) - (dist << 1) + (monster[m].mLevel << 1);
		} else {
			hper = 40;
		}
	}

	if (hper < 10)
		hper = 10;
	if (currlevel == 14 && hper < 20) {
		hper = 20;
	}
	if (currlevel == 15 && hper < 25) {
		hper = 25;
	}
	if (currlevel == 16 && hper < 30) {
		hper = 30;
	}

	if (!((plr[pnum]._pmode && plr[pnum]._pmode != PM_ATTACK) || !plr[pnum]._pBlockFlag)) {
		blk = random(73, 100);
	} else {
		blk = 100;
	}

	if (shift == TRUE)
		blk = 100;
	if (mtype == MIS_ACIDPUD)
		blk = 100;
	if (m != -1)
		blkper = plr[pnum]._pBaseToBlk + (plr[pnum]._pLevel << 1) - (monster[m].mLevel << 1) + plr[pnum]._pDexterity;
	else
		blkper = plr[pnum]._pBaseToBlk + plr[pnum]._pDexterity;
	if (blkper < 0)
		blkper = 0;
	if (blkper > 100)
		blkper = 100;

	switch (missiledata[mtype].mResist) {
	case MISR_FIRE:
		resper = plr[pnum]._pFireResist;
		break;
	case MISR_LIGHTNING:
		resper = plr[pnum]._pLghtResist;
		break;
	case MISR_MAGIC:
	case MISR_ACID:
		resper = plr[pnum]._pMagResist;
		break;
	default:
		resper = 0;
		break;
	}

	if (hit < hper) {
		if (mtype == MIS_BONESPIRIT) {
			dam = plr[pnum]._pHitPoints / 3;
		} else {
			if (shift == FALSE) {

				dam = (mind << 6) + random(75, (maxd - mind + 1) << 6);
				if (m == -1 && plr[pnum]._pIFlags & ISPL_ABSHALFTRAP)
					dam >>= 1;
				dam += (plr[pnum]._pIGetHit << 6);
			} else {
				dam = mind + random(75, maxd - mind + 1);
				if (m == -1 && plr[pnum]._pIFlags & ISPL_ABSHALFTRAP)
					dam >>= 1;
				dam += plr[pnum]._pIGetHit;
			}

			if (dam < 64)
				dam = 64;
		}
		if (resper > 0) {

			dam = dam - dam * resper / 100;
			if (pnum == myplr) {
				plr[pnum]._pHitPoints -= dam;
				plr[pnum]._pHPBase -= dam;
			}
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
				plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
				plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			}

			if (plr[pnum]._pHitPoints >> 6 <= 0) {
				SyncPlrKill(pnum, earflag);
			} else {
				if (plr[pnum]._pClass == PC_WARRIOR) {
					PlaySfxLoc(PS_WARR69, plr[pnum].WorldX, plr[pnum].WorldY);
				} else if (plr[pnum]._pClass == PC_ROGUE) {
					PlaySfxLoc(PS_ROGUE69, plr[pnum].WorldX, plr[pnum].WorldY);
				} else if (plr[pnum]._pClass == PC_SORCERER) {
					PlaySfxLoc(PS_MAGE69, plr[pnum].WorldX, plr[pnum].WorldY);
				}
				drawhpflag = TRUE;
			}
		} else {
			if (blk < blkper) {
				if (m != -1) {
					tac = GetDirection(plr[pnum].WorldX, plr[pnum].WorldY, monster[m]._mx, monster[m]._my);
				} else {
					tac = plr[pnum]._pdir;
				}
				StartPlrBlock(pnum, tac);
			} else {
				if (pnum == myplr) {
					plr[pnum]._pHitPoints -= dam;
					plr[pnum]._pHPBase -= dam;
				}
				if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
					plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
					plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
				}
				if (plr[pnum]._pHitPoints >> 6 <= 0) {
					SyncPlrKill(pnum, earflag);
				} else {
					StartPlrHit(pnum, dam, 0);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, BOOLEAN shift)
{
	int tac, resper, dam, blk, blkper, hper, hit;

	if (plr[p]._pInvincible) {
		return FALSE;
	}

	if (mtype == MIS_HBOLT) {
		return FALSE;
	}

	if (plr[p]._pSpellFlags & 1 && !missiledata[mtype].mType) {
		return FALSE;
	}

	switch (missiledata[mtype].mResist) {
	case MISR_FIRE:
		resper = plr[p]._pFireResist;
		break;
	case MISR_LIGHTNING:
		resper = plr[p]._pLghtResist;
		break;
	case MISR_MAGIC:
	case MISR_ACID:
		resper = plr[p]._pMagResist;
		break;
	default:
		resper = 0;
		break;
	}
	hper = random(69, 100);
	if (!missiledata[mtype].mType) {
		hit = plr[pnum]._pIBonusToHit
		    + plr[pnum]._pLevel
		    - (dist * dist >> 1)
		    - plr[p]._pDexterity / 5
		    - plr[p]._pIBonusAC
		    - plr[p]._pIAC
		    + plr[pnum]._pDexterity + 50;
		if (plr[pnum]._pClass == PC_ROGUE)
			hit += 20;
		if (plr[pnum]._pClass == PC_WARRIOR)
			hit += 10;
	} else {
		hit = plr[pnum]._pMagic
		    - (plr[p]._pLevel << 1)
		    - dist
		    + 50;
		if (plr[pnum]._pClass == PC_SORCERER)
			hit += 20;
	}
	if (hit < 5)
		hit = 5;
	if (hit > 95)
		hit = 95;
	if (hper < hit) {
		if (!((plr[p]._pmode && plr[p]._pmode != PM_ATTACK) || !plr[p]._pBlockFlag)) {
			blkper = random(73, 100);
		} else {
			blkper = 100;
		}
		if (shift == TRUE)
			blkper = 100;
		blk = plr[p]._pDexterity + plr[p]._pBaseToBlk + (plr[p]._pLevel << 1) - (plr[pnum]._pLevel << 1);

		if (blk < 0) {
			blk = 0;
		}
		if (blk > 100) {
			blk = 100;
		}

		if (mtype == MIS_BONESPIRIT) {
			dam = plr[p]._pHitPoints / 3;
		} else {
			dam = mindam + random(70, maxdam - mindam + 1);
			if (!missiledata[mtype].mType)
				dam += plr[pnum]._pIBonusDamMod + plr[pnum]._pDamageMod + dam * plr[pnum]._pIBonusDam / 100;
			if (!shift)
				dam <<= 6;
		}
		if (missiledata[mtype].mType)
			dam >>= 1;
		if (resper > 0) {
			if (pnum == myplr)
				NetSendCmdDamage(TRUE, p, dam - resper * dam / 100);
			if (plr[pnum]._pClass == PC_WARRIOR) {
				tac = PS_WARR69;
			} else if (plr[pnum]._pClass == PC_ROGUE) {
				tac = PS_ROGUE69;
			} else if (plr[pnum]._pClass == PC_SORCERER) {
				tac = PS_MAGE69;
			} else {
				return TRUE;
			}
			PlaySfxLoc(tac, plr[pnum].WorldX, plr[pnum].WorldY);
		} else {
			if (blkper < blk) {
				StartPlrBlock(p, GetDirection(plr[p].WorldX, plr[p].WorldY, plr[pnum].WorldX, plr[pnum].WorldY));
			} else {
				if (pnum == myplr)
					NetSendCmdDamage(TRUE, p, dam);
				StartPlrHit(p, dam, 0);
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CheckMissileCol(int i, int mindam, int maxdam, BOOL shift, int mx, int my, BOOLEAN nodel)
{
	int oi;

	if (missile[i]._miAnimType != 4 && missile[i]._misource != -1) {
		if (!missile[i]._micaster) {
			if (dMonster[mx][my] > 0) {
				if (MonsterMHit(
				        missile[i]._misource,
				        dMonster[mx][my] - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = 1;
				}
			} else {
				if (dMonster[mx][my] < 0
				    && monster[-(dMonster[mx][my] + 1)]._mmode == 15
				    && MonsterMHit(
				        missile[i]._misource,
				        -(dMonster[mx][my] + 1),
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = 1;
				}
			}
			if (dPlayer[mx][my] > 0
			    && dPlayer[mx][my] - 1 != missile[i]._misource
			    && Plr2PlrMHit(
			        missile[i]._misource,
			        dPlayer[mx][my] - 1,
			        mindam,
			        maxdam,
			        missile[i]._midist,
			        missile[i]._mitype,
			        shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = 1;
			}
		} else {
			if (monster[missile[i]._misource]._mFlags & 0x10
			    && dMonster[mx][my] > 0
			    && monster[dMonster[mx][my] - 1]._mFlags & 0x20
			    && MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = 1;
			}
			if (dPlayer[mx][my] > 0
			    && PlayerMHit(
			        dPlayer[mx][my] - 1,
			        missile[i]._misource,
			        missile[i]._midist,
			        mindam,
			        maxdam,
			        missile[i]._mitype,
			        shift,
			        0)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = 1;
			}
		}
	} else {
		if (dMonster[mx][my] > 0) {
			if (missile[i]._miAnimType == 4) {
				if (MonsterMHit(
				        missile[i]._misource,
				        dMonster[mx][my] - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = 1;
				}
			} else if (MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = 1;
			}
		}
		if (dPlayer[mx][my] > 0
		    && PlayerMHit(dPlayer[mx][my] - 1, -1, missile[i]._midist, mindam, maxdam, missile[i]._mitype, shift, missile[i]._miAnimType == MFILE_FIREWAL)) {
			if (!nodel)
				missile[i]._mirange = 0;
			missile[i]._miHitFlag = 1;
		}
	}
	if (dObject[mx][my]) {
		oi = dObject[mx][my] > 0 ? dObject[mx][my] - 1 : -(dObject[mx][my] + 1);
		if (!object[oi]._oMissFlag) {
			if (object[oi]._oBreak == 1)
				BreakObject(-1, oi);
			if (!nodel)
				missile[i]._mirange = 0;
			missile[i]._miHitFlag = 0;
		}
	}
	if (nMissileTable[dPiece[mx][my]]) {
		if (!nodel)
			missile[i]._mirange = 0;
		missile[i]._miHitFlag = 0;
	}
	if (!missile[i]._mirange && missiledata[missile[i]._mitype].miSFX != -1)
		PlaySfxLoc(missiledata[missile[i]._mitype].miSFX, missile[i]._mix, missile[i]._miy);
}

void SetMissAnim(int mi, int animtype)
{
	int dir = missile[mi]._mimfnum;

	missile[mi]._miAnimType = animtype;
	missile[mi]._miAnimFlags = misfiledata[animtype].mFlags;
	missile[mi]._miAnimData = misfiledata[animtype].mAnimData[dir];
	missile[mi]._miAnimDelay = misfiledata[animtype].mAnimDelay[dir];
	missile[mi]._miAnimLen = misfiledata[animtype].mAnimLen[dir];
	missile[mi]._miAnimWidth = misfiledata[animtype].mAnimWidth[dir];
	missile[mi]._miAnimWidth2 = misfiledata[animtype].mAnimWidth2[dir];
	missile[mi]._miAnimCnt = 0;
	missile[mi]._miAnimFrame = 1;
}

void SetMissDir(int mi, int dir)
{
	missile[mi]._mimfnum = dir;
	SetMissAnim(mi, missile[mi]._miAnimType);
}

void LoadMissileGFX(BYTE mi)
{
	char pszName[256];
	int i;
	BYTE *file;
	MisFileData *mfd;

	mfd = &misfiledata[mi];
	if (mfd->mFlags & MFLAG_ALLOW_SPECIAL) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		file = LoadFileInMem(pszName, NULL);
		for (i = 0; i < mfd->mAnimFAmt; i++)
			mfd->mAnimData[i] = &file[((int *)file)[i]];
	} else if (mfd->mAnimFAmt == 1) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		if (!mfd->mAnimData[0])
			mfd->mAnimData[0] = LoadFileInMem(pszName, NULL);
	} else {
		for (i = 0; i < mfd->mAnimFAmt; i++) {
			sprintf(pszName, "Missiles\\%s%i.CL2", mfd->mName, i + 1);
			if (!mfd->mAnimData[i]) {
				file = LoadFileInMem(pszName, NULL);
				mfd->mAnimData[i] = file;
			}
		}
	}
}

void InitMissileGFX()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (!(misfiledata[mi].mFlags & MFLAG_HIDDEN))
			LoadMissileGFX(mi);
	}
}

void FreeMissileGFX(int mi)
{
	int i;
	DWORD *pFrameTable;

	if (misfiledata[mi].mFlags & MFLAG_ALLOW_SPECIAL) {
		if (misfiledata[mi].mAnimData[0]) {
			pFrameTable = (DWORD *)misfiledata[mi].mAnimData[0];
			mem_free_dbg(&pFrameTable[-misfiledata[mi].mAnimFAmt]); // TODO find a cleaner way to access the offeset
			misfiledata[mi].mAnimData[0] = NULL;
		}
		return;
	}

	for (i = 0; i < misfiledata[mi].mAnimFAmt; i++) {
		if (misfiledata[mi].mAnimData[i]) {
			pFrameTable = (DWORD *)misfiledata[mi].mAnimData[i];
			misfiledata[mi].mAnimData[i] = NULL;
			mem_free_dbg(pFrameTable);
		}
	}
}

void FreeMissiles()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (!(misfiledata[mi].mFlags & MFLAG_HIDDEN))
			FreeMissileGFX(mi);
	}
}

void FreeMissiles2()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (misfiledata[mi].mFlags & MFLAG_HIDDEN)
			FreeMissileGFX(mi);
	}
}

void InitMissiles()
{
	int mi, src, i, j;

	plr[myplr]._pSpellFlags &= ~0x1;
	if (plr[myplr]._pInfraFlag == TRUE) {
		for (i = 0; i < nummissiles; ++i) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MIS_INFRA) {
				src = missile[mi]._misource;
				if (src == myplr)
					CalcPlrItemVals(src, 1);
			}
		}
	}
	nummissiles = 0;
	for (i = 0; i < MAXMISSILES; i++) {
		missileavail[i] = i;
		missileactive[i] = 0;
	}
	numchains = 0;
	for (i = 0; i < MAXMISSILES; i++) {
		chain[i].idx = -1;
		chain[i]._mitype = 0;
		chain[i]._mirange = 0;
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			dFlags[i][j] &= ~BFLAG_MISSILE;
		}
	}
}

void AddLArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!mienemy) {
		if (plr[id]._pClass == PC_ROGUE)
			GetMissileVel(mi, sx, sy, dx, dy, (plr[id]._pLevel >> 2) + 31);
		else if (plr[id]._pClass == PC_WARRIOR)
			GetMissileVel(mi, sx, sy, dx, dy, (plr[id]._pLevel >> 3) + 31);
		else
			GetMissileVel(mi, sx, sy, dx, dy, 32);
	} else
		GetMissileVel(mi, sx, sy, dx, dy, 32);

	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 5);
}

void AddArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int av;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!mienemy) {
		av = 32;
		if (plr[id]._pIFlags & ISPL_RNDARROWVEL) {
			av = random(64, 32) + 16;
		}
		if (plr[id]._pClass == PC_ROGUE)
			av += (plr[id]._pLevel - 1) >> 2;
		if (plr[id]._pClass == PC_WARRIOR)
			av += (plr[id]._pLevel - 1) >> 3;
		GetMissileVel(mi, sx, sy, dx, dy, av);
	} else {
		GetMissileVel(mi, sx, sy, dx, dy, 32);
	}
	missile[mi]._miAnimFrame = GetDirection16(sx, sy, dx, dy) + 1;
	missile[mi]._mirange = 256;
}

void GetVileMissPos(int mi, int dx, int dy)
{
	int xx, yy, k, j, i;

	for (k = 1; k < 50; k++) {
		for (j = -k; j <= k; j++) {
			yy = j + dy;
			for (i = -k; i <= k; i++) {
				xx = i + dx;
				if (PosOkPlayer(myplr, xx, yy)) {
					missile[mi]._mix = xx;
					missile[mi]._miy = yy;
					return;
				}
			}
		}
	}
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
}

void AddRndTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int pn, r1, r2, nTries;

	nTries = 0;
	do {
		nTries++;
		if (nTries > 500) {
			r1 = 0;
			r2 = 0;
			break;
		}
		r1 = random(58, 3) + 4;
		r2 = random(58, 3) + 4;
		if (random(58, 2) == 1)
			r1 = -r1;
		if (random(58, 2) == 1)
			r2 = -r2;

	} while (nSolidTable[dPiece[r1 + sx][sy + r2]] || dObject[r1 + sx][sy + r2] || dMonster[r1 + sx][sy + r2]);

	missile[mi]._miVar1 = 0;
	missile[mi]._mirange = 2;
	if (!setlevel || setlvlnum != SL_VILEBETRAYER) {
		missile[mi]._mix = sx + r1;
		missile[mi]._miy = sy + r2;
		if (!mienemy)
			UseMana(id, SPL_RNDTELEPORT);
	} else {
		pn = dObject[dx][dy] - 1;
		if (object[pn]._otype == OBJ_MCIRCLE1 || object[pn]._otype == OBJ_MCIRCLE2) {
			missile[mi]._mix = dx;
			missile[mi]._miy = dy;
			if (!PosOkPlayer(myplr, dx, dy))
				GetVileMissPos(mi, dx, dy);
		}
	}
}

void AddFirebolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
	int i, mx, sp;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!micaster) {
		for (i = 0; i < nummissiles; i++) {
			mx = missileactive[i];
			if (missile[mx]._mitype == MIS_GUARDIAN && missile[mx]._misource == id && missile[mx]._miVar3 == mi)
				break;
		}
		if (i == nummissiles)
			UseMana(id, SPL_FIREBOLT);
		if (id != -1) {
			sp = 2 * missile[mi]._mispllvl + 16;
			if (sp >= 63)
				sp = 63;
		} else {
			sp = 16;
		}
	} else {
		sp = 26;
	}
	GetMissileVel(mi, sx, sy, dx, dy, sp);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
}

void AddMagmaball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mitxoff += 3 * missile[mi]._mixvel;
	missile[mi]._mityoff += 3 * missile[mi]._miyvel;
	GetMissilePos(mi);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
}

void miss_null_33(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	PutMissile(mi);
}

void AddTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, pn, k, j, tx, ty;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			tx = dx + CrawlTable[pn - 1];
			ty = dy + CrawlTable[pn];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				if (!(dMonster[tx][ty] | dObject[tx][ty] | dPlayer[tx][ty] | nSolidTable[dPiece[tx][ty]])) {
					missile[mi]._miDelFlag = FALSE;
					missile[mi]._mix = tx;
					missile[mi]._miy = ty;
					missile[mi]._misx = tx;
					missile[mi]._misy = ty;
					i = 6;
					break;
				}
			}
			pn += 2;
		}
	}

	if (!missile[mi]._miDelFlag) {
		UseMana(id, SPL_TELEPORT);
		missile[mi]._mirange = 2;
	}
}

void AddLightball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._midam = dam;
	missile[mi]._miAnimFrame = random(63, 8) + 1;
	missile[mi]._mirange = 255;
	if (id < 0) {
		missile[mi]._miVar1 = sx;
		missile[mi]._miVar2 = sy;
	} else {
		missile[mi]._miVar1 = plr[id].WorldX;
		missile[mi]._miVar2 = plr[id].WorldY;
	}
}

void AddFirewall(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._midam = 16 * (random(53, 10) + random(53, 10) + plr[id]._pLevel + 2) >> 1;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 10;
	i = missile[mi]._mispllvl;
	if (i > 0)
		missile[mi]._mirange = 10 * (i + 1);
	missile[mi]._mirange = ((missile[mi]._mirange * plr[id]._pISplDur >> 3) & 0xFFFFFFF0) + 16 * missile[mi]._mirange;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void AddFireball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!mienemy) {
		missile[mi]._midam = 2 * (plr[id]._pLevel + random(60, 10) + random(60, 10)) + 4;
		for (i = 0; i < missile[mi]._mispllvl; i++) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		i = 2 * missile[mi]._mispllvl + 16;
		if (i > 50)
			i = 50;
		UseMana(id, SPL_FIREBALL);
	} else {
		i = 16;
	}
	GetMissileVel(mi, sx, sy, dx, dy, i);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = sx;
	missile[mi]._miVar5 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
}

void AddLightctrl(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (!dam && !mienemy)
		UseMana(id, SPL_LIGHTNING);
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	GetMissileVel(mi, sx, sy, dx, dy, 32);
	missile[mi]._miAnimFrame = random(52, 8) + 1;
	missile[mi]._mirange = 256;
}

void AddLightning(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	if (midir >= 0) {
		missile[mi]._mixoff = missile[midir]._mixoff;
		missile[mi]._miyoff = missile[midir]._miyoff;
		missile[mi]._mitxoff = missile[midir]._mitxoff;
		missile[mi]._mityoff = missile[midir]._mityoff;
	}
	missile[mi]._miAnimFrame = random(52, 8) + 1;

	if (midir < 0 || mienemy == 1 || id == -1) {
		if (midir < 0 || id == -1)
			missile[mi]._mirange = 8;
		else
			missile[mi]._mirange = 10;
	} else {
		missile[mi]._mirange = (missile[mi]._mispllvl >> 1) + 6;
	}
	missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 4);
}

void AddMisexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	CMonster *mon;

	if (mienemy && id > 0) {
		mon = monster[id].MType;
		if (mon->mtype == MT_SUCCUBUS)
			SetMissAnim(mi, MFILE_FLAREEXP);
		if (mon->mtype == MT_SNOWWICH)
			SetMissAnim(mi, MFILE_SCBSEXPB);
		if (mon->mtype == MT_HLSPWN)
			SetMissAnim(mi, MFILE_SCBSEXPD);
		if (mon->mtype == MT_SOLBRNR)
			SetMissAnim(mi, MFILE_SCBSEXPC);
	}

	missile[mi]._mix = missile[dx]._mix;
	missile[mi]._miy = missile[dx]._miy;
	missile[mi]._misx = missile[dx]._misx;
	missile[mi]._misy = missile[dx]._misy;
	missile[mi]._mixoff = missile[dx]._mixoff;
	missile[mi]._miyoff = missile[dx]._miyoff;
	missile[mi]._mitxoff = missile[dx]._mitxoff;
	missile[mi]._mityoff = missile[dx]._mityoff;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddWeapexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = dx;
	missile[mi]._mimfnum = 0;
	if (dx == 1)
		SetMissAnim(mi, MFILE_MAGBLOS);
	else
		SetMissAnim(mi, MFILE_MINILTNG);
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
}

BOOL CheckIfTrig(int x, int y)
{
	int i;

	for (i = 0; i < numtrigs; i++) {
		if ((x == trigs[i]._tx && y == trigs[i]._ty) || (abs(trigs[i]._tx - x) < 2 && abs(trigs[i]._ty - y) < 2))
			return TRUE;
	}
	return FALSE;
}

void AddTown(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, j, k, mx, tx, ty;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	tx = dx;
	if (currlevel) {
		missile[mi]._miDelFlag = TRUE;
		for (j = 0; j < 6; j++) {
			k = CrawlNum[j] + 2;
			for (i = (BYTE)CrawlTable[CrawlNum[j]]; i > 0; i--) {
				tx = dx + CrawlTable[k - 1];
				ty = dy + CrawlTable[k];
				if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					if (!(dObject[tx][ty] | dPlayer[tx][ty] | dMissile[tx][ty] | nSolidTable[dPiece[tx][ty]] | nMissileTable[dPiece[tx][ty]])) {
						if (!CheckIfTrig(tx, ty)) {
							missile[mi]._miDelFlag = FALSE;
							missile[mi]._mix = tx;
							missile[mi]._miy = ty;
							missile[mi]._misx = tx;
							missile[mi]._misy = ty;
							j = 6;
							break;
						}
					}
				}
				k += 2;
			}
		}
	} else {
		ty = dy;
		missile[mi]._mix = tx;
		missile[mi]._miy = ty;
		missile[mi]._misx = tx;
		missile[mi]._misy = ty;
		missile[mi]._miDelFlag = FALSE;
	}
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = 100 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_TOWN && mx != mi && missile[mx]._misource == id)
			missile[mx]._mirange = 0;
	}
	PutMissile(mi);
	if (id == myplr && !missile[mi]._miDelFlag && currlevel) {
		if (!setlevel) {
			NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, tx, ty, currlevel, leveltype, 0);
		} else {
			NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, tx, ty, setlvlnum, leveltype, 1);
		}
	}
}

void AddFlash(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (!mienemy && id != -1) {
		missile[mi]._midam = 0;
		for (i = 0; i <= plr[id]._pLevel; i++) {
			missile[mi]._midam += random(55, 20) + 1;
		}
		for (i = 0; i < missile[mi]._mispllvl; i++) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		missile[mi]._midam += missile[mi]._midam >> 1;
		UseMana(id, SPL_FLASH);
	} else {
		if (!mienemy) {
			missile[mi]._midam = currlevel >> 1;
		} else {
			missile[mi]._midam = monster[id].mLevel << 1;
		}
	}
	missile[mi]._mirange = 19;
}

void AddFlash2(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (!mienemy) {
		if (id != -1) {
			missile[mi]._midam = 0;
			for (i = 0; i <= plr[id]._pLevel; i++) {
				missile[mi]._midam += random(56, 2) + 1;
			}
			for (i = 0; i < missile[mi]._mispllvl; i++) {
				missile[mi]._midam += missile[mi]._midam >> 3;
			}
			missile[mi]._midam += missile[mi]._midam >> 1;
		} else {
			missile[mi]._midam = currlevel >> 1;
		}
	}
	missile[mi]._miPreFlag = TRUE;
	missile[mi]._mirange = 19;
}

void AddManashield(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mirange = 48 * plr[id]._pLevel;
	missile[mi]._miVar1 = plr[id]._pHitPoints;
	missile[mi]._miVar2 = plr[id]._pHPBase;
	missile[mi]._miVar8 = -1;
	if (!mienemy)
		UseMana(id, SPL_MANASHIELD);
	if (id == myplr)
		NetSendCmd(TRUE, CMD_SETSHIELD);
	plr[id].pManaShield = TRUE;
}

void AddFiremove(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = random(59, 10) + plr[id]._pLevel + 1;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 255;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._mix++;
	missile[mi]._miy++;
	missile[mi]._miyoff -= 32;
}

void AddGuardian(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, pn, k, j, tx, ty;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._midam = random(62, 10) + (plr[id]._pLevel >> 1) + 1;
	for (i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._midam += missile[mi]._midam >> 3;
	}

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		pn = CrawlNum[i];
		k = pn + 2;
		for (j = (BYTE)CrawlTable[pn]; j > 0; j--) {
			tx = dx + CrawlTable[k - 1];
			ty = dy + CrawlTable[k];
			pn = dPiece[tx][ty];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				if (LineClear(sx, sy, tx, ty)) {
					if (!(dMonster[tx][ty] | dObject[tx][ty] | dMissile[tx][ty] | nSolidTable[pn] | nMissileTable[pn])) {
						missile[mi]._miDelFlag = FALSE;
						missile[mi]._mix = tx;
						missile[mi]._miy = ty;
						missile[mi]._misx = tx;
						missile[mi]._misy = ty;
						UseMana(id, SPL_GUARDIAN);
						i = 6;
						break;
					}
				}
			}
			k += 2;
		}
	}

	if (missile[mi]._miDelFlag != TRUE) {
		missile[mi]._misource = id;
		missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 1);

		missile[mi]._mirange = plr[id]._pLevel >> 1;
		missile[mi]._mirange += missile[mi]._mispllvl;
		missile[mi]._mirange += (missile[mi]._mirange * plr[id]._pISplDur) >> 7;

		if (missile[mi]._mirange > 30)
			missile[mi]._mirange = 30;
		missile[mi]._mirange <<= 4;
		if (missile[mi]._mirange < 30)
			missile[mi]._mirange = 30;

		missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
		missile[mi]._miVar3 = 1;
		missile[mi]._miVar2 = 0;
	}
}

void AddChain(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	missile[mi]._mirange = 1;
	UseMana(id, SPL_CHAIN);
}

void miss_null_11(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	SetMissDir(mi, dx);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = 250;
}

void miss_null_12(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (dx > 3)
		dx = 2;
	SetMissDir(mi, dx);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = 250;
}

void miss_null_13(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (dx > 3)
		dx = 2;
	SetMissDir(mi, dx);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void AddRhino(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;

	if (monster[id].MType->mtype < MT_HORNED || monster[id].MType->mtype > MT_OBLORD) {
		if (monster[id].MType->mtype < MT_NSNAKE || monster[id].MType->mtype > MT_GSNAKE) {
			anim = &monster[id].MType->Anims[MA_WALK];
		} else {
			anim = &monster[id].MType->Anims[MA_ATTACK];
		}
	} else {
		anim = &monster[id].MType->Anims[MA_SPECIAL];
	}
	GetMissileVel(mi, sx, sy, dx, dy, 18);
	missile[mi]._miAnimFlags = 0;
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimData = anim->Data[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monster[id].MType->width;
	missile[mi]._miAnimWidth2 = monster[id].MType->width2;
	missile[mi]._miAnimAdd = 1;
	if (monster[id].MType->mtype >= MT_NSNAKE && monster[id].MType->mtype <= MT_GSNAKE)
		missile[mi]._miAnimFrame = 7;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = TRUE;
	if (monster[id]._uniqtype != 0) {
		missile[mi]._miUniqTrans = monster[id]._uniqtrans + 1;
		missile[mi]._mlid = monster[id].mlid;
	}
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void miss_null_32(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;

	anim = &monster[id].MType->Anims[MA_WALK];
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimFlags = 0;
	missile[mi]._miAnimData = anim->Data[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monster[id].MType->width;
	missile[mi]._miAnimWidth2 = monster[id].MType->width2;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = TRUE;
	if (monster[id]._uniqtype != 0)
		missile[mi]._miUniqTrans = monster[id]._uniqtrans + 1;
	dMonster[monster[id]._mx][monster[id]._my] = 0;
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void AddFlare(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	if (!mienemy) {
		UseMana(id, SPL_FLARE);
		drawhpflag = TRUE;
		plr[id]._pHitPoints -= 320;
		plr[id]._pHPBase -= 320;
		if (plr[id]._pHitPoints <= 0)
			SyncPlrKill(id, 0);
	} else {
		if (id > 0) {
			if (monster[id].MType->mtype == MT_SUCCUBUS)
				SetMissAnim(mi, MFILE_FLARE);
			if (monster[id].MType->mtype == MT_SNOWWICH)
				SetMissAnim(mi, MFILE_SCUBMISB);
			if (monster[id].MType->mtype == MT_HLSPWN)
				SetMissAnim(mi, MFILE_SCUBMISD);
			if (monster[id].MType->mtype == MT_SOLBRNR)
				SetMissAnim(mi, MFILE_SCUBMISC);
		}
	}
}

void AddAcid(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mlid = -1;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mirange = 5 * (monster[id]._mint + 4);
	PutMissile(mi);
}

void miss_null_1D(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mirange = 50;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._miVar1 = 50 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void AddAcidpud(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int monst;

	monst = missile[mi]._misource;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._mixoff = 0;
	missile[mi]._miyoff = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = random(50, 15) + 40 * (monster[monst]._mint + 1);
	missile[mi]._miPreFlag = TRUE;
}

void AddStone(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, j, k, l, tx, ty, mid;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._misource = id;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		l = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			tx = dx + CrawlTable[l - 1];
			ty = dy + CrawlTable[l];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = dMonster[tx][ty];
				mid = mid > 0 ? mid - 1 : -1 - mid;
				if (mid > 3 && monster[mid]._mAi != AI_DIABLO) {
					if (monster[mid]._mmode != MM_FADEIN && monster[mid]._mmode != MM_FADEOUT && monster[mid]._mmode != MM_CHARGE) {
						j = -99;
						i = 6;
						missile[mi]._miVar1 = monster[mid]._mmode;
						missile[mi]._miVar2 = mid;
						monster[mid]._mmode = MM_STONE;
						break;
					}
				}
			}
			l += 2;
		}
	}

	if (j != -99) {
		missile[mi]._miDelFlag = TRUE;
	} else {
		missile[mi]._mix = tx;
		missile[mi]._miy = ty;
		missile[mi]._misx = tx;
		missile[mi]._misy = ty;
		missile[mi]._mirange = 6;
		missile[mi]._mirange += missile[mi]._mispllvl;
		missile[mi]._mirange += (missile[mi]._mirange * plr[id]._pISplDur) >> 7;

		if (missile[mi]._mirange > 15)
			missile[mi]._mirange = 15;
		missile[mi]._mirange <<= 4;
		UseMana(id, SPL_STONE);
	}
}

void AddGolem(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;
	int mx;

	missile[mi]._miDelFlag = FALSE;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_GOLEM) {
			if (mx != mi && missile[mx]._misource == id) {
				missile[mi]._miDelFlag = TRUE;
				return;
			}
		}
	}
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;
	if ((monster[id]._mx != 1 || monster[id]._my) && id == myplr)
		M_StartKill(id, id);
	UseMana(id, SPL_GOLEM);
}

void AddEtherealize(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._mirange = 16 * plr[id]._pLevel >> 1;
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		missile[mi]._mirange += missile[mi]._mirange >> 3;
	}
	missile[mi]._mirange += missile[mi]._mirange * plr[id]._pISplDur >> 7;
	missile[mi]._miVar1 = plr[id]._pHitPoints;
	missile[mi]._miVar2 = plr[id]._pHPBase;
	if (!mienemy)
		UseMana(id, SPL_ETHEREALIZE);
}

void miss_null_1F(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
}

void miss_null_23(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._misource = id;
	if (dam == 1)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, 1);
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void AddBoom(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._midam = dam;
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddHeal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;
	int HealAmount;

	HealAmount = (random(57, 10) + 1) << 6;
	for (i = 0; i < plr[id]._pLevel; i++) {
		HealAmount += (random(57, 4) + 1) << 6;
	}
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		HealAmount += (random(57, 6) + 1) << 6;
	}

	if (plr[id]._pClass == PC_WARRIOR)
		HealAmount += HealAmount;

	if (plr[id]._pClass == PC_ROGUE)
		HealAmount += HealAmount >> 1;

	plr[id]._pHitPoints += HealAmount;
	if (plr[id]._pHitPoints > plr[id]._pMaxHP)
		plr[id]._pHitPoints = plr[id]._pMaxHP;

	plr[id]._pHPBase += HealAmount;
	if (plr[id]._pHPBase > plr[id]._pMaxHPBase)
		plr[id]._pHPBase = plr[id]._pMaxHPBase;

	UseMana(id, SPL_HEAL);
	missile[mi]._miDelFlag = TRUE;
	drawhpflag = TRUE;
}

void AddHealOther(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_HEALOTHER);
	if (id == myplr)
		SetCursor_(CURSOR_HEALOTHER);
}

void AddElement(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	missile[mi]._midam = 2 * (plr[id]._pLevel + random(60, 10) + random(60, 10)) + 4;
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		missile[mi]._midam += missile[mi]._midam >> 3;
	}
	missile[mi]._midam >>= 1;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection8(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	UseMana(id, SPL_ELEMENT);
}

void AddIdentify(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_IDENTIFY);
	if (id == myplr) {
		if (sbookflag)
			sbookflag = FALSE;
		if (!invflag)
			invflag = 1;
		SetCursor_(CURSOR_IDENTIFY);
	}
}

void AddFirewallC(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, j, k, tx, ty, pn;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			tx = dx + CrawlTable[pn - 1];
			ty = dy + CrawlTable[pn];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				k = dPiece[tx][ty];
				if (LineClear(sx, sy, tx, ty)) {
					if ((sx != tx || sy != ty) && !(nSolidTable[k] | dObject[tx][ty])) {
						missile[mi]._miDelFlag = FALSE;
						missile[mi]._miVar1 = tx;
						missile[mi]._miVar2 = ty;
						missile[mi]._miVar5 = tx;
						missile[mi]._miVar6 = ty;
						i = 6;
						break;
					}
				}
			}
			pn += 2;
		}
	}

	if (missile[mi]._miDelFlag != TRUE) {
		missile[mi]._miVar7 = 0;
		missile[mi]._miVar8 = 0;
		missile[mi]._miVar3 = (midir - 2) & 7;
		missile[mi]._miVar4 = (midir + 2) & 7;
		missile[mi]._mirange = 7;
		UseMana(id, SPL_FIREWALL);
	}
}

void AddInfra(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._mirange = 1584;
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		missile[mi]._mirange += missile[mi]._mirange >> 3;
	}
	missile[mi]._mirange += missile[mi]._mirange * plr[id]._pISplDur >> 7;
	if (!mienemy)
		UseMana(id, SPL_INFRA);
}

void AddWave(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._mirange = 1;
	missile[mi]._miAnimFrame = 4;
	UseMana(id, SPL_WAVE);
}

void AddNova(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int k;

	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	if (id != -1) {
		missile[mi]._midam = (random(66, 6) + random(66, 6) + random(66, 6) + random(66, 6) + random(66, 6) + plr[id]._pLevel + 5) >> 1;
		for (k = 0; k < missile[mi]._mispllvl; k++) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		if (!mienemy)
			UseMana(id, SPL_NOVA);
	} else {
		missile[mi]._midam = ((DWORD)currlevel >> 1) + random(66, 3) + random(66, 3) + random(66, 3);
	}
	missile[mi]._mirange = 1;
}

void AddRepair(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_REPAIR);
	if (id == myplr) {
		if (sbookflag)
			sbookflag = FALSE;
		if (!invflag)
			invflag = 1;
		SetCursor_(CURSOR_REPAIR);
	}
}

void AddRecharge(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_RECHARGE);
	if (id == myplr) {
		if (sbookflag)
			sbookflag = FALSE;
		if (!invflag)
			invflag = 1;
		SetCursor_(CURSOR_RECHARGE);
	}
}

void AddDisarm(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_DISARM);
	if (id == myplr)
		SetCursor_(CURSOR_DISARM);
}

void AddApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._miVar1 = 8;
	missile[mi]._miVar2 = sy - 8;
	missile[mi]._miVar3 = sy + 8;
	missile[mi]._miVar4 = sx - 8;
	missile[mi]._miVar5 = sx + 8;
	missile[mi]._miVar6 = sx - 8;
	if (sy - 8 <= 0)
		missile[mi]._miVar2 = 1;
	if (sy + 8 >= MAXDUNY)
		missile[mi]._miVar3 = MAXDUNY - 1;
	if (sx - 8 <= 0)
		missile[mi]._miVar4 = 1;
	if (sx + 8 >= MAXDUNX)
		missile[mi]._miVar5 = MAXDUNX - 1;
	for (i = 0; i < plr[id]._pLevel; i++) {
		missile[mi]._midam += random(67, 6) + 1;
	}
	missile[mi]._miDelFlag = FALSE;
	missile[mi]._mirange = 255;
	UseMana(id, SPL_APOCA);
}

void AddFlame(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._miVar2 = 0;
	if (dam > 0)
		missile[mi]._miVar2 = 5 * dam;
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	missile[mi]._mixoff = missile[midir]._mixoff;
	missile[mi]._miyoff = missile[midir]._miyoff;
	missile[mi]._mitxoff = missile[midir]._mitxoff;
	missile[mi]._mityoff = missile[midir]._mityoff;
	missile[mi]._mirange = missile[mi]._miVar2 + 20;
	missile[mi]._mlid = AddLight(sx, sy, 1);
	if (!mienemy) {
		i = random(79, plr[id]._pLevel) + random(79, 2);
		missile[mi]._midam = 8 * i + 16 + ((8 * i + 16) >> 1);
	} else {
		missile[mi]._midam = monster[id].mMinDamage + random(77, monster[id].mMaxDamage - monster[id].mMinDamage + 1);
	}
}

void AddFlamec(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	GetMissileVel(mi, sx, sy, dx, dy, 32);
	if (!mienemy)
		UseMana(id, SPL_FLAME);
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar1 = sx;
	missile[mi]._mirange = 256;
}

void AddCbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
	/// ASSERT: assert((DWORD)mi < MAXMISSILES);

	if (micaster == 0) {
		if (id == myplr) {
			missile[mi]._mirnd = random(63, 15) + 1;
			missile[mi]._midam = random(68, plr[id]._pMagic >> 2) + 1;
		} else {
			missile[mi]._mirnd = random(63, 15) + 1;
			missile[mi]._midam = random(68, plr[id]._pMagic >> 2) + 1;
		}
	} else {
		missile[mi]._mirnd = random(63, 15) + 1;
		missile[mi]._midam = 15;
	}

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}

	missile[mi]._miAnimFrame = random(63, 8) + 1;
	missile[mi]._mlid = AddLight(sx, sy, 5);

	GetMissileVel(mi, sx, sy, dx, dy, 8);
	missile[mi]._miVar1 = 5;
	missile[mi]._miVar2 = midir;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddHbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
	int sp;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (id != -1) {
		sp = 2 * missile[mi]._mispllvl + 16;
		if (sp >= 63) {
			sp = 63;
		}
	} else {
		sp = 16;
	}
	GetMissileVel(mi, sx, sy, dx, dy, sp);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	missile[mi]._midam = random(69, 10) + plr[id]._pLevel + 9;
	UseMana(id, SPL_HBOLT);
}

void AddResurrect(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	UseMana(id, SPL_RESURRECT);
	if (id == myplr)
		SetCursor_(CURSOR_RESURRECT);
	missile[mi]._miDelFlag = TRUE;
}

void AddResurrectBeam(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._mirange = misfiledata[36].mAnimLen[0];
}

void AddTelekinesis(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_TELEKINESIS);
	if (id == myplr)
		SetCursor_(CURSOR_TELEKINESIS);
}

void AddBoneSpirit(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx = XDirAdd[midir] + dx;
		dy = YDirAdd[midir] + dy;
	}
	missile[mi]._midam = 0;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection8(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	if (!mienemy) {
		UseMana(id, SPL_BONESPIRIT);
		drawhpflag = TRUE;
		plr[id]._pHitPoints -= 384;
		plr[id]._pHPBase -= 384;
		if (plr[id]._pHitPoints <= 0)
			SyncPlrKill(id, 0);
	}
}

void AddRportal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = 100 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	PutMissile(mi);
}

void AddDiabApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int pnum;

	for (pnum = 0; pnum < gbMaxPlayers; pnum++) {
		if (plr[pnum].plractive) {
			if (LineClear(sx, sy, plr[pnum]._px, plr[pnum]._py)) {
				AddMissile(0, 0, plr[pnum]._px, plr[pnum]._py, 0, MIS_BOOM2, mienemy, id, dam, 0);
			}
		}
	}
	missile[mi]._miDelFlag = TRUE;
}

int AddMissile(int sx, int sy, int dx, int dy, int midir, int mitype, char micaster, int id, int midam, int spllvl)
{
	int i, mi;

	if (nummissiles >= MAXMISSILES)
		return -1;

	if (mitype == MIS_MANASHIELD && plr[id].pManaShield == TRUE) {
		if (currlevel != plr[id].plrlevel)
			return -1;

		for (i = 0; i < nummissiles; i++) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == id)
				return -1;
		}
	}

	mi = missileavail[0];

	missileavail[0] = missileavail[MAXMISSILES - nummissiles - 1];
	missileactive[nummissiles] = mi;
	nummissiles++;

	missile[mi]._mitype = mitype;
	missile[mi]._micaster = micaster;
	missile[mi]._misource = id;
	missile[mi]._miAnimType = missiledata[mitype].mFileNum;
	missile[mi]._miDrawFlag = missiledata[mitype].mDraw;
	missile[mi]._mispllvl = spllvl;
	missile[mi]._mimfnum = midir;

	if (missile[mi]._miAnimType == 255 || misfiledata[missile[mi]._miAnimType].mAnimFAmt < 8)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, midir);

	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._mixoff = 0;
	missile[mi]._miyoff = 0;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._mitxoff = 0;
	missile[mi]._mityoff = 0;
	missile[mi]._miDelFlag = FALSE;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miLightFlag = FALSE;
	missile[mi]._miPreFlag = FALSE;
	missile[mi]._miUniqTrans = 0;
	missile[mi]._midam = midam;
	missile[mi]._miHitFlag = FALSE;
	missile[mi]._midist = 0;
	missile[mi]._mlid = -1;
	missile[mi]._mirnd = 0;

	if (missiledata[mitype].mlSFX != -1) {
		PlaySfxLoc(missiledata[mitype].mlSFX, sx, sy);
	}

	missiledata[mitype].mAddProc(mi, sx, sy, dx, dy, midir, micaster, id, midam);

	return mi;
}

int Sentfire(int i, int sx, int sy)
{
	int ex, dir;

	ex = 0;
	if (LineClear(missile[i]._mix, missile[i]._miy, sx, sy)) {
		if (dMonster[sx][sy] > 0 && monster[dMonster[sx][sy] - 1]._mhitpoints >> 6 > 0 && dMonster[sx][sy] - 1 > 3) {
			dir = GetDirection(missile[i]._mix, missile[i]._miy, sx, sy);
			missile[i]._miVar3 = missileavail[0];
			AddMissile(missile[i]._mix, missile[i]._miy, sx, sy, dir, MIS_FIREBOLT, 0, missile[i]._misource, missile[i]._midam, GetSpellLevel(missile[i]._misource, 1));
			ex = -1;
			SetMissDir(i, 2);
			missile[i]._miVar2 = 3;
		}
	}

	return ex;
}

void MI_Dummy(int i)
{
	return;
}

void MI_Golem(int i)
{
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };
	int tx, ty, dp, l, m, src, k, tid;

	src = missile[i]._misource;
	if (monster[src]._mx == 1 && !monster[src]._my) {
		for (l = 0; l < 6; l++) {
			k = CrawlNum[l];
			tid = k + 2;
			for (m = (BYTE)CrawlTable[k]; m > 0; m--) {
				tx = missile[i]._miVar4 + CrawlTable[tid - 1];
				ty = missile[i]._miVar5 + CrawlTable[tid];
				if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
					dp = dPiece[tx][ty];
					if (LineClear(missile[i]._miVar1, missile[i]._miVar2, tx, ty)) {
						if (!(dMonster[tx][ty] | nSolidTable[dp] | dObject[tx][ty])) {
							l = 6;
							SpawnGolum(src, tx, ty, i);
							break;
						}
					}
				}
				tid += 2;
			}
		}
	}
	missile[i]._miDelFlag = TRUE;
}

void MI_SetManashield(int i)
{
	ManashieldFlag = TRUE;
}

void MI_LArrow(int i)
{
	int p, mind, maxd, rst;

	missile[i]._mirange--;
	p = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_MINILTNG || missile[i]._miAnimType == MFILE_MAGBLOS) {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miAnimFrame + 5);
		rst = missiledata[missile[i]._mitype].mResist;
		if (missile[i]._mitype == MIS_LARROW) {
			if (p != -1) {
				mind = plr[p]._pILMinDam;
				maxd = plr[p]._pILMaxDam;
			} else {
				mind = random(68, 10) + 1 + currlevel;
				maxd = random(68, 10) + 1 + currlevel * 2;
			}
			missiledata[MIS_LARROW].mResist = MISR_LIGHTNING;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 1);
		}
		if (missile[i]._mitype == MIS_FARROW) {
			if (p != -1) {
				mind = plr[p]._pIFMinDam;
				maxd = plr[p]._pIFMaxDam;
			} else {
				mind = random(68, 10) + 1 + currlevel;
				maxd = random(68, 10) + 1 + currlevel * 2;
			}
			missiledata[MIS_FARROW].mResist = MISR_FIRE;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 1);
		}
		missiledata[missile[i]._mitype].mResist = rst;
	} else {
		missile[i]._midist++;
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);

		if (p != -1) {
			if (!missile[i]._micaster) {
				mind = plr[p]._pIMinDam;
				maxd = plr[p]._pIMaxDam;
			} else {
				mind = monster[p].mMinDamage;
				maxd = monster[p].mMaxDamage;
			}
		} else {
			mind = random(68, 10) + 1 + currlevel;
			maxd = random(68, 10) + 1 + currlevel * 2;
		}

		if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy) {
			rst = missiledata[missile[i]._mitype].mResist;
			missiledata[missile[i]._mitype].mResist = 0;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
			missiledata[missile[i]._mitype].mResist = rst;
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = 0;
			missile[i]._mitxoff -= missile[i]._mixvel;
			missile[i]._mityoff -= missile[i]._miyvel;
			GetMissilePos(i);
			if (missile[i]._mitype == MIS_LARROW)
				SetMissAnim(i, MFILE_MINILTNG);
			else
				SetMissAnim(i, MFILE_MAGBLOS);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mix;
				missile[i]._miVar2 = missile[i]._miy;
				ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 5);
			}
		}
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Arrow(int i)
{
	int p, mind, maxd;

	missile[i]._mirange--;
	missile[i]._midist++;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	p = missile[i]._misource;
	if (p != -1) {
		if (!missile[i]._micaster) {
			mind = plr[p]._pIMinDam;
			maxd = plr[p]._pIMaxDam;
		} else {
			mind = monster[p].mMinDamage;
			maxd = monster[p].mMaxDamage;
		}
	} else {
		mind = currlevel;
		maxd = 2 * currlevel;
	}
	if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy)
		CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Firebolt(int i)
{
	int omx, omy;
	int d, p;

	missile[i]._mirange--;
	if (missile[i]._mitype != MIS_BONESPIRIT || missile[i]._mimfnum != 8) {
		omx = missile[i]._mitxoff;
		omy = missile[i]._mityoff;
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		p = missile[i]._misource;
		if (p != -1) {
			if (!missile[i]._micaster) {
				switch (missile[i]._mitype) {
				case MIS_FIREBOLT:
					d = random(75, 10) + (plr[p]._pMagic >> 3) + missile[i]._mispllvl + 1;
					break;
				case MIS_FLARE:
					d = 3 * missile[i]._mispllvl - (plr[p]._pMagic >> 3) + (plr[p]._pMagic >> 1);
					break;
				case MIS_BONESPIRIT:
					d = 0;
					break;
				}
			} else {
				d = monster[p].mMinDamage + random(77, monster[p].mMaxDamage - monster[p].mMinDamage + 1);
			}
		} else {
			d = currlevel + random(78, 2 * currlevel);
		}
		if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy) {
			CheckMissileCol(i, d, d, 0, missile[i]._mix, missile[i]._miy, 0);
		}
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			missile[i]._mitxoff = omx;
			missile[i]._mityoff = omy;
			GetMissilePos(i);
			switch (missile[i]._mitype) {
			case MIS_FIREBOLT:
			case MIS_MAGMABALL:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIS_MISEXP, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_FLARE:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIS_MISEXP2, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_ACID:
				AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIS_MISEXP3, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_BONESPIRIT:
				SetMissDir(i, 8);
				missile[i]._mirange = 7;
				missile[i]._miDelFlag = FALSE;
				PutMissile(i);
				return;
			}
			if (missile[i]._mlid >= 0)
				AddUnLight(missile[i]._mlid);
			PutMissile(i);
		} else {
			if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mix;
				missile[i]._miVar2 = missile[i]._miy;
				if (missile[i]._mlid >= 0)
					ChangeLight(missile[i]._mlid, missile[i]._miVar1, missile[i]._miVar2, 8);
			}
			PutMissile(i);
		}
	} else if (!missile[i]._mirange) {
		if (missile[i]._mlid >= 0)
			AddUnLight(missile[i]._mlid);
		missile[i]._miDelFlag = TRUE;
		PlaySfxLoc(LS_BSIMPCT, missile[i]._mix, missile[i]._miy);
		PutMissile(i);
	} else
		PutMissile(i);
}

void MI_Lightball(int i)
{
	int tx, ty, j, oi;
	char obj;

	tx = missile[i]._miVar1;
	missile[i]._mitxoff += missile[i]._mixvel;
	ty = missile[i]._miVar2;
	missile[i]._mirange--;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	obj = dObject[tx][ty];
	if (obj && tx == missile[i]._mix && ty == missile[i]._miy) {
		if (obj > 0) {
			oi = obj - 1;
		} else {
			oi = -1 - obj;
		}
		if (object[oi]._otype == OBJ_SHRINEL || object[oi]._otype == OBJ_SHRINER)
			missile[i]._mirange = j;
	}
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void mi_null_33(int i)
{
	missile[i]._mirange--;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Acidpud(int i)
{
	int range;

	missile[i]._mirange--;
	range = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	missile[i]._mirange = range;
	if (!range) {
		if (missile[i]._mimfnum) {
			missile[i]._miDelFlag = TRUE;
		} else {
			SetMissDir(i, 1);
			missile[i]._mirange = missile[i]._miAnimLen;
		}
	}
	PutMissile(i);
}

void MI_Firewall(int i)
{
	int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = random(83, 11) + 1;
	}
	if (missile[i]._mirange == missile[i]._miAnimLen - 1) {
		SetMissDir(i, 0);
		missile[i]._miAnimAdd = -1;
		missile[i]._miAnimFrame = 13;
	}
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 1);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum && missile[i]._mirange && missile[i]._miAnimAdd != -1 && missile[i]._miVar2 < 12) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	PutMissile(i);
}

void MI_Fireball(int i)
{
	int dam, id, px, py, mx, my;

	dam = missile[i]._midam;
	id = missile[i]._misource;
	missile[i]._mirange--;

	if (missile[i]._micaster == 0) {
		px = plr[id].WorldX;
		py = plr[id].WorldY;
	} else {
		px = monster[id]._mx;
		py = monster[id]._my;
	}

	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy)
			CheckMissileCol(i, dam, dam, 0, missile[i]._mix, missile[i]._miy, 0);
		if (!missile[i]._mirange) {
			mx = missile[i]._mix;
			my = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._mix, my, missile[i]._miAnimFrame);
			if (!CheckBlock(px, py, mx, my))
				CheckMissileCol(i, dam, dam, 0, mx, my, 1);
			if (!CheckBlock(px, py, mx, my + 1))
				CheckMissileCol(i, dam, dam, 0, mx, my + 1, 1);
			if (!CheckBlock(px, py, mx, my - 1))
				CheckMissileCol(i, dam, dam, 0, mx, my - 1, 1);
			if (!CheckBlock(px, py, mx + 1, my))
				CheckMissileCol(i, dam, dam, 0, mx + 1, my, 1);
			if (!CheckBlock(px, py, mx + 1, my - 1))
				CheckMissileCol(i, dam, dam, 0, mx + 1, my - 1, 1);
			if (!CheckBlock(px, py, mx + 1, my + 1))
				CheckMissileCol(i, dam, dam, 0, mx + 1, my + 1, 1);
			if (!CheckBlock(px, py, mx - 1, my))
				CheckMissileCol(i, dam, dam, 0, mx - 1, my, 1);
			if (!CheckBlock(px, py, mx - 1, my + 1))
				CheckMissileCol(i, dam, dam, 0, mx - 1, my + 1, 1);
			if (!CheckBlock(px, py, mx - 1, my - 1))
				CheckMissileCol(i, dam, dam, 0, mx - 1, my - 1, 1);
			if (!TransList[dTransVal[mx][my]]
			    || (missile[i]._mixvel < 0 && ((TransList[dTransVal[mx][my + 1]] && nSolidTable[dPiece[mx][my + 1]]) || (TransList[dTransVal[mx][my - 1]] && nSolidTable[dPiece[mx][my - 1]])))) {
				missile[i]._mix++;
				missile[i]._miy++;
				missile[i]._miyoff -= 32;
			}
			if (missile[i]._miyvel > 0
			    && (TransList[dTransVal[mx + 1][my]] && nSolidTable[dPiece[mx + 1][my]]
			        || TransList[dTransVal[mx - 1][my]] && nSolidTable[dPiece[mx - 1][my]])) {
				missile[i]._miyoff -= 32;
			}
			if (missile[i]._mixvel > 0
			    && (TransList[dTransVal[mx][my + 1]] && nSolidTable[dPiece[mx][my + 1]]
			        || TransList[dTransVal[mx][my - 1]] && nSolidTable[dPiece[mx][my - 1]])) {
				missile[i]._mixoff -= 32;
			}
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
			missile[i]._miVar1 = missile[i]._mix;
			missile[i]._miVar2 = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 8);
		}
	}

	PutMissile(i);
}

void MI_Lightctrl(int i)
{
	int pn, dam, p, mx, my;

	/// ASSERT: assert((DWORD)i < MAXMISSILES);
	missile[i]._mirange--;

	p = missile[i]._misource;
	if (p != -1) {
		if (missile[i]._micaster == 0) {
			dam = (random(79, 2) + random(79, plr[p]._pLevel) + 2) << 6;
		} else {
			dam = 2 * (monster[p].mMinDamage + random(80, monster[p].mMaxDamage - monster[p].mMinDamage + 1));
		}
	} else {
		dam = random(81, currlevel) + 2 * currlevel;
	}

	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);

	mx = missile[i]._mix;
	my = missile[i]._miy;
	/// ASSERT: assert((DWORD)mx < MAXDUNX);
	/// ASSERT: assert((DWORD)my < MAXDUNY);
	pn = dPiece[mx][my];
	/// ASSERT: assert((DWORD)pn <= MAXTILES);

	if (missile[i]._misource == -1) {
		if ((mx != missile[i]._misx || my != missile[i]._misy) && nMissileTable[pn]) {
			missile[i]._mirange = 0;
		}
	} else if (nMissileTable[pn]) {
		missile[i]._mirange = 0;
	}
	if (!nMissileTable[pn]) {
		if ((mx != missile[i]._miVar1 || my != missile[i]._miVar2) && mx > 0 && my > 0 && mx < MAXDUNX && my < MAXDUNY) {
			if (missile[i]._misource != -1) {
				if (missile[i]._micaster == 1
				    && monster[missile[i]._misource].MType->mtype >= MT_STORM
				    && monster[missile[i]._misource].MType->mtype <= MT_MAEL) {
					AddMissile(
					    mx,
					    my,
					    missile[i]._misx,
					    missile[i]._misy,
					    i,
					    MIS_LIGHTNING2,
					    missile[i]._micaster,
					    missile[i]._misource,
					    dam,
					    missile[i]._mispllvl);
				} else {
					AddMissile(
					    mx,
					    my,
					    missile[i]._misx,
					    missile[i]._misy,
					    i,
					    MIS_LIGHTNING,
					    missile[i]._micaster,
					    missile[i]._misource,
					    dam,
					    missile[i]._mispllvl);
				}
			} else {
				AddMissile(
				    mx,
				    my,
				    missile[i]._misx,
				    missile[i]._misy,
				    i,
				    MIS_LIGHTNING,
				    missile[i]._micaster,
				    missile[i]._misource,
				    dam,
				    missile[i]._mispllvl);
			}
			missile[i]._miVar1 = missile[i]._mix;
			missile[i]._miVar2 = missile[i]._miy;
		}
	}
	if (missile[i]._mirange == 0 || mx <= 0 || my <= 0 || mx >= MAXDUNX || my > MAXDUNY) {
		missile[i]._miDelFlag = TRUE;
	}
}

void MI_Lightning(int i)
{
	int j;

	missile[i]._mirange--;
	j = missile[i]._mirange;
	if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Town(int i)
{
	int ExpLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };
	int p;

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);
	if (currlevel && missile[i]._mimfnum != 1 && missile[i]._mirange) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}

	for (p = 0; p < MAX_PLRS; p++) {
		if (plr[p].plractive && currlevel == plr[p].plrlevel && !plr[p]._pLvlChanging && !plr[p]._pmode && plr[p].WorldX == missile[i]._mix && plr[p].WorldY == missile[i]._miy) {
			ClrPlrPath(p);
			if (p == myplr) {
				NetSendCmdParam1(TRUE, CMD_WARP, missile[i]._misource);
				plr[p]._pmode = 10;
			}
		}
	}

	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Flash(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource]._pInvincible = TRUE;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix - 1, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix + 1, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix - 1, missile[i]._miy + 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy + 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix + 1, missile[i]._miy + 1, 1);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource]._pInvincible = FALSE;
		}
	}
	PutMissile(i);
}

void MI_Flash2(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource]._pInvincible = TRUE;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix - 1, missile[i]._miy - 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy - 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix + 1, missile[i]._miy - 1, 1);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource]._pInvincible = FALSE;
		}
	}
	PutMissile(i);
}

void MI_Manashield(int i)
{
	int id, diff;

	id = missile[i]._misource;
	missile[i]._mix = plr[id].WorldX;
	missile[i]._miy = plr[id].WorldY;
	missile[i]._mitxoff = plr[id]._pxoff << 16;
	missile[i]._mityoff = plr[id]._pyoff << 16;
	if (plr[id]._pmode == PM_WALK3) {
		missile[i]._misx = plr[id]._px;
		missile[i]._misy = plr[id]._py;
	} else {
		missile[i]._misx = plr[id].WorldX;
		missile[i]._misy = plr[id].WorldY;
	}
	GetMissilePos(i);
	if (plr[id]._pmode == PM_WALK3) {
		if (plr[id]._pdir == DIR_W)
			missile[i]._mix++;
		else
			missile[i]._miy++;
	}
	if (id != myplr) {
		if (currlevel != plr[id].plrlevel)
			missile[i]._miDelFlag = TRUE;
	} else {
		if (plr[id]._pMana <= 0 || !plr[id].plractive)
			missile[i]._mirange = 0;
		if (plr[id]._pHitPoints < missile[i]._miVar1) {
			diff = missile[i]._miVar1 - plr[id]._pHitPoints;
			if (missile[i]._mispllvl > 0) {
				diff += diff / -3;
			}

			if (diff < 0)
				diff = 0;
			drawmanaflag = TRUE;
			drawhpflag = TRUE;

			if (plr[id]._pMana >= diff) {
				plr[id]._pHitPoints = missile[i]._miVar1;
				plr[id]._pHPBase = missile[i]._miVar2;
				plr[id]._pMana -= diff;
				plr[id]._pManaBase -= diff;
			} else {
				missile[i]._miDelFlag = TRUE;
				plr[id]._pHitPoints = plr[id]._pMana + missile[i]._miVar1 - diff;
				plr[id]._pHPBase = plr[id]._pMana + missile[i]._miVar2 - diff;
				plr[id]._pMana = 0;
				plr[id]._pManaBase = plr[id]._pMaxManaBase - plr[id]._pMaxMana;
				missile[i]._mirange = 0;
				if (plr[id]._pHitPoints < 0)
					SetPlayerHitPoints(id, 0);
				if (!(plr[id]._pHitPoints >> 6) && id == myplr) {
					SyncPlrKill(id, missile[i]._miVar8);
				}
			}
		}

		if (id == myplr && !plr[id]._pHitPoints && !missile[i]._miVar1 && plr[id]._pmode != PM_DEATH) {
			missile[i]._mirange = 0;
			missile[i]._miDelFlag = TRUE;
			SyncPlrKill(id, -1);
		}
		missile[i]._miVar1 = plr[id]._pHitPoints;
		missile[i]._miVar2 = plr[id]._pHPBase;
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			NetSendCmd(TRUE, CMD_ENDSHIELD);
		}
	}
	PutMissile(i);
}

void MI_Etherealize(int i)
{
	int src;

	src = missile[i]._misource;
	missile[i]._mirange--;
	missile[i]._mix = plr[src].WorldX;
	missile[i]._miy = plr[src].WorldY;
	missile[i]._mitxoff = plr[src]._pxoff << 16;
	missile[i]._mityoff = plr[src]._pyoff << 16;
	if (plr[src]._pmode == PM_WALK3) {
		missile[i]._misx = plr[src]._px;
		missile[i]._misy = plr[src]._py;
	} else {
		missile[i]._misx = missile[i]._mix;
		missile[i]._misy = missile[i]._miy;
	}
	GetMissilePos(i);
	if (plr[src]._pmode == PM_WALK3) {
		if (plr[src]._pdir == 2)
			missile[i]._mix++;
		else
			missile[i]._miy++;
	}
	plr[src]._pSpellFlags |= 1;
	if (!missile[i]._mirange || plr[src]._pHitPoints <= 0) {
		missile[i]._miDelFlag = TRUE;
		plr[src]._pSpellFlags &= ~0x1;
	}
	PutMissile(i);
}

void MI_Firemove(int i)
{
	int j;
	int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i]._mix--;
	missile[i]._miy--;
	missile[i]._miyoff += 32;
	missile[i]._miVar1++;
	if (missile[i]._miVar1 == missile[i]._miAnimLen) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = random(82, 11) + 1;
	}
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum || !missile[i]._mirange) {
		if (missile[i]._mix != missile[i]._miVar3 || missile[i]._miy != missile[i]._miVar4) {
			missile[i]._miVar3 = missile[i]._mix;
			missile[i]._miVar4 = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 8);
		}
	} else {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	missile[i]._mix++;
	missile[i]._miy++;
	missile[i]._miyoff -= 32;
	PutMissile(i);
}

void MI_Guardian(int i)
{
	int j, k, sx, sy, sx1, sy1, ex;

	/// ASSERT: assert((DWORD)i < MAXMISSILES);

	sx1 = 0;
	sy1 = 0;
	missile[i]._mirange--;

	if (missile[i]._miVar2 > 0) {
		missile[i]._miVar2--;
	}
	if (missile[i]._mirange == missile[i]._miVar1 || missile[i]._mimfnum == MFILE_GUARD && missile[i]._miVar2 == 0) {
		SetMissDir(i, 1);
	}

	if (!(missile[i]._mirange % 16)) {
		ex = 0;
		for (j = 0; j < 23 && ex != -1; j++) {
			for (k = 10; k >= 0 && ex != -1 && (vCrawlTable[j][k] != 0 || vCrawlTable[j][k + 1] != 0); k -= 2) {
				if (sx1 == vCrawlTable[j][k] && sy1 == vCrawlTable[j][k + 1]) {
					continue;
				}
				sx = missile[i]._mix + vCrawlTable[j][k];
				sy = missile[i]._miy + vCrawlTable[j][k + 1];
				ex = Sentfire(i, sx, sy);
				if (ex == -1) {
					break;
				}
				sx = missile[i]._mix - vCrawlTable[j][k];
				sy = missile[i]._miy - vCrawlTable[j][k + 1];
				ex = Sentfire(i, sx, sy);
				if (ex == -1) {
					break;
				}
				sx = missile[i]._mix + vCrawlTable[j][k];
				sy = missile[i]._miy - vCrawlTable[j][k + 1];
				ex = Sentfire(i, sx, sy);
				if (ex == -1) {
					break;
				}
				sx = missile[i]._mix - vCrawlTable[j][k];
				sy = missile[i]._miy + vCrawlTable[j][k + 1];
				ex = Sentfire(i, sx, sy);
				if (ex == -1) {
					break;
				}
				sx1 = vCrawlTable[j][k];
				sy1 = vCrawlTable[j][k + 1];
			}
		}
	}

	if (missile[i]._mirange == 14) {
		SetMissDir(i, 0);
		missile[i]._miAnimFrame = 15;
		missile[i]._miAnimAdd = -1;
	}

	missile[i]._miVar3 += missile[i]._miAnimAdd;

	if (missile[i]._miVar3 > 15) {
		missile[i]._miVar3 = 15;
	} else if (missile[i]._miVar3 > 0) {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miVar3);
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}

	PutMissile(i);
}

void MI_Chain(int i)
{
	int sx, sy, id, l, n, m, k, rad, tx, ty, dir;
	int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

	id = missile[i]._misource;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	dir = GetDirection(sx, sy, missile[i]._miVar1, missile[i]._miVar2);
	AddMissile(sx, sy, missile[i]._miVar1, missile[i]._miVar2, dir, MIS_LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
	rad = missile[i]._mispllvl + 3;
	if (rad > 19)
		rad = 19;
	for (m = 1; m < rad; m++) {
		k = CrawlNum[m];
		l = k + 2;
		for (n = (BYTE)CrawlTable[k]; n > 0; n--) {
			tx = sx + CrawlTable[l - 1];
			ty = sy + CrawlTable[l];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY && dMonster[tx][ty] > 0) {
				dir = GetDirection(sx, sy, tx, ty);
				AddMissile(sx, sy, tx, ty, dir, MIS_LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
			}
			l += 2;
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void mi_null_11(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	if (missile[i]._miAnimFrame == missile[i]._miAnimLen)
		missile[i]._miPreFlag = TRUE;
	PutMissile(i);
}

void MI_Weapexp(int i)
{
	int id, mind, maxd;
	int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	missile[i]._mirange--;
	id = missile[i]._misource;
	if (missile[i]._miVar2 == 1) {
		mind = plr[id]._pIFMinDam;
		maxd = plr[id]._pIFMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_FIRE;
	} else {
		mind = plr[id]._pILMinDam;
		maxd = plr[id]._pILMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_LIGHTNING;
	}
	CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
	if (!missile[i]._miVar1) {
		missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 9);
	} else {
		if (missile[i]._mirange)
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar1]);
	}
	missile[i]._miVar1++;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	} else {
		PutMissile(i);
	}
}

void MI_Misexp(int i)
{
	int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	missile[i]._mirange--;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	} else {
		if (!missile[i]._miVar1)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 9);
		else
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar1]);
		missile[i]._miVar1++;
		PutMissile(i);
	}
}

void MI_Acidsplat(int i)
{
	int monst, dam;

	if (missile[i]._mirange == missile[i]._miAnimLen) {
		missile[i]._mix++;
		missile[i]._miy++;
		missile[i]._miyoff -= 32;
	}
	missile[i]._mirange--;
	if (!missile[i]._mirange) {
		monst = missile[i]._misource;
		dam = missile[i]._mispllvl;
		missile[i]._miDelFlag = TRUE;
		AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIS_ACIDPUD, 1, monst, (monster[monst].MData->mLevel >= 2) + 1, dam);
	} else {
		PutMissile(i);
	}
}

void MI_Teleport(int i)
{
	int id;

	id = missile[i]._misource;
	missile[i]._mirange--;
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = TRUE;
	} else {
		dPlayer[plr[id].WorldX][plr[id].WorldY] = 0;
		PlrClrTrans(plr[id].WorldX, plr[id].WorldY);
		plr[id].WorldX = missile[i]._mix;
		plr[id].WorldY = missile[i]._miy;
		plr[id]._px = missile[i]._mix;
		plr[id]._py = missile[i]._miy;
		plr[id]._poldx = missile[i]._mix;
		plr[id]._poldy = missile[i]._miy;
		PlrDoTrans(missile[i]._mix, missile[i]._miy);
		missile[i]._miVar1 = 1;
		dPlayer[plr[id].WorldX][plr[id].WorldY] = id + 1;
		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(plr[id]._plid, plr[id].WorldX, plr[id].WorldY);
			ChangeVisionXY(plr[id]._pvid, plr[id].WorldX, plr[id].WorldY);
		}
		if (id == myplr) {
			ViewX = plr[id].WorldX - ScrollInfo._sdx;
			ViewY = plr[id].WorldY - ScrollInfo._sdy;
		}
	}
}

void MI_Stone(int i)
{
	int m;

	missile[i]._mirange--;
	m = missile[i]._miVar2;
	if (!monster[m]._mhitpoints && missile[i]._miAnimType != MFILE_SHATTER1) {
		missile[i]._mimfnum = 0;
		missile[i]._miDrawFlag = TRUE;
		SetMissAnim(i, MFILE_SHATTER1);
		missile[i]._mirange = 11;
	}
	if (monster[m]._mmode != MM_STONE) {
		missile[i]._miDelFlag = TRUE;
	} else {
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			if (monster[m]._mhitpoints > 0)
				monster[m]._mmode = missile[i]._miVar1;
			else
				AddDead(monster[m]._mx, monster[m]._my, stonendx, (direction)monster[m]._mdir);
		}
		if (missile[i]._miAnimType == MFILE_SHATTER1)
			PutMissile(i);
	}
}

void MI_Boom(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._miVar1)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 1);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._miVar1 = 1;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Rhino(int i)
{
	int mix, miy, mix2, miy2, omx, omy, monst;

	monst = missile[i]._misource;
	if (monster[monst]._mmode != MM_CHARGE) {
		missile[i]._miDelFlag = TRUE;
		return;
	}
	GetMissilePos(i);
	mix = missile[i]._mix;
	miy = missile[i]._miy;
	dMonster[mix][miy] = 0;
	if (monster[monst]._mAi == AI_SNAKE) {
		missile[i]._mitxoff += 2 * missile[i]._mixvel;
		missile[i]._mityoff += 2 * missile[i]._miyvel;
		GetMissilePos(i);
		mix2 = missile[i]._mix;
		miy2 = missile[i]._miy;
		missile[i]._mitxoff -= missile[i]._mixvel;
		missile[i]._mityoff -= missile[i]._miyvel;
	} else {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
	}
	GetMissilePos(i);
	omx = missile[i]._mix;
	omy = missile[i]._miy;
	if (!PosOkMonst(monst, missile[i]._mix, missile[i]._miy) || (monster[monst]._mAi == AI_SNAKE && !PosOkMonst(monst, mix2, miy2))) {
		MissToMonst(i, mix, miy);
		missile[i]._miDelFlag = TRUE;
		return;
	}
	monster[monst]._mfutx = omx;
	monster[monst]._moldx = omx;
	dMonster[omx][omy] = -1 - monst;
	monster[monst]._mx = omx;
	monster[monst]._mfuty = omy;
	monster[monst]._moldy = omy;
	monster[monst]._my = omy;
	if (monster[monst]._uniqtype != 0)
		ChangeLightXY(missile[i]._mlid, omx, omy);
	MoveMissilePos(i);
	PutMissile(i);
}

void mi_null_32(int i)
{
	int src, enemy, ax, ay, bx, by, cx, cy, j;

	GetMissilePos(i);
	ax = missile[i]._mix;
	ay = missile[i]._miy;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	src = missile[i]._misource;
	bx = missile[i]._mix;
	by = missile[i]._miy;
	enemy = monster[src]._menemy;
	if (!(monster[src]._mFlags & MFLAG_TARGETS_MONSTER)) {
		cx = plr[enemy].WorldX;
		cy = plr[enemy].WorldY;
	} else {
		cx = monster[enemy]._mx;
		cy = monster[enemy]._my;
	}
	if ((bx != ax || by != ay) && (missile[i]._miVar1 & 1 && (abs(ax - cx) >= 4 || abs(ay - cy) >= 4) || missile[i]._miVar2 > 1) && PosOkMonst(missile[i]._misource, ax, ay)) {
		MissToMonst(i, ax, ay);
		missile[i]._miDelFlag = TRUE;
	} else if (!(monster[src]._mFlags & MFLAG_TARGETS_MONSTER)) {
		j = dPlayer[bx][by];
	} else {
		j = dMonster[bx][by];
	}
	if (!PosOkMissile(bx, by) || j > 0 && !(missile[i]._miVar1 & 1)) {
		missile[i]._mixvel *= -1;
		missile[i]._miyvel *= -1;
		missile[i]._mimfnum = opposite[missile[i]._mimfnum];
		missile[i]._miAnimData = monster[src].MType->Anims[MA_WALK].Data[missile[i]._mimfnum];
		missile[i]._miVar2++;
		if (j > 0)
			missile[i]._miVar1 |= 1;
	}
	MoveMissilePos(i);
	PutMissile(i);
}

void MI_FirewallC(int i)
{
	int tx, ty, id;

	id = missile[i]._misource;
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
	} else {
		tx = missile[i]._miVar1 + XDirAdd[missile[i]._miVar3];
		ty = missile[i]._miVar2 + YDirAdd[missile[i]._miVar3];
		if (!nMissileTable[dPiece[missile[i]._miVar1][missile[i]._miVar2]] && !missile[i]._miVar8 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar1, missile[i]._miVar2, missile[i]._miVar1, missile[i]._miVar2, plr[id]._pdir, MIS_FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar1 = tx;
			missile[i]._miVar2 = ty;
		} else {
			missile[i]._miVar8 = 1;
		}
		tx = missile[i]._miVar5 + XDirAdd[missile[i]._miVar4];
		ty = missile[i]._miVar6 + YDirAdd[missile[i]._miVar4];
		if (!nMissileTable[dPiece[missile[i]._miVar5][missile[i]._miVar6]] && !missile[i]._miVar7 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar5, missile[i]._miVar6, missile[i]._miVar5, missile[i]._miVar6, plr[id]._pdir, MIS_FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar5 = tx;
			missile[i]._miVar6 = ty;
		} else {
			missile[i]._miVar7 = 1;
		}
	}
}

void MI_Infra(int i)
{
	missile[i]._mirange--;
	plr[missile[i]._misource]._pInfraFlag = TRUE;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		CalcPlrItemVals(missile[i]._misource, TRUE);
	}
}

void MI_Apoca(int i)
{
	int j, k, id;
	BOOL exit;

	id = missile[i]._misource;
	exit = FALSE;
	for (j = missile[i]._miVar2; j < missile[i]._miVar3 && !exit; j++) {
		for (k = missile[i]._miVar4; k < missile[i]._miVar5 && !exit; k++) {
			if (dMonster[k][j] > 3 && !nSolidTable[dPiece[k][j]]) {
				AddMissile(k, j, k, j, plr[id]._pdir, MIS_BOOM, 0, id, missile[i]._midam, 0);
				exit = TRUE;
			}
		}
		if (!exit) {
			missile[i]._miVar4 = missile[i]._miVar6;
		}
	}

	if (exit == TRUE) {
		missile[i]._miVar2 = j - 1;
		missile[i]._miVar4 = k;
	} else {
		missile[i]._miDelFlag = TRUE;
	}
}

void MI_Wave(int i)
{
	int sx, sy, sd, nxa, nxb, nya, nyb, dira, dirb;
	int j, id, pn;
	BOOL f1, f2;
	int v1, v2;

	f1 = FALSE;
	f2 = FALSE;
	/// ASSERT: assert((DWORD)i < MAXMISSILES);

	id = missile[i]._misource;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	v1 = missile[i]._miVar1;
	v2 = missile[i]._miVar2;
	sd = GetDirection(sx, sy, v1, v2);
	dira = (sd - 2) & 7;
	dirb = (sd + 2) & 7;
	nxa = sx + XDirAdd[sd];
	nya = sy + YDirAdd[sd];
	pn = dPiece[nxa][nya];
	/// ASSERT: assert((DWORD)pn <= MAXTILES);
	if (!nMissileTable[pn]) {
		AddMissile(nxa, nya, nxa + XDirAdd[sd], nya + YDirAdd[sd], plr[id]._pdir, MIS_FIREMOVE, 0, id, 0, missile[i]._mispllvl);
		nxa += XDirAdd[dira];
		nya += YDirAdd[dira];
		nxb = sx + XDirAdd[sd] + XDirAdd[dirb];
		nyb = sy + YDirAdd[sd] + YDirAdd[dirb];
		for (j = 0; j < (missile[i]._mispllvl >> 1) + 2; j++) {
			pn = dPiece[nxa][nya]; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			                       /// ASSERT: assert((DWORD)pn <= MAXTILES);
			if (nMissileTable[pn] || f1 || nxa <= 0 || nxa >= MAXDUNX || nya <= 0 || nya >= MAXDUNY) {
				f1 = TRUE;
			} else {
				AddMissile(nxa, nya, nxa + XDirAdd[sd], nya + YDirAdd[sd], plr[id]._pdir, MIS_FIREMOVE, 0, id, 0, missile[i]._mispllvl);
				nxa += XDirAdd[dira];
				nya += YDirAdd[dira];
			}
			pn = dPiece[nxb][nyb]; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			                       /// ASSERT: assert((DWORD)pn <= MAXTILES);
			if (nMissileTable[pn] || f2 || nxb <= 0 || nxb >= MAXDUNX || nyb <= 0 || nyb >= MAXDUNY) {
				f2 = TRUE;
			} else {
				AddMissile(nxb, nyb, nxb + XDirAdd[sd], nyb + YDirAdd[sd], plr[id]._pdir, MIS_FIREMOVE, 0, id, 0, missile[i]._mispllvl);
				nxb += XDirAdd[dirb];
				nyb += YDirAdd[dirb];
			}
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void MI_Nova(int i)
{
	int k, id, sx, sy, dir, en, sx1, sy1, dam;

	sx1 = 0;
	sy1 = 0;
	id = missile[i]._misource;
	dam = missile[i]._midam;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	if (id != -1) {
		en = 0;
		dir = plr[id]._pdir;
	} else {
		dir = 0;
		en = 1;
	}
	for (k = 0; k < 23; k++) {
		if (sx1 != vCrawlTable[k][6] || sy1 != vCrawlTable[k][7]) {
			AddMissile(sx, sy, sx + vCrawlTable[k][6], sy + vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(sx, sy, sx - vCrawlTable[k][6], sy - vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(sx, sy, sx - vCrawlTable[k][6], sy + vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(sx, sy, sx + vCrawlTable[k][6], sy - vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			sx1 = vCrawlTable[k][6];
			sy1 = vCrawlTable[k][7];
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void MI_Blodboil(int i)
{
	missile[i]._miDelFlag = TRUE;
}

void MI_Flame(int i)
{
	int k;

	missile[i]._mirange--;
	k = missile[i]._mirange;
	missile[i]._miVar2--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._mirange == 0 && missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = k;
	if (!missile[i]._miVar2)
		missile[i]._miAnimFrame = 20;
	if (missile[i]._miVar2 <= 0) {
		k = missile[i]._miAnimFrame;
		if (k > 11)
			k = 24 - k;
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, k);
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._miVar2 <= 0)
		PutMissile(i);
}

void MI_Flamec(int i)
{
	int id, src;

	missile[i]._mirange--;
	missile[i]._mitxoff += missile[i]._mixvel;
	src = missile[i]._misource;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
		id = dPiece[missile[i]._mix][missile[i]._miy];
		if (!nMissileTable[id]) {
			AddMissile(
			    missile[i]._mix,
			    missile[i]._miy,
			    missile[i]._misx,
			    missile[i]._misy,
			    i,
			    MIS_FLAME,
			    missile[i]._micaster,
			    src,
			    missile[i]._miVar3,
			    missile[i]._mispllvl);
		} else {
			missile[i]._mirange = 0;
		}
		missile[i]._miVar1 = missile[i]._mix;
		missile[i]._miVar2 = missile[i]._miy;
		missile[i]._miVar3++;
	}
	if (!missile[i]._mirange || missile[i]._miVar3 == 3)
		missile[i]._miDelFlag = TRUE;
}

void MI_Cbolt(int i)
{
	int md;
	int bpath[16] = { -1, 0, 1, -1, 0, 1, -1, -1, 0, 0, 1, 1, 0, 1, -1, 0 };

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_LGHNING) {
		if (!missile[i]._miVar3) {
			md = (missile[i]._miVar2 + bpath[missile[i]._mirnd]) & 7;
			missile[i]._mirnd = (missile[i]._mirnd + 1) & 0xF;
			GetMissileVel(i, missile[i]._mix, missile[i]._miy, missile[i]._mix + XDirAdd[md], missile[i]._miy + YDirAdd[md], 8);
			missile[i]._miVar3 = 16;
		} else {
			missile[i]._miVar3--;
		}
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
		if (missile[i]._miHitFlag == TRUE) {
			missile[i]._miVar1 = 8;
			missile[i]._mimfnum = 0;
			missile[i]._mixoff = 0;
			missile[i]._miyoff = 0;
			SetMissAnim(i, MFILE_LGHNING);
			missile[i]._mirange = missile[i]._miAnimLen;
			GetMissilePos(i);
		}
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miVar1);
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Hbolt(int i)
{
	int dam;

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_HOLYEXPL) {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		dam = missile[i]._midam;
		if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy) {
			CheckMissileCol(i, dam, dam, 0, missile[i]._mix, missile[i]._miy, 0);
		}
		if (!missile[i]._mirange) {
			missile[i]._mitxoff -= missile[i]._mixvel;
			missile[i]._mityoff -= missile[i]._miyvel;
			GetMissilePos(i);
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_HOLYEXPL);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mix;
				missile[i]._miVar2 = missile[i]._miy;
				ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 8);
			}
		}
	} else {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miAnimFrame + 7);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	}
	PutMissile(i);
}

void MI_Element(int i)
{
	int mid, sd, dam, cx, cy, px, py, id;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		cx = missile[i]._mix;
		cy = missile[i]._miy;
		px = plr[id].WorldX;
		py = plr[id].WorldY;
		ChangeLight(missile[i]._mlid, cx, cy, missile[i]._miAnimFrame);
		if (!CheckBlock(px, py, cx, cy))
			CheckMissileCol(i, dam, dam, 1, cx, cy, 1);
		if (!CheckBlock(px, py, cx, cy + 1))
			CheckMissileCol(i, dam, dam, 1, cx, cy + 1, 1);
		if (!CheckBlock(px, py, cx, cy - 1))
			CheckMissileCol(i, dam, dam, 1, cx, cy - 1, 1);
		if (!CheckBlock(px, py, cx + 1, cy))
			CheckMissileCol(i, dam, dam, 1, cx + 1, cy, 1); /* check x/y */
		if (!CheckBlock(px, py, cx + 1, cy - 1))
			CheckMissileCol(i, dam, dam, 1, cx + 1, cy - 1, 1);
		if (!CheckBlock(px, py, cx + 1, cy + 1))
			CheckMissileCol(i, dam, dam, 1, cx + 1, cy + 1, 1);
		if (!CheckBlock(px, py, cx - 1, cy))
			CheckMissileCol(i, dam, dam, 1, cx - 1, cy, 1);
		if (!CheckBlock(px, py, cx - 1, cy + 1))
			CheckMissileCol(i, dam, dam, 1, cx - 1, cy + 1, 1);
		if (!CheckBlock(px, py, cx - 1, cy - 1))
			CheckMissileCol(i, dam, dam, 1, cx - 1, cy - 1, 1);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		cx = missile[i]._mix;
		cy = missile[i]._miy;
		CheckMissileCol(i, dam, dam, 0, cx, cy, 0);
		if (!missile[i]._miVar3 && cx == missile[i]._miVar4 && cy == missile[i]._miVar5)
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(cx, cy, 19);
			if (mid > 0) {
				SetMissDir(i, GetDirection8(cx, cy, monster[mid]._mx, monster[mid]._my));
				GetMissileVel(i, cx, cy, monster[mid]._mx, monster[mid]._my, 16);
			} else {
				sd = plr[id]._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, cx, cy, cx + XDirAdd[sd], cy + YDirAdd[sd], 16);
			}
		}
		if (cx != missile[i]._miVar1 || cy != missile[i]._miVar2) {
			missile[i]._miVar1 = cx;
			missile[i]._miVar2 = cy;
			ChangeLight(missile[i]._mlid, cx, cy, 8);
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		}
	}
	PutMissile(i);
}

void MI_Bonespirit(int i)
{
	int id, mid, sd, dam;
	int cx, cy;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._mimfnum == 8) {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miAnimFrame);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
		PutMissile(i);
	} else {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		cx = missile[i]._mix;
		cy = missile[i]._miy;
		CheckMissileCol(i, dam, dam, FALSE, cx, cy, 0);
		if (missile[i]._miVar3 == 0 && cx == missile[i]._miVar4 && cy == missile[i]._miVar5)
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(cx, cy, 19);
			if (mid > 0) {
				missile[i]._midam = monster[mid]._mhitpoints >> 7;
				SetMissDir(i, GetDirection8(cx, cy, monster[mid]._mx, monster[mid]._my));
				GetMissileVel(i, cx, cy, monster[mid]._mx, monster[mid]._my, 16);
			} else {
				sd = plr[id]._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, cx, cy, cx + XDirAdd[sd], cy + YDirAdd[sd], 16);
			}
		}
		if (cx != missile[i]._miVar1 || cy != missile[i]._miVar2) {
			missile[i]._miVar1 = cx;
			missile[i]._miVar2 = cy;
			ChangeLight(missile[i]._mlid, cx, cy, 8);
		}
		if (!missile[i]._mirange) {
			SetMissDir(i, DIR_OMNI);
			missile[i]._mirange = 7;
		}
		PutMissile(i);
	}
}

void MI_ResurrectBeam(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Rportal(int i)
{
	int ExpLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);

	if (currlevel && missile[i]._mimfnum != 1 && missile[i]._mirange != 0) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void ProcessMissiles()
{
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		dFlags[missile[missileactive[i]]._mix][missile[missileactive[i]]._miy] &= ~BFLAG_MISSILE;
		dMissile[missile[missileactive[i]]._mix][missile[missileactive[i]]._miy] = 0;
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}

	MissilePreFlag = FALSE;
	ManashieldFlag = FALSE;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		missiledata[missile[mi]._mitype].mProc(missileactive[i]);
		if (!(missile[mi]._miAnimFlags & MFLAG_LOCK_ANIMATION)) {
			missile[mi]._miAnimCnt++;
			if (missile[mi]._miAnimCnt >= missile[mi]._miAnimDelay) {
				missile[mi]._miAnimCnt = 0;
				missile[mi]._miAnimFrame += missile[mi]._miAnimAdd;
				if (missile[mi]._miAnimFrame > missile[mi]._miAnimLen)
					missile[mi]._miAnimFrame = 1;
				if (missile[mi]._miAnimFrame < 1)
					missile[mi]._miAnimFrame = missile[mi]._miAnimLen;
			}
		}
	}

	if (ManashieldFlag) {
		for (i = 0; i < nummissiles; i++) {
			if (missile[missileactive[i]]._mitype == MIS_MANASHIELD) {
				MI_Manashield(missileactive[i]);
			}
		}
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}
}

void missiles_process_charge()
{
	CMonster *mon;
	AnimStruct *anim;
	MissileStruct *mis;
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		mis = &missile[mi];
		mis->_miAnimData = misfiledata[mis->_miAnimType].mAnimData[mis->_mimfnum];
		if (mis->_mitype == MIS_RHINO) {
			mon = monster[mis->_misource].MType;
			if (mon->mtype >= MT_HORNED && mon->mtype <= MT_OBLORD) {
				anim = &mon->Anims[MA_SPECIAL];
			} else {
				if (mon->mtype >= MT_NSNAKE && mon->mtype <= MT_GSNAKE)
					anim = &mon->Anims[MA_ATTACK];
				else
					anim = &mon->Anims[MA_WALK];
			}
			missile[mi]._miAnimData = anim->Data[mis->_mimfnum];
		}
	}
}

void ClearMissileSpot(int mi)
{
	dFlags[missile[mi]._mix][missile[mi]._miy] &= ~BFLAG_MISSILE;
	dMissile[missile[mi]._mix][missile[mi]._miy] = 0;
}

DEVILUTION_END_NAMESPACE

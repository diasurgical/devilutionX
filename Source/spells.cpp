/**
 * @file spells.cpp
 *
 * Implementation of functionality for casting player spells.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int GetManaAmount(int id, int sn)
{
	int ma; // mana amount

	// mana adjust
	int adj = 0;

	// spell level
	int sl = plr[id]._pSplLvl[sn] + plr[id]._pISplLvlAdd - 1;

	if (sl < 0) {
		sl = 0;
	}

	if (sl > 0) {
		adj = sl * spelldata[sn].sManaAdj;
	}
	if (sn == SPL_FIREBOLT) {
		adj >>= 1;
	}
	if (sn == SPL_RESURRECT && sl > 0) {
		adj = sl * (spelldata[SPL_RESURRECT].sManaCost / 8);
	}

	if (sn == SPL_HEAL || sn == SPL_HEALOTHER) {
		ma = (spelldata[SPL_HEAL].sManaCost + 2 * plr[id]._pLevel - adj);
	} else if (spelldata[sn].sManaCost == 255) {
		ma = ((BYTE)plr[id]._pMaxManaBase - adj);
	} else {
		ma = (spelldata[sn].sManaCost - adj);
	}

	if (ma < 0)
		ma = 0;
	ma <<= 6;

#ifdef HELLFIRE
	if (plr[id]._pClass == PC_SORCERER) {
		ma >>= 1;
	} else if (plr[id]._pClass == PC_ROGUE || plr[id]._pClass == PC_MONK || plr[id]._pClass == PC_BARD) {
		ma -= ma >> 2;
	}
#else
	if (plr[id]._pClass == PC_ROGUE) {
		ma -= ma >> 2;
	}
#endif

	if (spelldata[sn].sMinMana > ma >> 6) {
		ma = spelldata[sn].sMinMana << 6;
	}

	return ma * (100 - plr[id]._pISplCost) / 100;
}

void UseMana(int id, int sn)
{
	int ma; // mana cost

	if (id == myplr) {
		switch (plr[id]._pSplType) {
		case RSPLTYPE_SKILL:
		case RSPLTYPE_INVALID:
			break;
		case RSPLTYPE_SCROLL:
			RemoveScroll(id);
			break;
		case RSPLTYPE_CHARGES:
			UseStaffCharge(id);
			break;
		case RSPLTYPE_SPELL:
#ifdef _DEBUG
			if (!debug_mode_key_inverted_v) {
#endif
				ma = GetManaAmount(id, sn);
				plr[id]._pMana -= ma;
				plr[id]._pManaBase -= ma;
				drawmanaflag = TRUE;
#ifdef _DEBUG
			}
#endif
			break;
		}
	}
}

BOOL CheckSpell(int id, int sn, char st, BOOL manaonly)
{
	BOOL result;

#ifdef _DEBUG
	if (debug_mode_key_inverted_v)
		return TRUE;
#endif

	result = TRUE;
	if (!manaonly && pcurs != CURSOR_HAND) {
		result = FALSE;
	} else {
		if (st != RSPLTYPE_SKILL) {
			if (GetSpellLevel(id, sn) <= 0) {
				result = FALSE;
			} else {
				result = plr[id]._pMana >= GetManaAmount(id, sn);
			}
		}
	}

	return result;
}

void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int caster, int spllvl)
{
	int i;
	int dir; // missile direction

	switch (caster) {
	case 1:
		dir = monster[id]._mdir;
		break;
	case 0:
		// caster must be 0 already in this case, but oh well,
		// it's needed to generate the right code
		caster = 0;
		dir = plr[id]._pdir;

#ifdef HELLFIRE
		if (spl == SPL_FIREWALL || spl == SPL_LIGHTWALL) {
#else
		if (spl == SPL_FIREWALL) {
#endif
			dir = plr[id]._pVar3;
		}
		break;
	}

	for (i = 0; spelldata[spl].sMissiles[i] != MIS_ARROW && i < 3; i++) {
		AddMissile(sx, sy, dx, dy, dir, spelldata[spl].sMissiles[i], caster, id, 0, spllvl);
	}

	if (spelldata[spl].sMissiles[0] == MIS_TOWN) {
		UseMana(id, SPL_TOWN);
	}
	if (spelldata[spl].sMissiles[0] == MIS_CBOLT) {
		UseMana(id, SPL_CBOLT);

		for (i = (spllvl >> 1) + 3; i > 0; i--) {
			AddMissile(sx, sy, dx, dy, dir, MIS_CBOLT, caster, id, 0, spllvl);
		}
	}
}

static void PlacePlayer(int pnum)
{
	int nx, ny, max, min, x, y;
	DWORD i;
	BOOL done;

	if (plr[pnum].plrlevel == currlevel) {
		for (i = 0; i < 8; i++) {
			nx = plr[pnum]._px + plrxoff2[i];
			ny = plr[pnum]._py + plryoff2[i];

			if (PosOkPlayer(pnum, nx, ny)) {
				break;
			}
		}

		if (!PosOkPlayer(pnum, nx, ny)) {
			done = FALSE;

			for (max = 1, min = -1; min > -50 && !done; max++, min--) {
				for (y = min; y <= max && !done; y++) {
					ny = plr[pnum]._py + y;

					for (x = min; x <= max && !done; x++) {
						nx = plr[pnum]._px + x;

						if (PosOkPlayer(pnum, nx, ny)) {
							done = TRUE;
						}
					}
				}
			}
		}

		plr[pnum]._px = nx;
		plr[pnum]._py = ny;

		dPlayer[nx][ny] = pnum + 1;

		if (pnum == myplr) {
			ViewX = nx;
			ViewY = ny;
		}
	}
}

/**
 * @param pnum player index
 * @param rid target player index
 */
void DoResurrect(int pnum, int rid)
{
	int hp;

	if ((char)rid != -1) {
		AddMissile(plr[rid]._px, plr[rid]._py, plr[rid]._px, plr[rid]._py, 0, MIS_RESURRECTBEAM, 0, pnum, 0, 0);
	}

	if (pnum == myplr) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && plr[rid]._pHitPoints == 0) {
		if (rid == myplr) {
			deathflag = FALSE;
			gamemenu_off();
			drawhpflag = TRUE;
			drawmanaflag = TRUE;
		}

		ClrPlrPath(rid);
		plr[rid].destAction = ACTION_NONE;
		plr[rid]._pInvincible = FALSE;
#ifndef HELLFIRE
		PlacePlayer(rid);
#endif

		hp = 640;
#ifndef HELLFIRE
		if (plr[rid]._pMaxHPBase < 640) {
			hp = plr[rid]._pMaxHPBase;
		}
#endif
		SetPlayerHitPoints(rid, hp);

		plr[rid]._pHPBase = plr[rid]._pHitPoints + (plr[rid]._pMaxHPBase - plr[rid]._pMaxHP);
		plr[rid]._pMana = 0;
		plr[rid]._pManaBase = plr[rid]._pMana + (plr[rid]._pMaxManaBase - plr[rid]._pMaxMana);

		CalcPlrInv(rid, TRUE);

		if (plr[rid].plrlevel == currlevel) {
			StartStand(rid, plr[rid]._pdir);
		} else {
			plr[rid]._pmode = PM_STAND;
		}
	}
}

void DoHealOther(int pnum, int rid)
{
	int i, j, hp;

	if (pnum == myplr) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && (plr[rid]._pHitPoints >> 6) > 0) {
		hp = (random_(57, 10) + 1) << 6;

		for (i = 0; i < plr[pnum]._pLevel; i++) {
			hp += (random_(57, 4) + 1) << 6;
		}

		for (j = 0; j < GetSpellLevel(pnum, SPL_HEALOTHER); ++j) {
			hp += (random_(57, 6) + 1) << 6;
		}

#ifdef HELLFIRE
		if (plr[pnum]._pClass == PC_WARRIOR || plr[pnum]._pClass == PC_BARBARIAN) {
			hp <<= 1;
		} else if (plr[pnum]._pClass == PC_ROGUE || plr[pnum]._pClass == PC_BARD) {
			hp += hp >> 1;
		} else if (plr[pnum]._pClass == PC_MONK) {
			hp *= 3;
		}
#else
		if (plr[pnum]._pClass == PC_WARRIOR) {
			hp <<= 1;
		}

		if (plr[pnum]._pClass == PC_ROGUE) {
			hp += hp >> 1;
		}
#endif

		plr[rid]._pHitPoints += hp;

		if (plr[rid]._pHitPoints > plr[rid]._pMaxHP) {
			plr[rid]._pHitPoints = plr[rid]._pMaxHP;
		}

		plr[rid]._pHPBase += hp;

		if (plr[rid]._pHPBase > plr[rid]._pMaxHPBase) {
			plr[rid]._pHPBase = plr[rid]._pMaxHPBase;
		}

		drawhpflag = TRUE;
	}
}

int GetSpellBookLevel(int s)
{
	if (gbIsSpawn) {
		switch (s) {
		case SPL_STONE:
		case SPL_GUARDIAN:
		case SPL_GOLEM:
		case SPL_FLARE:
		case SPL_BONESPIRIT:
			return -1;
		}
	}

	return spelldata[s].sBookLvl;
}

int GetSpellStaffLevel(int s)
{
	if (gbIsSpawn) {
		switch (s) {
		case SPL_STONE:
		case SPL_GUARDIAN:
		case SPL_GOLEM:
		case SPL_APOCA:
		case SPL_FLARE:
		case SPL_BONESPIRIT:
			return -1;
		}
	}

	return spelldata[s].sStaffLvl;
}

DEVILUTION_END_NAMESPACE

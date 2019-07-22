#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

int GetManaAmount(int id, int sn)
{
	int i;  // "raw" mana cost
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

	if (spelldata[sn].sManaCost == 255) {
		i = (BYTE)plr[id]._pMaxManaBase;
	} else {
		i = spelldata[sn].sManaCost;
	}

	ma = (i - adj) << 6;

	if (sn == SPL_HEAL) {
		ma = (spelldata[SPL_HEAL].sManaCost + 2 * plr[id]._pLevel - adj) << 6;
	}
	if (sn == SPL_HEALOTHER) {
		ma = (spelldata[SPL_HEAL].sManaCost + 2 * plr[id]._pLevel - adj) << 6;
	}

	if (plr[id]._pClass == PC_ROGUE) {
		ma -= ma >> 2;
	}

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
		case RSPLTYPE_SCROLL:
			RemoveScroll(id);
			break;
		case RSPLTYPE_CHARGES:
			UseStaffCharge(id);
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
	if (!manaonly && pcurs != 1) {
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

		if (spl == SPL_FIREWALL) {
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

		for (i = 0; i < (spllvl >> 1) + 3; i++) {
			AddMissile(sx, sy, dx, dy, dir, MIS_CBOLT, caster, id, 0, spllvl);
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
		AddMissile(plr[rid].WorldX, plr[rid].WorldY, plr[rid].WorldX, plr[rid].WorldY, 0, MIS_RESURRECTBEAM, 0, pnum, 0, 0);
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
		PlacePlayer(rid);

		hp = 640;
		if (plr[rid]._pMaxHPBase < 640) {
			hp = plr[rid]._pMaxHPBase;
		}
		SetPlayerHitPoints(rid, hp);

		plr[rid]._pMana = 0;
		plr[rid]._pHPBase = plr[rid]._pHitPoints + (plr[rid]._pMaxHPBase - plr[rid]._pMaxHP);
		plr[rid]._pManaBase = plr[rid]._pMaxManaBase - plr[rid]._pMaxMana;

		CalcPlrInv(rid, TRUE);

		if (plr[rid].plrlevel == currlevel) {
			StartStand(rid, plr[rid]._pdir);
		} else {
			plr[rid]._pmode = 0;
		}
	}
}

void PlacePlayer(int pnum)
{
	int nx, ny, max, min, x, y;
	DWORD i;
	BOOL done;

	if (plr[pnum].plrlevel == currlevel) {
		for (i = 0; i < 8; i++) {
			nx = plr[pnum].WorldX + plrxoff2[i];
			ny = plr[pnum].WorldY + plryoff2[i];

			if (PosOkPlayer(pnum, nx, ny)) {
				break;
			}
		}

		if (!PosOkPlayer(pnum, nx, ny)) {
			done = FALSE;

			for (max = 1, min = -1; min > -50 && !done; max++, min--) {
				for (y = min; y <= max && !done; y++) {
					ny = plr[pnum].WorldY + y;

					for (x = min; x <= max && !done; x++) {
						nx = plr[pnum].WorldX + x;

						if (PosOkPlayer(pnum, nx, ny)) {
							done = TRUE;
						}
					}
				}
			}
		}

		plr[pnum].WorldX = nx;
		plr[pnum].WorldY = ny;

		dPlayer[nx][ny] = pnum + 1;

		if (pnum == myplr) {
			ViewX = nx;
			ViewY = ny;
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
		hp = (random(57, 10) + 1) << 6;

		for (i = 0; i < plr[pnum]._pLevel; i++) {
			hp += (random(57, 4) + 1) << 6;
		}

		for (j = 0; j < GetSpellLevel(pnum, SPL_HEALOTHER); ++j) {
			hp += (random(57, 6) + 1) << 6;
		}

		if (plr[pnum]._pClass == PC_WARRIOR) {
			hp <<= 1;
		}

		if (plr[pnum]._pClass == PC_ROGUE) {
			hp += hp >> 1;
		}

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

DEVILUTION_END_NAMESPACE

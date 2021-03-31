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

	if (plr[id]._pClass == PC_SORCERER) {
		ma >>= 1;
	} else if (plr[id]._pClass == PC_ROGUE || plr[id]._pClass == PC_MONK || plr[id]._pClass == PC_BARD) {
		ma -= ma >> 2;
	}

	if (spelldata[sn].sMinMana > ma >> 6) {
		ma = spelldata[sn].sMinMana << 6;
	}

	return ma;
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

/**
 * @brief Gets a value that represents the specified spellID in 64bit bitmask format.
 * For example:
 *  - spell ID  1: 0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0001
 *  - spell ID 43: 0000.0000.0000.0000.0000.0100.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000
 * @param spellId The id of the spell to get a bitmask for.
 * @return A 64bit bitmask representation for the specified spell.
 */
Uint64 GetSpellBitmask(int spellId)
{
	return 1ULL << (spellId - 1);
}

/**
 * @brief Gets a value indicating whether the player's current readied spell is a valid spell. Readied spells can be
 * invalidaded in a few scenarios where the spell comes from items, for example (like dropping the only scroll that
 * provided the spell).
 * @param player The player whose readied spell is to be checked.
 * @return 'true' when the readied spell is currently valid, and 'false' otherwise.
 */
bool IsReadiedSpellValid(const PlayerStruct &player)
{
	switch (player._pRSplType) {
	case RSPLTYPE_SKILL:
	case RSPLTYPE_SPELL:
	case RSPLTYPE_INVALID:
		return true;

	case RSPLTYPE_CHARGES:
		return player._pISpells & GetSpellBitmask(player._pRSpell);

	case RSPLTYPE_SCROLL:
		return player._pScrlSpells & GetSpellBitmask(player._pRSpell);

	default:
		return false;
	}
}

/**
 * @brief Clears the current player's readied spell selection.
 * @note Will force a UI redraw in case the values actually change, so that the new spell reflects on the bottom panel.
 * @param player The player whose readied spell is to be cleared.
 */
void ClearReadiedSpell(PlayerStruct &player)
{
	if (player._pRSpell != SPL_INVALID) {
		player._pRSpell = SPL_INVALID;
		force_redraw = 255;
	}

	if (player._pRSplType != RSPLTYPE_INVALID) {
		player._pRSplType = RSPLTYPE_INVALID;
		force_redraw = 255;
	}
}

/**
 * @brief Ensures the player's current readied spell is a valid selection for the character. If the current selection is
 * incompatible with the player's items and spell (for example, if the player does not currently have access to the spell),
 * the selection is cleared.
 * @note Will force a UI redraw in case the values actually change, so that the new spell reflects on the bottom panel.
 * @param player The player whose readied spell is to be checked.
 */
void EnsureValidReadiedSpell(PlayerStruct &player)
{
	if (!IsReadiedSpellValid(player)) {
		ClearReadiedSpell(player);
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

void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int spllvl)
{
	int dir = plr[id]._pdir;
	if (spl == SPL_FIREWALL || spl == SPL_LIGHTWALL) {
		dir = plr[id]._pVar3;
	}

	for (int i = 0; spelldata[spl].sMissiles[i] != 0 && i < 3; i++) {
		AddMissile(sx, sy, dx, dy, dir, spelldata[spl].sMissiles[i], TARGET_MONSTERS, id, 0, spllvl);
	}

	if (spl == SPL_TOWN) {
		UseMana(id, SPL_TOWN);
	} else if (spl == SPL_CBOLT) {
		UseMana(id, SPL_CBOLT);

		for (int i = (spllvl >> 1) + 3; i > 0; i--) {
			AddMissile(sx, sy, dx, dy, dir, MIS_CBOLT, TARGET_MONSTERS, id, 0, spllvl);
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
		AddMissile(plr[rid]._px, plr[rid]._py, plr[rid]._px, plr[rid]._py, 0, MIS_RESURRECTBEAM, TARGET_MONSTERS, pnum, 0, 0);
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

		hp = 10 << 6;
		if (plr[rid]._pMaxHPBase < (10 << 6)) {
			hp = plr[rid]._pMaxHPBase;
		}
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

		if (plr[pnum]._pClass == PC_WARRIOR || plr[pnum]._pClass == PC_BARBARIAN) {
			hp <<= 1;
		} else if (plr[pnum]._pClass == PC_ROGUE || plr[pnum]._pClass == PC_BARD) {
			hp += hp >> 1;
		} else if (plr[pnum]._pClass == PC_MONK) {
			hp *= 3;
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

int GetSpellBookLevel(spell_id s)
{
	if (gbIsSpawn) {
		switch (s) {
		case SPL_STONE:
		case SPL_GUARDIAN:
		case SPL_GOLEM:
		case SPL_FLARE:
		case SPL_BONESPIRIT:
			return -1;
		default:
			break;
		}
	}

	if (!gbIsHellfire) {
		switch (s) {
		case SPL_NOVA:
		case SPL_APOCA:
			return -1;
		default:
			if (s > SPL_LASTDIABLO)
				return -1;
			break;
		}
	}

	if (gbIsHellfire) {
		switch (s) {
		case SPL_ELEMENT:
			return -1;
		default:
			break;
		}
	}

	return spelldata[s].sBookLvl;
}

int GetSpellStaffLevel(spell_id s)
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
		default:
			break;
		}
	}

	if (!gbIsHellfire && s > SPL_LASTDIABLO)
		return -1;

	if (gbIsHellfire) {
		switch (s) {
		case SPL_ELEMENT:
			return -1;
		default:
			break;
		}
	}

	return spelldata[s].sStaffLvl;
}

DEVILUTION_END_NAMESPACE

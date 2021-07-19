/**
 * @file spells.cpp
 *
 * Implementation of functionality for casting player spells.
 */
#include "spells.h"

#include "control.h"
#include "cursor.h"
#include "engine/point.hpp"
#include "engine/random.hpp"
#include "gamemenu.h"
#include "inv.h"
#include "missiles.h"

namespace devilution {

int GetManaAmount(PlayerStruct &player, spell_id sn)
{
	int ma; // mana amount

	// mana adjust
	int adj = 0;

	// spell level
	int sl = player._pSplLvl[sn] + player._pISplLvlAdd - 1;

	if (sl < 0) {
		sl = 0;
	}

	if (sl > 0) {
		adj = sl * spelldata[sn].sManaAdj;
	}
	if (sn == SPL_FIREBOLT) {
		adj /= 2;
	}
	if (sn == SPL_RESURRECT && sl > 0) {
		adj = sl * (spelldata[SPL_RESURRECT].sManaCost / 8);
	}

	if (sn == SPL_HEAL || sn == SPL_HEALOTHER) {
		ma = (spelldata[SPL_HEAL].sManaCost + 2 * player._pLevel - adj);
	} else if (spelldata[sn].sManaCost == 255) {
		ma = ((BYTE)player._pMaxManaBase - adj);
	} else {
		ma = (spelldata[sn].sManaCost - adj);
	}

	if (ma < 0)
		ma = 0;
	ma <<= 6;

	if (gbIsHellfire && player._pClass == HeroClass::Sorcerer) {
		ma /= 2;
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard) {
		ma -= ma / 4;
	}

	if (spelldata[sn].sMinMana > ma >> 6) {
		ma = spelldata[sn].sMinMana << 6;
	}

	return ma;
}

void UseMana(int id, spell_id sn)
{
	int ma; // mana cost

	if (id != MyPlayerId)
		return;

	auto &myPlayer = Players[MyPlayerId];

	switch (myPlayer._pSplType) {
	case RSPLTYPE_SKILL:
	case RSPLTYPE_INVALID:
		break;
	case RSPLTYPE_SCROLL:
		RemoveScroll(myPlayer);
		break;
	case RSPLTYPE_CHARGES:
		UseStaffCharge(myPlayer);
		break;
	case RSPLTYPE_SPELL:
#ifdef _DEBUG
		if (debug_mode_key_inverted_v)
			break;
#endif
		ma = GetManaAmount(myPlayer, sn);
		myPlayer._pMana -= ma;
		myPlayer._pManaBase -= ma;
		drawmanaflag = true;
		break;
	}
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
		return (player._pISpells & GetSpellBitmask(player._pRSpell)) != 0;

	case RSPLTYPE_SCROLL:
		return (player._pScrlSpells & GetSpellBitmask(player._pRSpell)) != 0;

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

bool CheckSpell(int id, spell_id sn, spell_type st, bool manaonly)
{
#ifdef _DEBUG
	if (debug_mode_key_inverted_v)
		return true;
#endif

	if (!manaonly && pcurs != CURSOR_HAND) {
		return false;
	}

	if (st == RSPLTYPE_SKILL) {
		return true;
	}

	if (GetSpellLevel(id, sn) <= 0) {
		return false;
	}

	auto &player = Players[id];

	return player._pMana >= GetManaAmount(player, sn);
}

void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int spllvl)
{
	Direction dir = Players[id]._pdir;
	if (spl == SPL_FIREWALL || spl == SPL_LIGHTWALL) {
		dir = Players[id].tempDirection;
	}

	for (int i = 0; spelldata[spl].sMissiles[i] != MIS_NULL && i < 3; i++) {
		AddMissile({ sx, sy }, { dx, dy }, dir, spelldata[spl].sMissiles[i], TARGET_MONSTERS, id, 0, spllvl);
	}

	if (spl == SPL_TOWN) {
		UseMana(id, SPL_TOWN);
	} else if (spl == SPL_CBOLT) {
		UseMana(id, SPL_CBOLT);

		for (int i = (spllvl / 2) + 3; i > 0; i--) {
			AddMissile({ sx, sy }, { dx, dy }, dir, MIS_CBOLT, TARGET_MONSTERS, id, 0, spllvl);
		}
	}
}

static void PlacePlayer(int pnum)
{
	auto &player = Players[pnum];
	Point newPosition = {};

	if (player.plrlevel == currlevel) {
		for (int i = 0; i < 8; i++) {
			newPosition = player.position.tile + Displacement { plrxoff2[i], plryoff2[i] };
			if (PosOkPlayer(player, newPosition)) {
				break;
			}
		}

		if (!PosOkPlayer(player, newPosition)) {
			bool done = false;

			int min = -1;
			for (int max = 1; min > -50 && !done; max++, min--) {
				for (int y = min; y <= max && !done; y++) {
					newPosition.y = player.position.tile.y + y;

					for (int x = min; x <= max && !done; x++) {
						newPosition.x = player.position.tile.x + x;

						if (PosOkPlayer(player, newPosition)) {
							done = true;
						}
					}
				}
			}
		}

		player.position.tile = newPosition;

		dPlayer[newPosition.x][newPosition.y] = pnum + 1;

		if (pnum == MyPlayerId) {
			ViewX = newPosition.x;
			ViewY = newPosition.y;
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
		AddMissile(Players[rid].position.tile, Players[rid].position.tile, 0, MIS_RESURRECTBEAM, TARGET_MONSTERS, pnum, 0, 0);
	}

	if (pnum == MyPlayerId) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && Players[rid]._pHitPoints == 0) {
		if (rid == MyPlayerId) {
			MyPlayerIsDead = false;
			gamemenu_off();
			drawhpflag = true;
			drawmanaflag = true;
		}

		ClrPlrPath(Players[rid]);
		Players[rid].destAction = ACTION_NONE;
		Players[rid]._pInvincible = false;
		PlacePlayer(rid);

		hp = 10 << 6;
		if (Players[rid]._pMaxHPBase < (10 << 6)) {
			hp = Players[rid]._pMaxHPBase;
		}
		SetPlayerHitPoints(rid, hp);

		Players[rid]._pHPBase = Players[rid]._pHitPoints + (Players[rid]._pMaxHPBase - Players[rid]._pMaxHP); // CODEFIX: does the same stuff as SetPlayerHitPoints above, can be removed
		Players[rid]._pMana = 0;
		Players[rid]._pManaBase = Players[rid]._pMana + (Players[rid]._pMaxManaBase - Players[rid]._pMaxMana);

		CalcPlrInv(rid, true);

		if (Players[rid].plrlevel == currlevel) {
			StartStand(rid, Players[rid]._pdir);
		} else {
			Players[rid]._pmode = PM_STAND;
		}
	}
}

void DoHealOther(int pnum, uint16_t rid)
{
	if (pnum == MyPlayerId) {
		NewCursor(CURSOR_HAND);
	}

	if ((DWORD)pnum >= MAX_PLRS || rid >= MAX_PLRS) {
		return;
	}
	auto &player = Players[pnum];
	auto &target = Players[rid];

	if ((target._pHitPoints >> 6) <= 0) {
		return;
	}

	int hp = (GenerateRnd(10) + 1) << 6;
	for (int i = 0; i < player._pLevel; i++) {
		hp += (GenerateRnd(4) + 1) << 6;
	}
	for (int i = 0; i < GetSpellLevel(pnum, SPL_HEALOTHER); i++) {
		hp += (GenerateRnd(6) + 1) << 6;
	}

	if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian) {
		hp *= 2;
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Bard) {
		hp += hp / 2;
	} else if (player._pClass == HeroClass::Monk) {
		hp *= 3;
	}

	target._pHitPoints = std::min(target._pHitPoints + hp, target._pMaxHP);
	target._pHPBase = std::min(target._pHPBase + hp, target._pMaxHPBase);

	if (rid == MyPlayerId) {
		drawhpflag = true;
	}
}

int GetSpellBookLevel(spell_id s)
{
	if (gbIsSpawn) {
		switch (s) {
		case SPL_STONE:
		case SPL_GUARDIAN:
		case SPL_GOLEM:
		case SPL_ELEMENT:
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
		case SPL_ELEMENT:
		case SPL_FLARE:
		case SPL_BONESPIRIT:
			return -1;
		default:
			break;
		}
	}

	if (!gbIsHellfire && s > SPL_LASTDIABLO)
		return -1;

	return spelldata[s].sStaffLvl;
}

} // namespace devilution

/**
 * @file spells.cpp
 *
 * Implementation of functionality for casting player spells.
 */
#include "spells.h"

#include "control.h"
#include "cursor.h"
#ifdef _DEBUG
#include "debug.h"
#endif
#include "engine/point.hpp"
#include "engine/random.hpp"
#include "gamemenu.h"
#include "inv.h"
#include "missiles.h"

namespace devilution {

namespace {

/**
 * @brief Gets a value indicating whether the player's current readied spell is a valid spell. Readied spells can be
 * invalidaded in a few scenarios where the spell comes from items, for example (like dropping the only scroll that
 * provided the spell).
 * @param player The player whose readied spell is to be checked.
 * @return 'true' when the readied spell is currently valid, and 'false' otherwise.
 */
bool IsReadiedSpellValid(const Player &player)
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
void ClearReadiedSpell(Player &player)
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

void PlacePlayer(Player &player)
{
	if (!player.isOnActiveLevel())
		return;

	Point newPosition = [&]() {
		Point okPosition = {};

		for (int i = 0; i < 8; i++) {
			okPosition = player.position.tile + Displacement { plrxoff2[i], plryoff2[i] };
			if (PosOkPlayer(player, okPosition))
				return okPosition;
		}

		for (int max = 1, min = -1; min > -50; max++, min--) {
			for (int y = min; y <= max; y++) {
				okPosition.y = player.position.tile.y + y;

				for (int x = min; x <= max; x++) {
					okPosition.x = player.position.tile.x + x;

					if (PosOkPlayer(player, okPosition))
						return okPosition;
				}
			}
		}

		return okPosition;
	}();

	player.position.tile = newPosition;

	dPlayer[newPosition.x][newPosition.y] = player.getId() + 1;

	if (&player == MyPlayer) {
		ViewPosition = newPosition;
	}
}

} // namespace

bool IsValidSpell(spell_id spl)
{
	return spl > SPL_NULL
	    && spl <= SPL_LAST
	    && (spl <= SPL_LASTDIABLO || gbIsHellfire);
}

bool IsWallSpell(spell_id spl)
{
	return spl == SPL_FIREWALL || spl == SPL_LIGHTWALL;
}

bool TargetsMonster(spell_id id)
{
	return id == SPL_FIREBALL
	    || id == SPL_FIREWALL
	    || id == SPL_FLAME
	    || id == SPL_LIGHTNING
	    || id == SPL_STONE
	    || id == SPL_WAVE;
}

int GetManaAmount(const Player &player, spell_id sn)
{
	int ma; // mana amount

	// mana adjust
	int adj = 0;

	// spell level
	int sl = std::max(player.GetSpellLevel(sn) - 1, 0);

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
		ma = (player._pMaxManaBase >> 6) - adj;
	} else {
		ma = (spelldata[sn].sManaCost - adj);
	}

	ma = std::max(ma, 0);
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

void ConsumeSpell(Player &player, spell_id sn)
{
	switch (player.executedSpell.spellType) {
	case RSPLTYPE_SKILL:
	case RSPLTYPE_INVALID:
		break;
	case RSPLTYPE_SCROLL:
		ConsumeScroll(player);
		break;
	case RSPLTYPE_CHARGES:
		ConsumeStaffCharge(player);
		break;
	case RSPLTYPE_SPELL:
#ifdef _DEBUG
		if (DebugGodMode)
			break;
#endif
		int ma = GetManaAmount(player, sn);
		player._pMana -= ma;
		player._pManaBase -= ma;
		drawmanaflag = true;
		break;
	}
}

void EnsureValidReadiedSpell(Player &player)
{
	if (!IsReadiedSpellValid(player)) {
		ClearReadiedSpell(player);
	}
}

SpellCheckResult CheckSpell(const Player &player, spell_id sn, spell_type st, bool manaonly)
{
#ifdef _DEBUG
	if (DebugGodMode)
		return SpellCheckResult::Success;
#endif

	if (!manaonly && pcurs != CURSOR_HAND) {
		return SpellCheckResult::Fail_Busy;
	}

	if (st == RSPLTYPE_SKILL) {
		return SpellCheckResult::Success;
	}

	if (player.GetSpellLevel(sn) <= 0) {
		return SpellCheckResult::Fail_Level0;
	}

	if (player._pMana < GetManaAmount(player, sn)) {
		return SpellCheckResult::Fail_NoMana;
	}

	return SpellCheckResult::Success;
}

void CastSpell(int id, spell_id spl, int sx, int sy, int dx, int dy, int spllvl)
{
	Player &player = Players[id];
	Direction dir = player._pdir;
	if (IsWallSpell(spl)) {
		dir = player.tempDirection;
	}

	for (int i = 0; i < 3 && spelldata[spl].sMissiles[i] != MIS_NULL; i++) {
		AddMissile({ sx, sy }, { dx, dy }, dir, spelldata[spl].sMissiles[i], TARGET_MONSTERS, id, 0, spllvl);
	}

	if (spl == SPL_TOWN) {
		ConsumeSpell(player, SPL_TOWN);
	} else if (spl == SPL_CBOLT) {
		ConsumeSpell(player, SPL_CBOLT);

		for (int i = (spllvl / 2) + 3; i > 0; i--) {
			AddMissile({ sx, sy }, { dx, dy }, dir, MIS_CBOLT, TARGET_MONSTERS, id, 0, spllvl);
		}
	}
}

void DoResurrect(size_t pnum, Player &target)
{
	if (pnum >= Players.size()) {
		return;
	}

	AddMissile(target.position.tile, target.position.tile, Direction::South, MIS_RESURRECTBEAM, TARGET_MONSTERS, pnum, 0, 0);

	if (target._pHitPoints != 0)
		return;

	if (&target == MyPlayer) {
		MyPlayerIsDead = false;
		gamemenu_off();
		drawhpflag = true;
		drawmanaflag = true;
	}

	ClrPlrPath(target);
	target.destAction = ACTION_NONE;
	target._pInvincible = false;
	PlacePlayer(target);

	int hp = 10 << 6;
	if (target._pMaxHPBase < (10 << 6)) {
		hp = target._pMaxHPBase;
	}
	SetPlayerHitPoints(target, hp);

	target._pHPBase = target._pHitPoints + (target._pMaxHPBase - target._pMaxHP); // CODEFIX: does the same stuff as SetPlayerHitPoints above, can be removed
	target._pMana = 0;
	target._pManaBase = target._pMana + (target._pMaxManaBase - target._pMaxMana);

	CalcPlrInv(target, true);

	if (target.isOnActiveLevel()) {
		StartStand(target, target._pdir);
	} else {
		target._pmode = PM_STAND;
	}
}

void DoHealOther(const Player &caster, Player &target)
{
	if ((target._pHitPoints >> 6) <= 0) {
		return;
	}

	int hp = (GenerateRnd(10) + 1) << 6;
	for (int i = 0; i < caster._pLevel; i++) {
		hp += (GenerateRnd(4) + 1) << 6;
	}
	for (int i = 0; i < caster.GetSpellLevel(SPL_HEALOTHER); i++) {
		hp += (GenerateRnd(6) + 1) << 6;
	}

	if (caster._pClass == HeroClass::Warrior || caster._pClass == HeroClass::Barbarian) {
		hp *= 2;
	} else if (caster._pClass == HeroClass::Rogue || caster._pClass == HeroClass::Bard) {
		hp += hp / 2;
	} else if (caster._pClass == HeroClass::Monk) {
		hp *= 3;
	}

	target._pHitPoints = std::min(target._pHitPoints + hp, target._pMaxHP);
	target._pHPBase = std::min(target._pHPBase + hp, target._pMaxHPBase);

	if (&target == MyPlayer) {
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

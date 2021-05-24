/**
 * @file loadsave.cpp
 *
 * Implementation of save game functionality.
 */
#include "loadsave.h"

#include <SDL.h>
#include <climits>

#include "automap.h"
#include "codec.h"
#include "control.h"
#include "cursor.h"
#include "dead.h"
#include "doom.h"
#include "engine.h"
#include "engine/point.hpp"
#include "init.h"
#include "inv.h"
#include "lighting.h"
#include "missiles.h"
#include "mpqapi.h"
#include "pfile.h"
#include "stores.h"
#include "utils/endian.hpp"
#include "utils/language.h"

namespace devilution {

bool gbIsHellfireSaveGame;
uint8_t giNumberOfLevels;
uint8_t giNumberQuests;
uint8_t giNumberOfSmithPremiumItems;

namespace {

template <class T>
T SwapLE(T in)
{
	switch (sizeof(T)) {
	case 2:
		return SDL_SwapLE16(in);
	case 4:
		return SDL_SwapLE32(in);
	case 8:
		return SDL_SwapLE64(in);
	default:
		return in;
	}
}

template <class T>
T SwapBE(T in)
{
	switch (sizeof(T)) {
	case 2:
		return SDL_SwapBE16(in);
	case 4:
		return SDL_SwapBE32(in);
	case 8:
		return SDL_SwapBE64(in);
	default:
		return in;
	}
}

class LoadHelper {
	std::unique_ptr<byte[]> m_buffer;
	uint32_t m_cur = 0;
	size_t m_size;

	template <class T>
	T next()
	{
		const auto size = sizeof(T);
		if (!isValid(size))
			return 0;

		T value;
		memcpy(&value, &m_buffer[m_cur], size);
		m_cur += size;

		return value;
	}

public:
	LoadHelper(const char *szFileName)
	{
		m_buffer = pfile_read(szFileName, &m_size);
	}

	bool isValid(uint32_t size = 1)
	{
		return m_buffer != nullptr
		    && m_size >= (m_cur + size);
	}

	void skip(uint32_t size)
	{
		m_cur += size;
	}

	void nextBytes(void *bytes, size_t size)
	{
		if (!isValid(size))
			return;

		memcpy(bytes, &m_buffer[m_cur], size);
		m_cur += size;
	}

	template <class T>
	T nextLE()
	{
		return SwapLE(next<T>());
	}

	template <class T>
	T nextBE()
	{
		return SwapBE(next<T>());
	}

	bool nextBool8()
	{
		return next<uint8_t>() != 0;
	}

	bool nextBool32()
	{
		return next<uint32_t>() != 0;
	}
};

class SaveHelper {
	const char *m_szFileName;
	std::unique_ptr<byte[]> m_buffer;
	uint32_t m_cur = 0;
	uint32_t m_capacity;

public:
	SaveHelper(const char *szFileName, size_t bufferLen)
	    : m_szFileName(szFileName)
	    , m_buffer(new byte[codec_get_encoded_len(bufferLen)])
	    , m_capacity(bufferLen)
	{
	}

	bool isValid(uint32_t len = 1)
	{
		return m_buffer != nullptr
		    && m_capacity >= (m_cur + len);
	}

	void skip(uint32_t len)
	{
		m_cur += len;
	}

	void writeBytes(const void *bytes, size_t len)
	{
		if (!isValid(len))
			return;

		memcpy(&m_buffer[m_cur], bytes, len);
		m_cur += len;
	}

	template <class T>
	void writeLE(T value)
	{
		value = SwapLE(value);
		writeBytes(&value, sizeof(value));
	}

	template <class T>
	void writeBE(T value)
	{
		value = SwapBE(value);
		writeBytes(&value, sizeof(value));
	}

	~SaveHelper()
	{
		const auto encoded_len = codec_get_encoded_len(m_cur);
		const char *const password = pfile_get_password();
		codec_encode(m_buffer.get(), m_cur, encoded_len, password);
		mpqapi_write_file(m_szFileName, m_buffer.get(), encoded_len);
	}
};

} // namespace

void RemoveInvalidItem(ItemStruct *pItem)
{
	bool isInvalid = !IsItemAvailable(pItem->IDidx) || !IsUniqueAvailable(pItem->_iUid);

	if (!gbIsHellfire) {
		isInvalid = isInvalid || (pItem->_itype == ITYPE_STAFF && GetSpellStaffLevel(pItem->_iSpell) == -1);
		isInvalid = isInvalid || (pItem->_iMiscId == IMISC_BOOK && GetSpellBookLevel(pItem->_iSpell) == -1);
		isInvalid = isInvalid || pItem->_iDamAcFlags != 0;
		isInvalid = isInvalid || pItem->_iPrePower > IDI_LASTDIABLO;
		isInvalid = isInvalid || pItem->_iSufPower > IDI_LASTDIABLO;
	}

	if (isInvalid) {
		pItem->_itype = ITYPE_NONE;
	}
}

static void LoadItemData(LoadHelper *file, ItemStruct *pItem)
{
	pItem->_iSeed = file->nextLE<int32_t>();
	pItem->_iCreateInfo = file->nextLE<uint16_t>();
	file->skip(2); // Alignment
	pItem->_itype = static_cast<item_type>(file->nextLE<uint32_t>());
	pItem->position.x = file->nextLE<int32_t>();
	pItem->position.y = file->nextLE<int32_t>();
	pItem->_iAnimFlag = file->nextBool32();
	file->skip(4); // Skip pointer _iAnimData
	pItem->AnimInfo = {};
	pItem->AnimInfo.NumberOfFrames = file->nextLE<int32_t>();
	pItem->AnimInfo.CurrentFrame = file->nextLE<int32_t>();
	file->skip(8); // Skip _iAnimWidth and _iAnimWidth2
	file->skip(4); // Unused since 1.02
	pItem->_iSelFlag = file->nextLE<uint8_t>();
	file->skip(3); // Alignment
	pItem->_iPostDraw = file->nextBool32();
	pItem->_iIdentified = file->nextBool32();
	pItem->_iMagical = file->nextLE<int8_t>();
	file->nextBytes(pItem->_iName, 64);
	file->nextBytes(pItem->_iIName, 64);
	pItem->_iLoc = static_cast<item_equip_type>(file->nextLE<int8_t>());
	pItem->_iClass = static_cast<item_class>(file->nextLE<uint8_t>());
	file->skip(1); // Alignment
	pItem->_iCurs = file->nextLE<int32_t>();
	pItem->_ivalue = file->nextLE<int32_t>();
	pItem->_iIvalue = file->nextLE<int32_t>();
	pItem->_iMinDam = file->nextLE<int32_t>();
	pItem->_iMaxDam = file->nextLE<int32_t>();
	pItem->_iAC = file->nextLE<int32_t>();
	pItem->_iFlags = file->nextLE<uint32_t>();
	pItem->_iMiscId = static_cast<item_misc_id>(file->nextLE<int32_t>());
	pItem->_iSpell = static_cast<spell_id>(file->nextLE<int32_t>());
	pItem->_iCharges = file->nextLE<int32_t>();
	pItem->_iMaxCharges = file->nextLE<int32_t>();
	pItem->_iDurability = file->nextLE<int32_t>();
	pItem->_iMaxDur = file->nextLE<int32_t>();
	pItem->_iPLDam = file->nextLE<int32_t>();
	pItem->_iPLToHit = file->nextLE<int32_t>();
	pItem->_iPLAC = file->nextLE<int32_t>();
	pItem->_iPLStr = file->nextLE<int32_t>();
	pItem->_iPLMag = file->nextLE<int32_t>();
	pItem->_iPLDex = file->nextLE<int32_t>();
	pItem->_iPLVit = file->nextLE<int32_t>();
	pItem->_iPLFR = file->nextLE<int32_t>();
	pItem->_iPLLR = file->nextLE<int32_t>();
	pItem->_iPLMR = file->nextLE<int32_t>();
	pItem->_iPLMana = file->nextLE<int32_t>();
	pItem->_iPLHP = file->nextLE<int32_t>();
	pItem->_iPLDamMod = file->nextLE<int32_t>();
	pItem->_iPLGetHit = file->nextLE<int32_t>();
	pItem->_iPLLight = file->nextLE<int32_t>();
	pItem->_iSplLvlAdd = file->nextLE<int8_t>();
	pItem->_iRequest = file->nextLE<int8_t>();
	file->skip(2); // Alignment
	pItem->_iUid = file->nextLE<int32_t>();
	pItem->_iFMinDam = file->nextLE<int32_t>();
	pItem->_iFMaxDam = file->nextLE<int32_t>();
	pItem->_iLMinDam = file->nextLE<int32_t>();
	pItem->_iLMaxDam = file->nextLE<int32_t>();
	pItem->_iPLEnAc = file->nextLE<int32_t>();
	pItem->_iPrePower = static_cast<item_effect_type>(file->nextLE<int8_t>());
	pItem->_iSufPower = static_cast<item_effect_type>(file->nextLE<int8_t>());
	file->skip(2); // Alignment
	pItem->_iVAdd1 = file->nextLE<int32_t>();
	pItem->_iVMult1 = file->nextLE<int32_t>();
	pItem->_iVAdd2 = file->nextLE<int32_t>();
	pItem->_iVMult2 = file->nextLE<int32_t>();
	pItem->_iMinStr = file->nextLE<int8_t>();
	pItem->_iMinMag = file->nextLE<uint8_t>();
	pItem->_iMinDex = file->nextLE<int8_t>();
	file->skip(1); // Alignment
	pItem->_iStatFlag = file->nextBool32();
	pItem->IDidx = static_cast<_item_indexes>(file->nextLE<int32_t>());
	if (gbIsSpawn) {
		pItem->IDidx = RemapItemIdxFromSpawn(pItem->IDidx);
	}
	if (!gbIsHellfireSaveGame) {
		pItem->IDidx = RemapItemIdxFromDiablo(pItem->IDidx);
	}
	pItem->dwBuff = file->nextLE<uint32_t>();
	if (gbIsHellfireSaveGame)
		pItem->_iDamAcFlags = file->nextLE<uint32_t>();
	else
		pItem->_iDamAcFlags = 0;

	RemoveInvalidItem(pItem);
}

static void LoadItems(LoadHelper *file, const int n, ItemStruct *pItem)
{
	for (int i = 0; i < n; i++) {
		LoadItemData(file, &pItem[i]);
	}
}

static void LoadPlayer(LoadHelper *file, int p)
{
	auto &player = plr[p];

	player._pmode = static_cast<PLR_MODE>(file->nextLE<int32_t>());

	for (int8_t &step : player.walkpath) {
		step = file->nextLE<int8_t>();
	}
	player.plractive = file->nextBool8();
	file->skip(2); // Alignment
	player.destAction = static_cast<action_id>(file->nextLE<int32_t>());
	player.destParam1 = file->nextLE<int32_t>();
	player.destParam2 = file->nextLE<int32_t>();
	player.destParam3 = static_cast<Direction>(file->nextLE<int32_t>());
	player.destParam4 = file->nextLE<int32_t>();
	player.plrlevel = file->nextLE<int32_t>();
	player.position.tile.x = file->nextLE<int32_t>();
	player.position.tile.y = file->nextLE<int32_t>();
	player.position.future.x = file->nextLE<int32_t>();
	player.position.future.y = file->nextLE<int32_t>();
	file->skip(8); // Skip _ptargx and _ptargy
	player.position.last.x = file->nextLE<int32_t>();
	player.position.last.y = file->nextLE<int32_t>();
	player.position.old.x = file->nextLE<int32_t>();
	player.position.old.y = file->nextLE<int32_t>();
	player.position.offset.x = file->nextLE<int32_t>();
	player.position.offset.y = file->nextLE<int32_t>();
	player.position.velocity.x = file->nextLE<int32_t>();
	player.position.velocity.y = file->nextLE<int32_t>();
	player._pdir = static_cast<Direction>(file->nextLE<int32_t>());
	file->skip(4); // Unused
	player._pgfxnum = file->nextLE<int32_t>();
	file->skip(4); // Skip pointer pData
	player.AnimInfo = {};
	player.AnimInfo.DelayLen = file->nextLE<int32_t>();
	player.AnimInfo.DelayCounter = file->nextLE<int32_t>();
	player.AnimInfo.NumberOfFrames = file->nextLE<int32_t>();
	player.AnimInfo.CurrentFrame = file->nextLE<int32_t>();
	file->skip(4); // Skip _pAnimWidth
	file->skip(4); // Skip _pAnimWidth2
	file->skip(4); // Skip _peflag
	player._plid = file->nextLE<int32_t>();
	player._pvid = file->nextLE<int32_t>();

	player._pSpell = static_cast<spell_id>(file->nextLE<int32_t>());
	player._pSplType = static_cast<spell_type>(file->nextLE<int8_t>());
	player._pSplFrom = file->nextLE<int8_t>();
	file->skip(2); // Alignment
	player._pTSpell = static_cast<spell_id>(file->nextLE<int32_t>());
	player._pTSplType = static_cast<spell_type>(file->nextLE<int8_t>());
	file->skip(3); // Alignment
	player._pRSpell = static_cast<spell_id>(file->nextLE<int32_t>());
	player._pRSplType = static_cast<spell_type>(file->nextLE<int8_t>());
	file->skip(3); // Alignment
	player._pSBkSpell = static_cast<spell_id>(file->nextLE<int32_t>());
	player._pSBkSplType = static_cast<spell_type>(file->nextLE<int8_t>());
	for (int8_t &spellLevel : player._pSplLvl)
		spellLevel = file->nextLE<int8_t>();
	file->skip(7); // Alignment
	player._pMemSpells = file->nextLE<uint64_t>();
	player._pAblSpells = file->nextLE<uint64_t>();
	player._pScrlSpells = file->nextLE<uint64_t>();
	player._pSpellFlags = file->nextLE<uint8_t>();
	file->skip(3); // Alignment
	for (auto &spell : player._pSplHotKey)
		spell = static_cast<spell_id>(file->nextLE<int32_t>());
	for (auto &spellType : player._pSplTHotKey)
		spellType = static_cast<spell_type>(file->nextLE<int8_t>());

	player._pwtype = static_cast<player_weapon_type>(file->nextLE<int32_t>());
	player._pBlockFlag = file->nextBool8();
	player._pInvincible = file->nextBool8();
	player._pLightRad = file->nextLE<int8_t>();
	player._pLvlChanging = file->nextBool8();

	file->nextBytes(player._pName, PLR_NAME_LEN);
	player._pClass = static_cast<HeroClass>(file->nextLE<int8_t>());
	file->skip(3); // Alignment
	player._pStrength = file->nextLE<int32_t>();
	player._pBaseStr = file->nextLE<int32_t>();
	player._pMagic = file->nextLE<int32_t>();
	player._pBaseMag = file->nextLE<int32_t>();
	player._pDexterity = file->nextLE<int32_t>();
	player._pBaseDex = file->nextLE<int32_t>();
	player._pVitality = file->nextLE<int32_t>();
	player._pBaseVit = file->nextLE<int32_t>();
	player._pStatPts = file->nextLE<int32_t>();
	player._pDamageMod = file->nextLE<int32_t>();
	player._pBaseToBlk = file->nextLE<int32_t>();
	if (player._pBaseToBlk == 0)
		player._pBaseToBlk = ToBlkTbl[static_cast<std::size_t>(player._pClass)];
	player._pHPBase = file->nextLE<int32_t>();
	player._pMaxHPBase = file->nextLE<int32_t>();
	player._pHitPoints = file->nextLE<int32_t>();
	player._pMaxHP = file->nextLE<int32_t>();
	player._pHPPer = file->nextLE<int32_t>();
	player._pManaBase = file->nextLE<int32_t>();
	player._pMaxManaBase = file->nextLE<int32_t>();
	player._pMana = file->nextLE<int32_t>();
	player._pMaxMana = file->nextLE<int32_t>();
	player._pManaPer = file->nextLE<int32_t>();
	player._pLevel = file->nextLE<int8_t>();
	player._pMaxLvl = file->nextLE<int8_t>();
	file->skip(2); // Alignment
	player._pExperience = file->nextLE<int32_t>();
	player._pMaxExp = file->nextLE<int32_t>();
	player._pNextExper = file->nextLE<int32_t>();
	player._pArmorClass = file->nextLE<int8_t>();
	player._pMagResist = file->nextLE<int8_t>();
	player._pFireResist = file->nextLE<int8_t>();
	player._pLghtResist = file->nextLE<int8_t>();
	player._pGold = file->nextLE<int32_t>();

	player._pInfraFlag = file->nextBool32();
	player.position.temp.x = file->nextLE<int32_t>();
	player.position.temp.y = file->nextLE<int32_t>();
	player.tempDirection = static_cast<Direction>(file->nextLE<int32_t>());
	player._pVar4 = file->nextLE<int32_t>();
	player._pVar5 = file->nextLE<int32_t>();
	player.position.offset2.x = file->nextLE<int32_t>();
	player.position.offset2.y = file->nextLE<int32_t>();
	file->skip(4); // Skip actionFrame
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		player._pLvlVisited[i] = file->nextBool8();
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		player._pSLvlVisited[i] = file->nextBool8();

	file->skip(2);     // Alignment
	file->skip(4);     // skip _pGFXLoad
	file->skip(4 * 8); // Skip pointers _pNAnim
	player._pNFrames = file->nextLE<int32_t>();
	file->skip(4);     // skip _pNWidth
	file->skip(4 * 8); // Skip pointers _pWAnim
	player._pWFrames = file->nextLE<int32_t>();
	file->skip(4);     // skip _pWWidth
	file->skip(4 * 8); // Skip pointers _pAAnim
	player._pAFrames = file->nextLE<int32_t>();
	file->skip(4); // skip _pAWidth
	player._pAFNum = file->nextLE<int32_t>();
	file->skip(4 * 8); // Skip pointers _pLAnim
	file->skip(4 * 8); // Skip pointers _pFAnim
	file->skip(4 * 8); // Skip pointers _pTAnim
	player._pSFrames = file->nextLE<int32_t>();
	file->skip(4); // skip _pSWidth
	player._pSFNum = file->nextLE<int32_t>();
	file->skip(4 * 8); // Skip pointers _pHAnim
	player._pHFrames = file->nextLE<int32_t>();
	file->skip(4);     // skip _pHWidth
	file->skip(4 * 8); // Skip pointers _pDAnim
	player._pDFrames = file->nextLE<int32_t>();
	file->skip(4);     // skip _pDWidth
	file->skip(4 * 8); // Skip pointers _pBAnim
	player._pBFrames = file->nextLE<int32_t>();
	file->skip(4); // skip _pBWidth

	LoadItems(file, NUM_INVLOC, player.InvBody);
	LoadItems(file, NUM_INV_GRID_ELEM, player.InvList);
	player._pNumInv = file->nextLE<int32_t>();
	for (int8_t &cell : player.InvGrid)
		cell = file->nextLE<int8_t>();
	LoadItems(file, MAXBELTITEMS, player.SpdList);
	LoadItemData(file, &player.HoldItem);

	player._pIMinDam = file->nextLE<int32_t>();
	player._pIMaxDam = file->nextLE<int32_t>();
	player._pIAC = file->nextLE<int32_t>();
	player._pIBonusDam = file->nextLE<int32_t>();
	player._pIBonusToHit = file->nextLE<int32_t>();
	player._pIBonusAC = file->nextLE<int32_t>();
	player._pIBonusDamMod = file->nextLE<int32_t>();
	file->skip(4); // Alignment

	player._pISpells = file->nextLE<uint64_t>();
	player._pIFlags = file->nextLE<int32_t>();
	player._pIGetHit = file->nextLE<int32_t>();
	player._pISplLvlAdd = file->nextLE<int8_t>();
	file->skip(1); // Unused
	file->skip(2); // Alignment
	player._pISplDur = file->nextLE<int32_t>();
	player._pIEnAc = file->nextLE<int32_t>();
	player._pIFMinDam = file->nextLE<int32_t>();
	player._pIFMaxDam = file->nextLE<int32_t>();
	player._pILMinDam = file->nextLE<int32_t>();
	player._pILMaxDam = file->nextLE<int32_t>();
	player._pOilType = static_cast<item_misc_id>(file->nextLE<int32_t>());
	player.pTownWarps = file->nextLE<uint8_t>();
	player.pDungMsgs = file->nextLE<uint8_t>();
	player.pLvlLoad = file->nextLE<uint8_t>();

	if (gbIsHellfireSaveGame) {
		player.pDungMsgs2 = file->nextLE<uint8_t>();
		player.pBattleNet = false;
	} else {
		player.pDungMsgs2 = 0;
		player.pBattleNet = file->nextBool8();
	}
	player.pManaShield = file->nextBool8();
	if (gbIsHellfireSaveGame) {
		player.pOriginalCathedral = file->nextBool8();
	} else {
		file->skip(1);
		player.pOriginalCathedral = true;
	}
	file->skip(2); // Available bytes
	player.wReflections = file->nextLE<uint16_t>();
	file->skip(14); // Available bytes

	player.pDiabloKillLevel = file->nextLE<uint32_t>();
	player.pDifficulty = static_cast<_difficulty>(file->nextLE<uint32_t>());
	player.pDamAcFlags = file->nextLE<uint32_t>();
	file->skip(20); // Available bytes
	CalcPlrItemVals(p, false);

	// Omit pointer _pNData
	// Omit pointer _pWData
	// Omit pointer _pAData
	// Omit pointer _pLData
	// Omit pointer _pFData
	// Omit pointer  _pTData
	// Omit pointer _pHData
	// Omit pointer _pDData
	// Omit pointer _pBData
	// Omit pointer pReserved
}

bool gbSkipSync = false;

static void LoadMonster(LoadHelper *file, int i)
{
	MonsterStruct *pMonster = &monster[i];

	pMonster->_mMTidx = file->nextLE<int32_t>();
	pMonster->_mmode = static_cast<MON_MODE>(file->nextLE<int32_t>());
	pMonster->_mgoal = static_cast<monster_goal>(file->nextLE<uint8_t>());
	file->skip(3); // Alignment
	pMonster->_mgoalvar1 = file->nextLE<int32_t>();
	pMonster->_mgoalvar2 = file->nextLE<int32_t>();
	pMonster->_mgoalvar3 = file->nextLE<int32_t>();
	file->skip(4); // Unused
	pMonster->_pathcount = file->nextLE<uint8_t>();
	file->skip(3); // Alignment
	pMonster->position.tile.x = file->nextLE<int32_t>();
	pMonster->position.tile.y = file->nextLE<int32_t>();
	pMonster->position.future.x = file->nextLE<int32_t>();
	pMonster->position.future.y = file->nextLE<int32_t>();
	pMonster->position.old.x = file->nextLE<int32_t>();
	pMonster->position.old.y = file->nextLE<int32_t>();
	pMonster->position.offset.x = file->nextLE<int32_t>();
	pMonster->position.offset.y = file->nextLE<int32_t>();
	pMonster->position.velocity.x = file->nextLE<int32_t>();
	pMonster->position.velocity.y = file->nextLE<int32_t>();
	pMonster->_mdir = static_cast<Direction>(file->nextLE<int32_t>());
	pMonster->_menemy = file->nextLE<int32_t>();
	pMonster->enemyPosition.x = file->nextLE<uint8_t>();
	pMonster->enemyPosition.y = file->nextLE<uint8_t>();
	file->skip(2); // Unused

	file->skip(4); // Skip pointer _mAnimData
	pMonster->_mAnimDelay = file->nextLE<int32_t>();
	pMonster->_mAnimCnt = file->nextLE<int32_t>();
	pMonster->_mAnimLen = file->nextLE<int32_t>();
	pMonster->_mAnimFrame = file->nextLE<int32_t>();
	file->skip(4); // Skip _meflag
	pMonster->_mDelFlag = file->nextBool32();
	pMonster->_mVar1 = file->nextLE<int32_t>();
	pMonster->_mVar2 = file->nextLE<int32_t>();
	pMonster->_mVar3 = file->nextLE<int32_t>();
	pMonster->position.temp.x = file->nextLE<int32_t>();
	pMonster->position.temp.y = file->nextLE<int32_t>();
	pMonster->position.offset2.x = file->nextLE<int32_t>();
	pMonster->position.offset2.y = file->nextLE<int32_t>();
	pMonster->actionFrame = file->nextLE<int32_t>();
	pMonster->_mmaxhp = file->nextLE<int32_t>();
	pMonster->_mhitpoints = file->nextLE<int32_t>();

	pMonster->_mAi = static_cast<_mai_id>(file->nextLE<uint8_t>());
	pMonster->_mint = file->nextLE<uint8_t>();
	file->skip(2); // Alignment
	pMonster->_mFlags = file->nextLE<uint32_t>();
	pMonster->_msquelch = file->nextLE<uint8_t>();
	file->skip(3); // Alignment
	file->skip(4); // Unused
	pMonster->position.last.x = file->nextLE<int32_t>();
	pMonster->position.last.y = file->nextLE<int32_t>();
	pMonster->_mRndSeed = file->nextLE<int32_t>();
	pMonster->_mAISeed = file->nextLE<int32_t>();
	file->skip(4); // Unused

	pMonster->_uniqtype = file->nextLE<uint8_t>();
	pMonster->_uniqtrans = file->nextLE<uint8_t>();
	pMonster->_udeadval = file->nextLE<int8_t>();

	pMonster->mWhoHit = file->nextLE<int8_t>();
	pMonster->mLevel = file->nextLE<int8_t>();
	file->skip(1); // Alignment
	pMonster->mExp = file->nextLE<uint16_t>();

	if (i < MAX_PLRS) // Don't skip for golems
		pMonster->mHit = file->nextLE<uint8_t>();
	else
		file->skip(1); // Skip mHit as it's already initialized
	pMonster->mMinDamage = file->nextLE<uint8_t>();
	pMonster->mMaxDamage = file->nextLE<uint8_t>();
	file->skip(1); // Skip mHit2 as it's already initialized
	pMonster->mMinDamage2 = file->nextLE<uint8_t>();
	pMonster->mMaxDamage2 = file->nextLE<uint8_t>();
	pMonster->mArmorClass = file->nextLE<uint8_t>();
	file->skip(1); // Alignment
	pMonster->mMagicRes = file->nextLE<uint16_t>();
	file->skip(2); // Alignment

	pMonster->mtalkmsg = static_cast<_speech_id>(file->nextLE<int32_t>());
	if (pMonster->mtalkmsg == TEXT_KING1) // Fix original bad mapping of NONE for monsters
		pMonster->mtalkmsg = TEXT_NONE;
	pMonster->leader = file->nextLE<uint8_t>();
	pMonster->leaderflag = file->nextLE<uint8_t>();
	pMonster->packsize = file->nextLE<uint8_t>();
	pMonster->mlid = file->nextLE<int8_t>();
	if (pMonster->mlid == plr[myplr]._plid)
		pMonster->mlid = NO_LIGHT; // Correct incorect values in old saves

	// Omit pointer mName;
	// Omit pointer MType;
	// Omit pointer MData;

	if (gbSkipSync)
		return;

	SyncMonsterAnim(i);
}

static void LoadMissile(LoadHelper *file, int i)
{
	MissileStruct *pMissile = &missile[i];

	pMissile->_mitype = file->nextLE<int32_t>();
	pMissile->position.tile.x = file->nextLE<int32_t>();
	pMissile->position.tile.y = file->nextLE<int32_t>();
	pMissile->position.offset.x = file->nextLE<int32_t>();
	pMissile->position.offset.y = file->nextLE<int32_t>();
	pMissile->position.velocity.x = file->nextLE<int32_t>();
	pMissile->position.velocity.y = file->nextLE<int32_t>();
	pMissile->position.start.x = file->nextLE<int32_t>();
	pMissile->position.start.y = file->nextLE<int32_t>();
	pMissile->position.traveled.x = file->nextLE<int32_t>();
	pMissile->position.traveled.y = file->nextLE<int32_t>();
	pMissile->_mimfnum = file->nextLE<int32_t>();
	pMissile->_mispllvl = file->nextLE<int32_t>();
	pMissile->_miDelFlag = file->nextBool32();
	pMissile->_miAnimType = file->nextLE<uint8_t>();
	file->skip(3); // Alignment
	pMissile->_miAnimFlags = file->nextLE<int32_t>();
	file->skip(4); // Skip pointer _miAnimData
	pMissile->_miAnimDelay = file->nextLE<int32_t>();
	pMissile->_miAnimLen = file->nextLE<int32_t>();
	pMissile->_miAnimWidth = file->nextLE<int32_t>();
	pMissile->_miAnimWidth2 = file->nextLE<int32_t>();
	pMissile->_miAnimCnt = file->nextLE<int32_t>();
	pMissile->_miAnimAdd = file->nextLE<int32_t>();
	pMissile->_miAnimFrame = file->nextLE<int32_t>();
	pMissile->_miDrawFlag = file->nextBool32();
	pMissile->_miLightFlag = file->nextBool32();
	pMissile->_miPreFlag = file->nextBool32();
	pMissile->_miUniqTrans = file->nextLE<uint32_t>();
	pMissile->_mirange = file->nextLE<int32_t>();
	pMissile->_misource = file->nextLE<int32_t>();
	pMissile->_micaster = file->nextLE<int32_t>();
	pMissile->_midam = file->nextLE<int32_t>();
	pMissile->_miHitFlag = file->nextBool32();
	pMissile->_midist = file->nextLE<int32_t>();
	pMissile->_mlid = file->nextLE<int32_t>();
	pMissile->_mirnd = file->nextLE<int32_t>();
	pMissile->_miVar1 = file->nextLE<int32_t>();
	pMissile->_miVar2 = file->nextLE<int32_t>();
	pMissile->_miVar3 = file->nextLE<int32_t>();
	pMissile->_miVar4 = file->nextLE<int32_t>();
	pMissile->_miVar5 = file->nextLE<int32_t>();
	pMissile->_miVar6 = file->nextLE<int32_t>();
	pMissile->_miVar7 = file->nextLE<int32_t>();
	pMissile->_miVar8 = file->nextBool32();
}

static void LoadObject(LoadHelper *file, int i)
{
	ObjectStruct *pObject = &object[i];

	pObject->_otype = static_cast<_object_id>(file->nextLE<int32_t>());
	pObject->position.x = file->nextLE<int32_t>();
	pObject->position.y = file->nextLE<int32_t>();
	pObject->_oLight = file->nextBool32();
	pObject->_oAnimFlag = file->nextLE<uint32_t>();
	file->skip(4); // Skip pointer _oAnimData
	pObject->_oAnimDelay = file->nextLE<int32_t>();
	pObject->_oAnimCnt = file->nextLE<int32_t>();
	pObject->_oAnimLen = file->nextLE<uint32_t>();
	pObject->_oAnimFrame = file->nextLE<uint32_t>();
	pObject->_oAnimWidth = file->nextLE<int32_t>();
	file->skip(4); // Skip _oAnimWidth2
	pObject->_oDelFlag = file->nextBool32();
	pObject->_oBreak = file->nextLE<int8_t>();
	file->skip(3); // Alignment
	pObject->_oSolidFlag = file->nextBool32();
	pObject->_oMissFlag = file->nextBool32();

	pObject->_oSelFlag = file->nextLE<int8_t>();
	file->skip(3); // Alignment
	pObject->_oPreFlag = file->nextBool32();
	pObject->_oTrapFlag = file->nextBool32();
	pObject->_oDoorFlag = file->nextBool32();
	pObject->_olid = file->nextLE<int32_t>();
	pObject->_oRndSeed = file->nextLE<int32_t>();
	pObject->_oVar1 = file->nextLE<int32_t>();
	pObject->_oVar2 = file->nextLE<int32_t>();
	pObject->_oVar3 = file->nextLE<int32_t>();
	pObject->_oVar4 = file->nextLE<int32_t>();
	pObject->_oVar5 = file->nextLE<int32_t>();
	pObject->_oVar6 = file->nextLE<uint32_t>();
	pObject->_oVar7 = static_cast<_speech_id>(file->nextLE<int32_t>());
	pObject->_oVar8 = file->nextLE<int32_t>();
}

static void LoadItem(LoadHelper *file, int i)
{
	LoadItemData(file, &items[i]);
	GetItemFrm(i);
}

static void LoadPremium(LoadHelper *file, int i)
{
	LoadItemData(file, &premiumitems[i]);
}

static void LoadQuest(LoadHelper *file, int i)
{
	QuestStruct *pQuest = &quests[i];

	pQuest->_qlevel = file->nextLE<uint8_t>();
	pQuest->_qtype = file->nextLE<uint8_t>();
	pQuest->_qactive = static_cast<quest_state>(file->nextLE<uint8_t>());
	pQuest->_qlvltype = static_cast<dungeon_type>(file->nextLE<uint8_t>());
	pQuest->position.x = file->nextLE<int32_t>();
	pQuest->position.y = file->nextLE<int32_t>();
	pQuest->_qslvl = static_cast<_setlevels>(file->nextLE<uint8_t>());
	pQuest->_qidx = file->nextLE<uint8_t>();
	if (gbIsHellfireSaveGame) {
		file->skip(2); // Alignment
		pQuest->_qmsg = static_cast<_speech_id>(file->nextLE<int32_t>());
	} else {
		pQuest->_qmsg = static_cast<_speech_id>(file->nextLE<uint8_t>());
	}
	pQuest->_qvar1 = file->nextLE<uint8_t>();
	pQuest->_qvar2 = file->nextLE<uint8_t>();
	file->skip(2); // Alignment
	if (!gbIsHellfireSaveGame)
		file->skip(1); // Alignment
	pQuest->_qlog = file->nextBool32();

	ReturnLvlX = file->nextBE<int32_t>();
	ReturnLvlY = file->nextBE<int32_t>();
	ReturnLvl = file->nextBE<int32_t>();
	ReturnLvlT = static_cast<dungeon_type>(file->nextBE<int32_t>());
	DoomQuestState = file->nextBE<int32_t>();
}

static void LoadLighting(LoadHelper *file, LightListStruct *pLight)
{
	pLight->position.tile.x = file->nextLE<int32_t>();
	pLight->position.tile.y = file->nextLE<int32_t>();
	pLight->_lradius = file->nextLE<int32_t>();
	pLight->_lid = file->nextLE<int32_t>();
	pLight->_ldel = file->nextBool32();
	pLight->_lunflag = file->nextBool32();
	file->skip(4); // Unused
	pLight->position.old.x = file->nextLE<int32_t>();
	pLight->position.old.y = file->nextLE<int32_t>();
	pLight->oldRadious = file->nextLE<int32_t>();
	pLight->position.offset.x = file->nextLE<int32_t>();
	pLight->position.offset.y = file->nextLE<int32_t>();
	pLight->_lflags = file->nextBool32();
}

static void LoadPortal(LoadHelper *file, int i)
{
	PortalStruct *pPortal = &portal[i];

	pPortal->open = file->nextBool32();
	pPortal->position.x = file->nextLE<int32_t>();
	pPortal->position.y = file->nextLE<int32_t>();
	pPortal->level = file->nextLE<int32_t>();
	pPortal->ltype = static_cast<dungeon_type>(file->nextLE<int32_t>());
	pPortal->setlvl = file->nextBool32();
}

_item_indexes RemapItemIdxFromDiablo(_item_indexes i)
{
	constexpr auto getItemIdValue = [](int i) -> int {
		if (i == IDI_SORCERER) {
			return 166;
		}
		if (i >= 156) {
			i += 5; // Hellfire exclusive items
		}
		if (i >= 88) {
			i += 1; // Scroll of Search
		}
		if (i >= 83) {
			i += 4; // Oils
		}

		return i;
	};

	return static_cast<_item_indexes>(getItemIdValue(i));
}

_item_indexes RemapItemIdxToDiablo(_item_indexes i)
{
	constexpr auto getItemIdValue = [](int i) -> int {
		if (i == 166) {
			return IDI_SORCERER;
		}
		if ((i >= 83 && i <= 86) || i == 92 || i >= 161) {
			return -1; // Hellfire exclusive items
		}
		if (i >= 93) {
			i -= 1; // Scroll of Search
		}
		if (i >= 87) {
			i -= 4; // Oils
		}

		return i;
	};

	return static_cast<_item_indexes>(getItemIdValue(i));
}

_item_indexes RemapItemIdxFromSpawn(_item_indexes i)
{
	constexpr auto getItemIdValue = [](int i) {
		if (i >= 62) {
			i += 9; // Medium and heavy armors
		}
		if (i >= 96) {
			i += 1; // Scroll of Stone Curse
		}
		if (i >= 98) {
			i += 1; // Scroll of Guardian
		}
		if (i >= 99) {
			i += 1; // Scroll of ...
		}
		if (i >= 101) {
			i += 1; // Scroll of Golem
		}
		if (i >= 102) {
			i += 1; // Scroll of None
		}
		if (i >= 104) {
			i += 1; // Scroll of Apocalypse
		}

		return i;
	};

	return static_cast<_item_indexes>(getItemIdValue(i));
}

_item_indexes RemapItemIdxToSpawn(_item_indexes i)
{
	constexpr auto getItemIdValue = [](int i) {
		if (i >= 104) {
			i -= 1; // Scroll of Apocalypse
		}
		if (i >= 102) {
			i -= 1; // Scroll of None
		}
		if (i >= 101) {
			i -= 1; // Scroll of Golem
		}
		if (i >= 99) {
			i -= 1; // Scroll of ...
		}
		if (i >= 98) {
			i -= 1; // Scroll of Guardian
		}
		if (i >= 96) {
			i -= 1; // Scroll of Stone Curse
		}
		if (i >= 71) {
			i -= 9; // Medium and heavy armors
		}

		return i;
	};

	return static_cast<_item_indexes>(getItemIdValue(i));
}

bool IsHeaderValid(uint32_t magicNumber)
{
	gbIsHellfireSaveGame = false;
	if (magicNumber == LoadLE32("SHAR")) {
		return true;
	}
	if (magicNumber == LoadLE32("SHLF")) {
		gbIsHellfireSaveGame = true;
		return true;
	} else if (!gbIsSpawn && magicNumber == LoadLE32("RETL")) {
		return true;
	} else if (!gbIsSpawn && magicNumber == LoadLE32("HELF")) {
		gbIsHellfireSaveGame = true;
		return true;
	}

	return false;
}

static void ConvertLevels()
{
	// Backup current level state
	bool _setlevel = setlevel;
	_setlevels _setlvlnum = setlvlnum;
	int _currlevel = currlevel;
	dungeon_type _leveltype = leveltype;

	gbSkipSync = true;

	setlevel = false; // Convert regular levels
	for (int i = 0; i < giNumberOfLevels; i++) {
		currlevel = i;
		if (!LevelFileExists())
			continue;

		leveltype = gnLevelTypeTbl[i];

		LoadLevel();
		SaveLevel();
	}

	setlevel = true; // Convert quest levels
	for (auto &quest : quests) {
		if (quest._qactive == QUEST_NOTAVAIL) {
			continue;
		}

		leveltype = quest._qlvltype;
		if (leveltype == DTYPE_NONE) {
			continue;
		}

		setlvlnum = quest._qslvl;
		if (!LevelFileExists())
			continue;

		LoadLevel();
		SaveLevel();
	}

	gbSkipSync = false;

	// Restor current level state
	setlevel = _setlevel;
	setlvlnum = _setlvlnum;
	currlevel = _currlevel;
	leveltype = _leveltype;
}

void LoadHotkeys()
{
	LoadHelper file("hotkeys");
	if (!file.isValid())
		return;

	auto &myPlayer = plr[myplr];

	for (auto &spellId : myPlayer._pSplHotKey) {
		spellId = static_cast<spell_id>(file.nextLE<int32_t>());
	}
	for (auto &spellType : myPlayer._pSplTHotKey) {
		spellType = static_cast<spell_type>(file.nextLE<int8_t>());
	}
	myPlayer._pRSpell = static_cast<spell_id>(file.nextLE<int32_t>());
	myPlayer._pRSplType = static_cast<spell_type>(file.nextLE<int8_t>());
}

void SaveHotkeys()
{
	auto &myPlayer = plr[myplr];

	const size_t nHotkeyTypes = sizeof(myPlayer._pSplHotKey) / sizeof(myPlayer._pSplHotKey[0]);
	const size_t nHotkeySpells = sizeof(myPlayer._pSplTHotKey) / sizeof(myPlayer._pSplTHotKey[0]);

	SaveHelper file("hotkeys", (nHotkeyTypes * 4) + nHotkeySpells + 4 + 1);

	for (auto &spellId : myPlayer._pSplHotKey) {
		file.writeLE<int32_t>(spellId);
	}
	for (auto &spellType : myPlayer._pSplTHotKey) {
		file.writeLE<uint8_t>(spellType);
	}
	file.writeLE<int32_t>(myPlayer._pRSpell);
	file.writeLE<uint8_t>(myPlayer._pRSplType);
}

static void LoadMatchingItems(LoadHelper *file, const int n, ItemStruct *pItem)
{
	ItemStruct tempItem;

	for (int i = 0; i < n; i++) {
		LoadItemData(file, &tempItem);
		if (pItem[i].isEmpty() || tempItem.isEmpty())
			continue;
		if (pItem[i]._iSeed != tempItem._iSeed)
			continue;
		pItem[i] = tempItem;
	}
}

void LoadHeroItems(PlayerStruct &player)
{
	LoadHelper file("heroitems");
	if (!file.isValid())
		return;

	gbIsHellfireSaveGame = file.nextBool8();

	LoadMatchingItems(&file, NUM_INVLOC, player.InvBody);
	LoadMatchingItems(&file, NUM_INV_GRID_ELEM, player.InvList);
	LoadMatchingItems(&file, MAXBELTITEMS, player.SpdList);

	gbIsHellfireSaveGame = gbIsHellfire;
}

void RemoveEmptyInventory(PlayerStruct &player)
{
	for (int i = NUM_INV_GRID_ELEM; i > 0; i--) {
		int idx = player.InvGrid[i - 1];
		if (idx > 0 && player.InvList[idx - 1].isEmpty()) {
			player.RemoveInvItem(idx - 1);
		}
	};
}

void RemoveEmptyLevelItems()
{
	for (int i = numitems; i > 0; i--) {
		int ii = itemactive[i];
		if (items[ii].isEmpty()) {
			dItem[items[ii].position.x][items[ii].position.y] = 0;
			DeleteItem(ii, i);
		}
	}
}

/**
 * @brief Load game state
 * @param firstflag Can be set to false if we are simply reloading the current game
 */
void LoadGame(bool firstflag)
{
	FreeGameMem();
	pfile_remove_temp_files();

	LoadHelper file("game");
	if (!file.isValid())
		app_fatal("%s", _("Unable to open save file archive"));

	if (!IsHeaderValid(file.nextLE<uint32_t>()))
		app_fatal("%s", _("Invalid save file"));

	if (gbIsHellfireSaveGame) {
		giNumberOfLevels = 25;
		giNumberQuests = 24;
		giNumberOfSmithPremiumItems = 15;
	} else {
		// Todo initialize additional levels and quests if we are running Hellfire
		giNumberOfLevels = 17;
		giNumberQuests = 16;
		giNumberOfSmithPremiumItems = 6;
	}

	setlevel = file.nextBool8();
	setlvlnum = static_cast<_setlevels>(file.nextBE<uint32_t>());
	currlevel = file.nextBE<uint32_t>();
	leveltype = static_cast<dungeon_type>(file.nextBE<uint32_t>());
	if (!setlevel)
		leveltype = gnLevelTypeTbl[currlevel];
	int _ViewX = file.nextBE<int32_t>();
	int _ViewY = file.nextBE<int32_t>();
	invflag = file.nextBool8();
	chrflag = file.nextBool8();
	int _nummonsters = file.nextBE<int32_t>();
	int _numitems = file.nextBE<int32_t>();
	int _nummissiles = file.nextBE<int32_t>();
	int _nobjects = file.nextBE<int32_t>();

	if (!gbIsHellfire && currlevel > 17)
		app_fatal("%s", _("Player is on a Hellfire only level"));

	for (uint8_t i = 0; i < giNumberOfLevels; i++) {
		glSeedTbl[i] = file.nextBE<uint32_t>();
		file.skip(4); // Skip loading gnLevelTypeTbl
	}

	LoadPlayer(&file, myplr);

	sgGameInitInfo.nDifficulty = plr[myplr].pDifficulty;
	if (sgGameInitInfo.nDifficulty < DIFF_NORMAL || sgGameInitInfo.nDifficulty > DIFF_HELL)
		sgGameInitInfo.nDifficulty = DIFF_NORMAL;

	for (int i = 0; i < giNumberQuests; i++)
		LoadQuest(&file, i);
	for (int i = 0; i < MAXPORTAL; i++)
		LoadPortal(&file, i);

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		ConvertLevels();
		RemoveEmptyInventory(plr[myplr]);
	}

	LoadGameLevel(firstflag, ENTRY_LOAD);
	SyncInitPlr(myplr);
	SyncPlrAnim(myplr);

	ViewX = _ViewX;
	ViewY = _ViewY;
	nummonsters = _nummonsters;
	numitems = _numitems;
	nummissiles = _nummissiles;
	nobjects = _nobjects;

	for (int &monstkill : monstkills)
		monstkill = file.nextBE<int32_t>();

	if (leveltype != DTYPE_TOWN) {
		for (int &monsterId : monstactive)
			monsterId = file.nextBE<int32_t>();
		for (int i = 0; i < nummonsters; i++)
			LoadMonster(&file, monstactive[i]);
		for (int &missileId : missileactive)
			missileId = file.nextLE<int8_t>();
		for (int &missileId : missileavail)
			missileId = file.nextLE<int8_t>();
		for (int i = 0; i < nummissiles; i++)
			LoadMissile(&file, missileactive[i]);
		for (int &objectId : objectactive)
			objectId = file.nextLE<int8_t>();
		for (int &objectId : objectavail)
			objectId = file.nextLE<int8_t>();
		for (int i = 0; i < nobjects; i++)
			LoadObject(&file, objectactive[i]);
		for (int i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);

		numlights = file.nextBE<int32_t>();

		for (uint8_t &lightId : lightactive)
			lightId = file.nextLE<uint8_t>();
		for (int i = 0; i < numlights; i++)
			LoadLighting(&file, &LightList[lightactive[i]]);

		visionid = file.nextBE<int32_t>();
		numvision = file.nextBE<int32_t>();

		for (int i = 0; i < numvision; i++)
			LoadLighting(&file, &VisionList[i]);
	}

	for (int &itemId : itemactive)
		itemId = file.nextLE<int8_t>();
	for (int &itemId : itemavail)
		itemId = file.nextLE<int8_t>();
	for (int i = 0; i < numitems; i++)
		LoadItem(&file, itemactive[i]);
	for (bool &UniqueItemFlag : UniqueItemFlags)
		UniqueItemFlag = file.nextBool8();

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dLight[i][j] = file.nextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dFlags[i][j] = file.nextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dPlayer[i][j] = file.nextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dItem[i][j] = file.nextLE<int8_t>();
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMonster[i][j] = file.nextBE<int32_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dDead[i][j] = file.nextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dObject[i][j] = file.nextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dLight[i][j] = file.nextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dPreLight[i][j] = file.nextLE<int8_t>();
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				AutomapView[i][j] = file.nextBool8();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMissile[i][j] = file.nextLE<int8_t>();
		}
	}

	numpremium = file.nextBE<int32_t>();
	premiumlevel = file.nextBE<int32_t>();

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		LoadPremium(&file, i);
	if (gbIsHellfire && !gbIsHellfireSaveGame)
		SpawnPremium(myplr);

	AutomapActive = file.nextBool8();
	AutoMapScale = file.nextBE<int32_t>();
	AutomapZoomReset();
	ResyncQuests();

	if (leveltype != DTYPE_TOWN)
		ProcessLightList();

	RedoPlayerVision();
	ProcessVisionList();
	missiles_process_charge();
	ResetPal();
	NewCursor(CURSOR_HAND);
	gbProcessPlayers = true;

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		RemoveEmptyLevelItems();
		SaveGame();
	}

	gbIsHellfireSaveGame = gbIsHellfire;
}

static void SaveItem(SaveHelper *file, ItemStruct *pItem)
{
	auto idx = pItem->IDidx;
	if (!gbIsHellfire)
		idx = RemapItemIdxToDiablo(idx);
	if (gbIsSpawn)
		idx = RemapItemIdxToSpawn(idx);
	int iType = pItem->_itype;
	if (idx == -1) {
		idx = _item_indexes::IDI_GOLD;
		iType = ITYPE_NONE;
	}

	file->writeLE<int32_t>(pItem->_iSeed);
	file->writeLE<int16_t>(pItem->_iCreateInfo);
	file->skip(2); // Alignment
	file->writeLE<int32_t>(iType);
	file->writeLE<int32_t>(pItem->position.x);
	file->writeLE<int32_t>(pItem->position.y);
	file->writeLE<uint32_t>(pItem->_iAnimFlag);
	file->skip(4); // Skip pointer _iAnimData
	file->writeLE<int32_t>(pItem->AnimInfo.NumberOfFrames);
	file->writeLE<int32_t>(pItem->AnimInfo.CurrentFrame);
	// write _iAnimWidth for vanilla compatibility
	file->writeLE<int32_t>(ItemAnimWidth);
	// write _iAnimWidth2 for vanilla compatibility
	file->writeLE<int32_t>(CalculateWidth2(ItemAnimWidth));
	file->skip(4); // Unused since 1.02
	file->writeLE<uint8_t>(pItem->_iSelFlag);
	file->skip(3); // Alignment
	file->writeLE<uint32_t>(pItem->_iPostDraw);
	file->writeLE<uint32_t>(pItem->_iIdentified);
	file->writeLE<int8_t>(pItem->_iMagical);
	file->writeBytes(pItem->_iName, 64);
	file->writeBytes(pItem->_iIName, 64);
	file->writeLE<int8_t>(pItem->_iLoc);
	file->writeLE<uint8_t>(pItem->_iClass);
	file->skip(1); // Alignment
	file->writeLE<int32_t>(pItem->_iCurs);
	file->writeLE<int32_t>(pItem->_ivalue);
	file->writeLE<int32_t>(pItem->_iIvalue);
	file->writeLE<int32_t>(pItem->_iMinDam);
	file->writeLE<int32_t>(pItem->_iMaxDam);
	file->writeLE<int32_t>(pItem->_iAC);
	file->writeLE<uint32_t>(pItem->_iFlags);
	file->writeLE<int32_t>(pItem->_iMiscId);
	file->writeLE<int32_t>(pItem->_iSpell);
	file->writeLE<int32_t>(pItem->_iCharges);
	file->writeLE<int32_t>(pItem->_iMaxCharges);
	file->writeLE<int32_t>(pItem->_iDurability);
	file->writeLE<int32_t>(pItem->_iMaxDur);
	file->writeLE<int32_t>(pItem->_iPLDam);
	file->writeLE<int32_t>(pItem->_iPLToHit);
	file->writeLE<int32_t>(pItem->_iPLAC);
	file->writeLE<int32_t>(pItem->_iPLStr);
	file->writeLE<int32_t>(pItem->_iPLMag);
	file->writeLE<int32_t>(pItem->_iPLDex);
	file->writeLE<int32_t>(pItem->_iPLVit);
	file->writeLE<int32_t>(pItem->_iPLFR);
	file->writeLE<int32_t>(pItem->_iPLLR);
	file->writeLE<int32_t>(pItem->_iPLMR);
	file->writeLE<int32_t>(pItem->_iPLMana);
	file->writeLE<int32_t>(pItem->_iPLHP);
	file->writeLE<int32_t>(pItem->_iPLDamMod);
	file->writeLE<int32_t>(pItem->_iPLGetHit);
	file->writeLE<int32_t>(pItem->_iPLLight);
	file->writeLE<int8_t>(pItem->_iSplLvlAdd);
	file->writeLE<int8_t>(pItem->_iRequest);
	file->skip(2); // Alignment
	file->writeLE<int32_t>(pItem->_iUid);
	file->writeLE<int32_t>(pItem->_iFMinDam);
	file->writeLE<int32_t>(pItem->_iFMaxDam);
	file->writeLE<int32_t>(pItem->_iLMinDam);
	file->writeLE<int32_t>(pItem->_iLMaxDam);
	file->writeLE<int32_t>(pItem->_iPLEnAc);
	file->writeLE<int8_t>(pItem->_iPrePower);
	file->writeLE<int8_t>(pItem->_iSufPower);
	file->skip(2); // Alignment
	file->writeLE<int32_t>(pItem->_iVAdd1);
	file->writeLE<int32_t>(pItem->_iVMult1);
	file->writeLE<int32_t>(pItem->_iVAdd2);
	file->writeLE<int32_t>(pItem->_iVMult2);
	file->writeLE<int8_t>(pItem->_iMinStr);
	file->writeLE<uint8_t>(pItem->_iMinMag);
	file->writeLE<int8_t>(pItem->_iMinDex);
	file->skip(1); // Alignment
	file->writeLE<uint32_t>(pItem->_iStatFlag);
	file->writeLE<int32_t>(idx);
	file->writeLE<uint32_t>(pItem->dwBuff);
	if (gbIsHellfire)
		file->writeLE<uint32_t>(pItem->_iDamAcFlags);
}

static void SaveItems(SaveHelper *file, ItemStruct *pItem, const int n)
{
	for (int i = 0; i < n; i++) {
		SaveItem(file, &pItem[i]);
	}
}

static void SavePlayer(SaveHelper *file, int p)
{
	auto &player = plr[p];

	file->writeLE<int32_t>(player._pmode);
	for (int8_t step : player.walkpath)
		file->writeLE<int8_t>(step);
	file->writeLE<uint8_t>(player.plractive);
	file->skip(2); // Alignment
	file->writeLE<int32_t>(player.destAction);
	file->writeLE<int32_t>(player.destParam1);
	file->writeLE<int32_t>(player.destParam2);
	file->writeLE<int32_t>(player.destParam3);
	file->writeLE<int32_t>(player.destParam4);
	file->writeLE<int32_t>(player.plrlevel);
	file->writeLE<int32_t>(player.position.tile.x);
	file->writeLE<int32_t>(player.position.tile.y);
	file->writeLE<int32_t>(player.position.future.x);
	file->writeLE<int32_t>(player.position.future.y);

	// For backwards compatibility
	const Point target = player.GetTargetPosition();
	file->writeLE<int32_t>(target.x);
	file->writeLE<int32_t>(target.y);

	file->writeLE<int32_t>(player.position.last.x);
	file->writeLE<int32_t>(player.position.last.y);
	file->writeLE<int32_t>(player.position.old.x);
	file->writeLE<int32_t>(player.position.old.y);
	file->writeLE<int32_t>(player.position.offset.x);
	file->writeLE<int32_t>(player.position.offset.y);
	file->writeLE<int32_t>(player.position.velocity.x);
	file->writeLE<int32_t>(player.position.velocity.y);
	file->writeLE<int32_t>(player._pdir);
	file->skip(4); // Unused
	file->writeLE<int32_t>(player._pgfxnum);
	file->skip(4); // Skip pointer _pAnimData
	file->writeLE<int32_t>(player.AnimInfo.DelayLen);
	file->writeLE<int32_t>(player.AnimInfo.DelayCounter);
	file->writeLE<int32_t>(player.AnimInfo.NumberOfFrames);
	file->writeLE<int32_t>(player.AnimInfo.CurrentFrame);
	// write _pAnimWidth for vanilla compatibility
	int animWidth = player.AnimInfo.pCelSprite == nullptr ? 96 : player.AnimInfo.pCelSprite->Width();
	file->writeLE<int32_t>(animWidth);
	// write _pAnimWidth2 for vanilla compatibility
	file->writeLE<int32_t>(CalculateWidth2(animWidth));
	file->skip(4); // Skip _peflag
	file->writeLE<int32_t>(player._plid);
	file->writeLE<int32_t>(player._pvid);

	file->writeLE<int32_t>(player._pSpell);
	file->writeLE<int8_t>(player._pSplType);
	file->writeLE<int8_t>(player._pSplFrom);
	file->skip(2); // Alignment
	file->writeLE<int32_t>(player._pTSpell);
	file->writeLE<int8_t>(player._pTSplType);
	file->skip(3); // Alignment
	file->writeLE<int32_t>(player._pRSpell);
	file->writeLE<int8_t>(player._pRSplType);
	file->skip(3); // Alignment
	file->writeLE<int32_t>(player._pSBkSpell);
	file->writeLE<int8_t>(player._pSBkSplType);
	for (int8_t spellLevel : player._pSplLvl)
		file->writeLE<int8_t>(spellLevel);
	file->skip(7); // Alignment
	file->writeLE<uint64_t>(player._pMemSpells);
	file->writeLE<uint64_t>(player._pAblSpells);
	file->writeLE<uint64_t>(player._pScrlSpells);
	file->writeLE<uint8_t>(player._pSpellFlags);
	file->skip(3); // Alignment
	for (auto &spellId : player._pSplHotKey)
		file->writeLE<int32_t>(spellId);
	for (auto &spellType : player._pSplTHotKey)
		file->writeLE<int8_t>(spellType);

	file->writeLE<int32_t>(player._pwtype);
	file->writeLE<uint8_t>(player._pBlockFlag);
	file->writeLE<uint8_t>(player._pInvincible);
	file->writeLE<int8_t>(player._pLightRad);
	file->writeLE<uint8_t>(player._pLvlChanging);

	file->writeBytes(player._pName, PLR_NAME_LEN);
	file->writeLE<int8_t>(static_cast<int8_t>(player._pClass));
	file->skip(3); // Alignment
	file->writeLE<int32_t>(player._pStrength);
	file->writeLE<int32_t>(player._pBaseStr);
	file->writeLE<int32_t>(player._pMagic);
	file->writeLE<int32_t>(player._pBaseMag);
	file->writeLE<int32_t>(player._pDexterity);
	file->writeLE<int32_t>(player._pBaseDex);
	file->writeLE<int32_t>(player._pVitality);
	file->writeLE<int32_t>(player._pBaseVit);
	file->writeLE<int32_t>(player._pStatPts);
	file->writeLE<int32_t>(player._pDamageMod);

	file->writeLE<int32_t>(player._pBaseToBlk);
	file->writeLE<int32_t>(player._pHPBase);
	file->writeLE<int32_t>(player._pMaxHPBase);
	file->writeLE<int32_t>(player._pHitPoints);
	file->writeLE<int32_t>(player._pMaxHP);
	file->writeLE<int32_t>(player._pHPPer);
	file->writeLE<int32_t>(player._pManaBase);
	file->writeLE<int32_t>(player._pMaxManaBase);
	file->writeLE<int32_t>(player._pMana);
	file->writeLE<int32_t>(player._pMaxMana);
	file->writeLE<int32_t>(player._pManaPer);
	file->writeLE<int8_t>(player._pLevel);
	file->writeLE<int8_t>(player._pMaxLvl);
	file->skip(2); // Alignment
	file->writeLE<int32_t>(player._pExperience);
	file->writeLE<int32_t>(player._pMaxExp);
	file->writeLE<int32_t>(player._pNextExper);
	file->writeLE<int8_t>(player._pArmorClass);
	file->writeLE<int8_t>(player._pMagResist);
	file->writeLE<int8_t>(player._pFireResist);
	file->writeLE<int8_t>(player._pLghtResist);
	file->writeLE<int32_t>(player._pGold);

	file->writeLE<uint32_t>(player._pInfraFlag);
	file->writeLE<int32_t>(player.position.temp.x);
	file->writeLE<int32_t>(player.position.temp.y);
	file->writeLE<int32_t>(player.tempDirection);
	file->writeLE<int32_t>(player._pVar4);
	file->writeLE<int32_t>(player._pVar5);
	file->writeLE<int32_t>(player.position.offset2.x);
	file->writeLE<int32_t>(player.position.offset2.y);
	// Write actionFrame for vanilla compatibility
	file->writeLE<int32_t>(0);
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		file->writeLE<uint8_t>(player._pLvlVisited[i]);
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		file->writeLE<uint8_t>(player._pSLvlVisited[i]); // only 10 used

	file->skip(2); // Alignment

	// Write _pGFXLoad for vanilla compatibility
	file->writeLE<int32_t>(0);
	file->skip(4 * 8); // Skip pointers _pNAnim
	file->writeLE<int32_t>(player._pNFrames);
	file->skip(4);     // Skip _pNWidth
	file->skip(4 * 8); // Skip pointers _pWAnim
	file->writeLE<int32_t>(player._pWFrames);
	file->skip(4);     // Skip _pWWidth
	file->skip(4 * 8); // Skip pointers _pAAnim
	file->writeLE<int32_t>(player._pAFrames);
	file->skip(4); // Skip _pAWidth
	file->writeLE<int32_t>(player._pAFNum);
	file->skip(4 * 8); // Skip pointers _pLAnim
	file->skip(4 * 8); // Skip pointers _pFAnim
	file->skip(4 * 8); // Skip pointers _pTAnim
	file->writeLE<int32_t>(player._pSFrames);
	file->skip(4); // Skip _pSWidth
	file->writeLE<int32_t>(player._pSFNum);
	file->skip(4 * 8); // Skip pointers _pHAnim
	file->writeLE<int32_t>(player._pHFrames);
	file->skip(4);     // Skip _pHWidth
	file->skip(4 * 8); // Skip pointers _pDAnim
	file->writeLE<int32_t>(player._pDFrames);
	file->skip(4);     // Skip _pDWidth
	file->skip(4 * 8); // Skip pointers _pBAnim
	file->writeLE<int32_t>(player._pBFrames);
	file->skip(4); // Skip _pBWidth

	SaveItems(file, player.InvBody, NUM_INVLOC);
	SaveItems(file, player.InvList, NUM_INV_GRID_ELEM);
	file->writeLE<int32_t>(player._pNumInv);
	for (int8_t cell : player.InvGrid)
		file->writeLE<int8_t>(cell);
	SaveItems(file, player.SpdList, MAXBELTITEMS);
	SaveItem(file, &player.HoldItem);

	file->writeLE<int32_t>(player._pIMinDam);
	file->writeLE<int32_t>(player._pIMaxDam);
	file->writeLE<int32_t>(player._pIAC);
	file->writeLE<int32_t>(player._pIBonusDam);
	file->writeLE<int32_t>(player._pIBonusToHit);
	file->writeLE<int32_t>(player._pIBonusAC);
	file->writeLE<int32_t>(player._pIBonusDamMod);
	file->skip(4); // Alignment

	file->writeLE<uint64_t>(player._pISpells);
	file->writeLE<int32_t>(player._pIFlags);
	file->writeLE<int32_t>(player._pIGetHit);

	file->writeLE<int8_t>(player._pISplLvlAdd);
	file->skip(1); // Unused
	file->skip(2); // Alignment
	file->writeLE<int32_t>(player._pISplDur);
	file->writeLE<int32_t>(player._pIEnAc);
	file->writeLE<int32_t>(player._pIFMinDam);
	file->writeLE<int32_t>(player._pIFMaxDam);
	file->writeLE<int32_t>(player._pILMinDam);
	file->writeLE<int32_t>(player._pILMaxDam);
	file->writeLE<int32_t>(player._pOilType);
	file->writeLE<uint8_t>(player.pTownWarps);
	file->writeLE<uint8_t>(player.pDungMsgs);
	file->writeLE<uint8_t>(player.pLvlLoad);
	if (gbIsHellfire)
		file->writeLE<uint8_t>(player.pDungMsgs2);
	else
		file->writeLE<uint8_t>(player.pBattleNet);
	file->writeLE<uint8_t>(player.pManaShield);
	file->writeLE<uint8_t>(player.pOriginalCathedral);
	file->skip(2); // Available bytes
	file->writeLE<uint16_t>(player.wReflections);
	file->skip(14); // Available bytes

	file->writeLE<uint32_t>(player.pDiabloKillLevel);
	file->writeLE<uint32_t>(player.pDifficulty);
	file->writeLE<uint32_t>(player.pDamAcFlags);
	file->skip(20); // Available bytes

	// Omit pointer _pNData
	// Omit pointer _pWData
	// Omit pointer _pAData
	// Omit pointer _pLData
	// Omit pointer _pFData
	// Omit pointer  _pTData
	// Omit pointer _pHData
	// Omit pointer _pDData
	// Omit pointer _pBData
	// Omit pointer pReserved
}

static void SaveMonster(SaveHelper *file, int i)
{
	MonsterStruct *pMonster = &monster[i];

	file->writeLE<int32_t>(pMonster->_mMTidx);
	file->writeLE<int32_t>(pMonster->_mmode);
	file->writeLE<uint8_t>(pMonster->_mgoal);
	file->skip(3); // Alignment
	file->writeLE<int32_t>(pMonster->_mgoalvar1);
	file->writeLE<int32_t>(pMonster->_mgoalvar2);
	file->writeLE<int32_t>(pMonster->_mgoalvar3);
	file->skip(4); // Unused
	file->writeLE<uint8_t>(pMonster->_pathcount);
	file->skip(3); // Alignment
	file->writeLE<int32_t>(pMonster->position.tile.x);
	file->writeLE<int32_t>(pMonster->position.tile.y);
	file->writeLE<int32_t>(pMonster->position.future.x);
	file->writeLE<int32_t>(pMonster->position.future.y);
	file->writeLE<int32_t>(pMonster->position.old.x);
	file->writeLE<int32_t>(pMonster->position.old.y);
	file->writeLE<int32_t>(pMonster->position.offset.x);
	file->writeLE<int32_t>(pMonster->position.offset.y);
	file->writeLE<int32_t>(pMonster->position.velocity.x);
	file->writeLE<int32_t>(pMonster->position.velocity.y);
	file->writeLE<int32_t>(pMonster->_mdir);
	file->writeLE<int32_t>(pMonster->_menemy);
	file->writeLE<uint8_t>(pMonster->enemyPosition.x);
	file->writeLE<uint8_t>(pMonster->enemyPosition.y);
	file->skip(2); // Unused

	file->skip(4); // Skip pointer _mAnimData
	file->writeLE<int32_t>(pMonster->_mAnimDelay);
	file->writeLE<int32_t>(pMonster->_mAnimCnt);
	file->writeLE<int32_t>(pMonster->_mAnimLen);
	file->writeLE<int32_t>(pMonster->_mAnimFrame);
	file->skip(4); // Skip _meflag
	file->writeLE<uint32_t>(pMonster->_mDelFlag);
	file->writeLE<int32_t>(pMonster->_mVar1);
	file->writeLE<int32_t>(pMonster->_mVar2);
	file->writeLE<int32_t>(pMonster->_mVar3);
	file->writeLE<int32_t>(pMonster->position.temp.x);
	file->writeLE<int32_t>(pMonster->position.temp.y);
	file->writeLE<int32_t>(pMonster->position.offset2.x);
	file->writeLE<int32_t>(pMonster->position.offset2.y);
	file->writeLE<int32_t>(pMonster->actionFrame);
	file->writeLE<int32_t>(pMonster->_mmaxhp);
	file->writeLE<int32_t>(pMonster->_mhitpoints);

	file->writeLE<uint8_t>(pMonster->_mAi);
	file->writeLE<uint8_t>(pMonster->_mint);
	file->skip(2); // Alignment
	file->writeLE<uint32_t>(pMonster->_mFlags);
	file->writeLE<uint8_t>(pMonster->_msquelch);
	file->skip(3); // Alignment
	file->skip(4); // Unused
	file->writeLE<int32_t>(pMonster->position.last.x);
	file->writeLE<int32_t>(pMonster->position.last.y);
	file->writeLE<int32_t>(pMonster->_mRndSeed);
	file->writeLE<int32_t>(pMonster->_mAISeed);
	file->skip(4); // Unused

	file->writeLE<uint8_t>(pMonster->_uniqtype);
	file->writeLE<uint8_t>(pMonster->_uniqtrans);
	file->writeLE<int8_t>(pMonster->_udeadval);

	file->writeLE<int8_t>(pMonster->mWhoHit);
	file->writeLE<int8_t>(pMonster->mLevel);
	file->skip(1); // Alignment
	file->writeLE<uint16_t>(pMonster->mExp);

	file->writeLE<uint8_t>(pMonster->mHit < UINT8_MAX ? pMonster->mHit : UINT8_MAX); // For backwards compatibility
	file->writeLE<uint8_t>(pMonster->mMinDamage);
	file->writeLE<uint8_t>(pMonster->mMaxDamage);
	file->writeLE<uint8_t>(pMonster->mHit2 < UINT8_MAX ? pMonster->mHit2 : UINT8_MAX); // For backwards compatibility
	file->writeLE<uint8_t>(pMonster->mMinDamage2);
	file->writeLE<uint8_t>(pMonster->mMaxDamage2);
	file->writeLE<uint8_t>(pMonster->mArmorClass);
	file->skip(1); // Alignment
	file->writeLE<uint16_t>(pMonster->mMagicRes);
	file->skip(2); // Alignment

	file->writeLE<int32_t>(pMonster->mtalkmsg == TEXT_NONE ? 0 : pMonster->mtalkmsg); // Replicate original bad mapping of none for monsters
	file->writeLE<uint8_t>(pMonster->leader);
	file->writeLE<uint8_t>(pMonster->leaderflag);
	file->writeLE<uint8_t>(pMonster->packsize);
	file->writeLE<int8_t>(pMonster->mlid);

	// Omit pointer mName;
	// Omit pointer MType;
	// Omit pointer MData;
}

static void SaveMissile(SaveHelper *file, int i)
{
	MissileStruct *pMissile = &missile[i];

	file->writeLE<int32_t>(pMissile->_mitype);
	file->writeLE<int32_t>(pMissile->position.tile.x);
	file->writeLE<int32_t>(pMissile->position.tile.y);
	file->writeLE<int32_t>(pMissile->position.offset.x);
	file->writeLE<int32_t>(pMissile->position.offset.y);
	file->writeLE<int32_t>(pMissile->position.velocity.x);
	file->writeLE<int32_t>(pMissile->position.velocity.y);
	file->writeLE<int32_t>(pMissile->position.start.x);
	file->writeLE<int32_t>(pMissile->position.start.y);
	file->writeLE<int32_t>(pMissile->position.traveled.x);
	file->writeLE<int32_t>(pMissile->position.traveled.y);
	file->writeLE<int32_t>(pMissile->_mimfnum);
	file->writeLE<int32_t>(pMissile->_mispllvl);
	file->writeLE<uint32_t>(pMissile->_miDelFlag);
	file->writeLE<uint8_t>(pMissile->_miAnimType);
	file->skip(3); // Alignment
	file->writeLE<int32_t>(pMissile->_miAnimFlags);
	file->skip(4); // Skip pointer _miAnimData
	file->writeLE<int32_t>(pMissile->_miAnimDelay);
	file->writeLE<int32_t>(pMissile->_miAnimLen);
	file->writeLE<int32_t>(pMissile->_miAnimWidth);
	file->writeLE<int32_t>(pMissile->_miAnimWidth2);
	file->writeLE<int32_t>(pMissile->_miAnimCnt);
	file->writeLE<int32_t>(pMissile->_miAnimAdd);
	file->writeLE<int32_t>(pMissile->_miAnimFrame);
	file->writeLE<uint32_t>(pMissile->_miDrawFlag);
	file->writeLE<uint32_t>(pMissile->_miLightFlag);
	file->writeLE<uint32_t>(pMissile->_miPreFlag);
	file->writeLE<uint32_t>(pMissile->_miUniqTrans);
	file->writeLE<int32_t>(pMissile->_mirange);
	file->writeLE<int32_t>(pMissile->_misource);
	file->writeLE<int32_t>(pMissile->_micaster);
	file->writeLE<int32_t>(pMissile->_midam);
	file->writeLE<uint32_t>(pMissile->_miHitFlag);
	file->writeLE<int32_t>(pMissile->_midist);
	file->writeLE<int32_t>(pMissile->_mlid);
	file->writeLE<int32_t>(pMissile->_mirnd);
	file->writeLE<int32_t>(pMissile->_miVar1);
	file->writeLE<int32_t>(pMissile->_miVar2);
	file->writeLE<int32_t>(pMissile->_miVar3);
	file->writeLE<int32_t>(pMissile->_miVar4);
	file->writeLE<int32_t>(pMissile->_miVar5);
	file->writeLE<int32_t>(pMissile->_miVar6);
	file->writeLE<int32_t>(pMissile->_miVar7);
	file->writeLE<int32_t>(pMissile->_miVar8);
}

static void SaveObject(SaveHelper *file, int i)
{
	ObjectStruct *pObject = &object[i];

	file->writeLE<int32_t>(pObject->_otype);
	file->writeLE<int32_t>(pObject->position.x);
	file->writeLE<int32_t>(pObject->position.y);
	file->writeLE<uint32_t>(pObject->_oLight);
	file->writeLE<uint32_t>(pObject->_oAnimFlag);
	file->skip(4); // Skip pointer _oAnimData
	file->writeLE<int32_t>(pObject->_oAnimDelay);
	file->writeLE<int32_t>(pObject->_oAnimCnt);
	file->writeLE<uint32_t>(pObject->_oAnimLen);
	file->writeLE<uint32_t>(pObject->_oAnimFrame);
	file->writeLE<int32_t>(pObject->_oAnimWidth);
	file->writeLE<int32_t>(CalculateWidth2(pObject->_oAnimWidth));
	// Write _oAnimWidth2 for vanilla compatibility
	file->writeLE<uint32_t>(pObject->_oDelFlag);
	file->writeLE<int8_t>(pObject->_oBreak);
	file->skip(3); // Alignment
	file->writeLE<uint32_t>(pObject->_oSolidFlag);
	file->writeLE<uint32_t>(pObject->_oMissFlag);

	file->writeLE<int8_t>(pObject->_oSelFlag);
	file->skip(3); // Alignment
	file->writeLE<uint32_t>(pObject->_oPreFlag);
	file->writeLE<uint32_t>(pObject->_oTrapFlag);
	file->writeLE<uint32_t>(pObject->_oDoorFlag);
	file->writeLE<int32_t>(pObject->_olid);
	file->writeLE<int32_t>(pObject->_oRndSeed);
	file->writeLE<int32_t>(pObject->_oVar1);
	file->writeLE<int32_t>(pObject->_oVar2);
	file->writeLE<int32_t>(pObject->_oVar3);
	file->writeLE<int32_t>(pObject->_oVar4);
	file->writeLE<int32_t>(pObject->_oVar5);
	file->writeLE<uint32_t>(pObject->_oVar6);
	file->writeLE<int32_t>(pObject->_oVar7);
	file->writeLE<int32_t>(pObject->_oVar8);
}

static void SavePremium(SaveHelper *file, int i)
{
	SaveItem(file, &premiumitems[i]);
}

static void SaveQuest(SaveHelper *file, int i)
{
	QuestStruct *pQuest = &quests[i];

	file->writeLE<uint8_t>(pQuest->_qlevel);
	file->writeLE<uint8_t>(pQuest->_qtype);
	file->writeLE<uint8_t>(pQuest->_qactive);
	file->writeLE<uint8_t>(pQuest->_qlvltype);
	file->writeLE<int32_t>(pQuest->position.x);
	file->writeLE<int32_t>(pQuest->position.y);
	file->writeLE<uint8_t>(pQuest->_qslvl);
	file->writeLE<uint8_t>(pQuest->_qidx);
	if (gbIsHellfire) {
		file->skip(2); // Alignment
		file->writeLE<int32_t>(pQuest->_qmsg);
	} else {
		file->writeLE<uint8_t>(pQuest->_qmsg);
	}
	file->writeLE<uint8_t>(pQuest->_qvar1);
	file->writeLE<uint8_t>(pQuest->_qvar2);
	file->skip(2); // Alignment
	if (!gbIsHellfire)
		file->skip(1); // Alignment
	file->writeLE<uint32_t>(pQuest->_qlog);

	file->writeBE<int32_t>(ReturnLvlX);
	file->writeBE<int32_t>(ReturnLvlY);
	file->writeBE<int32_t>(ReturnLvl);
	file->writeBE<int32_t>(ReturnLvlT);
	file->writeBE<int32_t>(DoomQuestState);
}

static void SaveLighting(SaveHelper *file, LightListStruct *pLight)
{
	file->writeLE<int32_t>(pLight->position.tile.x);
	file->writeLE<int32_t>(pLight->position.tile.y);
	file->writeLE<int32_t>(pLight->_lradius);
	file->writeLE<int32_t>(pLight->_lid);
	file->writeLE<uint32_t>(pLight->_ldel);
	file->writeLE<uint32_t>(pLight->_lunflag);
	file->skip(4); // Unused
	file->writeLE<int32_t>(pLight->position.old.x);
	file->writeLE<int32_t>(pLight->position.old.y);
	file->writeLE<int32_t>(pLight->oldRadious);
	file->writeLE<int32_t>(pLight->position.offset.x);
	file->writeLE<int32_t>(pLight->position.offset.y);
	file->writeLE<uint32_t>(pLight->_lflags);
}

static void SavePortal(SaveHelper *file, int i)
{
	PortalStruct *pPortal = &portal[i];

	file->writeLE<uint32_t>(pPortal->open);
	file->writeLE<int32_t>(pPortal->position.x);
	file->writeLE<int32_t>(pPortal->position.y);
	file->writeLE<int32_t>(pPortal->level);
	file->writeLE<int32_t>(pPortal->ltype);
	file->writeLE<uint32_t>(pPortal->setlvl);
}

const int DiabloItemSaveSize = 368;
const int HellfireItemSaveSize = 372;

void SaveHeroItems(PlayerStruct &player)
{
	size_t items = NUM_INVLOC + NUM_INV_GRID_ELEM + MAXBELTITEMS;
	SaveHelper file("heroitems", items * (gbIsHellfire ? HellfireItemSaveSize : DiabloItemSaveSize) + sizeof(uint8_t));

	file.writeLE<uint8_t>(gbIsHellfire);

	SaveItems(&file, player.InvBody, NUM_INVLOC);
	SaveItems(&file, player.InvList, NUM_INV_GRID_ELEM);
	SaveItems(&file, player.SpdList, MAXBELTITEMS);
}

// 256 kilobytes + 3 bytes (demo leftover) for file magic (262147)
// final game uses 4-byte magic instead of 3
#define FILEBUFF ((256 * 1024) + 3)

void SaveGameData()
{
	SaveHelper file("game", FILEBUFF);

	if (gbIsSpawn && !gbIsHellfire)
		file.writeLE<uint32_t>(LoadLE32("SHAR"));
	else if (gbIsSpawn && gbIsHellfire)
		file.writeLE<uint32_t>(LoadLE32("SHLF"));
	else if (!gbIsSpawn && gbIsHellfire)
		file.writeLE<uint32_t>(LoadLE32("HELF"));
	else if (!gbIsSpawn && !gbIsHellfire)
		file.writeLE<uint32_t>(LoadLE32("RETL"));
	else
		app_fatal("%s", _("Invalid game state"));

	if (gbIsHellfire) {
		giNumberOfLevels = 25;
		giNumberQuests = 24;
		giNumberOfSmithPremiumItems = 15;
	} else {
		giNumberOfLevels = 17;
		giNumberQuests = 16;
		giNumberOfSmithPremiumItems = 6;
	}

	file.writeLE<uint8_t>(setlevel);
	file.writeBE<uint32_t>(setlvlnum);
	file.writeBE<uint32_t>(currlevel);
	file.writeBE<uint32_t>(leveltype);
	file.writeBE<int32_t>(ViewX);
	file.writeBE<int32_t>(ViewY);
	file.writeLE<uint8_t>(invflag);
	file.writeLE<uint8_t>(chrflag);
	file.writeBE<int32_t>(nummonsters);
	file.writeBE<int32_t>(numitems);
	file.writeBE<int32_t>(nummissiles);
	file.writeBE<int32_t>(nobjects);

	for (uint8_t i = 0; i < giNumberOfLevels; i++) {
		file.writeBE<uint32_t>(glSeedTbl[i]);
		file.writeBE<int32_t>(gnLevelTypeTbl[i]);
	}

	plr[myplr].pDifficulty = sgGameInitInfo.nDifficulty;
	SavePlayer(&file, myplr);

	for (int i = 0; i < giNumberQuests; i++)
		SaveQuest(&file, i);
	for (int i = 0; i < MAXPORTAL; i++)
		SavePortal(&file, i);
	for (int monstkill : monstkills)
		file.writeBE<int32_t>(monstkill);

	if (leveltype != DTYPE_TOWN) {
		for (int monsterId : monstactive)
			file.writeBE<int32_t>(monsterId);
		for (int i = 0; i < nummonsters; i++)
			SaveMonster(&file, monstactive[i]);
		for (int missileId : missileactive)
			file.writeLE<int8_t>(missileId);
		for (int missileId : missileavail)
			file.writeLE<int8_t>(missileId);
		for (int i = 0; i < nummissiles; i++)
			SaveMissile(&file, missileactive[i]);
		for (int objectId : objectactive)
			file.writeLE<int8_t>(objectId);
		for (int objectId : objectavail)
			file.writeLE<int8_t>(objectId);
		for (int i = 0; i < nobjects; i++)
			SaveObject(&file, objectactive[i]);

		file.writeBE<int32_t>(numlights);

		for (uint8_t lightId : lightactive)
			file.writeLE<uint8_t>(lightId);
		for (int i = 0; i < numlights; i++)
			SaveLighting(&file, &LightList[lightactive[i]]);

		file.writeBE<int32_t>(visionid);
		file.writeBE<int32_t>(numvision);

		for (int i = 0; i < numvision; i++)
			SaveLighting(&file, &VisionList[i]);
	}

	for (int itemId : itemactive)
		file.writeLE<int8_t>(itemId);
	for (int itemId : itemavail)
		file.writeLE<int8_t>(itemId);
	for (int i = 0; i < numitems; i++)
		SaveItem(&file, &items[itemactive[i]]);
	for (bool UniqueItemFlag : UniqueItemFlags)
		file.writeLE<int8_t>(UniqueItemFlag);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<int8_t>(dLight[i][j]);
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<int8_t>(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<int8_t>(dPlayer[i][j]);
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<int8_t>(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeBE<int32_t>(dMonster[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dDead[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dObject[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dLight[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dPreLight[i][j]);
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				file.writeLE<uint8_t>(AutomapView[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dMissile[i][j]);
		}
	}

	file.writeBE<int32_t>(numpremium);
	file.writeBE<int32_t>(premiumlevel);

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		SavePremium(&file, i);

	file.writeLE<uint8_t>(AutomapActive);
	file.writeBE<int32_t>(AutoMapScale);
}

void SaveGame()
{
	gbValidSaveFile = true;
	pfile_write_hero(/*write_game_data=*/true);
}

void SaveLevel()
{
	PFileScopedArchiveWriter scoped_writer;

	DoUnVision(plr[myplr].position.tile, plr[myplr]._pLightRad); // fix for vision staying on the level

	if (currlevel == 0)
		glSeedTbl[0] = AdvanceRndSeed();

	char szName[MAX_PATH];
	GetTempLevelNames(szName);
	SaveHelper file(szName, FILEBUFF);

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dDead[i][j]);
		}
	}

	file.writeBE<int32_t>(nummonsters);
	file.writeBE<int32_t>(numitems);
	file.writeBE<int32_t>(nobjects);

	if (leveltype != DTYPE_TOWN) {
		for (int monsterId : monstactive)
			file.writeBE<int32_t>(monsterId);
		for (int i = 0; i < nummonsters; i++)
			SaveMonster(&file, monstactive[i]);
		for (int objectId : objectactive)
			file.writeLE<int8_t>(objectId);
		for (int objectId : objectavail)
			file.writeLE<int8_t>(objectId);
		for (int i = 0; i < nobjects; i++)
			SaveObject(&file, objectactive[i]);
	}

	for (int itemId : itemactive)
		file.writeLE<int8_t>(itemId);
	for (int itemId : itemavail)
		file.writeLE<int8_t>(itemId);

	for (int i = 0; i < numitems; i++)
		SaveItem(&file, &items[itemactive[i]]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<int8_t>(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<int8_t>(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeBE<int32_t>(dMonster[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dObject[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dLight[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dPreLight[i][j]);
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				file.writeLE<uint8_t>(AutomapView[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<int8_t>(dMissile[i][j]);
		}
	}

	if (!setlevel)
		plr[myplr]._pLvlVisited[currlevel] = true;
	else
		plr[myplr]._pSLvlVisited[setlvlnum] = true;
}

void LoadLevel()
{
	char szName[MAX_PATH];
	GetPermLevelNames(szName);
	LoadHelper file(szName);
	if (!file.isValid())
		app_fatal("%s", _("Unable to open save file archive"));

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dDead[i][j] = file.nextLE<int8_t>();
		}
		SetDead();
	}

	nummonsters = file.nextBE<int32_t>();
	numitems = file.nextBE<int32_t>();
	nobjects = file.nextBE<int32_t>();

	if (leveltype != DTYPE_TOWN) {
		for (int &monsterId : monstactive)
			monsterId = file.nextBE<int32_t>();
		for (int i = 0; i < nummonsters; i++)
			LoadMonster(&file, monstactive[i]);
		for (int &objectId : objectactive)
			objectId = file.nextLE<int8_t>();
		for (int &objectId : objectavail)
			objectId = file.nextLE<int8_t>();
		for (int i = 0; i < nobjects; i++)
			LoadObject(&file, objectactive[i]);
		if (!gbSkipSync) {
			for (int i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
		}
	}

	for (int &itemId : itemactive)
		itemId = file.nextLE<int8_t>();
	for (int &itemId : itemavail)
		itemId = file.nextLE<int8_t>();
	for (int i = 0; i < numitems; i++)
		LoadItem(&file, itemactive[i]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dFlags[i][j] = file.nextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dItem[i][j] = file.nextLE<int8_t>();
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMonster[i][j] = file.nextBE<int32_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dObject[i][j] = file.nextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dLight[i][j] = file.nextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dPreLight[i][j] = file.nextLE<int8_t>();
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				AutomapView[i][j] = file.nextBool8();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMissile[i][j] = 0; /// BUGFIX: supposed to load saved missiles with "file.nextLE<int8_t>()"?
		}
	}

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		RemoveEmptyLevelItems();
	}

	if (!gbSkipSync) {
		AutomapZoomReset();
		ResyncQuests();
		SyncPortals();
		dolighting = true;
	}

	for (auto &player : plr) {
		if (player.plractive && currlevel == player.plrlevel)
			LightList[player._plid]._lunflag = true;
	}
}

} // namespace devilution

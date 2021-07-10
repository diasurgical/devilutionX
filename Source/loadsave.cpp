/**
 * @file loadsave.cpp
 *
 * Implementation of save game functionality.
 */
#include "loadsave.h"

#include <climits>
#include <cstring>

#include <SDL.h>

#include "automap.h"
#include "codec.h"
#include "control.h"
#include "cursor.h"
#include "dead.h"
#include "doom.h"
#include "engine.h"
#include "engine/point.hpp"
#include "engine/random.hpp"
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
	std::unique_ptr<byte[]> m_buffer_;
	size_t m_cur_ = 0;
	size_t m_size_;

	template <class T>
	T Next()
	{
		const auto size = sizeof(T);
		if (!IsValid(size))
			return 0;

		T value;
		memcpy(&value, &m_buffer_[m_cur_], size);
		m_cur_ += size;

		return value;
	}

public:
	LoadHelper(const char *szFileName)
	{
		m_buffer_ = pfile_read(szFileName, &m_size_);
	}

	bool IsValid(size_t size = 1)
	{
		return m_buffer_ != nullptr
		    && m_size_ >= (m_cur_ + size);
	}

	template <typename T>
	constexpr void Skip()
	{
		Skip(sizeof(T));
	}

	void Skip(size_t size)
	{
		m_cur_ += size;
	}

	void NextBytes(void *bytes, size_t size)
	{
		if (!IsValid(size))
			return;

		memcpy(bytes, &m_buffer_[m_cur_], size);
		m_cur_ += size;
	}

	template <class T>
	T NextLE()
	{
		return SwapLE(Next<T>());
	}

	template <class T>
	T NextBE()
	{
		return SwapBE(Next<T>());
	}

	bool NextBool8()
	{
		return Next<uint8_t>() != 0;
	}

	bool NextBool32()
	{
		return Next<uint32_t>() != 0;
	}
};

class SaveHelper {
	const char *m_szFileName_;
	std::unique_ptr<byte[]> m_buffer_;
	size_t m_cur_ = 0;
	size_t m_capacity_;

public:
	SaveHelper(const char *szFileName, size_t bufferLen)
	    : m_szFileName_(szFileName)
	    , m_buffer_(new byte[codec_get_encoded_len(bufferLen)])
	    , m_capacity_(bufferLen)
	{
	}

	bool IsValid(size_t len = 1)
	{
		return m_buffer_ != nullptr
		    && m_capacity_ >= (m_cur_ + len);
	}

	template <typename T>
	constexpr void Skip()
	{
		Skip(sizeof(T));
	}

	void Skip(size_t len)
	{
		std::memset(&m_buffer_[m_cur_], 0, len);
		m_cur_ += len;
	}

	void WriteBytes(const void *bytes, size_t len)
	{
		if (!IsValid(len))
			return;

		memcpy(&m_buffer_[m_cur_], bytes, len);
		m_cur_ += len;
	}

	template <class T>
	void WriteLE(T value)
	{
		value = SwapLE(value);
		WriteBytes(&value, sizeof(value));
	}

	template <class T>
	void WriteBE(T value)
	{
		value = SwapBE(value);
		WriteBytes(&value, sizeof(value));
	}

	~SaveHelper()
	{
		const auto encodedLen = codec_get_encoded_len(m_cur_);
		const char *const password = pfile_get_password();
		codec_encode(m_buffer_.get(), m_cur_, encodedLen, password);
		mpqapi_write_file(m_szFileName_, m_buffer_.get(), encodedLen);
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
	pItem->_iSeed = file->NextLE<int32_t>();
	pItem->_iCreateInfo = file->NextLE<uint16_t>();
	file->Skip(2); // Alignment
	pItem->_itype = static_cast<item_type>(file->NextLE<uint32_t>());
	pItem->position.x = file->NextLE<int32_t>();
	pItem->position.y = file->NextLE<int32_t>();
	pItem->_iAnimFlag = file->NextBool32();
	file->Skip(4); // Skip pointer _iAnimData
	pItem->AnimInfo = {};
	pItem->AnimInfo.NumberOfFrames = file->NextLE<int32_t>();
	pItem->AnimInfo.CurrentFrame = file->NextLE<int32_t>();
	file->Skip(8); // Skip _iAnimWidth and _iAnimWidth2
	file->Skip(4); // Unused since 1.02
	pItem->_iSelFlag = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	pItem->_iPostDraw = file->NextBool32();
	pItem->_iIdentified = file->NextBool32();
	pItem->_iMagical = static_cast<item_quality>(file->NextLE<int8_t>());
	file->NextBytes(pItem->_iName, 64);
	file->NextBytes(pItem->_iIName, 64);
	pItem->_iLoc = static_cast<item_equip_type>(file->NextLE<int8_t>());
	pItem->_iClass = static_cast<item_class>(file->NextLE<uint8_t>());
	file->Skip(1); // Alignment
	pItem->_iCurs = file->NextLE<int32_t>();
	pItem->_ivalue = file->NextLE<int32_t>();
	pItem->_iIvalue = file->NextLE<int32_t>();
	pItem->_iMinDam = file->NextLE<int32_t>();
	pItem->_iMaxDam = file->NextLE<int32_t>();
	pItem->_iAC = file->NextLE<int32_t>();
	pItem->_iFlags = file->NextLE<uint32_t>();
	pItem->_iMiscId = static_cast<item_misc_id>(file->NextLE<int32_t>());
	pItem->_iSpell = static_cast<spell_id>(file->NextLE<int32_t>());
	pItem->_iCharges = file->NextLE<int32_t>();
	pItem->_iMaxCharges = file->NextLE<int32_t>();
	pItem->_iDurability = file->NextLE<int32_t>();
	pItem->_iMaxDur = file->NextLE<int32_t>();
	pItem->_iPLDam = file->NextLE<int32_t>();
	pItem->_iPLToHit = file->NextLE<int32_t>();
	pItem->_iPLAC = file->NextLE<int32_t>();
	pItem->_iPLStr = file->NextLE<int32_t>();
	pItem->_iPLMag = file->NextLE<int32_t>();
	pItem->_iPLDex = file->NextLE<int32_t>();
	pItem->_iPLVit = file->NextLE<int32_t>();
	pItem->_iPLFR = file->NextLE<int32_t>();
	pItem->_iPLLR = file->NextLE<int32_t>();
	pItem->_iPLMR = file->NextLE<int32_t>();
	pItem->_iPLMana = file->NextLE<int32_t>();
	pItem->_iPLHP = file->NextLE<int32_t>();
	pItem->_iPLDamMod = file->NextLE<int32_t>();
	pItem->_iPLGetHit = file->NextLE<int32_t>();
	pItem->_iPLLight = file->NextLE<int32_t>();
	pItem->_iSplLvlAdd = file->NextLE<int8_t>();
	pItem->_iRequest = file->NextBool8();
	file->Skip(2); // Alignment
	pItem->_iUid = file->NextLE<int32_t>();
	pItem->_iFMinDam = file->NextLE<int32_t>();
	pItem->_iFMaxDam = file->NextLE<int32_t>();
	pItem->_iLMinDam = file->NextLE<int32_t>();
	pItem->_iLMaxDam = file->NextLE<int32_t>();
	pItem->_iPLEnAc = file->NextLE<int32_t>();
	pItem->_iPrePower = static_cast<item_effect_type>(file->NextLE<int8_t>());
	pItem->_iSufPower = static_cast<item_effect_type>(file->NextLE<int8_t>());
	file->Skip(2); // Alignment
	pItem->_iVAdd1 = file->NextLE<int32_t>();
	pItem->_iVMult1 = file->NextLE<int32_t>();
	pItem->_iVAdd2 = file->NextLE<int32_t>();
	pItem->_iVMult2 = file->NextLE<int32_t>();
	pItem->_iMinStr = file->NextLE<int8_t>();
	pItem->_iMinMag = file->NextLE<uint8_t>();
	pItem->_iMinDex = file->NextLE<int8_t>();
	file->Skip(1); // Alignment
	pItem->_iStatFlag = file->NextBool32();
	pItem->IDidx = static_cast<_item_indexes>(file->NextLE<int32_t>());
	if (gbIsSpawn) {
		pItem->IDidx = RemapItemIdxFromSpawn(pItem->IDidx);
	}
	if (!gbIsHellfireSaveGame) {
		pItem->IDidx = RemapItemIdxFromDiablo(pItem->IDidx);
	}
	pItem->dwBuff = file->NextLE<uint32_t>();
	if (gbIsHellfireSaveGame)
		pItem->_iDamAcFlags = file->NextLE<uint32_t>();
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
	auto &player = Players[p];

	player._pmode = static_cast<PLR_MODE>(file->NextLE<int32_t>());

	for (int8_t &step : player.walkpath) {
		step = file->NextLE<int8_t>();
	}
	player.plractive = file->NextBool8();
	file->Skip(2); // Alignment
	player.destAction = static_cast<action_id>(file->NextLE<int32_t>());
	player.destParam1 = file->NextLE<int32_t>();
	player.destParam2 = file->NextLE<int32_t>();
	player.destParam3 = static_cast<Direction>(file->NextLE<int32_t>());
	player.destParam4 = file->NextLE<int32_t>();
	player.plrlevel = file->NextLE<int32_t>();
	player.position.tile.x = file->NextLE<int32_t>();
	player.position.tile.y = file->NextLE<int32_t>();
	player.position.future.x = file->NextLE<int32_t>();
	player.position.future.y = file->NextLE<int32_t>();
	file->Skip(8); // Skip _ptargx and _ptargy
	player.position.last.x = file->NextLE<int32_t>();
	player.position.last.y = file->NextLE<int32_t>();
	player.position.old.x = file->NextLE<int32_t>();
	player.position.old.y = file->NextLE<int32_t>();
	player.position.offset.deltaX = file->NextLE<int32_t>();
	player.position.offset.deltaY = file->NextLE<int32_t>();
	player.position.velocity.deltaX = file->NextLE<int32_t>();
	player.position.velocity.deltaY = file->NextLE<int32_t>();
	player._pdir = static_cast<Direction>(file->NextLE<int32_t>());
	file->Skip(4); // Unused
	player._pgfxnum = file->NextLE<int32_t>();
	file->Skip(4); // Skip pointer pData
	player.AnimInfo = {};
	player.AnimInfo.TicksPerFrame = file->NextLE<int32_t>() + 1;
	player.AnimInfo.TickCounterOfCurrentFrame = file->NextLE<int32_t>();
	player.AnimInfo.NumberOfFrames = file->NextLE<int32_t>();
	player.AnimInfo.CurrentFrame = file->NextLE<int32_t>();
	file->Skip(4); // Skip _pAnimWidth
	file->Skip(4); // Skip _pAnimWidth2
	file->Skip(4); // Skip _peflag
	player._plid = file->NextLE<int32_t>();
	player._pvid = file->NextLE<int32_t>();

	player._pSpell = static_cast<spell_id>(file->NextLE<int32_t>());
	player._pSplType = static_cast<spell_type>(file->NextLE<int8_t>());
	player._pSplFrom = file->NextLE<int8_t>();
	file->Skip(2); // Alignment
	player._pTSpell = static_cast<spell_id>(file->NextLE<int32_t>());
	file->Skip<int8_t>(); // Skip _pTSplType
	file->Skip(3);        // Alignment
	player._pRSpell = static_cast<spell_id>(file->NextLE<int32_t>());
	player._pRSplType = static_cast<spell_type>(file->NextLE<int8_t>());
	file->Skip(3); // Alignment
	player._pSBkSpell = static_cast<spell_id>(file->NextLE<int32_t>());
	file->Skip<int8_t>(); // Skip _pSBkSplType
	for (int8_t &spellLevel : player._pSplLvl)
		spellLevel = file->NextLE<int8_t>();
	file->Skip(7); // Alignment
	player._pMemSpells = file->NextLE<uint64_t>();
	player._pAblSpells = file->NextLE<uint64_t>();
	player._pScrlSpells = file->NextLE<uint64_t>();
	player._pSpellFlags = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	for (auto &spell : player._pSplHotKey)
		spell = static_cast<spell_id>(file->NextLE<int32_t>());
	for (auto &spellType : player._pSplTHotKey)
		spellType = static_cast<spell_type>(file->NextLE<int8_t>());

	player._pwtype = static_cast<player_weapon_type>(file->NextLE<int32_t>());
	player._pBlockFlag = file->NextBool8();
	player._pInvincible = file->NextBool8();
	player._pLightRad = file->NextLE<int8_t>();
	player._pLvlChanging = file->NextBool8();

	file->NextBytes(player._pName, PLR_NAME_LEN);
	player._pClass = static_cast<HeroClass>(file->NextLE<int8_t>());
	file->Skip(3); // Alignment
	player._pStrength = file->NextLE<int32_t>();
	player._pBaseStr = file->NextLE<int32_t>();
	player._pMagic = file->NextLE<int32_t>();
	player._pBaseMag = file->NextLE<int32_t>();
	player._pDexterity = file->NextLE<int32_t>();
	player._pBaseDex = file->NextLE<int32_t>();
	player._pVitality = file->NextLE<int32_t>();
	player._pBaseVit = file->NextLE<int32_t>();
	player._pStatPts = file->NextLE<int32_t>();
	player._pDamageMod = file->NextLE<int32_t>();
	player._pBaseToBlk = file->NextLE<int32_t>();
	if (player._pBaseToBlk == 0)
		player._pBaseToBlk = BlockBonuses[static_cast<std::size_t>(player._pClass)];
	player._pHPBase = file->NextLE<int32_t>();
	player._pMaxHPBase = file->NextLE<int32_t>();
	player._pHitPoints = file->NextLE<int32_t>();
	player._pMaxHP = file->NextLE<int32_t>();
	file->Skip(sizeof(int32_t)); // Skip _pHPPer - always derived from hp and maxHP.
	player._pManaBase = file->NextLE<int32_t>();
	player._pMaxManaBase = file->NextLE<int32_t>();
	player._pMana = file->NextLE<int32_t>();
	player._pMaxMana = file->NextLE<int32_t>();
	file->Skip(sizeof(int32_t)); // Skip _pManaPer - always derived from mana and maxMana
	player._pLevel = file->NextLE<int8_t>();
	player._pMaxLvl = file->NextLE<int8_t>();
	file->Skip(2); // Alignment
	player._pExperience = file->NextLE<int32_t>();
	player._pMaxExp = file->NextLE<int32_t>();
	player._pNextExper = file->NextLE<int32_t>();
	player._pArmorClass = file->NextLE<int8_t>();
	player._pMagResist = file->NextLE<int8_t>();
	player._pFireResist = file->NextLE<int8_t>();
	player._pLghtResist = file->NextLE<int8_t>();
	player._pGold = file->NextLE<int32_t>();

	player._pInfraFlag = file->NextBool32();
	player.position.temp.x = file->NextLE<int32_t>();
	player.position.temp.y = file->NextLE<int32_t>();
	player.tempDirection = static_cast<Direction>(file->NextLE<int32_t>());
	player._pVar4 = file->NextLE<int32_t>();
	player._pVar5 = file->NextLE<int32_t>();
	player.position.offset2.deltaX = file->NextLE<int32_t>();
	player.position.offset2.deltaY = file->NextLE<int32_t>();
	file->Skip(4); // Skip actionFrame
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		player._pLvlVisited[i] = file->NextBool8();
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		player._pSLvlVisited[i] = file->NextBool8();

	file->Skip(2);     // Alignment
	file->Skip(4);     // skip _pGFXLoad
	file->Skip(4 * 8); // Skip pointers _pNAnim
	player._pNFrames = file->NextLE<int32_t>();
	file->Skip(4);     // skip _pNWidth
	file->Skip(4 * 8); // Skip pointers _pWAnim
	player._pWFrames = file->NextLE<int32_t>();
	file->Skip(4);     // skip _pWWidth
	file->Skip(4 * 8); // Skip pointers _pAAnim
	player._pAFrames = file->NextLE<int32_t>();
	file->Skip(4); // skip _pAWidth
	player._pAFNum = file->NextLE<int32_t>();
	file->Skip(4 * 8); // Skip pointers _pLAnim
	file->Skip(4 * 8); // Skip pointers _pFAnim
	file->Skip(4 * 8); // Skip pointers _pTAnim
	player._pSFrames = file->NextLE<int32_t>();
	file->Skip(4); // skip _pSWidth
	player._pSFNum = file->NextLE<int32_t>();
	file->Skip(4 * 8); // Skip pointers _pHAnim
	player._pHFrames = file->NextLE<int32_t>();
	file->Skip(4);     // skip _pHWidth
	file->Skip(4 * 8); // Skip pointers _pDAnim
	player._pDFrames = file->NextLE<int32_t>();
	file->Skip(4);     // skip _pDWidth
	file->Skip(4 * 8); // Skip pointers _pBAnim
	player._pBFrames = file->NextLE<int32_t>();
	file->Skip(4); // skip _pBWidth

	LoadItems(file, NUM_INVLOC, player.InvBody);
	LoadItems(file, NUM_INV_GRID_ELEM, player.InvList);
	player._pNumInv = file->NextLE<int32_t>();
	for (int8_t &cell : player.InvGrid)
		cell = file->NextLE<int8_t>();
	LoadItems(file, MAXBELTITEMS, player.SpdList);
	LoadItemData(file, &player.HoldItem);

	player._pIMinDam = file->NextLE<int32_t>();
	player._pIMaxDam = file->NextLE<int32_t>();
	player._pIAC = file->NextLE<int32_t>();
	player._pIBonusDam = file->NextLE<int32_t>();
	player._pIBonusToHit = file->NextLE<int32_t>();
	player._pIBonusAC = file->NextLE<int32_t>();
	player._pIBonusDamMod = file->NextLE<int32_t>();
	file->Skip(4); // Alignment

	player._pISpells = file->NextLE<uint64_t>();
	player._pIFlags = file->NextLE<int32_t>();
	player._pIGetHit = file->NextLE<int32_t>();
	player._pISplLvlAdd = file->NextLE<int8_t>();
	file->Skip(1); // Unused
	file->Skip(2); // Alignment
	player._pISplDur = file->NextLE<int32_t>();
	player._pIEnAc = file->NextLE<int32_t>();
	player._pIFMinDam = file->NextLE<int32_t>();
	player._pIFMaxDam = file->NextLE<int32_t>();
	player._pILMinDam = file->NextLE<int32_t>();
	player._pILMaxDam = file->NextLE<int32_t>();
	player._pOilType = static_cast<item_misc_id>(file->NextLE<int32_t>());
	player.pTownWarps = file->NextLE<uint8_t>();
	player.pDungMsgs = file->NextLE<uint8_t>();
	player.pLvlLoad = file->NextLE<uint8_t>();

	if (gbIsHellfireSaveGame) {
		player.pDungMsgs2 = file->NextLE<uint8_t>();
		player.pBattleNet = false;
	} else {
		player.pDungMsgs2 = 0;
		player.pBattleNet = file->NextBool8();
	}
	player.pManaShield = file->NextBool8();
	if (gbIsHellfireSaveGame) {
		player.pOriginalCathedral = file->NextBool8();
	} else {
		file->Skip(1);
		player.pOriginalCathedral = true;
	}
	file->Skip(2); // Available bytes
	player.wReflections = file->NextLE<uint16_t>();
	file->Skip(14); // Available bytes

	player.pDiabloKillLevel = file->NextLE<uint32_t>();
	player.pDifficulty = static_cast<_difficulty>(file->NextLE<uint32_t>());
	player.pDamAcFlags = file->NextLE<uint32_t>();
	file->Skip(20); // Available bytes
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
	MonsterStruct *pMonster = &Monsters[i];

	pMonster->_mMTidx = file->NextLE<int32_t>();
	pMonster->_mmode = static_cast<MON_MODE>(file->NextLE<int32_t>());
	pMonster->_mgoal = static_cast<monster_goal>(file->NextLE<uint8_t>());
	file->Skip(3); // Alignment
	pMonster->_mgoalvar1 = file->NextLE<int32_t>();
	pMonster->_mgoalvar2 = file->NextLE<int32_t>();
	pMonster->_mgoalvar3 = file->NextLE<int32_t>();
	file->Skip(4); // Unused
	pMonster->_pathcount = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	pMonster->position.tile.x = file->NextLE<int32_t>();
	pMonster->position.tile.y = file->NextLE<int32_t>();
	pMonster->position.future.x = file->NextLE<int32_t>();
	pMonster->position.future.y = file->NextLE<int32_t>();
	pMonster->position.old.x = file->NextLE<int32_t>();
	pMonster->position.old.y = file->NextLE<int32_t>();
	pMonster->position.offset.deltaX = file->NextLE<int32_t>();
	pMonster->position.offset.deltaY = file->NextLE<int32_t>();
	pMonster->position.velocity.deltaX = file->NextLE<int32_t>();
	pMonster->position.velocity.deltaY = file->NextLE<int32_t>();
	pMonster->_mdir = static_cast<Direction>(file->NextLE<int32_t>());
	pMonster->_menemy = file->NextLE<int32_t>();
	pMonster->enemyPosition.x = file->NextLE<uint8_t>();
	pMonster->enemyPosition.y = file->NextLE<uint8_t>();
	file->Skip(2); // Unused

	file->Skip(4); // Skip pointer _mAnimData
	pMonster->AnimInfo = {};
	pMonster->AnimInfo.TicksPerFrame = file->NextLE<int32_t>();
	pMonster->AnimInfo.TickCounterOfCurrentFrame = file->NextLE<int32_t>();
	pMonster->AnimInfo.NumberOfFrames = file->NextLE<int32_t>();
	pMonster->AnimInfo.CurrentFrame = file->NextLE<int32_t>();
	file->Skip(4); // Skip _meflag
	pMonster->_mDelFlag = file->NextBool32();
	pMonster->_mVar1 = file->NextLE<int32_t>();
	pMonster->_mVar2 = file->NextLE<int32_t>();
	pMonster->_mVar3 = file->NextLE<int32_t>();
	pMonster->position.temp.x = file->NextLE<int32_t>();
	pMonster->position.temp.y = file->NextLE<int32_t>();
	pMonster->position.offset2.deltaX = file->NextLE<int32_t>();
	pMonster->position.offset2.deltaY = file->NextLE<int32_t>();
	file->Skip(4); // Skip actionFrame
	pMonster->_mmaxhp = file->NextLE<int32_t>();
	pMonster->_mhitpoints = file->NextLE<int32_t>();

	pMonster->_mAi = static_cast<_mai_id>(file->NextLE<uint8_t>());
	pMonster->_mint = file->NextLE<uint8_t>();
	file->Skip(2); // Alignment
	pMonster->_mFlags = file->NextLE<uint32_t>();
	pMonster->_msquelch = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	file->Skip(4); // Unused
	pMonster->position.last.x = file->NextLE<int32_t>();
	pMonster->position.last.y = file->NextLE<int32_t>();
	pMonster->_mRndSeed = file->NextLE<uint32_t>();
	pMonster->_mAISeed = file->NextLE<uint32_t>();
	file->Skip(4); // Unused

	pMonster->_uniqtype = file->NextLE<uint8_t>();
	pMonster->_uniqtrans = file->NextLE<uint8_t>();
	pMonster->_udeadval = file->NextLE<int8_t>();

	pMonster->mWhoHit = file->NextLE<int8_t>();
	pMonster->mLevel = file->NextLE<int8_t>();
	file->Skip(1); // Alignment
	pMonster->mExp = file->NextLE<uint16_t>();

	if (i < MAX_PLRS) // Don't skip for golems
		pMonster->mHit = file->NextLE<uint8_t>();
	else
		file->Skip(1); // Skip mHit as it's already initialized
	pMonster->mMinDamage = file->NextLE<uint8_t>();
	pMonster->mMaxDamage = file->NextLE<uint8_t>();
	file->Skip(1); // Skip mHit2 as it's already initialized
	pMonster->mMinDamage2 = file->NextLE<uint8_t>();
	pMonster->mMaxDamage2 = file->NextLE<uint8_t>();
	pMonster->mArmorClass = file->NextLE<uint8_t>();
	file->Skip(1); // Alignment
	pMonster->mMagicRes = file->NextLE<uint16_t>();
	file->Skip(2); // Alignment

	pMonster->mtalkmsg = static_cast<_speech_id>(file->NextLE<int32_t>());
	if (pMonster->mtalkmsg == TEXT_KING1) // Fix original bad mapping of NONE for monsters
		pMonster->mtalkmsg = TEXT_NONE;
	pMonster->leader = file->NextLE<uint8_t>();
	pMonster->leaderflag = file->NextLE<uint8_t>();
	pMonster->packsize = file->NextLE<uint8_t>();
	pMonster->mlid = file->NextLE<int8_t>();
	if (pMonster->mlid == Players[MyPlayerId]._plid)
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
	MissileStruct *pMissile = &Missiles[i];

	pMissile->_mitype = file->NextLE<int32_t>();
	pMissile->position.tile.x = file->NextLE<int32_t>();
	pMissile->position.tile.y = file->NextLE<int32_t>();
	pMissile->position.offset.deltaX = file->NextLE<int32_t>();
	pMissile->position.offset.deltaY = file->NextLE<int32_t>();
	pMissile->position.velocity.deltaX = file->NextLE<int32_t>();
	pMissile->position.velocity.deltaY = file->NextLE<int32_t>();
	pMissile->position.start.x = file->NextLE<int32_t>();
	pMissile->position.start.y = file->NextLE<int32_t>();
	pMissile->position.traveled.deltaX = file->NextLE<int32_t>();
	pMissile->position.traveled.deltaY = file->NextLE<int32_t>();
	pMissile->_mimfnum = file->NextLE<int32_t>();
	pMissile->_mispllvl = file->NextLE<int32_t>();
	pMissile->_miDelFlag = file->NextBool32();
	pMissile->_miAnimType = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	pMissile->_miAnimFlags = file->NextLE<int32_t>();
	file->Skip(4); // Skip pointer _miAnimData
	pMissile->_miAnimDelay = file->NextLE<int32_t>();
	pMissile->_miAnimLen = file->NextLE<int32_t>();
	pMissile->_miAnimWidth = file->NextLE<int32_t>();
	pMissile->_miAnimWidth2 = file->NextLE<int32_t>();
	pMissile->_miAnimCnt = file->NextLE<int32_t>();
	pMissile->_miAnimAdd = file->NextLE<int32_t>();
	pMissile->_miAnimFrame = file->NextLE<int32_t>();
	pMissile->_miDrawFlag = file->NextBool32();
	pMissile->_miLightFlag = file->NextBool32();
	pMissile->_miPreFlag = file->NextBool32();
	pMissile->_miUniqTrans = file->NextLE<uint32_t>();
	pMissile->_mirange = file->NextLE<int32_t>();
	pMissile->_misource = file->NextLE<int32_t>();
	pMissile->_micaster = file->NextLE<int32_t>();
	pMissile->_midam = file->NextLE<int32_t>();
	pMissile->_miHitFlag = file->NextBool32();
	pMissile->_midist = file->NextLE<int32_t>();
	pMissile->_mlid = file->NextLE<int32_t>();
	pMissile->_mirnd = file->NextLE<int32_t>();
	pMissile->_miVar1 = file->NextLE<int32_t>();
	pMissile->_miVar2 = file->NextLE<int32_t>();
	pMissile->_miVar3 = file->NextLE<int32_t>();
	pMissile->_miVar4 = file->NextLE<int32_t>();
	pMissile->_miVar5 = file->NextLE<int32_t>();
	pMissile->_miVar6 = file->NextLE<int32_t>();
	pMissile->_miVar7 = file->NextLE<int32_t>();
	pMissile->limitReached = file->NextBool32();
}

static void LoadObject(LoadHelper *file, int i)
{
	ObjectStruct *pObject = &Objects[i];

	pObject->_otype = static_cast<_object_id>(file->NextLE<int32_t>());
	pObject->position.x = file->NextLE<int32_t>();
	pObject->position.y = file->NextLE<int32_t>();
	pObject->_oLight = file->NextBool32();
	pObject->_oAnimFlag = file->NextLE<uint32_t>();
	file->Skip(4); // Skip pointer _oAnimData
	pObject->_oAnimDelay = file->NextLE<int32_t>();
	pObject->_oAnimCnt = file->NextLE<int32_t>();
	pObject->_oAnimLen = file->NextLE<uint32_t>();
	pObject->_oAnimFrame = file->NextLE<uint32_t>();
	pObject->_oAnimWidth = file->NextLE<int32_t>();
	file->Skip(4); // Skip _oAnimWidth2
	pObject->_oDelFlag = file->NextBool32();
	pObject->_oBreak = file->NextLE<int8_t>();
	file->Skip(3); // Alignment
	pObject->_oSolidFlag = file->NextBool32();
	pObject->_oMissFlag = file->NextBool32();

	pObject->_oSelFlag = file->NextLE<int8_t>();
	file->Skip(3); // Alignment
	pObject->_oPreFlag = file->NextBool32();
	pObject->_oTrapFlag = file->NextBool32();
	pObject->_oDoorFlag = file->NextBool32();
	pObject->_olid = file->NextLE<int32_t>();
	pObject->_oRndSeed = file->NextLE<uint32_t>();
	pObject->_oVar1 = file->NextLE<int32_t>();
	pObject->_oVar2 = file->NextLE<int32_t>();
	pObject->_oVar3 = file->NextLE<int32_t>();
	pObject->_oVar4 = file->NextLE<int32_t>();
	pObject->_oVar5 = file->NextLE<int32_t>();
	pObject->_oVar6 = file->NextLE<uint32_t>();
	pObject->_oVar7 = static_cast<_speech_id>(file->NextLE<int32_t>());
	pObject->_oVar8 = file->NextLE<int32_t>();
}

static void LoadItem(LoadHelper *file, int i)
{
	LoadItemData(file, &Items[i]);
	GetItemFrm(i);
}

static void LoadPremium(LoadHelper *file, int i)
{
	LoadItemData(file, &premiumitems[i]);
}

static void LoadQuest(LoadHelper *file, int i)
{
	QuestStruct *pQuest = &Quests[i];

	pQuest->_qlevel = file->NextLE<uint8_t>();
	pQuest->_qtype = file->NextLE<uint8_t>();
	pQuest->_qactive = static_cast<quest_state>(file->NextLE<uint8_t>());
	pQuest->_qlvltype = static_cast<dungeon_type>(file->NextLE<uint8_t>());
	pQuest->position.x = file->NextLE<int32_t>();
	pQuest->position.y = file->NextLE<int32_t>();
	pQuest->_qslvl = static_cast<_setlevels>(file->NextLE<uint8_t>());
	pQuest->_qidx = file->NextLE<uint8_t>();
	if (gbIsHellfireSaveGame) {
		file->Skip(2); // Alignment
		pQuest->_qmsg = static_cast<_speech_id>(file->NextLE<int32_t>());
	} else {
		pQuest->_qmsg = static_cast<_speech_id>(file->NextLE<uint8_t>());
	}
	pQuest->_qvar1 = file->NextLE<uint8_t>();
	pQuest->_qvar2 = file->NextLE<uint8_t>();
	file->Skip(2); // Alignment
	if (!gbIsHellfireSaveGame)
		file->Skip(1); // Alignment
	pQuest->_qlog = file->NextBool32();

	ReturnLvlX = file->NextBE<int32_t>();
	ReturnLvlY = file->NextBE<int32_t>();
	ReturnLevel = file->NextBE<int32_t>();
	ReturnLevelType = static_cast<dungeon_type>(file->NextBE<int32_t>());
	file->Skip(sizeof(int32_t)); // Skip DoomQuestState
}

static void LoadLighting(LoadHelper *file, LightStruct *pLight)
{
	pLight->position.tile.x = file->NextLE<int32_t>();
	pLight->position.tile.y = file->NextLE<int32_t>();
	pLight->_lradius = file->NextLE<int32_t>();
	pLight->_lid = file->NextLE<int32_t>();
	pLight->_ldel = file->NextBool32();
	pLight->_lunflag = file->NextBool32();
	file->Skip(4); // Unused
	pLight->position.old.x = file->NextLE<int32_t>();
	pLight->position.old.y = file->NextLE<int32_t>();
	pLight->oldRadius = file->NextLE<int32_t>();
	pLight->position.offset.x = file->NextLE<int32_t>();
	pLight->position.offset.y = file->NextLE<int32_t>();
	pLight->_lflags = file->NextBool32();
}

static void LoadPortal(LoadHelper *file, int i)
{
	PortalStruct *pPortal = &Portals[i];

	pPortal->open = file->NextBool32();
	pPortal->position.x = file->NextLE<int32_t>();
	pPortal->position.y = file->NextLE<int32_t>();
	pPortal->level = file->NextLE<int32_t>();
	pPortal->ltype = static_cast<dungeon_type>(file->NextLE<int32_t>());
	pPortal->setlvl = file->NextBool32();
}

_item_indexes RemapItemIdxFromDiablo(_item_indexes i)
{
	constexpr auto GetItemIdValue = [](int i) -> int {
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

	return static_cast<_item_indexes>(GetItemIdValue(i));
}

_item_indexes RemapItemIdxToDiablo(_item_indexes i)
{
	constexpr auto GetItemIdValue = [](int i) -> int {
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

	return static_cast<_item_indexes>(GetItemIdValue(i));
}

_item_indexes RemapItemIdxFromSpawn(_item_indexes i)
{
	constexpr auto GetItemIdValue = [](int i) {
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

	return static_cast<_item_indexes>(GetItemIdValue(i));
}

_item_indexes RemapItemIdxToSpawn(_item_indexes i)
{
	constexpr auto GetItemIdValue = [](int i) {
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

	return static_cast<_item_indexes>(GetItemIdValue(i));
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
	}
	if (!gbIsSpawn && magicNumber == LoadLE32("RETL")) {
		return true;
	}
	if (!gbIsSpawn && magicNumber == LoadLE32("HELF")) {
		gbIsHellfireSaveGame = true;
		return true;
	}

	return false;
}

static void ConvertLevels()
{
	// Backup current level state
	bool tmpSetlevel = setlevel;
	_setlevels tmpSetlvlnum = setlvlnum;
	int tmpCurrlevel = currlevel;
	dungeon_type tmpLeveltype = leveltype;

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
	for (auto &quest : Quests) {
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
	setlevel = tmpSetlevel;
	setlvlnum = tmpSetlvlnum;
	currlevel = tmpCurrlevel;
	leveltype = tmpLeveltype;
}

void LoadHotkeys()
{
	LoadHelper file("hotkeys");
	if (!file.IsValid())
		return;

	auto &myPlayer = Players[MyPlayerId];

	for (auto &spellId : myPlayer._pSplHotKey) {
		spellId = static_cast<spell_id>(file.NextLE<int32_t>());
	}
	for (auto &spellType : myPlayer._pSplTHotKey) {
		spellType = static_cast<spell_type>(file.NextLE<int8_t>());
	}
	myPlayer._pRSpell = static_cast<spell_id>(file.NextLE<int32_t>());
	myPlayer._pRSplType = static_cast<spell_type>(file.NextLE<int8_t>());
}

void SaveHotkeys()
{
	auto &myPlayer = Players[MyPlayerId];

	const size_t nHotkeyTypes = sizeof(myPlayer._pSplHotKey) / sizeof(myPlayer._pSplHotKey[0]);
	const size_t nHotkeySpells = sizeof(myPlayer._pSplTHotKey) / sizeof(myPlayer._pSplTHotKey[0]);

	SaveHelper file("hotkeys", (nHotkeyTypes * 4) + nHotkeySpells + 4 + 1);

	for (auto &spellId : myPlayer._pSplHotKey) {
		file.WriteLE<int32_t>(spellId);
	}
	for (auto &spellType : myPlayer._pSplTHotKey) {
		file.WriteLE<uint8_t>(spellType);
	}
	file.WriteLE<int32_t>(myPlayer._pRSpell);
	file.WriteLE<uint8_t>(myPlayer._pRSplType);
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
	if (!file.IsValid())
		return;

	gbIsHellfireSaveGame = file.NextBool8();

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
	}
}

void RemoveEmptyLevelItems()
{
	for (int i = ActiveItemCount; i > 0; i--) {
		int ii = ActiveItems[i];
		if (Items[ii].isEmpty()) {
			dItem[Items[ii].position.x][Items[ii].position.y] = 0;
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
	if (!file.IsValid())
		app_fatal("%s", _("Unable to open save file archive"));

	if (!IsHeaderValid(file.NextLE<uint32_t>()))
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

	setlevel = file.NextBool8();
	setlvlnum = static_cast<_setlevels>(file.NextBE<uint32_t>());
	currlevel = file.NextBE<uint32_t>();
	leveltype = static_cast<dungeon_type>(file.NextBE<uint32_t>());
	if (!setlevel)
		leveltype = gnLevelTypeTbl[currlevel];
	int viewX = file.NextBE<int32_t>();
	int viewY = file.NextBE<int32_t>();
	invflag = file.NextBool8();
	chrflag = file.NextBool8();
	int tmpNummonsters = file.NextBE<int32_t>();
	int tmpNumitems = file.NextBE<int32_t>();
	int tmpNummissiles = file.NextBE<int32_t>();
	int tmpNobjects = file.NextBE<int32_t>();

	if (!gbIsHellfire && currlevel > 17)
		app_fatal("%s", _("Player is on a Hellfire only level"));

	for (uint8_t i = 0; i < giNumberOfLevels; i++) {
		glSeedTbl[i] = file.NextBE<uint32_t>();
		file.Skip(4); // Skip loading gnLevelTypeTbl
	}

	LoadPlayer(&file, MyPlayerId);

	sgGameInitInfo.nDifficulty = Players[MyPlayerId].pDifficulty;
	if (sgGameInitInfo.nDifficulty < DIFF_NORMAL || sgGameInitInfo.nDifficulty > DIFF_HELL)
		sgGameInitInfo.nDifficulty = DIFF_NORMAL;

	for (int i = 0; i < giNumberQuests; i++)
		LoadQuest(&file, i);
	for (int i = 0; i < MAXPORTAL; i++)
		LoadPortal(&file, i);

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		ConvertLevels();
		RemoveEmptyInventory(Players[MyPlayerId]);
	}

	LoadGameLevel(firstflag, ENTRY_LOAD);
	SyncInitPlr(MyPlayerId);
	SyncPlrAnim(MyPlayerId);

	ViewX = viewX;
	ViewY = viewY;
	ActiveMonsterCount = tmpNummonsters;
	ActiveItemCount = tmpNumitems;
	ActiveMissileCount = tmpNummissiles;
	ActiveObjectCount = tmpNobjects;

	for (int &monstkill : MonsterKillCounts)
		monstkill = file.NextBE<int32_t>();

	if (leveltype != DTYPE_TOWN) {
		for (int &monsterId : ActiveMonsters)
			monsterId = file.NextBE<int32_t>();
		for (int i = 0; i < ActiveMonsterCount; i++)
			LoadMonster(&file, ActiveMonsters[i]);
		for (int &missileId : ActiveMissiles)
			missileId = file.NextLE<int8_t>();
		for (int &missileId : AvailableMissiles)
			missileId = file.NextLE<int8_t>();
		for (int i = 0; i < ActiveMissileCount; i++)
			LoadMissile(&file, ActiveMissiles[i]);
		for (int &objectId : ActiveObjects)
			objectId = file.NextLE<int8_t>();
		for (int &objectId : AvailableObjects)
			objectId = file.NextLE<int8_t>();
		for (int i = 0; i < ActiveObjectCount; i++)
			LoadObject(&file, ActiveObjects[i]);
		for (int i = 0; i < ActiveObjectCount; i++)
			SyncObjectAnim(Objects[ActiveObjects[i]]);

		ActiveLightCount = file.NextBE<int32_t>();

		for (uint8_t &lightId : ActiveLights)
			lightId = file.NextLE<uint8_t>();
		for (int i = 0; i < ActiveLightCount; i++)
			LoadLighting(&file, &Lights[ActiveLights[i]]);

		VisionId = file.NextBE<int32_t>();
		VisionCount = file.NextBE<int32_t>();

		for (int i = 0; i < VisionCount; i++)
			LoadLighting(&file, &VisionList[i]);
	}

	for (int &itemId : ActiveItems)
		itemId = file.NextLE<int8_t>();
	for (int &itemId : AvailableItems)
		itemId = file.NextLE<int8_t>();
	for (int i = 0; i < ActiveItemCount; i++)
		LoadItem(&file, ActiveItems[i]);
	for (bool &uniqueItemFlag : UniqueItemFlags)
		uniqueItemFlag = file.NextBool8();

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dLight[i][j] = file.NextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dFlags[i][j] = file.NextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dPlayer[i][j] = file.NextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dItem[i][j] = file.NextLE<int8_t>();
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dMonster[i][j] = file.NextBE<int32_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dDead[i][j] = file.NextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dObject[i][j] = file.NextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dLight[i][j] = file.NextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dPreLight[i][j] = file.NextLE<int8_t>();
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) // NOLINT(modernize-loop-convert)
				AutomapView[i][j] = file.NextBool8();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dMissile[i][j] = file.NextLE<int8_t>();
		}
	}

	numpremium = file.NextBE<int32_t>();
	premiumlevel = file.NextBE<int32_t>();

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		LoadPremium(&file, i);
	if (gbIsHellfire && !gbIsHellfireSaveGame)
		SpawnPremium(MyPlayerId);

	AutomapActive = file.NextBool8();
	AutoMapScale = file.NextBE<int32_t>();
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

	file->WriteLE<int32_t>(pItem->_iSeed);
	file->WriteLE<int16_t>(pItem->_iCreateInfo);
	file->Skip(2); // Alignment
	file->WriteLE<int32_t>(iType);
	file->WriteLE<int32_t>(pItem->position.x);
	file->WriteLE<int32_t>(pItem->position.y);
	file->WriteLE<uint32_t>(pItem->_iAnimFlag ? 1 : 0);
	file->Skip(4); // Skip pointer _iAnimData
	file->WriteLE<int32_t>(pItem->AnimInfo.NumberOfFrames);
	file->WriteLE<int32_t>(pItem->AnimInfo.CurrentFrame);
	// write _iAnimWidth for vanilla compatibility
	file->WriteLE<int32_t>(ItemAnimWidth);
	// write _iAnimWidth2 for vanilla compatibility
	file->WriteLE<int32_t>(CalculateWidth2(ItemAnimWidth));
	file->Skip<uint32_t>(); // _delFlag, unused since 1.02
	file->WriteLE<uint8_t>(pItem->_iSelFlag);
	file->Skip(3); // Alignment
	file->WriteLE<uint32_t>(pItem->_iPostDraw ? 1 : 0);
	file->WriteLE<uint32_t>(pItem->_iIdentified ? 1 : 0);
	file->WriteLE<int8_t>(pItem->_iMagical);
	file->WriteBytes(pItem->_iName, 64);
	file->WriteBytes(pItem->_iIName, 64);
	file->WriteLE<int8_t>(pItem->_iLoc);
	file->WriteLE<uint8_t>(pItem->_iClass);
	file->Skip(1); // Alignment
	file->WriteLE<int32_t>(pItem->_iCurs);
	file->WriteLE<int32_t>(pItem->_ivalue);
	file->WriteLE<int32_t>(pItem->_iIvalue);
	file->WriteLE<int32_t>(pItem->_iMinDam);
	file->WriteLE<int32_t>(pItem->_iMaxDam);
	file->WriteLE<int32_t>(pItem->_iAC);
	file->WriteLE<uint32_t>(pItem->_iFlags);
	file->WriteLE<int32_t>(pItem->_iMiscId);
	file->WriteLE<int32_t>(pItem->_iSpell);
	file->WriteLE<int32_t>(pItem->_iCharges);
	file->WriteLE<int32_t>(pItem->_iMaxCharges);
	file->WriteLE<int32_t>(pItem->_iDurability);
	file->WriteLE<int32_t>(pItem->_iMaxDur);
	file->WriteLE<int32_t>(pItem->_iPLDam);
	file->WriteLE<int32_t>(pItem->_iPLToHit);
	file->WriteLE<int32_t>(pItem->_iPLAC);
	file->WriteLE<int32_t>(pItem->_iPLStr);
	file->WriteLE<int32_t>(pItem->_iPLMag);
	file->WriteLE<int32_t>(pItem->_iPLDex);
	file->WriteLE<int32_t>(pItem->_iPLVit);
	file->WriteLE<int32_t>(pItem->_iPLFR);
	file->WriteLE<int32_t>(pItem->_iPLLR);
	file->WriteLE<int32_t>(pItem->_iPLMR);
	file->WriteLE<int32_t>(pItem->_iPLMana);
	file->WriteLE<int32_t>(pItem->_iPLHP);
	file->WriteLE<int32_t>(pItem->_iPLDamMod);
	file->WriteLE<int32_t>(pItem->_iPLGetHit);
	file->WriteLE<int32_t>(pItem->_iPLLight);
	file->WriteLE<int8_t>(pItem->_iSplLvlAdd);
	file->WriteLE<int8_t>(pItem->_iRequest ? 1 : 0);
	file->Skip(2); // Alignment
	file->WriteLE<int32_t>(pItem->_iUid);
	file->WriteLE<int32_t>(pItem->_iFMinDam);
	file->WriteLE<int32_t>(pItem->_iFMaxDam);
	file->WriteLE<int32_t>(pItem->_iLMinDam);
	file->WriteLE<int32_t>(pItem->_iLMaxDam);
	file->WriteLE<int32_t>(pItem->_iPLEnAc);
	file->WriteLE<int8_t>(pItem->_iPrePower);
	file->WriteLE<int8_t>(pItem->_iSufPower);
	file->Skip(2); // Alignment
	file->WriteLE<int32_t>(pItem->_iVAdd1);
	file->WriteLE<int32_t>(pItem->_iVMult1);
	file->WriteLE<int32_t>(pItem->_iVAdd2);
	file->WriteLE<int32_t>(pItem->_iVMult2);
	file->WriteLE<int8_t>(pItem->_iMinStr);
	file->WriteLE<uint8_t>(pItem->_iMinMag);
	file->WriteLE<int8_t>(pItem->_iMinDex);
	file->Skip(1); // Alignment
	file->WriteLE<uint32_t>(pItem->_iStatFlag ? 1 : 0);
	file->WriteLE<int32_t>(idx);
	file->WriteLE<uint32_t>(pItem->dwBuff);
	if (gbIsHellfire)
		file->WriteLE<uint32_t>(pItem->_iDamAcFlags);
}

static void SaveItems(SaveHelper *file, ItemStruct *pItem, const int n)
{
	for (int i = 0; i < n; i++) {
		SaveItem(file, &pItem[i]);
	}
}

static void SavePlayer(SaveHelper *file, int p)
{
	auto &player = Players[p];

	file->WriteLE<int32_t>(player._pmode);
	for (int8_t step : player.walkpath)
		file->WriteLE<int8_t>(step);
	file->WriteLE<uint8_t>(player.plractive ? 1 : 0);
	file->Skip(2); // Alignment
	file->WriteLE<int32_t>(player.destAction);
	file->WriteLE<int32_t>(player.destParam1);
	file->WriteLE<int32_t>(player.destParam2);
	file->WriteLE<int32_t>(player.destParam3);
	file->WriteLE<int32_t>(player.destParam4);
	file->WriteLE<int32_t>(player.plrlevel);
	file->WriteLE<int32_t>(player.position.tile.x);
	file->WriteLE<int32_t>(player.position.tile.y);
	file->WriteLE<int32_t>(player.position.future.x);
	file->WriteLE<int32_t>(player.position.future.y);

	// For backwards compatibility
	const Point target = player.GetTargetPosition();
	file->WriteLE<int32_t>(target.x);
	file->WriteLE<int32_t>(target.y);

	file->WriteLE<int32_t>(player.position.last.x);
	file->WriteLE<int32_t>(player.position.last.y);
	file->WriteLE<int32_t>(player.position.old.x);
	file->WriteLE<int32_t>(player.position.old.y);
	file->WriteLE<int32_t>(player.position.offset.deltaX);
	file->WriteLE<int32_t>(player.position.offset.deltaY);
	file->WriteLE<int32_t>(player.position.velocity.deltaX);
	file->WriteLE<int32_t>(player.position.velocity.deltaY);
	file->WriteLE<int32_t>(player._pdir);
	file->Skip(4); // Unused
	file->WriteLE<int32_t>(player._pgfxnum);
	file->Skip(4); // Skip pointer _pAnimData
	file->WriteLE<int32_t>(std::max(0, player.AnimInfo.TicksPerFrame - 1));
	file->WriteLE<int32_t>(player.AnimInfo.TickCounterOfCurrentFrame);
	file->WriteLE<int32_t>(player.AnimInfo.NumberOfFrames);
	file->WriteLE<int32_t>(player.AnimInfo.CurrentFrame);
	// write _pAnimWidth for vanilla compatibility
	int animWidth = player.AnimInfo.pCelSprite == nullptr ? 96 : player.AnimInfo.pCelSprite->Width();
	file->WriteLE<int32_t>(animWidth);
	// write _pAnimWidth2 for vanilla compatibility
	file->WriteLE<int32_t>(CalculateWidth2(animWidth));
	file->Skip<uint32_t>(); // Skip _peflag
	file->WriteLE<int32_t>(player._plid);
	file->WriteLE<int32_t>(player._pvid);

	file->WriteLE<int32_t>(player._pSpell);
	file->WriteLE<int8_t>(player._pSplType);
	file->WriteLE<int8_t>(player._pSplFrom);
	file->Skip(2); // Alignment
	file->WriteLE<int32_t>(player._pTSpell);
	file->Skip<int8_t>(); // Skip _pTSplType
	file->Skip(3);        // Alignment
	file->WriteLE<int32_t>(player._pRSpell);
	file->WriteLE<int8_t>(player._pRSplType);
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(player._pSBkSpell);
	file->Skip<int8_t>(); // Skip _pSBkSplType
	for (int8_t spellLevel : player._pSplLvl)
		file->WriteLE<int8_t>(spellLevel);
	file->Skip(7); // Alignment
	file->WriteLE<uint64_t>(player._pMemSpells);
	file->WriteLE<uint64_t>(player._pAblSpells);
	file->WriteLE<uint64_t>(player._pScrlSpells);
	file->WriteLE<uint8_t>(player._pSpellFlags);
	file->Skip(3); // Alignment
	for (auto &spellId : player._pSplHotKey)
		file->WriteLE<int32_t>(spellId);
	for (auto &spellType : player._pSplTHotKey)
		file->WriteLE<int8_t>(spellType);

	file->WriteLE<int32_t>(player._pwtype);
	file->WriteLE<uint8_t>(player._pBlockFlag ? 1 : 0);
	file->WriteLE<uint8_t>(player._pInvincible ? 1 : 0);
	file->WriteLE<int8_t>(player._pLightRad);
	file->WriteLE<uint8_t>(player._pLvlChanging ? 1 : 0);

	file->WriteBytes(player._pName, PLR_NAME_LEN);
	file->WriteLE<int8_t>(static_cast<int8_t>(player._pClass));
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(player._pStrength);
	file->WriteLE<int32_t>(player._pBaseStr);
	file->WriteLE<int32_t>(player._pMagic);
	file->WriteLE<int32_t>(player._pBaseMag);
	file->WriteLE<int32_t>(player._pDexterity);
	file->WriteLE<int32_t>(player._pBaseDex);
	file->WriteLE<int32_t>(player._pVitality);
	file->WriteLE<int32_t>(player._pBaseVit);
	file->WriteLE<int32_t>(player._pStatPts);
	file->WriteLE<int32_t>(player._pDamageMod);

	file->WriteLE<int32_t>(player._pBaseToBlk);
	file->WriteLE<int32_t>(player._pHPBase);
	file->WriteLE<int32_t>(player._pMaxHPBase);
	file->WriteLE<int32_t>(player._pHitPoints);
	file->WriteLE<int32_t>(player._pMaxHP);
	file->Skip<int32_t>(); // Skip _pHPPer
	file->WriteLE<int32_t>(player._pManaBase);
	file->WriteLE<int32_t>(player._pMaxManaBase);
	file->WriteLE<int32_t>(player._pMana);
	file->WriteLE<int32_t>(player._pMaxMana);
	file->Skip<int32_t>(); // Skip _pManaPer
	file->WriteLE<int8_t>(player._pLevel);
	file->WriteLE<int8_t>(player._pMaxLvl);
	file->Skip(2); // Alignment
	file->WriteLE<int32_t>(player._pExperience);
	file->WriteLE<int32_t>(player._pMaxExp);
	file->WriteLE<int32_t>(player._pNextExper);
	file->WriteLE<int8_t>(player._pArmorClass);
	file->WriteLE<int8_t>(player._pMagResist);
	file->WriteLE<int8_t>(player._pFireResist);
	file->WriteLE<int8_t>(player._pLghtResist);
	file->WriteLE<int32_t>(player._pGold);

	file->WriteLE<uint32_t>(player._pInfraFlag ? 1 : 0);
	file->WriteLE<int32_t>(player.position.temp.x);
	file->WriteLE<int32_t>(player.position.temp.y);
	file->WriteLE<int32_t>(player.tempDirection);
	file->WriteLE<int32_t>(player._pVar4);
	file->WriteLE<int32_t>(player._pVar5);
	file->WriteLE<int32_t>(player.position.offset2.deltaX);
	file->WriteLE<int32_t>(player.position.offset2.deltaY);
	file->Skip<int32_t>(); // Skip _pVar8
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		file->WriteLE<uint8_t>(player._pLvlVisited[i] ? 1 : 0);
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		file->WriteLE<uint8_t>(player._pSLvlVisited[i] ? 1 : 0); // only 10 used

	file->Skip(2); // Alignment

	file->Skip<int32_t>(); // Skip _pGFXLoad
	file->Skip(4 * 8);     // Skip pointers _pNAnim
	file->WriteLE<int32_t>(player._pNFrames);
	file->Skip(4);     // Skip _pNWidth
	file->Skip(4 * 8); // Skip pointers _pWAnim
	file->WriteLE<int32_t>(player._pWFrames);
	file->Skip(4);     // Skip _pWWidth
	file->Skip(4 * 8); // Skip pointers _pAAnim
	file->WriteLE<int32_t>(player._pAFrames);
	file->Skip(4); // Skip _pAWidth
	file->WriteLE<int32_t>(player._pAFNum);
	file->Skip(4 * 8); // Skip pointers _pLAnim
	file->Skip(4 * 8); // Skip pointers _pFAnim
	file->Skip(4 * 8); // Skip pointers _pTAnim
	file->WriteLE<int32_t>(player._pSFrames);
	file->Skip(4); // Skip _pSWidth
	file->WriteLE<int32_t>(player._pSFNum);
	file->Skip(4 * 8); // Skip pointers _pHAnim
	file->WriteLE<int32_t>(player._pHFrames);
	file->Skip(4);     // Skip _pHWidth
	file->Skip(4 * 8); // Skip pointers _pDAnim
	file->WriteLE<int32_t>(player._pDFrames);
	file->Skip(4);     // Skip _pDWidth
	file->Skip(4 * 8); // Skip pointers _pBAnim
	file->WriteLE<int32_t>(player._pBFrames);
	file->Skip(4); // Skip _pBWidth

	SaveItems(file, player.InvBody, NUM_INVLOC);
	SaveItems(file, player.InvList, NUM_INV_GRID_ELEM);
	file->WriteLE<int32_t>(player._pNumInv);
	for (int8_t cell : player.InvGrid)
		file->WriteLE<int8_t>(cell);
	SaveItems(file, player.SpdList, MAXBELTITEMS);
	SaveItem(file, &player.HoldItem);

	file->WriteLE<int32_t>(player._pIMinDam);
	file->WriteLE<int32_t>(player._pIMaxDam);
	file->WriteLE<int32_t>(player._pIAC);
	file->WriteLE<int32_t>(player._pIBonusDam);
	file->WriteLE<int32_t>(player._pIBonusToHit);
	file->WriteLE<int32_t>(player._pIBonusAC);
	file->WriteLE<int32_t>(player._pIBonusDamMod);
	file->Skip(4); // Alignment

	file->WriteLE<uint64_t>(player._pISpells);
	file->WriteLE<int32_t>(player._pIFlags);
	file->WriteLE<int32_t>(player._pIGetHit);

	file->WriteLE<int8_t>(player._pISplLvlAdd);
	file->Skip<uint8_t>(); // Skip _pISplCost
	file->Skip(2);         // Alignment
	file->WriteLE<int32_t>(player._pISplDur);
	file->WriteLE<int32_t>(player._pIEnAc);
	file->WriteLE<int32_t>(player._pIFMinDam);
	file->WriteLE<int32_t>(player._pIFMaxDam);
	file->WriteLE<int32_t>(player._pILMinDam);
	file->WriteLE<int32_t>(player._pILMaxDam);
	file->WriteLE<int32_t>(player._pOilType);
	file->WriteLE<uint8_t>(player.pTownWarps);
	file->WriteLE<uint8_t>(player.pDungMsgs);
	file->WriteLE<uint8_t>(player.pLvlLoad);
	if (gbIsHellfire)
		file->WriteLE<uint8_t>(player.pDungMsgs2);
	else
		file->WriteLE<uint8_t>(player.pBattleNet ? 1 : 0);
	file->WriteLE<uint8_t>(player.pManaShield ? 1 : 0);
	file->WriteLE<uint8_t>(player.pOriginalCathedral ? 1 : 0);
	file->Skip(2); // Available bytes
	file->WriteLE<uint16_t>(player.wReflections);
	file->Skip(14); // Available bytes

	file->WriteLE<uint32_t>(player.pDiabloKillLevel);
	file->WriteLE<uint32_t>(player.pDifficulty);
	file->WriteLE<uint32_t>(player.pDamAcFlags);
	file->Skip(20); // Available bytes

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
	MonsterStruct *pMonster = &Monsters[i];

	file->WriteLE<int32_t>(pMonster->_mMTidx);
	file->WriteLE<int32_t>(pMonster->_mmode);
	file->WriteLE<uint8_t>(pMonster->_mgoal);
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(pMonster->_mgoalvar1);
	file->WriteLE<int32_t>(pMonster->_mgoalvar2);
	file->WriteLE<int32_t>(pMonster->_mgoalvar3);
	file->Skip(4); // Unused
	file->WriteLE<uint8_t>(pMonster->_pathcount);
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(pMonster->position.tile.x);
	file->WriteLE<int32_t>(pMonster->position.tile.y);
	file->WriteLE<int32_t>(pMonster->position.future.x);
	file->WriteLE<int32_t>(pMonster->position.future.y);
	file->WriteLE<int32_t>(pMonster->position.old.x);
	file->WriteLE<int32_t>(pMonster->position.old.y);
	file->WriteLE<int32_t>(pMonster->position.offset.deltaX);
	file->WriteLE<int32_t>(pMonster->position.offset.deltaY);
	file->WriteLE<int32_t>(pMonster->position.velocity.deltaX);
	file->WriteLE<int32_t>(pMonster->position.velocity.deltaY);
	file->WriteLE<int32_t>(pMonster->_mdir);
	file->WriteLE<int32_t>(pMonster->_menemy);
	file->WriteLE<uint8_t>(pMonster->enemyPosition.x);
	file->WriteLE<uint8_t>(pMonster->enemyPosition.y);
	file->Skip(2); // Unused

	file->Skip(4); // Skip pointer _mAnimData
	file->WriteLE<int32_t>(pMonster->AnimInfo.TicksPerFrame);
	file->WriteLE<int32_t>(pMonster->AnimInfo.TickCounterOfCurrentFrame);
	file->WriteLE<int32_t>(pMonster->AnimInfo.NumberOfFrames);
	file->WriteLE<int32_t>(pMonster->AnimInfo.CurrentFrame);
	file->Skip<uint32_t>(); // Skip _meflag
	file->WriteLE<uint32_t>(pMonster->_mDelFlag ? 1 : 0);
	file->WriteLE<int32_t>(pMonster->_mVar1);
	file->WriteLE<int32_t>(pMonster->_mVar2);
	file->WriteLE<int32_t>(pMonster->_mVar3);
	file->WriteLE<int32_t>(pMonster->position.temp.x);
	file->WriteLE<int32_t>(pMonster->position.temp.y);
	file->WriteLE<int32_t>(pMonster->position.offset2.deltaX);
	file->WriteLE<int32_t>(pMonster->position.offset2.deltaY);
	file->Skip<int32_t>(); // Skip _mVar8
	file->WriteLE<int32_t>(pMonster->_mmaxhp);
	file->WriteLE<int32_t>(pMonster->_mhitpoints);

	file->WriteLE<uint8_t>(pMonster->_mAi);
	file->WriteLE<uint8_t>(pMonster->_mint);
	file->Skip(2); // Alignment
	file->WriteLE<uint32_t>(pMonster->_mFlags);
	file->WriteLE<uint8_t>(pMonster->_msquelch);
	file->Skip(3); // Alignment
	file->Skip(4); // Unused
	file->WriteLE<int32_t>(pMonster->position.last.x);
	file->WriteLE<int32_t>(pMonster->position.last.y);
	file->WriteLE<uint32_t>(pMonster->_mRndSeed);
	file->WriteLE<uint32_t>(pMonster->_mAISeed);
	file->Skip(4); // Unused

	file->WriteLE<uint8_t>(pMonster->_uniqtype);
	file->WriteLE<uint8_t>(pMonster->_uniqtrans);
	file->WriteLE<int8_t>(pMonster->_udeadval);

	file->WriteLE<int8_t>(pMonster->mWhoHit);
	file->WriteLE<int8_t>(pMonster->mLevel);
	file->Skip(1); // Alignment
	file->WriteLE<uint16_t>(pMonster->mExp);

	file->WriteLE<uint8_t>(std::min<uint16_t>(pMonster->mHit, std::numeric_limits<uint8_t>::max())); // For backwards compatibility
	file->WriteLE<uint8_t>(pMonster->mMinDamage);
	file->WriteLE<uint8_t>(pMonster->mMaxDamage);
	file->WriteLE<uint8_t>(std::min<uint16_t>(pMonster->mHit2, std::numeric_limits<uint8_t>::max())); // For backwards compatibility
	file->WriteLE<uint8_t>(pMonster->mMinDamage2);
	file->WriteLE<uint8_t>(pMonster->mMaxDamage2);
	file->WriteLE<uint8_t>(pMonster->mArmorClass);
	file->Skip(1); // Alignment
	file->WriteLE<uint16_t>(pMonster->mMagicRes);
	file->Skip(2); // Alignment

	file->WriteLE<int32_t>(pMonster->mtalkmsg == TEXT_NONE ? 0 : pMonster->mtalkmsg); // Replicate original bad mapping of none for monsters
	file->WriteLE<uint8_t>(pMonster->leader);
	file->WriteLE<uint8_t>(pMonster->leaderflag);
	file->WriteLE<uint8_t>(pMonster->packsize);
	file->WriteLE<int8_t>(pMonster->mlid);

	// Omit pointer mName;
	// Omit pointer MType;
	// Omit pointer MData;
}

static void SaveMissile(SaveHelper *file, int i)
{
	MissileStruct *pMissile = &Missiles[i];

	file->WriteLE<int32_t>(pMissile->_mitype);
	file->WriteLE<int32_t>(pMissile->position.tile.x);
	file->WriteLE<int32_t>(pMissile->position.tile.y);
	file->WriteLE<int32_t>(pMissile->position.offset.deltaX);
	file->WriteLE<int32_t>(pMissile->position.offset.deltaY);
	file->WriteLE<int32_t>(pMissile->position.velocity.deltaX);
	file->WriteLE<int32_t>(pMissile->position.velocity.deltaY);
	file->WriteLE<int32_t>(pMissile->position.start.x);
	file->WriteLE<int32_t>(pMissile->position.start.y);
	file->WriteLE<int32_t>(pMissile->position.traveled.deltaX);
	file->WriteLE<int32_t>(pMissile->position.traveled.deltaY);
	file->WriteLE<int32_t>(pMissile->_mimfnum);
	file->WriteLE<int32_t>(pMissile->_mispllvl);
	file->WriteLE<uint32_t>(pMissile->_miDelFlag ? 1 : 0);
	file->WriteLE<uint8_t>(pMissile->_miAnimType);
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(pMissile->_miAnimFlags);
	file->Skip(4); // Skip pointer _miAnimData
	file->WriteLE<int32_t>(pMissile->_miAnimDelay);
	file->WriteLE<int32_t>(pMissile->_miAnimLen);
	file->WriteLE<int32_t>(pMissile->_miAnimWidth);
	file->WriteLE<int32_t>(pMissile->_miAnimWidth2);
	file->WriteLE<int32_t>(pMissile->_miAnimCnt);
	file->WriteLE<int32_t>(pMissile->_miAnimAdd);
	file->WriteLE<int32_t>(pMissile->_miAnimFrame);
	file->WriteLE<uint32_t>(pMissile->_miDrawFlag ? 1 : 0);
	file->WriteLE<uint32_t>(pMissile->_miLightFlag ? 1 : 0);
	file->WriteLE<uint32_t>(pMissile->_miPreFlag ? 1 : 0);
	file->WriteLE<uint32_t>(pMissile->_miUniqTrans);
	file->WriteLE<int32_t>(pMissile->_mirange);
	file->WriteLE<int32_t>(pMissile->_misource);
	file->WriteLE<int32_t>(pMissile->_micaster);
	file->WriteLE<int32_t>(pMissile->_midam);
	file->WriteLE<uint32_t>(pMissile->_miHitFlag ? 1 : 0);
	file->WriteLE<int32_t>(pMissile->_midist);
	file->WriteLE<int32_t>(pMissile->_mlid);
	file->WriteLE<int32_t>(pMissile->_mirnd);
	file->WriteLE<int32_t>(pMissile->_miVar1);
	file->WriteLE<int32_t>(pMissile->_miVar2);
	file->WriteLE<int32_t>(pMissile->_miVar3);
	file->WriteLE<int32_t>(pMissile->_miVar4);
	file->WriteLE<int32_t>(pMissile->_miVar5);
	file->WriteLE<int32_t>(pMissile->_miVar6);
	file->WriteLE<int32_t>(pMissile->_miVar7);
	file->WriteLE<uint32_t>(pMissile->limitReached ? 1 : 0);
}

static void SaveObject(SaveHelper *file, int i)
{
	ObjectStruct *pObject = &Objects[i];

	file->WriteLE<int32_t>(pObject->_otype);
	file->WriteLE<int32_t>(pObject->position.x);
	file->WriteLE<int32_t>(pObject->position.y);
	file->WriteLE<uint32_t>(pObject->_oLight ? 1 : 0);
	file->WriteLE<uint32_t>(pObject->_oAnimFlag);
	file->Skip(4); // Skip pointer _oAnimData
	file->WriteLE<int32_t>(pObject->_oAnimDelay);
	file->WriteLE<int32_t>(pObject->_oAnimCnt);
	file->WriteLE<uint32_t>(pObject->_oAnimLen);
	file->WriteLE<uint32_t>(pObject->_oAnimFrame);
	file->WriteLE<int32_t>(pObject->_oAnimWidth);
	file->WriteLE<int32_t>(CalculateWidth2(pObject->_oAnimWidth)); // Write _oAnimWidth2 for vanilla compatibility
	file->WriteLE<uint32_t>(pObject->_oDelFlag ? 1 : 0);
	file->WriteLE<int8_t>(pObject->_oBreak);
	file->Skip(3); // Alignment
	file->WriteLE<uint32_t>(pObject->_oSolidFlag ? 1 : 0);
	file->WriteLE<uint32_t>(pObject->_oMissFlag ? 1 : 0);

	file->WriteLE<int8_t>(pObject->_oSelFlag);
	file->Skip(3); // Alignment
	file->WriteLE<uint32_t>(pObject->_oPreFlag ? 1 : 0);
	file->WriteLE<uint32_t>(pObject->_oTrapFlag ? 1 : 0);
	file->WriteLE<uint32_t>(pObject->_oDoorFlag ? 1 : 0);
	file->WriteLE<int32_t>(pObject->_olid);
	file->WriteLE<uint32_t>(pObject->_oRndSeed);
	file->WriteLE<int32_t>(pObject->_oVar1);
	file->WriteLE<int32_t>(pObject->_oVar2);
	file->WriteLE<int32_t>(pObject->_oVar3);
	file->WriteLE<int32_t>(pObject->_oVar4);
	file->WriteLE<int32_t>(pObject->_oVar5);
	file->WriteLE<uint32_t>(pObject->_oVar6);
	file->WriteLE<int32_t>(pObject->_oVar7);
	file->WriteLE<int32_t>(pObject->_oVar8);
}

static void SavePremium(SaveHelper *file, int i)
{
	SaveItem(file, &premiumitems[i]);
}

static void SaveQuest(SaveHelper *file, int i)
{
	QuestStruct *pQuest = &Quests[i];

	file->WriteLE<uint8_t>(pQuest->_qlevel);
	file->WriteLE<uint8_t>(pQuest->_qtype);
	file->WriteLE<uint8_t>(pQuest->_qactive);
	file->WriteLE<uint8_t>(pQuest->_qlvltype);
	file->WriteLE<int32_t>(pQuest->position.x);
	file->WriteLE<int32_t>(pQuest->position.y);
	file->WriteLE<uint8_t>(pQuest->_qslvl);
	file->WriteLE<uint8_t>(pQuest->_qidx);
	if (gbIsHellfire) {
		file->Skip(2); // Alignment
		file->WriteLE<int32_t>(pQuest->_qmsg);
	} else {
		file->WriteLE<uint8_t>(pQuest->_qmsg);
	}
	file->WriteLE<uint8_t>(pQuest->_qvar1);
	file->WriteLE<uint8_t>(pQuest->_qvar2);
	file->Skip(2); // Alignment
	if (!gbIsHellfire)
		file->Skip(1); // Alignment
	file->WriteLE<uint32_t>(pQuest->_qlog ? 1 : 0);

	file->WriteBE<int32_t>(ReturnLvlX);
	file->WriteBE<int32_t>(ReturnLvlY);
	file->WriteBE<int32_t>(ReturnLevel);
	file->WriteBE<int32_t>(ReturnLevelType);
	file->Skip(sizeof(int32_t)); // Skip DoomQuestState
}

static void SaveLighting(SaveHelper *file, LightStruct *pLight)
{
	file->WriteLE<int32_t>(pLight->position.tile.x);
	file->WriteLE<int32_t>(pLight->position.tile.y);
	file->WriteLE<int32_t>(pLight->_lradius);
	file->WriteLE<int32_t>(pLight->_lid);
	file->WriteLE<uint32_t>(pLight->_ldel ? 1 : 0);
	file->WriteLE<uint32_t>(pLight->_lunflag ? 1 : 0);
	file->Skip(4); // Unused
	file->WriteLE<int32_t>(pLight->position.old.x);
	file->WriteLE<int32_t>(pLight->position.old.y);
	file->WriteLE<int32_t>(pLight->oldRadius);
	file->WriteLE<int32_t>(pLight->position.offset.x);
	file->WriteLE<int32_t>(pLight->position.offset.y);
	file->WriteLE<uint32_t>(pLight->_lflags ? 1 : 0);
}

static void SavePortal(SaveHelper *file, int i)
{
	PortalStruct *pPortal = &Portals[i];

	file->WriteLE<uint32_t>(pPortal->open ? 1 : 0);
	file->WriteLE<int32_t>(pPortal->position.x);
	file->WriteLE<int32_t>(pPortal->position.y);
	file->WriteLE<int32_t>(pPortal->level);
	file->WriteLE<int32_t>(pPortal->ltype);
	file->WriteLE<uint32_t>(pPortal->setlvl ? 1 : 0);
}

const int DiabloItemSaveSize = 368;
const int HellfireItemSaveSize = 372;

void SaveHeroItems(PlayerStruct &player)
{
	size_t itemCount = NUM_INVLOC + NUM_INV_GRID_ELEM + MAXBELTITEMS;
	SaveHelper file("heroitems", itemCount * (gbIsHellfire ? HellfireItemSaveSize : DiabloItemSaveSize) + sizeof(uint8_t));

	file.WriteLE<uint8_t>(gbIsHellfire ? 1 : 0);

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
		file.WriteLE<uint32_t>(LoadLE32("SHAR"));
	else if (gbIsSpawn && gbIsHellfire)
		file.WriteLE<uint32_t>(LoadLE32("SHLF"));
	else if (!gbIsSpawn && gbIsHellfire)
		file.WriteLE<uint32_t>(LoadLE32("HELF"));
	else if (!gbIsSpawn && !gbIsHellfire)
		file.WriteLE<uint32_t>(LoadLE32("RETL"));
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

	file.WriteLE<uint8_t>(setlevel ? 1 : 0);
	file.WriteBE<uint32_t>(setlvlnum);
	file.WriteBE<uint32_t>(currlevel);
	file.WriteBE<uint32_t>(leveltype);
	file.WriteBE<int32_t>(ViewX);
	file.WriteBE<int32_t>(ViewY);
	file.WriteLE<uint8_t>(invflag ? 1 : 0);
	file.WriteLE<uint8_t>(chrflag ? 1 : 0);
	file.WriteBE<int32_t>(ActiveMonsterCount);
	file.WriteBE<int32_t>(ActiveItemCount);
	file.WriteBE<int32_t>(ActiveMissileCount);
	file.WriteBE<int32_t>(ActiveObjectCount);

	for (uint8_t i = 0; i < giNumberOfLevels; i++) {
		file.WriteBE<uint32_t>(glSeedTbl[i]);
		file.WriteBE<int32_t>(gnLevelTypeTbl[i]);
	}

	Players[MyPlayerId].pDifficulty = sgGameInitInfo.nDifficulty;
	SavePlayer(&file, MyPlayerId);

	for (int i = 0; i < giNumberQuests; i++)
		SaveQuest(&file, i);
	for (int i = 0; i < MAXPORTAL; i++)
		SavePortal(&file, i);
	for (int monstkill : MonsterKillCounts)
		file.WriteBE<int32_t>(monstkill);

	if (leveltype != DTYPE_TOWN) {
		for (int monsterId : ActiveMonsters)
			file.WriteBE<int32_t>(monsterId);
		for (int i = 0; i < ActiveMonsterCount; i++)
			SaveMonster(&file, ActiveMonsters[i]);
		for (int missileId : ActiveMissiles)
			file.WriteLE<int8_t>(missileId);
		for (int missileId : AvailableMissiles)
			file.WriteLE<int8_t>(missileId);
		for (int i = 0; i < ActiveMissileCount; i++)
			SaveMissile(&file, ActiveMissiles[i]);
		for (int objectId : ActiveObjects)
			file.WriteLE<int8_t>(objectId);
		for (int objectId : AvailableObjects)
			file.WriteLE<int8_t>(objectId);
		for (int i = 0; i < ActiveObjectCount; i++)
			SaveObject(&file, ActiveObjects[i]);

		file.WriteBE<int32_t>(ActiveLightCount);

		for (uint8_t lightId : ActiveLights)
			file.WriteLE<uint8_t>(lightId);
		for (int i = 0; i < ActiveLightCount; i++)
			SaveLighting(&file, &Lights[ActiveLights[i]]);

		file.WriteBE<int32_t>(VisionId);
		file.WriteBE<int32_t>(VisionCount);

		for (int i = 0; i < VisionCount; i++)
			SaveLighting(&file, &VisionList[i]);
	}

	for (int itemId : ActiveItems)
		file.WriteLE<int8_t>(itemId);
	for (int itemId : AvailableItems)
		file.WriteLE<int8_t>(itemId);
	for (int i = 0; i < ActiveItemCount; i++)
		SaveItem(&file, &Items[ActiveItems[i]]);
	for (bool uniqueItemFlag : UniqueItemFlags)
		file.WriteLE<uint8_t>(uniqueItemFlag ? 1 : 0);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<int8_t>(dLight[i][j]);
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<int8_t>(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<int8_t>(dPlayer[i][j]);
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<int8_t>(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteBE<int32_t>(dMonster[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dDead[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dObject[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dLight[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dPreLight[i][j]);
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<uint8_t>(AutomapView[i][j] ? 1 : 0);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dMissile[i][j]);
		}
	}

	file.WriteBE<int32_t>(numpremium);
	file.WriteBE<int32_t>(premiumlevel);

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		SavePremium(&file, i);

	file.WriteLE<uint8_t>(AutomapActive ? 1 : 0);
	file.WriteBE<int32_t>(AutoMapScale);
}

void SaveGame()
{
	gbValidSaveFile = true;
	pfile_write_hero(/*writeGameData=*/true);
}

void SaveLevel()
{
	PFileScopedArchiveWriter scopedWriter;

	DoUnVision(Players[MyPlayerId].position.tile, Players[MyPlayerId]._pLightRad); // fix for vision staying on the level

	if (currlevel == 0)
		glSeedTbl[0] = AdvanceRndSeed();

	char szName[MAX_PATH];
	GetTempLevelNames(szName);
	SaveHelper file(szName, FILEBUFF);

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dDead[i][j]);
		}
	}

	file.WriteBE<int32_t>(ActiveMonsterCount);
	file.WriteBE<int32_t>(ActiveItemCount);
	file.WriteBE<int32_t>(ActiveObjectCount);

	if (leveltype != DTYPE_TOWN) {
		for (int monsterId : ActiveMonsters)
			file.WriteBE<int32_t>(monsterId);
		for (int i = 0; i < ActiveMonsterCount; i++)
			SaveMonster(&file, ActiveMonsters[i]);
		for (int objectId : ActiveObjects)
			file.WriteLE<int8_t>(objectId);
		for (int objectId : AvailableObjects)
			file.WriteLE<int8_t>(objectId);
		for (int i = 0; i < ActiveObjectCount; i++)
			SaveObject(&file, ActiveObjects[i]);
	}

	for (int itemId : ActiveItems)
		file.WriteLE<int8_t>(itemId);
	for (int itemId : AvailableItems)
		file.WriteLE<int8_t>(itemId);

	for (int i = 0; i < ActiveItemCount; i++)
		SaveItem(&file, &Items[ActiveItems[i]]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<int8_t>(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<int8_t>(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteBE<int32_t>(dMonster[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dObject[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dLight[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dPreLight[i][j]);
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<uint8_t>(AutomapView[i][j] ? 1 : 0);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dMissile[i][j]);
		}
	}

	if (!setlevel)
		Players[MyPlayerId]._pLvlVisited[currlevel] = true;
	else
		Players[MyPlayerId]._pSLvlVisited[setlvlnum] = true;
}

void LoadLevel()
{
	char szName[MAX_PATH];
	GetPermLevelNames(szName);
	LoadHelper file(szName);
	if (!file.IsValid())
		app_fatal("%s", _("Unable to open save file archive"));

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dDead[i][j] = file.NextLE<int8_t>();
		}
		SetDead();
	}

	ActiveMonsterCount = file.NextBE<int32_t>();
	ActiveItemCount = file.NextBE<int32_t>();
	ActiveObjectCount = file.NextBE<int32_t>();

	if (leveltype != DTYPE_TOWN) {
		for (int &monsterId : ActiveMonsters)
			monsterId = file.NextBE<int32_t>();
		for (int i = 0; i < ActiveMonsterCount; i++)
			LoadMonster(&file, ActiveMonsters[i]);
		for (int &objectId : ActiveObjects)
			objectId = file.NextLE<int8_t>();
		for (int &objectId : AvailableObjects)
			objectId = file.NextLE<int8_t>();
		for (int i = 0; i < ActiveObjectCount; i++)
			LoadObject(&file, ActiveObjects[i]);
		if (!gbSkipSync) {
			for (int i = 0; i < ActiveObjectCount; i++)
				SyncObjectAnim(Objects[ActiveObjects[i]]);
		}
	}

	for (int &itemId : ActiveItems)
		itemId = file.NextLE<int8_t>();
	for (int &itemId : AvailableItems)
		itemId = file.NextLE<int8_t>();
	for (int i = 0; i < ActiveItemCount; i++)
		LoadItem(&file, ActiveItems[i]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dFlags[i][j] = file.NextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dItem[i][j] = file.NextLE<int8_t>();
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dMonster[i][j] = file.NextBE<int32_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dObject[i][j] = file.NextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dLight[i][j] = file.NextLE<int8_t>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dPreLight[i][j] = file.NextLE<int8_t>();
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) // NOLINT(modernize-loop-convert)
				AutomapView[i][j] = file.NextBool8();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				dMissile[i][j] = 0;           /// BUGFIX: supposed to load saved missiles with "file.NextLE<int8_t>()"?
		}
	}

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		RemoveEmptyLevelItems();
	}

	if (!gbSkipSync) {
		AutomapZoomReset();
		ResyncQuests();
		SyncPortals();
		UpdateLighting = true;
	}

	for (auto &player : Players) {
		if (player.plractive && currlevel == player.plrlevel)
			Lights[player._plid]._lunflag = true;
	}
}

} // namespace devilution

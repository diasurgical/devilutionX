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
#include "pfile.h"
#include "stores.h"
#include "utils/endian.hpp"
#include "utils/language.h"
#include "utils/mpq_writer.hpp"

namespace devilution {

bool gbIsHellfireSaveGame;
uint8_t giNumberOfLevels;

namespace {

uint8_t giNumberQuests;
uint8_t giNumberOfSmithPremiumItems;

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
		CurrentSaveArchive().WriteFile(m_szFileName_, m_buffer_.get(), encodedLen);
	}
};

void LoadItemData(LoadHelper &file, Item &item)
{
	item._iSeed = file.NextLE<int32_t>();
	item._iCreateInfo = file.NextLE<uint16_t>();
	file.Skip(2); // Alignment
	item._itype = static_cast<ItemType>(file.NextLE<uint32_t>());
	item.position.x = file.NextLE<int32_t>();
	item.position.y = file.NextLE<int32_t>();
	item._iAnimFlag = file.NextBool32();
	file.Skip(4); // Skip pointer _iAnimData
	item.AnimInfo = {};
	item.AnimInfo.NumberOfFrames = file.NextLE<int32_t>();
	item.AnimInfo.CurrentFrame = file.NextLE<int32_t>();
	file.Skip(8); // Skip _iAnimWidth and _iAnimWidth2
	file.Skip(4); // Unused since 1.02
	item._iSelFlag = file.NextLE<uint8_t>();
	file.Skip(3); // Alignment
	item._iPostDraw = file.NextBool32();
	item._iIdentified = file.NextBool32();
	item._iMagical = static_cast<item_quality>(file.NextLE<int8_t>());
	file.NextBytes(item._iName, 64);
	file.NextBytes(item._iIName, 64);
	item._iLoc = static_cast<item_equip_type>(file.NextLE<int8_t>());
	item._iClass = static_cast<item_class>(file.NextLE<uint8_t>());
	file.Skip(1); // Alignment
	item._iCurs = file.NextLE<int32_t>();
	item._ivalue = file.NextLE<int32_t>();
	item._iIvalue = file.NextLE<int32_t>();
	item._iMinDam = file.NextLE<int32_t>();
	item._iMaxDam = file.NextLE<int32_t>();
	item._iAC = file.NextLE<int32_t>();
	item._iFlags = file.NextLE<uint32_t>();
	item._iMiscId = static_cast<item_misc_id>(file.NextLE<int32_t>());
	item._iSpell = static_cast<spell_id>(file.NextLE<int32_t>());
	item._iCharges = file.NextLE<int32_t>();
	item._iMaxCharges = file.NextLE<int32_t>();
	item._iDurability = file.NextLE<int32_t>();
	item._iMaxDur = file.NextLE<int32_t>();
	item._iPLDam = file.NextLE<int32_t>();
	item._iPLToHit = file.NextLE<int32_t>();
	item._iPLAC = file.NextLE<int32_t>();
	item._iPLStr = file.NextLE<int32_t>();
	item._iPLMag = file.NextLE<int32_t>();
	item._iPLDex = file.NextLE<int32_t>();
	item._iPLVit = file.NextLE<int32_t>();
	item._iPLFR = file.NextLE<int32_t>();
	item._iPLLR = file.NextLE<int32_t>();
	item._iPLMR = file.NextLE<int32_t>();
	item._iPLMana = file.NextLE<int32_t>();
	item._iPLHP = file.NextLE<int32_t>();
	item._iPLDamMod = file.NextLE<int32_t>();
	item._iPLGetHit = file.NextLE<int32_t>();
	item._iPLLight = file.NextLE<int32_t>();
	item._iSplLvlAdd = file.NextLE<int8_t>();
	item._iRequest = file.NextBool8();
	file.Skip(2); // Alignment
	item._iUid = file.NextLE<int32_t>();
	item._iFMinDam = file.NextLE<int32_t>();
	item._iFMaxDam = file.NextLE<int32_t>();
	item._iLMinDam = file.NextLE<int32_t>();
	item._iLMaxDam = file.NextLE<int32_t>();
	item._iPLEnAc = file.NextLE<int32_t>();
	item._iPrePower = static_cast<item_effect_type>(file.NextLE<int8_t>());
	item._iSufPower = static_cast<item_effect_type>(file.NextLE<int8_t>());
	file.Skip(2); // Alignment
	item._iVAdd1 = file.NextLE<int32_t>();
	item._iVMult1 = file.NextLE<int32_t>();
	item._iVAdd2 = file.NextLE<int32_t>();
	item._iVMult2 = file.NextLE<int32_t>();
	item._iMinStr = file.NextLE<int8_t>();
	item._iMinMag = file.NextLE<uint8_t>();
	item._iMinDex = file.NextLE<int8_t>();
	file.Skip(1); // Alignment
	item._iStatFlag = file.NextBool32();
	item.IDidx = static_cast<_item_indexes>(file.NextLE<int32_t>());
	if (gbIsSpawn) {
		item.IDidx = RemapItemIdxFromSpawn(item.IDidx);
	}
	if (!gbIsHellfireSaveGame) {
		item.IDidx = RemapItemIdxFromDiablo(item.IDidx);
	}
	item.dwBuff = file.NextLE<uint32_t>();
	if (gbIsHellfireSaveGame)
		item._iDamAcFlags = file.NextLE<uint32_t>();
	else
		item._iDamAcFlags = 0;

	RemoveInvalidItem(item);
}

void LoadPlayer(LoadHelper &file, Player &player)
{
	player._pmode = static_cast<PLR_MODE>(file.NextLE<int32_t>());

	for (int8_t &step : player.walkpath) {
		step = file.NextLE<int8_t>();
	}
	player.plractive = file.NextBool8();
	file.Skip(2); // Alignment
	player.destAction = static_cast<action_id>(file.NextLE<int32_t>());
	player.destParam1 = file.NextLE<int32_t>();
	player.destParam2 = file.NextLE<int32_t>();
	player.destParam3 = file.NextLE<int32_t>();
	player.destParam4 = file.NextLE<int32_t>();
	player.plrlevel = file.NextLE<uint32_t>();
	player.position.tile.x = file.NextLE<int32_t>();
	player.position.tile.y = file.NextLE<int32_t>();
	player.position.future.x = file.NextLE<int32_t>();
	player.position.future.y = file.NextLE<int32_t>();
	file.Skip(8); // Skip _ptargx and _ptargy
	player.position.last.x = file.NextLE<int32_t>();
	player.position.last.y = file.NextLE<int32_t>();
	player.position.old.x = file.NextLE<int32_t>();
	player.position.old.y = file.NextLE<int32_t>();
	player.position.offset.deltaX = file.NextLE<int32_t>();
	player.position.offset.deltaY = file.NextLE<int32_t>();
	player.position.velocity.deltaX = file.NextLE<int32_t>();
	player.position.velocity.deltaY = file.NextLE<int32_t>();
	player._pdir = static_cast<Direction>(file.NextLE<int32_t>());
	file.Skip(4); // Unused
	player._pgfxnum = file.NextLE<int32_t>();
	file.Skip(4); // Skip pointer pData
	player.AnimInfo = {};
	player.AnimInfo.TicksPerFrame = file.NextLE<int32_t>() + 1;
	player.AnimInfo.TickCounterOfCurrentFrame = file.NextLE<int32_t>();
	player.AnimInfo.NumberOfFrames = file.NextLE<int32_t>();
	player.AnimInfo.CurrentFrame = file.NextLE<int32_t>();
	file.Skip(4); // Skip _pAnimWidth
	file.Skip(4); // Skip _pAnimWidth2
	file.Skip(4); // Skip _peflag
	player._plid = file.NextLE<int32_t>();
	player._pvid = file.NextLE<int32_t>();

	player._pSpell = static_cast<spell_id>(file.NextLE<int32_t>());
	player._pSplType = static_cast<spell_type>(file.NextLE<int8_t>());
	player._pSplFrom = file.NextLE<int8_t>();
	file.Skip(2); // Alignment
	player._pTSpell = static_cast<spell_id>(file.NextLE<int32_t>());
	file.Skip<int8_t>(); // Skip _pTSplType
	file.Skip(3);        // Alignment
	player._pRSpell = static_cast<spell_id>(file.NextLE<int32_t>());
	player._pRSplType = static_cast<spell_type>(file.NextLE<int8_t>());
	file.Skip(3); // Alignment
	player._pSBkSpell = static_cast<spell_id>(file.NextLE<int32_t>());
	file.Skip<int8_t>(); // Skip _pSBkSplType
	for (int8_t &spellLevel : player._pSplLvl)
		spellLevel = file.NextLE<int8_t>();
	file.Skip(7); // Alignment
	player._pMemSpells = file.NextLE<uint64_t>();
	player._pAblSpells = file.NextLE<uint64_t>();
	player._pScrlSpells = file.NextLE<uint64_t>();
	player._pSpellFlags = file.NextLE<uint8_t>();
	file.Skip(3); // Alignment

	for (auto &spell : player._pSplHotKey)
		spell = static_cast<spell_id>(file.NextLE<int32_t>());

	for (auto &spellType : player._pSplTHotKey)
		spellType = static_cast<spell_type>(file.NextLE<int8_t>());

	file.Skip<int32_t>(); // Skip _pwtype
	player._pBlockFlag = file.NextBool8();
	player._pInvincible = file.NextBool8();
	player._pLightRad = file.NextLE<int8_t>();
	player._pLvlChanging = file.NextBool8();

	file.NextBytes(player._pName, PLR_NAME_LEN);
	player._pClass = static_cast<HeroClass>(file.NextLE<int8_t>());
	file.Skip(3); // Alignment
	player._pStrength = file.NextLE<int32_t>();
	player._pBaseStr = file.NextLE<int32_t>();
	player._pMagic = file.NextLE<int32_t>();
	player._pBaseMag = file.NextLE<int32_t>();
	player._pDexterity = file.NextLE<int32_t>();
	player._pBaseDex = file.NextLE<int32_t>();
	player._pVitality = file.NextLE<int32_t>();
	player._pBaseVit = file.NextLE<int32_t>();
	player._pStatPts = file.NextLE<int32_t>();
	player._pDamageMod = file.NextLE<int32_t>();
	player._pBaseToBlk = file.NextLE<int32_t>();
	if (player._pBaseToBlk == 0)
		player._pBaseToBlk = BlockBonuses[static_cast<std::size_t>(player._pClass)];
	player._pHPBase = file.NextLE<int32_t>();
	player._pMaxHPBase = file.NextLE<int32_t>();
	player._pHitPoints = file.NextLE<int32_t>();
	player._pMaxHP = file.NextLE<int32_t>();
	file.Skip(sizeof(int32_t)); // Skip _pHPPer - always derived from hp and maxHP.
	player._pManaBase = file.NextLE<int32_t>();
	player._pMaxManaBase = file.NextLE<int32_t>();
	player._pMana = file.NextLE<int32_t>();
	player._pMaxMana = file.NextLE<int32_t>();
	file.Skip(sizeof(int32_t)); // Skip _pManaPer - always derived from mana and maxMana
	player._pLevel = file.NextLE<int8_t>();
	player._pMaxLvl = file.NextLE<int8_t>();
	file.Skip(2); // Alignment
	player._pExperience = file.NextLE<uint32_t>();
	file.Skip<uint32_t>();                        // Skip _pMaxExp - unused
	player._pNextExper = file.NextLE<uint32_t>(); // This can be calculated based on pLevel (which in turn could be calculated based on pExperience)
	player._pArmorClass = file.NextLE<int8_t>();
	player._pMagResist = file.NextLE<int8_t>();
	player._pFireResist = file.NextLE<int8_t>();
	player._pLghtResist = file.NextLE<int8_t>();
	player._pGold = file.NextLE<int32_t>();

	player._pInfraFlag = file.NextBool32();
	player.position.temp.x = file.NextLE<int32_t>();
	player.position.temp.y = file.NextLE<int32_t>();
	player.tempDirection = static_cast<Direction>(file.NextLE<int32_t>());
	player.spellLevel = file.NextLE<int32_t>();
	file.Skip(4); // skip _pVar5, was used for storing position of a tile which should have its HorizontalMovingPlayer flag removed after walking
	player.position.offset2.deltaX = file.NextLE<int32_t>();
	player.position.offset2.deltaY = file.NextLE<int32_t>();
	file.Skip(4); // Skip actionFrame

	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		player._pLvlVisited[i] = file.NextBool8();

	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		player._pSLvlVisited[i] = file.NextBool8();

	file.Skip(2);     // Alignment
	file.Skip(4);     // skip _pGFXLoad
	file.Skip(4 * 8); // Skip pointers _pNAnim
	player._pNFrames = file.NextLE<int32_t>();
	file.Skip(4);     // skip _pNWidth
	file.Skip(4 * 8); // Skip pointers _pWAnim
	player._pWFrames = file.NextLE<int32_t>();
	file.Skip(4);     // skip _pWWidth
	file.Skip(4 * 8); // Skip pointers _pAAnim
	player._pAFrames = file.NextLE<int32_t>();
	file.Skip(4); // skip _pAWidth
	player._pAFNum = file.NextLE<int32_t>();
	file.Skip(4 * 8); // Skip pointers _pLAnim
	file.Skip(4 * 8); // Skip pointers _pFAnim
	file.Skip(4 * 8); // Skip pointers _pTAnim
	player._pSFrames = file.NextLE<int32_t>();
	file.Skip(4); // skip _pSWidth
	player._pSFNum = file.NextLE<int32_t>();
	file.Skip(4 * 8); // Skip pointers _pHAnim
	player._pHFrames = file.NextLE<int32_t>();
	file.Skip(4);     // skip _pHWidth
	file.Skip(4 * 8); // Skip pointers _pDAnim
	player._pDFrames = file.NextLE<int32_t>();
	file.Skip(4);     // skip _pDWidth
	file.Skip(4 * 8); // Skip pointers _pBAnim
	player._pBFrames = file.NextLE<int32_t>();
	file.Skip(4); // skip _pBWidth

	for (Item &item : player.InvBody)
		LoadItemData(file, item);

	for (Item &item : player.InvList)
		LoadItemData(file, item);

	player._pNumInv = file.NextLE<int32_t>();

	for (int8_t &cell : player.InvGrid)
		cell = file.NextLE<int8_t>();

	for (Item &item : player.SpdList)
		LoadItemData(file, item);

	LoadItemData(file, player.HoldItem);

	player._pIMinDam = file.NextLE<int32_t>();
	player._pIMaxDam = file.NextLE<int32_t>();
	player._pIAC = file.NextLE<int32_t>();
	player._pIBonusDam = file.NextLE<int32_t>();
	player._pIBonusToHit = file.NextLE<int32_t>();
	player._pIBonusAC = file.NextLE<int32_t>();
	player._pIBonusDamMod = file.NextLE<int32_t>();
	file.Skip(4); // Alignment

	player._pISpells = file.NextLE<uint64_t>();
	player._pIFlags = file.NextLE<int32_t>();
	player._pIGetHit = file.NextLE<int32_t>();
	player._pISplLvlAdd = file.NextLE<int8_t>();
	file.Skip(1); // Unused
	file.Skip(2); // Alignment
	player._pISplDur = file.NextLE<int32_t>();
	player._pIEnAc = file.NextLE<int32_t>();
	player._pIFMinDam = file.NextLE<int32_t>();
	player._pIFMaxDam = file.NextLE<int32_t>();
	player._pILMinDam = file.NextLE<int32_t>();
	player._pILMaxDam = file.NextLE<int32_t>();
	player._pOilType = static_cast<item_misc_id>(file.NextLE<int32_t>());
	player.pTownWarps = file.NextLE<uint8_t>();
	player.pDungMsgs = file.NextLE<uint8_t>();
	player.pLvlLoad = file.NextLE<uint8_t>();

	if (gbIsHellfireSaveGame) {
		player.pDungMsgs2 = file.NextLE<uint8_t>();
		player.pBattleNet = false;
	} else {
		player.pDungMsgs2 = 0;
		player.pBattleNet = file.NextBool8();
	}
	player.pManaShield = file.NextBool8();
	if (gbIsHellfireSaveGame) {
		player.pOriginalCathedral = file.NextBool8();
	} else {
		file.Skip(1);
		player.pOriginalCathedral = true;
	}
	file.Skip(2); // Available bytes
	player.wReflections = file.NextLE<uint16_t>();
	file.Skip(14); // Available bytes

	player.pDiabloKillLevel = file.NextLE<uint32_t>();
	player.pDifficulty = static_cast<_difficulty>(file.NextLE<uint32_t>());
	player.pDamAcFlags = file.NextLE<uint32_t>();
	file.Skip(20); // Available bytes
	CalcPlrItemVals(player, false);

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

void LoadMonster(LoadHelper *file, Monster &monster)
{
	monster._mMTidx = file->NextLE<int32_t>();
	monster._mmode = static_cast<MonsterMode>(file->NextLE<int32_t>());
	monster._mgoal = static_cast<monster_goal>(file->NextLE<uint8_t>());
	file->Skip(3); // Alignment
	monster._mgoalvar1 = file->NextLE<int32_t>();
	monster._mgoalvar2 = file->NextLE<int32_t>();
	monster._mgoalvar3 = file->NextLE<int32_t>();
	file->Skip(4); // Unused
	monster._pathcount = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	monster.position.tile.x = file->NextLE<int32_t>();
	monster.position.tile.y = file->NextLE<int32_t>();
	monster.position.future.x = file->NextLE<int32_t>();
	monster.position.future.y = file->NextLE<int32_t>();
	monster.position.old.x = file->NextLE<int32_t>();
	monster.position.old.y = file->NextLE<int32_t>();
	monster.position.offset.deltaX = file->NextLE<int32_t>();
	monster.position.offset.deltaY = file->NextLE<int32_t>();
	monster.position.velocity.deltaX = file->NextLE<int32_t>();
	monster.position.velocity.deltaY = file->NextLE<int32_t>();
	monster._mdir = static_cast<Direction>(file->NextLE<int32_t>());
	monster._menemy = file->NextLE<int32_t>();
	monster.enemyPosition.x = file->NextLE<uint8_t>();
	monster.enemyPosition.y = file->NextLE<uint8_t>();
	file->Skip(2); // Unused

	file->Skip(4); // Skip pointer _mAnimData
	monster.AnimInfo = {};
	monster.AnimInfo.TicksPerFrame = file->NextLE<int32_t>();
	monster.AnimInfo.TickCounterOfCurrentFrame = file->NextLE<int32_t>();
	monster.AnimInfo.NumberOfFrames = file->NextLE<int32_t>();
	monster.AnimInfo.CurrentFrame = file->NextLE<int32_t>();
	file->Skip(4); // Skip _meflag
	monster._mDelFlag = file->NextBool32();
	monster._mVar1 = file->NextLE<int32_t>();
	monster._mVar2 = file->NextLE<int32_t>();
	monster._mVar3 = file->NextLE<int32_t>();
	monster.position.temp.x = file->NextLE<int32_t>();
	monster.position.temp.y = file->NextLE<int32_t>();
	monster.position.offset2.deltaX = file->NextLE<int32_t>();
	monster.position.offset2.deltaY = file->NextLE<int32_t>();
	file->Skip(4); // Skip actionFrame
	monster._mmaxhp = file->NextLE<int32_t>();
	monster._mhitpoints = file->NextLE<int32_t>();

	monster._mAi = static_cast<_mai_id>(file->NextLE<uint8_t>());
	monster._mint = file->NextLE<uint8_t>();
	file->Skip(2); // Alignment
	monster._mFlags = file->NextLE<uint32_t>();
	monster._msquelch = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	file->Skip(4); // Unused
	monster.position.last.x = file->NextLE<int32_t>();
	monster.position.last.y = file->NextLE<int32_t>();
	monster._mRndSeed = file->NextLE<uint32_t>();
	monster._mAISeed = file->NextLE<uint32_t>();
	file->Skip(4); // Unused

	monster._uniqtype = file->NextLE<uint8_t>();
	monster._uniqtrans = file->NextLE<uint8_t>();
	monster._udeadval = file->NextLE<int8_t>();

	monster.mWhoHit = file->NextLE<int8_t>();
	monster.mLevel = file->NextLE<int8_t>();
	file->Skip(1); // Alignment
	monster.mExp = file->NextLE<uint16_t>();

	if ((monster._mFlags & MFLAG_GOLEM) != 0) // Don't skip for golems
		monster.mHit = file->NextLE<uint8_t>();
	else
		file->Skip(1); // Skip mHit as it's already initialized
	monster.mMinDamage = file->NextLE<uint8_t>();
	monster.mMaxDamage = file->NextLE<uint8_t>();
	file->Skip(1); // Skip mHit2 as it's already initialized
	monster.mMinDamage2 = file->NextLE<uint8_t>();
	monster.mMaxDamage2 = file->NextLE<uint8_t>();
	monster.mArmorClass = file->NextLE<uint8_t>();
	file->Skip(1); // Alignment
	monster.mMagicRes = file->NextLE<uint16_t>();
	file->Skip(2); // Alignment

	monster.mtalkmsg = static_cast<_speech_id>(file->NextLE<int32_t>());
	if (monster.mtalkmsg == TEXT_KING1) // Fix original bad mapping of NONE for monsters
		monster.mtalkmsg = TEXT_NONE;
	monster.leader = file->NextLE<uint8_t>();
	monster.leaderRelation = static_cast<LeaderRelation>(file->NextLE<uint8_t>());
	monster.packsize = file->NextLE<uint8_t>();
	monster.mlid = file->NextLE<int8_t>();
	if (monster.mlid == 0)
		monster.mlid = NO_LIGHT; // Correct incorect values in old saves

	if ((monster._mFlags & MFLAG_BERSERK) != 0) {
		int lightRadius = (currlevel < 17 || currlevel > 20) ? 3 : 9;
		monster.mlid = AddLight(monster.position.tile, lightRadius);
	}

	// Omit pointer mName;
	// Omit pointer MType;
	// Omit pointer MData;

	if (gbSkipSync)
		return;

	SyncMonsterAnim(monster);
}

/**
 * @brief Recalculate the pack size of monster group that may have underflown
 */
void SyncPackSize(Monster &leader)
{
	if (leader._uniqtype == 0)
		return;
	if (leader._mAi != AI_SCAV)
		return;

	leader.packsize = 0;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		auto &minion = Monsters[ActiveMonsters[i]];
		if (minion.leaderRelation == LeaderRelation::Leashed && &Monsters[minion.leader] == &leader)
			leader.packsize++;
	}
}

void LoadMissile(LoadHelper *file, Missile &missile)
{
	missile._mitype = static_cast<missile_id>(file->NextLE<int32_t>());
	missile.position.tile.x = file->NextLE<int32_t>();
	missile.position.tile.y = file->NextLE<int32_t>();
	missile.position.offset.deltaX = file->NextLE<int32_t>();
	missile.position.offset.deltaY = file->NextLE<int32_t>();
	missile.position.velocity.deltaX = file->NextLE<int32_t>();
	missile.position.velocity.deltaY = file->NextLE<int32_t>();
	missile.position.start.x = file->NextLE<int32_t>();
	missile.position.start.y = file->NextLE<int32_t>();
	missile.position.traveled.deltaX = file->NextLE<int32_t>();
	missile.position.traveled.deltaY = file->NextLE<int32_t>();
	missile._mimfnum = file->NextLE<int32_t>();
	missile._mispllvl = file->NextLE<int32_t>();
	missile._miDelFlag = file->NextBool32();
	missile._miAnimType = file->NextLE<uint8_t>();
	file->Skip(3); // Alignment
	missile._miAnimFlags = static_cast<MissileDataFlags>(file->NextLE<int32_t>());
	file->Skip(4); // Skip pointer _miAnimData
	missile._miAnimDelay = file->NextLE<int32_t>();
	missile._miAnimLen = file->NextLE<int32_t>();
	missile._miAnimWidth = file->NextLE<int32_t>();
	missile._miAnimWidth2 = file->NextLE<int32_t>();
	missile._miAnimCnt = file->NextLE<int32_t>();
	missile._miAnimAdd = file->NextLE<int32_t>();
	missile._miAnimFrame = file->NextLE<int32_t>();
	missile._miDrawFlag = file->NextBool32();
	missile._miLightFlag = file->NextBool32();
	missile._miPreFlag = file->NextBool32();
	missile._miUniqTrans = file->NextLE<uint32_t>();
	missile._mirange = file->NextLE<int32_t>();
	missile._misource = file->NextLE<int32_t>();
	missile._micaster = static_cast<mienemy_type>(file->NextLE<int32_t>());
	missile._midam = file->NextLE<int32_t>();
	missile._miHitFlag = file->NextBool32();
	missile._midist = file->NextLE<int32_t>();
	missile._mlid = file->NextLE<int32_t>();
	missile._mirnd = file->NextLE<int32_t>();
	missile.var1 = file->NextLE<int32_t>();
	missile.var2 = file->NextLE<int32_t>();
	missile.var3 = file->NextLE<int32_t>();
	missile.var4 = file->NextLE<int32_t>();
	missile.var5 = file->NextLE<int32_t>();
	missile.var6 = file->NextLE<int32_t>();
	missile.var7 = file->NextLE<int32_t>();
	missile.limitReached = file->NextBool32();
	missile.lastCollisionTargetHash = 0;
}

void LoadObject(LoadHelper &file, Object &object)
{
	object._otype = static_cast<_object_id>(file.NextLE<int32_t>());
	object.position.x = file.NextLE<int32_t>();
	object.position.y = file.NextLE<int32_t>();
	object._oLight = file.NextBool32();
	object._oAnimFlag = file.NextLE<uint32_t>();
	file.Skip(4); // Skip pointer _oAnimData
	object._oAnimDelay = file.NextLE<int32_t>();
	object._oAnimCnt = file.NextLE<int32_t>();
	object._oAnimLen = file.NextLE<uint32_t>();
	object._oAnimFrame = file.NextLE<uint32_t>();
	object._oAnimWidth = file.NextLE<int32_t>();
	file.Skip(4); // Skip _oAnimWidth2
	object._oDelFlag = file.NextBool32();
	object._oBreak = file.NextLE<int8_t>();
	file.Skip(3); // Alignment
	object._oSolidFlag = file.NextBool32();
	object._oMissFlag = file.NextBool32();

	object._oSelFlag = file.NextLE<int8_t>();
	file.Skip(3); // Alignment
	object._oPreFlag = file.NextBool32();
	object._oTrapFlag = file.NextBool32();
	object._oDoorFlag = file.NextBool32();
	object._olid = file.NextLE<int32_t>();
	object._oRndSeed = file.NextLE<uint32_t>();
	object._oVar1 = file.NextLE<int32_t>();
	object._oVar2 = file.NextLE<int32_t>();
	object._oVar3 = file.NextLE<int32_t>();
	object._oVar4 = file.NextLE<int32_t>();
	object._oVar5 = file.NextLE<int32_t>();
	object._oVar6 = file.NextLE<uint32_t>();
	object.bookMessage = static_cast<_speech_id>(file.NextLE<int32_t>());
	object._oVar8 = file.NextLE<int32_t>();
}

void LoadItem(LoadHelper &file, Item &item)
{
	LoadItemData(file, item);
	GetItemFrm(item);
}

void LoadPremium(LoadHelper &file, int i)
{
	LoadItemData(file, premiumitems[i]);
}

void LoadQuest(LoadHelper *file, int i)
{
	auto &quest = Quests[i];

	quest._qlevel = file->NextLE<uint8_t>();
	file->Skip<uint8_t>(); // _qtype, identical to _qidx
	quest._qactive = static_cast<quest_state>(file->NextLE<uint8_t>());
	quest._qlvltype = static_cast<dungeon_type>(file->NextLE<uint8_t>());
	quest.position.x = file->NextLE<int32_t>();
	quest.position.y = file->NextLE<int32_t>();
	quest._qslvl = static_cast<_setlevels>(file->NextLE<uint8_t>());
	quest._qidx = static_cast<quest_id>(file->NextLE<uint8_t>());
	if (gbIsHellfireSaveGame) {
		file->Skip(2); // Alignment
		quest._qmsg = static_cast<_speech_id>(file->NextLE<int32_t>());
	} else {
		quest._qmsg = static_cast<_speech_id>(file->NextLE<uint8_t>());
	}
	quest._qvar1 = file->NextLE<uint8_t>();
	quest._qvar2 = file->NextLE<uint8_t>();
	file->Skip(2); // Alignment
	if (!gbIsHellfireSaveGame)
		file->Skip(1); // Alignment
	quest._qlog = file->NextBool32();

	ReturnLvlPosition.x = file->NextBE<int32_t>();
	ReturnLvlPosition.y = file->NextBE<int32_t>();
	ReturnLevel = file->NextBE<int32_t>();
	ReturnLevelType = static_cast<dungeon_type>(file->NextBE<int32_t>());
	file->Skip(sizeof(int32_t)); // Skip DoomQuestState
}

void LoadLighting(LoadHelper *file, Light *pLight)
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

void LoadPortal(LoadHelper *file, int i)
{
	Portal *pPortal = &Portals[i];

	pPortal->open = file->NextBool32();
	pPortal->position.x = file->NextLE<int32_t>();
	pPortal->position.y = file->NextLE<int32_t>();
	pPortal->level = file->NextLE<int32_t>();
	pPortal->ltype = static_cast<dungeon_type>(file->NextLE<int32_t>());
	pPortal->setlvl = file->NextBool32();
}

void ConvertLevels()
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

void LoadMatchingItems(LoadHelper &file, const int n, Item *pItem)
{
	Item tempItem;

	for (int i = 0; i < n; i++) {
		LoadItemData(file, tempItem);
		if (pItem[i].isEmpty() || tempItem.isEmpty())
			continue;
		if (pItem[i]._iSeed != tempItem._iSeed)
			continue;
		pItem[i] = tempItem;
	}
}

void RemoveEmptyLevelItems()
{
	for (int i = ActiveItemCount; i > 0; i--) {
		int ii = ActiveItems[i];
		auto &item = Items[ii];
		if (item.isEmpty()) {
			dItem[item.position.x][item.position.y] = 0;
			DeleteItem(ii, i);
		}
	}
}

void SaveItem(SaveHelper &file, const Item &item)
{
	auto idx = item.IDidx;
	if (!gbIsHellfire)
		idx = RemapItemIdxToDiablo(idx);
	if (gbIsSpawn)
		idx = RemapItemIdxToSpawn(idx);
	ItemType iType = item._itype;
	if (idx == -1) {
		idx = _item_indexes::IDI_GOLD;
		iType = ItemType::None;
	}

	file.WriteLE<int32_t>(item._iSeed);
	file.WriteLE<int16_t>(item._iCreateInfo);
	file.Skip(2); // Alignment
	file.WriteLE<int32_t>(static_cast<int32_t>(iType));
	file.WriteLE<int32_t>(item.position.x);
	file.WriteLE<int32_t>(item.position.y);
	file.WriteLE<uint32_t>(item._iAnimFlag ? 1 : 0);
	file.Skip(4); // Skip pointer _iAnimData
	file.WriteLE<int32_t>(item.AnimInfo.NumberOfFrames);
	file.WriteLE<int32_t>(item.AnimInfo.CurrentFrame);
	// write _iAnimWidth for vanilla compatibility
	file.WriteLE<int32_t>(ItemAnimWidth);
	// write _iAnimWidth2 for vanilla compatibility
	file.WriteLE<int32_t>(CalculateWidth2(ItemAnimWidth));
	file.Skip<uint32_t>(); // _delFlag, unused since 1.02
	file.WriteLE<uint8_t>(item._iSelFlag);
	file.Skip(3); // Alignment
	file.WriteLE<uint32_t>(item._iPostDraw ? 1 : 0);
	file.WriteLE<uint32_t>(item._iIdentified ? 1 : 0);
	file.WriteLE<int8_t>(item._iMagical);
	file.WriteBytes(item._iName, 64);
	file.WriteBytes(item._iIName, 64);
	file.WriteLE<int8_t>(item._iLoc);
	file.WriteLE<uint8_t>(item._iClass);
	file.Skip(1); // Alignment
	file.WriteLE<int32_t>(item._iCurs);
	file.WriteLE<int32_t>(item._ivalue);
	file.WriteLE<int32_t>(item._iIvalue);
	file.WriteLE<int32_t>(item._iMinDam);
	file.WriteLE<int32_t>(item._iMaxDam);
	file.WriteLE<int32_t>(item._iAC);
	file.WriteLE<uint32_t>(item._iFlags);
	file.WriteLE<int32_t>(item._iMiscId);
	file.WriteLE<int32_t>(item._iSpell);
	file.WriteLE<int32_t>(item._iCharges);
	file.WriteLE<int32_t>(item._iMaxCharges);
	file.WriteLE<int32_t>(item._iDurability);
	file.WriteLE<int32_t>(item._iMaxDur);
	file.WriteLE<int32_t>(item._iPLDam);
	file.WriteLE<int32_t>(item._iPLToHit);
	file.WriteLE<int32_t>(item._iPLAC);
	file.WriteLE<int32_t>(item._iPLStr);
	file.WriteLE<int32_t>(item._iPLMag);
	file.WriteLE<int32_t>(item._iPLDex);
	file.WriteLE<int32_t>(item._iPLVit);
	file.WriteLE<int32_t>(item._iPLFR);
	file.WriteLE<int32_t>(item._iPLLR);
	file.WriteLE<int32_t>(item._iPLMR);
	file.WriteLE<int32_t>(item._iPLMana);
	file.WriteLE<int32_t>(item._iPLHP);
	file.WriteLE<int32_t>(item._iPLDamMod);
	file.WriteLE<int32_t>(item._iPLGetHit);
	file.WriteLE<int32_t>(item._iPLLight);
	file.WriteLE<int8_t>(item._iSplLvlAdd);
	file.WriteLE<int8_t>(item._iRequest ? 1 : 0);
	file.Skip(2); // Alignment
	file.WriteLE<int32_t>(item._iUid);
	file.WriteLE<int32_t>(item._iFMinDam);
	file.WriteLE<int32_t>(item._iFMaxDam);
	file.WriteLE<int32_t>(item._iLMinDam);
	file.WriteLE<int32_t>(item._iLMaxDam);
	file.WriteLE<int32_t>(item._iPLEnAc);
	file.WriteLE<int8_t>(item._iPrePower);
	file.WriteLE<int8_t>(item._iSufPower);
	file.Skip(2); // Alignment
	file.WriteLE<int32_t>(item._iVAdd1);
	file.WriteLE<int32_t>(item._iVMult1);
	file.WriteLE<int32_t>(item._iVAdd2);
	file.WriteLE<int32_t>(item._iVMult2);
	file.WriteLE<int8_t>(item._iMinStr);
	file.WriteLE<uint8_t>(item._iMinMag);
	file.WriteLE<int8_t>(item._iMinDex);
	file.Skip(1); // Alignment
	file.WriteLE<uint32_t>(item._iStatFlag ? 1 : 0);
	file.WriteLE<int32_t>(idx);
	file.WriteLE<uint32_t>(item.dwBuff);
	if (gbIsHellfire)
		file.WriteLE<uint32_t>(item._iDamAcFlags);
}

void SavePlayer(SaveHelper &file, const Player &player)
{
	file.WriteLE<int32_t>(player._pmode);
	for (int8_t step : player.walkpath)
		file.WriteLE<int8_t>(step);
	file.WriteLE<uint8_t>(player.plractive ? 1 : 0);
	file.Skip(2); // Alignment
	file.WriteLE<int32_t>(player.destAction);
	file.WriteLE<int32_t>(player.destParam1);
	file.WriteLE<int32_t>(player.destParam2);
	file.WriteLE<int32_t>(static_cast<int32_t>(player.destParam3));
	file.WriteLE<int32_t>(player.destParam4);
	file.WriteLE<uint32_t>(player.plrlevel);
	file.WriteLE<int32_t>(player.position.tile.x);
	file.WriteLE<int32_t>(player.position.tile.y);
	file.WriteLE<int32_t>(player.position.future.x);
	file.WriteLE<int32_t>(player.position.future.y);

	// For backwards compatibility
	const Point target = player.GetTargetPosition();
	file.WriteLE<int32_t>(target.x);
	file.WriteLE<int32_t>(target.y);

	file.WriteLE<int32_t>(player.position.last.x);
	file.WriteLE<int32_t>(player.position.last.y);
	file.WriteLE<int32_t>(player.position.old.x);
	file.WriteLE<int32_t>(player.position.old.y);
	file.WriteLE<int32_t>(player.position.offset.deltaX);
	file.WriteLE<int32_t>(player.position.offset.deltaY);
	file.WriteLE<int32_t>(player.position.velocity.deltaX);
	file.WriteLE<int32_t>(player.position.velocity.deltaY);
	file.WriteLE<int32_t>(static_cast<int32_t>(player._pdir));
	file.Skip(4); // Unused
	file.WriteLE<int32_t>(player._pgfxnum);
	file.Skip(4); // Skip pointer _pAnimData
	file.WriteLE<int32_t>(std::max(0, player.AnimInfo.TicksPerFrame - 1));
	file.WriteLE<int32_t>(player.AnimInfo.TickCounterOfCurrentFrame);
	file.WriteLE<int32_t>(player.AnimInfo.NumberOfFrames);
	file.WriteLE<int32_t>(player.AnimInfo.CurrentFrame);
	// write _pAnimWidth for vanilla compatibility
	int animWidth = player.AnimInfo.pCelSprite == nullptr ? 96 : player.AnimInfo.pCelSprite->Width();
	file.WriteLE<int32_t>(animWidth);
	// write _pAnimWidth2 for vanilla compatibility
	file.WriteLE<int32_t>(CalculateWidth2(animWidth));
	file.Skip<uint32_t>(); // Skip _peflag
	file.WriteLE<int32_t>(player._plid);
	file.WriteLE<int32_t>(player._pvid);

	file.WriteLE<int32_t>(player._pSpell);
	file.WriteLE<int8_t>(player._pSplType);
	file.WriteLE<int8_t>(player._pSplFrom);
	file.Skip(2); // Alignment
	file.WriteLE<int32_t>(player._pTSpell);
	file.Skip<int8_t>(); // Skip _pTSplType
	file.Skip(3);        // Alignment
	file.WriteLE<int32_t>(player._pRSpell);
	file.WriteLE<int8_t>(player._pRSplType);
	file.Skip(3); // Alignment
	file.WriteLE<int32_t>(player._pSBkSpell);
	file.Skip<int8_t>(); // Skip _pSBkSplType

	for (int8_t spellLevel : player._pSplLvl)
		file.WriteLE<int8_t>(spellLevel);

	file.Skip(7); // Alignment
	file.WriteLE<uint64_t>(player._pMemSpells);
	file.WriteLE<uint64_t>(player._pAblSpells);
	file.WriteLE<uint64_t>(player._pScrlSpells);
	file.WriteLE<uint8_t>(player._pSpellFlags);
	file.Skip(3); // Alignment

	for (auto &spellId : player._pSplHotKey)
		file.WriteLE<int32_t>(spellId);

	for (auto &spellType : player._pSplTHotKey)
		file.WriteLE<int8_t>(spellType);

	file.WriteLE<int32_t>(player.UsesRangedWeapon() ? 1 : 0);
	file.WriteLE<uint8_t>(player._pBlockFlag ? 1 : 0);
	file.WriteLE<uint8_t>(player._pInvincible ? 1 : 0);
	file.WriteLE<int8_t>(player._pLightRad);
	file.WriteLE<uint8_t>(player._pLvlChanging ? 1 : 0);

	file.WriteBytes(player._pName, PLR_NAME_LEN);
	file.WriteLE<int8_t>(static_cast<int8_t>(player._pClass));
	file.Skip(3); // Alignment
	file.WriteLE<int32_t>(player._pStrength);
	file.WriteLE<int32_t>(player._pBaseStr);
	file.WriteLE<int32_t>(player._pMagic);
	file.WriteLE<int32_t>(player._pBaseMag);
	file.WriteLE<int32_t>(player._pDexterity);
	file.WriteLE<int32_t>(player._pBaseDex);
	file.WriteLE<int32_t>(player._pVitality);
	file.WriteLE<int32_t>(player._pBaseVit);
	file.WriteLE<int32_t>(player._pStatPts);
	file.WriteLE<int32_t>(player._pDamageMod);

	file.WriteLE<int32_t>(player._pBaseToBlk);
	file.WriteLE<int32_t>(player._pHPBase);
	file.WriteLE<int32_t>(player._pMaxHPBase);
	file.WriteLE<int32_t>(player._pHitPoints);
	file.WriteLE<int32_t>(player._pMaxHP);
	file.Skip<int32_t>(); // Skip _pHPPer
	file.WriteLE<int32_t>(player._pManaBase);
	file.WriteLE<int32_t>(player._pMaxManaBase);
	file.WriteLE<int32_t>(player._pMana);
	file.WriteLE<int32_t>(player._pMaxMana);
	file.Skip<int32_t>(); // Skip _pManaPer
	file.WriteLE<int8_t>(player._pLevel);
	file.WriteLE<int8_t>(player._pMaxLvl);
	file.Skip(2); // Alignment
	file.WriteLE<uint32_t>(player._pExperience);
	file.Skip<uint32_t>(); // Skip _pMaxExp
	file.WriteLE<uint32_t>(player._pNextExper);
	file.WriteLE<int8_t>(player._pArmorClass);
	file.WriteLE<int8_t>(player._pMagResist);
	file.WriteLE<int8_t>(player._pFireResist);
	file.WriteLE<int8_t>(player._pLghtResist);
	file.WriteLE<int32_t>(player._pGold);

	file.WriteLE<uint32_t>(player._pInfraFlag ? 1 : 0);
	file.WriteLE<int32_t>(player.position.temp.x);
	file.WriteLE<int32_t>(player.position.temp.y);
	file.WriteLE<int32_t>(static_cast<int32_t>(player.tempDirection));
	file.WriteLE<int32_t>(player.spellLevel);
	file.Skip<int32_t>(); // skip _pVar5, was used for storing position of a tile which should have its HorizontalMovingPlayer flag removed after walking
	file.WriteLE<int32_t>(player.position.offset2.deltaX);
	file.WriteLE<int32_t>(player.position.offset2.deltaY);
	file.Skip<int32_t>(); // Skip _pVar8
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		file.WriteLE<uint8_t>(player._pLvlVisited[i] ? 1 : 0);
	for (uint8_t i = 0; i < giNumberOfLevels; i++)
		file.WriteLE<uint8_t>(player._pSLvlVisited[i] ? 1 : 0); // only 10 used

	file.Skip(2); // Alignment

	file.Skip<int32_t>(); // Skip _pGFXLoad
	file.Skip(4 * 8);     // Skip pointers _pNAnim
	file.WriteLE<int32_t>(player._pNFrames);
	file.Skip(4);     // Skip _pNWidth
	file.Skip(4 * 8); // Skip pointers _pWAnim
	file.WriteLE<int32_t>(player._pWFrames);
	file.Skip(4);     // Skip _pWWidth
	file.Skip(4 * 8); // Skip pointers _pAAnim
	file.WriteLE<int32_t>(player._pAFrames);
	file.Skip(4); // Skip _pAWidth
	file.WriteLE<int32_t>(player._pAFNum);
	file.Skip(4 * 8); // Skip pointers _pLAnim
	file.Skip(4 * 8); // Skip pointers _pFAnim
	file.Skip(4 * 8); // Skip pointers _pTAnim
	file.WriteLE<int32_t>(player._pSFrames);
	file.Skip(4); // Skip _pSWidth
	file.WriteLE<int32_t>(player._pSFNum);
	file.Skip(4 * 8); // Skip pointers _pHAnim
	file.WriteLE<int32_t>(player._pHFrames);
	file.Skip(4);     // Skip _pHWidth
	file.Skip(4 * 8); // Skip pointers _pDAnim
	file.WriteLE<int32_t>(player._pDFrames);
	file.Skip(4);     // Skip _pDWidth
	file.Skip(4 * 8); // Skip pointers _pBAnim
	file.WriteLE<int32_t>(player._pBFrames);
	file.Skip(4); // Skip _pBWidth

	for (const Item &item : player.InvBody)
		SaveItem(file, item);

	for (const Item &item : player.InvList)
		SaveItem(file, item);

	file.WriteLE<int32_t>(player._pNumInv);

	for (int8_t cell : player.InvGrid)
		file.WriteLE<int8_t>(cell);

	for (const Item &item : player.SpdList)
		SaveItem(file, item);

	SaveItem(file, player.HoldItem);

	file.WriteLE<int32_t>(player._pIMinDam);
	file.WriteLE<int32_t>(player._pIMaxDam);
	file.WriteLE<int32_t>(player._pIAC);
	file.WriteLE<int32_t>(player._pIBonusDam);
	file.WriteLE<int32_t>(player._pIBonusToHit);
	file.WriteLE<int32_t>(player._pIBonusAC);
	file.WriteLE<int32_t>(player._pIBonusDamMod);
	file.Skip(4); // Alignment

	file.WriteLE<uint64_t>(player._pISpells);
	file.WriteLE<int32_t>(player._pIFlags);
	file.WriteLE<int32_t>(player._pIGetHit);

	file.WriteLE<int8_t>(player._pISplLvlAdd);
	file.Skip<uint8_t>(); // Skip _pISplCost
	file.Skip(2);         // Alignment
	file.WriteLE<int32_t>(player._pISplDur);
	file.WriteLE<int32_t>(player._pIEnAc);
	file.WriteLE<int32_t>(player._pIFMinDam);
	file.WriteLE<int32_t>(player._pIFMaxDam);
	file.WriteLE<int32_t>(player._pILMinDam);
	file.WriteLE<int32_t>(player._pILMaxDam);
	file.WriteLE<int32_t>(player._pOilType);
	file.WriteLE<uint8_t>(player.pTownWarps);
	file.WriteLE<uint8_t>(player.pDungMsgs);
	file.WriteLE<uint8_t>(player.pLvlLoad);
	if (gbIsHellfire)
		file.WriteLE<uint8_t>(player.pDungMsgs2);
	else
		file.WriteLE<uint8_t>(player.pBattleNet ? 1 : 0);
	file.WriteLE<uint8_t>(player.pManaShield ? 1 : 0);
	file.WriteLE<uint8_t>(player.pOriginalCathedral ? 1 : 0);
	file.Skip(2); // Available bytes
	file.WriteLE<uint16_t>(player.wReflections);
	file.Skip(14); // Available bytes

	file.WriteLE<uint32_t>(player.pDiabloKillLevel);
	file.WriteLE<uint32_t>(player.pDifficulty);
	file.WriteLE<uint32_t>(player.pDamAcFlags);
	file.Skip(20); // Available bytes

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

void SaveMonster(SaveHelper *file, Monster &monster)
{
	file->WriteLE<int32_t>(monster._mMTidx);
	file->WriteLE<int32_t>(static_cast<int>(monster._mmode));
	file->WriteLE<uint8_t>(monster._mgoal);
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(monster._mgoalvar1);
	file->WriteLE<int32_t>(monster._mgoalvar2);
	file->WriteLE<int32_t>(monster._mgoalvar3);
	file->Skip(4); // Unused
	file->WriteLE<uint8_t>(monster._pathcount);
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(monster.position.tile.x);
	file->WriteLE<int32_t>(monster.position.tile.y);
	file->WriteLE<int32_t>(monster.position.future.x);
	file->WriteLE<int32_t>(monster.position.future.y);
	file->WriteLE<int32_t>(monster.position.old.x);
	file->WriteLE<int32_t>(monster.position.old.y);
	file->WriteLE<int32_t>(monster.position.offset.deltaX);
	file->WriteLE<int32_t>(monster.position.offset.deltaY);
	file->WriteLE<int32_t>(monster.position.velocity.deltaX);
	file->WriteLE<int32_t>(monster.position.velocity.deltaY);
	file->WriteLE<int32_t>(static_cast<int32_t>(monster._mdir));
	file->WriteLE<int32_t>(monster._menemy);
	file->WriteLE<uint8_t>(monster.enemyPosition.x);
	file->WriteLE<uint8_t>(monster.enemyPosition.y);
	file->Skip(2); // Unused

	file->Skip(4); // Skip pointer _mAnimData
	file->WriteLE<int32_t>(monster.AnimInfo.TicksPerFrame);
	file->WriteLE<int32_t>(monster.AnimInfo.TickCounterOfCurrentFrame);
	file->WriteLE<int32_t>(monster.AnimInfo.NumberOfFrames);
	file->WriteLE<int32_t>(monster.AnimInfo.CurrentFrame);
	file->Skip<uint32_t>(); // Skip _meflag
	file->WriteLE<uint32_t>(monster._mDelFlag ? 1 : 0);
	file->WriteLE<int32_t>(monster._mVar1);
	file->WriteLE<int32_t>(monster._mVar2);
	file->WriteLE<int32_t>(monster._mVar3);
	file->WriteLE<int32_t>(monster.position.temp.x);
	file->WriteLE<int32_t>(monster.position.temp.y);
	file->WriteLE<int32_t>(monster.position.offset2.deltaX);
	file->WriteLE<int32_t>(monster.position.offset2.deltaY);
	file->Skip<int32_t>(); // Skip _mVar8
	file->WriteLE<int32_t>(monster._mmaxhp);
	file->WriteLE<int32_t>(monster._mhitpoints);

	file->WriteLE<uint8_t>(monster._mAi);
	file->WriteLE<uint8_t>(monster._mint);
	file->Skip(2); // Alignment
	file->WriteLE<uint32_t>(monster._mFlags);
	file->WriteLE<uint8_t>(monster._msquelch);
	file->Skip(3); // Alignment
	file->Skip(4); // Unused
	file->WriteLE<int32_t>(monster.position.last.x);
	file->WriteLE<int32_t>(monster.position.last.y);
	file->WriteLE<uint32_t>(monster._mRndSeed);
	file->WriteLE<uint32_t>(monster._mAISeed);
	file->Skip(4); // Unused

	file->WriteLE<uint8_t>(monster._uniqtype);
	file->WriteLE<uint8_t>(monster._uniqtrans);
	file->WriteLE<int8_t>(monster._udeadval);

	file->WriteLE<int8_t>(monster.mWhoHit);
	file->WriteLE<int8_t>(monster.mLevel);
	file->Skip(1); // Alignment
	file->WriteLE<uint16_t>(monster.mExp);

	file->WriteLE<uint8_t>(std::min<uint16_t>(monster.mHit, std::numeric_limits<uint8_t>::max())); // For backwards compatibility
	file->WriteLE<uint8_t>(monster.mMinDamage);
	file->WriteLE<uint8_t>(monster.mMaxDamage);
	file->WriteLE<uint8_t>(std::min<uint16_t>(monster.mHit2, std::numeric_limits<uint8_t>::max())); // For backwards compatibility
	file->WriteLE<uint8_t>(monster.mMinDamage2);
	file->WriteLE<uint8_t>(monster.mMaxDamage2);
	file->WriteLE<uint8_t>(monster.mArmorClass);
	file->Skip(1); // Alignment
	file->WriteLE<uint16_t>(monster.mMagicRes);
	file->Skip(2); // Alignment

	file->WriteLE<int32_t>(monster.mtalkmsg == TEXT_NONE ? 0 : monster.mtalkmsg); // Replicate original bad mapping of none for monsters
	file->WriteLE<uint8_t>(monster.leader);
	file->WriteLE<uint8_t>(static_cast<std::uint8_t>(monster.leaderRelation));
	file->WriteLE<uint8_t>(monster.packsize);
	// vanilla compatibility
	if (monster.mlid == NO_LIGHT)
		file->WriteLE<int8_t>(0);
	else
		file->WriteLE<int8_t>(monster.mlid);

	// Omit pointer mName;
	// Omit pointer MType;
	// Omit pointer MData;
}

void SaveMissile(SaveHelper *file, Missile &missile)
{
	file->WriteLE<int32_t>(missile._mitype);
	file->WriteLE<int32_t>(missile.position.tile.x);
	file->WriteLE<int32_t>(missile.position.tile.y);
	file->WriteLE<int32_t>(missile.position.offset.deltaX);
	file->WriteLE<int32_t>(missile.position.offset.deltaY);
	file->WriteLE<int32_t>(missile.position.velocity.deltaX);
	file->WriteLE<int32_t>(missile.position.velocity.deltaY);
	file->WriteLE<int32_t>(missile.position.start.x);
	file->WriteLE<int32_t>(missile.position.start.y);
	file->WriteLE<int32_t>(missile.position.traveled.deltaX);
	file->WriteLE<int32_t>(missile.position.traveled.deltaY);
	file->WriteLE<int32_t>(missile._mimfnum);
	file->WriteLE<int32_t>(missile._mispllvl);
	file->WriteLE<uint32_t>(missile._miDelFlag ? 1 : 0);
	file->WriteLE<uint8_t>(missile._miAnimType);
	file->Skip(3); // Alignment
	file->WriteLE<int32_t>(static_cast<int32_t>(missile._miAnimFlags));
	file->Skip(4); // Skip pointer _miAnimData
	file->WriteLE<int32_t>(missile._miAnimDelay);
	file->WriteLE<int32_t>(missile._miAnimLen);
	file->WriteLE<int32_t>(missile._miAnimWidth);
	file->WriteLE<int32_t>(missile._miAnimWidth2);
	file->WriteLE<int32_t>(missile._miAnimCnt);
	file->WriteLE<int32_t>(missile._miAnimAdd);
	file->WriteLE<int32_t>(missile._miAnimFrame);
	file->WriteLE<uint32_t>(missile._miDrawFlag ? 1 : 0);
	file->WriteLE<uint32_t>(missile._miLightFlag ? 1 : 0);
	file->WriteLE<uint32_t>(missile._miPreFlag ? 1 : 0);
	file->WriteLE<uint32_t>(missile._miUniqTrans);
	file->WriteLE<int32_t>(missile._mirange);
	file->WriteLE<int32_t>(missile._misource);
	file->WriteLE<int32_t>(missile._micaster);
	file->WriteLE<int32_t>(missile._midam);
	file->WriteLE<uint32_t>(missile._miHitFlag ? 1 : 0);
	file->WriteLE<int32_t>(missile._midist);
	file->WriteLE<int32_t>(missile._mlid);
	file->WriteLE<int32_t>(missile._mirnd);
	file->WriteLE<int32_t>(missile.var1);
	file->WriteLE<int32_t>(missile.var2);
	file->WriteLE<int32_t>(missile.var3);
	file->WriteLE<int32_t>(missile.var4);
	file->WriteLE<int32_t>(missile.var5);
	file->WriteLE<int32_t>(missile.var6);
	file->WriteLE<int32_t>(missile.var7);
	file->WriteLE<uint32_t>(missile.limitReached ? 1 : 0);
}

void SaveObject(SaveHelper &file, const Object &object)
{
	file.WriteLE<int32_t>(object._otype);
	file.WriteLE<int32_t>(object.position.x);
	file.WriteLE<int32_t>(object.position.y);
	file.WriteLE<uint32_t>(object._oLight ? 1 : 0);
	file.WriteLE<uint32_t>(object._oAnimFlag);
	file.Skip(4); // Skip pointer _oAnimData
	file.WriteLE<int32_t>(object._oAnimDelay);
	file.WriteLE<int32_t>(object._oAnimCnt);
	file.WriteLE<uint32_t>(object._oAnimLen);
	file.WriteLE<uint32_t>(object._oAnimFrame);
	file.WriteLE<int32_t>(object._oAnimWidth);
	file.WriteLE<int32_t>(CalculateWidth2(object._oAnimWidth)); // Write _oAnimWidth2 for vanilla compatibility
	file.WriteLE<uint32_t>(object._oDelFlag ? 1 : 0);
	file.WriteLE<int8_t>(object._oBreak);
	file.Skip(3); // Alignment
	file.WriteLE<uint32_t>(object._oSolidFlag ? 1 : 0);
	file.WriteLE<uint32_t>(object._oMissFlag ? 1 : 0);

	file.WriteLE<int8_t>(object._oSelFlag);
	file.Skip(3); // Alignment
	file.WriteLE<uint32_t>(object._oPreFlag ? 1 : 0);
	file.WriteLE<uint32_t>(object._oTrapFlag ? 1 : 0);
	file.WriteLE<uint32_t>(object._oDoorFlag ? 1 : 0);
	file.WriteLE<int32_t>(object._olid);
	file.WriteLE<uint32_t>(object._oRndSeed);
	file.WriteLE<int32_t>(object._oVar1);
	file.WriteLE<int32_t>(object._oVar2);
	file.WriteLE<int32_t>(object._oVar3);
	file.WriteLE<int32_t>(object._oVar4);
	file.WriteLE<int32_t>(object._oVar5);
	file.WriteLE<uint32_t>(object._oVar6);
	file.WriteLE<int32_t>(object.bookMessage);
	file.WriteLE<int32_t>(object._oVar8);
}

void SaveQuest(SaveHelper *file, int i)
{
	auto &quest = Quests[i];

	file->WriteLE<uint8_t>(quest._qlevel);
	file->WriteLE<uint8_t>(quest._qidx); // _qtype for compatability, used in DRLG_CheckQuests
	file->WriteLE<uint8_t>(quest._qactive);
	file->WriteLE<uint8_t>(quest._qlvltype);
	file->WriteLE<int32_t>(quest.position.x);
	file->WriteLE<int32_t>(quest.position.y);
	file->WriteLE<uint8_t>(quest._qslvl);
	file->WriteLE<uint8_t>(quest._qidx);
	if (gbIsHellfire) {
		file->Skip(2); // Alignment
		file->WriteLE<int32_t>(quest._qmsg);
	} else {
		file->WriteLE<uint8_t>(quest._qmsg);
	}
	file->WriteLE<uint8_t>(quest._qvar1);
	file->WriteLE<uint8_t>(quest._qvar2);
	file->Skip(2); // Alignment
	if (!gbIsHellfire)
		file->Skip(1); // Alignment
	file->WriteLE<uint32_t>(quest._qlog ? 1 : 0);

	file->WriteBE<int32_t>(ReturnLvlPosition.x);
	file->WriteBE<int32_t>(ReturnLvlPosition.y);
	file->WriteBE<int32_t>(ReturnLevel);
	file->WriteBE<int32_t>(ReturnLevelType);
	file->Skip(sizeof(int32_t)); // Skip DoomQuestState
}

void SaveLighting(SaveHelper *file, Light *pLight)
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

void SavePortal(SaveHelper *file, int i)
{
	Portal *pPortal = &Portals[i];

	file->WriteLE<uint32_t>(pPortal->open ? 1 : 0);
	file->WriteLE<int32_t>(pPortal->position.x);
	file->WriteLE<int32_t>(pPortal->position.y);
	file->WriteLE<int32_t>(pPortal->level);
	file->WriteLE<int32_t>(pPortal->ltype);
	file->WriteLE<uint32_t>(pPortal->setlvl ? 1 : 0);
}

const int DiabloItemSaveSize = 368;
const int HellfireItemSaveSize = 372;

} // namespace

void RemoveInvalidItem(Item &item)
{
	bool isInvalid = !IsItemAvailable(item.IDidx) || !IsUniqueAvailable(item._iUid);

	if (!gbIsHellfire) {
		isInvalid = isInvalid || (item._itype == ItemType::Staff && GetSpellStaffLevel(item._iSpell) == -1);
		isInvalid = isInvalid || (item._iMiscId == IMISC_BOOK && GetSpellBookLevel(item._iSpell) == -1);
		isInvalid = isInvalid || item._iDamAcFlags != 0;
		isInvalid = isInvalid || item._iPrePower > IPL_LASTDIABLO;
		isInvalid = isInvalid || item._iSufPower > IPL_LASTDIABLO;
	}

	if (isInvalid) {
		item._itype = ItemType::None;
	}
}

_item_indexes RemapItemIdxFromDiablo(_item_indexes i)
{
	constexpr auto GetItemIdValue = [](int i) -> int {
		if (i == IDI_SORCERER) {
			return IDI_SORCERER_DIABLO;
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
		if (i == IDI_SORCERER_DIABLO) {
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

void LoadHeroItems(Player &player)
{
	LoadHelper file("heroitems");
	if (!file.IsValid())
		return;

	gbIsHellfireSaveGame = file.NextBool8();

	LoadMatchingItems(file, NUM_INVLOC, player.InvBody);
	LoadMatchingItems(file, NUM_INV_GRID_ELEM, player.InvList);
	LoadMatchingItems(file, MAXBELTITEMS, player.SpdList);

	gbIsHellfireSaveGame = gbIsHellfire;
}

void RemoveEmptyInventory(Player &player)
{
	for (int i = NUM_INV_GRID_ELEM; i > 0; i--) {
		int8_t idx = player.InvGrid[i - 1];
		if (idx > 0 && player.InvList[idx - 1].isEmpty()) {
			player.RemoveInvItem(idx - 1);
		}
	}
}

void LoadGame(bool firstflag)
{
	FreeGameMem();

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

	pfile_remove_temp_files();

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

	auto &myPlayer = Players[MyPlayerId];

	LoadPlayer(file, myPlayer);

	sgGameInitInfo.nDifficulty = myPlayer.pDifficulty;
	if (sgGameInitInfo.nDifficulty < DIFF_NORMAL || sgGameInitInfo.nDifficulty > DIFF_HELL)
		sgGameInitInfo.nDifficulty = DIFF_NORMAL;

	for (int i = 0; i < giNumberQuests; i++)
		LoadQuest(&file, i);
	for (int i = 0; i < MAXPORTAL; i++)
		LoadPortal(&file, i);

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		ConvertLevels();
		RemoveEmptyInventory(myPlayer);
	}

	LoadGameLevel(firstflag, ENTRY_LOAD);
	SyncInitPlr(MyPlayerId);
	SyncPlrAnim(MyPlayerId);

	ViewPosition = { viewX, viewY };
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
			LoadMonster(&file, Monsters[ActiveMonsters[i]]);
		for (int i = 0; i < ActiveMonsterCount; i++)
			SyncPackSize(Monsters[ActiveMonsters[i]]);
		for (int &missileId : ActiveMissiles)
			missileId = file.NextLE<int8_t>();
		for (int &missileId : AvailableMissiles)
			missileId = file.NextLE<int8_t>();
		for (int i = 0; i < ActiveMissileCount; i++)
			LoadMissile(&file, Missiles[ActiveMissiles[i]]);
		for (int &objectId : ActiveObjects)
			objectId = file.NextLE<int8_t>();
		for (int &objectId : AvailableObjects)
			objectId = file.NextLE<int8_t>();
		for (int i = 0; i < ActiveObjectCount; i++)
			LoadObject(file, Objects[ActiveObjects[i]]);
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
		LoadItem(file, Items[ActiveItems[i]]);
	for (bool &uniqueItemFlag : UniqueItemFlags)
		uniqueItemFlag = file.NextBool8();

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dLight[i][j] = file.NextLE<int8_t>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dFlags[i][j] = static_cast<DungeonFlag>(file.NextLE<uint8_t>()) & DungeonFlag::LoadedFlags;
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
				dCorpse[i][j] = file.NextLE<int8_t>();
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
				AutomapView[i][j] = file.NextLE<uint8_t>();
		}
		file.Skip(MAXDUNX * MAXDUNY); // dMissile
	}

	numpremium = file.NextBE<int32_t>();
	premiumlevel = file.NextBE<int32_t>();

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		LoadPremium(file, i);
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
	// convert stray manashield missiles into pManaShield flag
	for (auto &missile : Missiles) {
		if (missile._mitype == MIS_MANASHIELD && missile._miDelFlag == false) {
			Players[missile._misource].pManaShield = true;
			missile._miDelFlag = true;
		}
	}

	missiles_process_charge();
	RedoMissileFlags();
	NewCursor(CURSOR_HAND);
	gbProcessPlayers = true;

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		RemoveEmptyLevelItems();
		SaveGame();
	}

	gbIsHellfireSaveGame = gbIsHellfire;
}

void SaveHeroItems(Player &player)
{
	size_t itemCount = NUM_INVLOC + NUM_INV_GRID_ELEM + MAXBELTITEMS;
	SaveHelper file("heroitems", itemCount * (gbIsHellfire ? HellfireItemSaveSize : DiabloItemSaveSize) + sizeof(uint8_t));

	file.WriteLE<uint8_t>(gbIsHellfire ? 1 : 0);

	for (const Item &item : player.InvBody)
		SaveItem(file, item);
	for (const Item &item : player.InvList)
		SaveItem(file, item);
	for (const Item &item : player.SpdList)
		SaveItem(file, item);
}

void SaveGameData()
{
	SaveHelper file("game", 320 * 1024);

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
	file.WriteBE<int32_t>(ViewPosition.x);
	file.WriteBE<int32_t>(ViewPosition.y);
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

	auto &myPlayer = Players[MyPlayerId];
	myPlayer.pDifficulty = sgGameInitInfo.nDifficulty;
	SavePlayer(file, myPlayer);

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
			SaveMonster(&file, Monsters[ActiveMonsters[i]]);
		for (int missileId : ActiveMissiles)
			file.WriteLE<int8_t>(missileId);
		for (int missileId : AvailableMissiles)
			file.WriteLE<int8_t>(missileId);
		for (int i = 0; i < ActiveMissileCount; i++)
			SaveMissile(&file, Missiles[ActiveMissiles[i]]);
		for (int objectId : ActiveObjects)
			file.WriteLE<int8_t>(objectId);
		for (int objectId : AvailableObjects)
			file.WriteLE<int8_t>(objectId);
		for (int i = 0; i < ActiveObjectCount; i++)
			SaveObject(file, Objects[ActiveObjects[i]]);

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
		SaveItem(file, Items[ActiveItems[i]]);
	for (bool uniqueItemFlag : UniqueItemFlags)
		file.WriteLE<uint8_t>(uniqueItemFlag ? 1 : 0);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<int8_t>(dLight[i][j]);
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<uint8_t>(static_cast<uint8_t>(dFlags[i][j] & DungeonFlag::SavedFlags));
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
				file.WriteLE<int8_t>(dCorpse[i][j]);
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
				file.WriteLE<uint8_t>(AutomapView[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)                                 // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(TileContainsMissile({ i, j }) ? -1 : 0); // For backwards compatability
		}
	}

	file.WriteBE<int32_t>(numpremium);
	file.WriteBE<int32_t>(premiumlevel);

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		SaveItem(file, premiumitems[i]);

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

	auto &myPlayer = Players[MyPlayerId];

	DoUnVision(myPlayer.position.tile, myPlayer._pLightRad); // fix for vision staying on the level

	if (currlevel == 0)
		glSeedTbl[0] = AdvanceRndSeed();

	char szName[MAX_PATH];
	GetTempLevelNames(szName);
	SaveHelper file(szName, 256 * 1024);

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
				file.WriteLE<int8_t>(dCorpse[i][j]);
		}
	}

	file.WriteBE<int32_t>(ActiveMonsterCount);
	file.WriteBE<int32_t>(ActiveItemCount);
	file.WriteBE<int32_t>(ActiveObjectCount);

	if (leveltype != DTYPE_TOWN) {
		for (int monsterId : ActiveMonsters)
			file.WriteBE<int32_t>(monsterId);
		for (int i = 0; i < ActiveMonsterCount; i++)
			SaveMonster(&file, Monsters[ActiveMonsters[i]]);
		for (int objectId : ActiveObjects)
			file.WriteLE<int8_t>(objectId);
		for (int objectId : AvailableObjects)
			file.WriteLE<int8_t>(objectId);
		for (int i = 0; i < ActiveObjectCount; i++)
			SaveObject(file, Objects[ActiveObjects[i]]);
	}

	for (int itemId : ActiveItems)
		file.WriteLE<int8_t>(itemId);
	for (int itemId : AvailableItems)
		file.WriteLE<int8_t>(itemId);

	for (int i = 0; i < ActiveItemCount; i++)
		SaveItem(file, Items[ActiveItems[i]]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			file.WriteLE<uint8_t>(static_cast<uint8_t>(dFlags[i][j] & DungeonFlag::SavedFlags));
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
				file.WriteLE<uint8_t>(AutomapView[i][j]);
		}
	}

	if (!setlevel)
		myPlayer._pLvlVisited[currlevel] = true;
	else
		myPlayer._pSLvlVisited[setlvlnum] = true;
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
				dCorpse[i][j] = file.NextLE<int8_t>();
		}
		SyncUniqDead();
	}

	ActiveMonsterCount = file.NextBE<int32_t>();
	ActiveItemCount = file.NextBE<int32_t>();
	ActiveObjectCount = file.NextBE<int32_t>();

	if (leveltype != DTYPE_TOWN) {
		for (int &monsterId : ActiveMonsters)
			monsterId = file.NextBE<int32_t>();
		for (int i = 0; i < ActiveMonsterCount; i++)
			LoadMonster(&file, Monsters[ActiveMonsters[i]]);
		for (int &objectId : ActiveObjects)
			objectId = file.NextLE<int8_t>();
		for (int &objectId : AvailableObjects)
			objectId = file.NextLE<int8_t>();
		for (int i = 0; i < ActiveObjectCount; i++)
			LoadObject(file, Objects[ActiveObjects[i]]);
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
		LoadItem(file, Items[ActiveItems[i]]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) // NOLINT(modernize-loop-convert)
			dFlags[i][j] = static_cast<DungeonFlag>(file.NextLE<uint8_t>()) & DungeonFlag::LoadedFlags;
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
			for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
				const auto automapView = static_cast<MapExplorationType>(file.NextLE<uint8_t>());
				AutomapView[i][j] = automapView == MAP_EXP_OLD ? MAP_EXP_SELF : automapView;
			}
		}
	}

	if (gbIsHellfireSaveGame != gbIsHellfire) {
		RemoveEmptyLevelItems();
	}

	if (!gbSkipSync) {
		AutomapZoomReset();
		ResyncQuests();
		RedoMissileFlags();
		SyncPortals();
		UpdateLighting = true;
	}

	for (auto &player : Players) {
		if (player.plractive && currlevel == player.plrlevel)
			Lights[player._plid]._lunflag = true;
	}
}

} // namespace devilution

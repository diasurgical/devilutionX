/**
 * @file loadsave.cpp
 *
 * Implementation of save game functionality.
 */
#include "all.h"

namespace devilution {

bool gbIsHellfireSaveGame;
int giNumberOfLevels;
int giNumberQuests;
int giNumberOfSmithPremiumItems;

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
	Uint8 *m_buffer;
	Uint32 m_cur = 0;
	Uint32 m_size;

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

	bool isValid(Uint32 size = 1)
	{
		return m_buffer != nullptr
		    && m_size >= (m_cur + size);
	}

	void skip(Uint32 size)
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
		return next<Uint8>() != 0;
	}

	bool nextBool32()
	{
		return next<Uint32>() != 0;
	}

	~LoadHelper()
	{
		mem_free_dbg(m_buffer);
	}
};

class SaveHelper {
	const char *m_szFileName;
	Uint8 *m_buffer;
	Uint32 m_cur = 0;
	Uint32 m_capacity;

public:
	SaveHelper(const char *szFileName, size_t bufferLen)
	{
		m_szFileName = szFileName;
		m_capacity = bufferLen;
		m_buffer = DiabloAllocPtr(codec_get_encoded_len(m_capacity));
	}

	bool isValid(Uint32 len = 1)
	{
		return m_buffer != nullptr
		    && m_capacity >= (m_cur + len);
	}

	void skip(Uint32 len)
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
		codec_encode(m_buffer, m_cur, encoded_len, password);
		mpqapi_write_file(m_szFileName, m_buffer, encoded_len);

		mem_free_dbg(m_buffer);
		m_buffer = nullptr;
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
	pItem->_iSeed = file->nextLE<Sint32>();
	pItem->_iCreateInfo = file->nextLE<Uint16>();
	file->skip(2); // Alignment
	pItem->_itype = (item_type)file->nextLE<Uint32>();
	pItem->_ix = file->nextLE<Sint32>();
	pItem->_iy = file->nextLE<Sint32>();
	pItem->_iAnimFlag = file->nextBool32();
	file->skip(4); // Skip pointer _iAnimData
	pItem->_iAnimLen = file->nextLE<Sint32>();
	pItem->_iAnimFrame = file->nextLE<Sint32>();
	pItem->_iAnimWidth = file->nextLE<Sint32>();
	pItem->_iAnimWidth2 = file->nextLE<Sint32>();
	file->skip(4); // Unused since 1.02
	pItem->_iSelFlag = file->nextLE<Uint8>();
	file->skip(3); // Alignment
	pItem->_iPostDraw = file->nextBool32();
	pItem->_iIdentified = file->nextBool32();
	pItem->_iMagical = file->nextLE<Sint8>();
	file->nextBytes(pItem->_iName, 64);
	file->nextBytes(pItem->_iIName, 64);
	pItem->_iLoc = (item_equip_type)file->nextLE<Sint8>();
	pItem->_iClass = (item_class)file->nextLE<Uint8>();
	file->skip(1); // Alignment
	pItem->_iCurs = file->nextLE<Sint32>();
	pItem->_ivalue = file->nextLE<Sint32>();
	pItem->_iIvalue = file->nextLE<Sint32>();
	pItem->_iMinDam = file->nextLE<Sint32>();
	pItem->_iMaxDam = file->nextLE<Sint32>();
	pItem->_iAC = file->nextLE<Sint32>();
	pItem->_iFlags = file->nextLE<Sint32>();
	pItem->_iMiscId = (item_misc_id)file->nextLE<Sint32>();
	pItem->_iSpell = (spell_id)file->nextLE<Sint32>();
	pItem->_iCharges = file->nextLE<Sint32>();
	pItem->_iMaxCharges = file->nextLE<Sint32>();
	pItem->_iDurability = file->nextLE<Sint32>();
	pItem->_iMaxDur = file->nextLE<Sint32>();
	pItem->_iPLDam = file->nextLE<Sint32>();
	pItem->_iPLToHit = file->nextLE<Sint32>();
	pItem->_iPLAC = file->nextLE<Sint32>();
	pItem->_iPLStr = file->nextLE<Sint32>();
	pItem->_iPLMag = file->nextLE<Sint32>();
	pItem->_iPLDex = file->nextLE<Sint32>();
	pItem->_iPLVit = file->nextLE<Sint32>();
	pItem->_iPLFR = file->nextLE<Sint32>();
	pItem->_iPLLR = file->nextLE<Sint32>();
	pItem->_iPLMR = file->nextLE<Sint32>();
	pItem->_iPLMana = file->nextLE<Sint32>();
	pItem->_iPLHP = file->nextLE<Sint32>();
	pItem->_iPLDamMod = file->nextLE<Sint32>();
	pItem->_iPLGetHit = file->nextLE<Sint32>();
	pItem->_iPLLight = file->nextLE<Sint32>();
	pItem->_iSplLvlAdd = file->nextLE<Sint8>();
	pItem->_iRequest = file->nextLE<Sint8>();
	file->skip(2); // Alignment
	pItem->_iUid = file->nextLE<Sint32>();
	pItem->_iFMinDam = file->nextLE<Sint32>();
	pItem->_iFMaxDam = file->nextLE<Sint32>();
	pItem->_iLMinDam = file->nextLE<Sint32>();
	pItem->_iLMaxDam = file->nextLE<Sint32>();
	pItem->_iPLEnAc = file->nextLE<Sint32>();
	pItem->_iPrePower = (item_effect_type)file->nextLE<Sint8>();
	pItem->_iSufPower = (item_effect_type)file->nextLE<Sint8>();
	file->skip(2); // Alignment
	pItem->_iVAdd1 = file->nextLE<Sint32>();
	pItem->_iVMult1 = file->nextLE<Sint32>();
	pItem->_iVAdd2 = file->nextLE<Sint32>();
	pItem->_iVMult2 = file->nextLE<Sint32>();
	pItem->_iMinStr = file->nextLE<Sint8>();
	pItem->_iMinMag = file->nextLE<Uint8>();
	pItem->_iMinDex = file->nextLE<Sint8>();
	file->skip(1); // Alignment
	pItem->_iStatFlag = file->nextBool32();
	pItem->IDidx = file->nextLE<Sint32>();
	if (!gbIsHellfireSaveGame) {
		pItem->IDidx = RemapItemIdxFromDiablo(pItem->IDidx);
	}
	pItem->dwBuff = file->nextLE<Uint32>();
	if (gbIsHellfireSaveGame)
		pItem->_iDamAcFlags = file->nextLE<Sint32>();
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
	PlayerStruct *pPlayer = &plr[p];

	pPlayer->_pmode = (PLR_MODE)file->nextLE<Sint32>();

	for (int i = 0; i < MAX_PATH_LENGTH; i++) {
		pPlayer->walkpath[i] = file->nextLE<Sint8>();
	}
	pPlayer->plractive = file->nextBool8();
	file->skip(2); // Alignment
	pPlayer->destAction = (action_id)file->nextLE<Sint32>();
	pPlayer->destParam1 = file->nextLE<Sint32>();
	pPlayer->destParam2 = file->nextLE<Sint32>();
	pPlayer->destParam3 = (direction)file->nextLE<Sint32>();
	pPlayer->destParam4 = file->nextLE<Sint32>();
	pPlayer->plrlevel = file->nextLE<Sint32>();
	pPlayer->_px = file->nextLE<Sint32>();
	pPlayer->_py = file->nextLE<Sint32>();
	pPlayer->_pfutx = file->nextLE<Sint32>();
	pPlayer->_pfuty = file->nextLE<Sint32>();
	pPlayer->_ptargx = file->nextLE<Sint32>();
	pPlayer->_ptargy = file->nextLE<Sint32>();
	pPlayer->_pownerx = file->nextLE<Sint32>();
	pPlayer->_pownery = file->nextLE<Sint32>();
	pPlayer->_poldx = file->nextLE<Sint32>();
	pPlayer->_poldy = file->nextLE<Sint32>();
	pPlayer->_pxoff = file->nextLE<Sint32>();
	pPlayer->_pyoff = file->nextLE<Sint32>();
	pPlayer->_pxvel = file->nextLE<Sint32>();
	pPlayer->_pyvel = file->nextLE<Sint32>();
	pPlayer->_pdir = (direction)file->nextLE<Sint32>();
	file->skip(4); // Unused
	pPlayer->_pgfxnum = file->nextLE<Sint32>();
	file->skip(4); // Skip pointer _pAnimData
	pPlayer->_pAnimDelay = file->nextLE<Sint32>();
	pPlayer->_pAnimCnt = file->nextLE<Sint32>();
	pPlayer->_pAnimLen = file->nextLE<Sint32>();
	pPlayer->_pAnimFrame = file->nextLE<Sint32>();
	pPlayer->_pAnimWidth = file->nextLE<Sint32>();
	pPlayer->_pAnimWidth2 = file->nextLE<Sint32>();
	file->skip(4); // Skip _peflag
	pPlayer->_plid = file->nextLE<Sint32>();
	pPlayer->_pvid = file->nextLE<Sint32>();

	pPlayer->_pSpell = (spell_id)file->nextLE<Sint32>();
	pPlayer->_pSplType = (spell_type)file->nextLE<Sint8>();
	pPlayer->_pSplFrom = file->nextLE<Sint8>();
	file->skip(2); // Alignment
	pPlayer->_pTSpell = (spell_id)file->nextLE<Sint32>();
	pPlayer->_pTSplType = (spell_type)file->nextLE<Sint8>();
	file->skip(3); // Alignment
	pPlayer->_pRSpell = (spell_id)file->nextLE<Sint32>();
	pPlayer->_pRSplType = (spell_type)file->nextLE<Sint8>();
	file->skip(3); // Alignment
	pPlayer->_pSBkSpell = (spell_id)file->nextLE<Sint32>();
	pPlayer->_pSBkSplType = (spell_type)file->nextLE<Sint8>();
	for (int i = 0; i < 64; i++)
		pPlayer->_pSplLvl[i] = file->nextLE<Sint8>();
	file->skip(7); // Alignment
	pPlayer->_pMemSpells = file->nextLE<Uint64>();
	pPlayer->_pAblSpells = file->nextLE<Uint64>();
	pPlayer->_pScrlSpells = file->nextLE<Uint64>();
	pPlayer->_pSpellFlags = file->nextLE<Uint8>();
	file->skip(3); // Alignment
	for (int i = 0; i < 4; i++)
		pPlayer->_pSplHotKey[i] = (spell_id)file->nextLE<Sint32>();
	for (int i = 0; i < 4; i++)
		pPlayer->_pSplTHotKey[i] = (spell_type)file->nextLE<Sint8>();

	pPlayer->_pwtype = (player_weapon_type)file->nextLE<Sint32>();
	pPlayer->_pBlockFlag = file->nextBool8();
	pPlayer->_pInvincible = file->nextBool8();
	pPlayer->_pLightRad = file->nextLE<Sint8>();
	pPlayer->_pLvlChanging = file->nextBool8();

	file->nextBytes(pPlayer->_pName, PLR_NAME_LEN);
	pPlayer->_pClass = (plr_class)file->nextLE<Sint8>();
	file->skip(3); // Alignment
	pPlayer->_pStrength = file->nextLE<Sint32>();
	pPlayer->_pBaseStr = file->nextLE<Sint32>();
	pPlayer->_pMagic = file->nextLE<Sint32>();
	pPlayer->_pBaseMag = file->nextLE<Sint32>();
	pPlayer->_pDexterity = file->nextLE<Sint32>();
	pPlayer->_pBaseDex = file->nextLE<Sint32>();
	pPlayer->_pVitality = file->nextLE<Sint32>();
	pPlayer->_pBaseVit = file->nextLE<Sint32>();
	pPlayer->_pStatPts = file->nextLE<Sint32>();
	pPlayer->_pDamageMod = file->nextLE<Sint32>();
	pPlayer->_pBaseToBlk = file->nextLE<Sint32>();
	if (pPlayer->_pBaseToBlk == 0)
		pPlayer->_pBaseToBlk = ToBlkTbl[pPlayer->_pClass];
	pPlayer->_pHPBase = file->nextLE<Sint32>();
	pPlayer->_pMaxHPBase = file->nextLE<Sint32>();
	pPlayer->_pHitPoints = file->nextLE<Sint32>();
	pPlayer->_pMaxHP = file->nextLE<Sint32>();
	pPlayer->_pHPPer = file->nextLE<Sint32>();
	pPlayer->_pManaBase = file->nextLE<Sint32>();
	pPlayer->_pMaxManaBase = file->nextLE<Sint32>();
	pPlayer->_pMana = file->nextLE<Sint32>();
	pPlayer->_pMaxMana = file->nextLE<Sint32>();
	pPlayer->_pManaPer = file->nextLE<Sint32>();
	pPlayer->_pLevel = file->nextLE<Sint8>();
	pPlayer->_pMaxLvl = file->nextLE<Sint8>();
	file->skip(2); // Alignment
	pPlayer->_pExperience = file->nextLE<Sint32>();
	pPlayer->_pMaxExp = file->nextLE<Sint32>();
	pPlayer->_pNextExper = file->nextLE<Sint32>();
	pPlayer->_pArmorClass = file->nextLE<Sint8>();
	pPlayer->_pMagResist = file->nextLE<Sint8>();
	pPlayer->_pFireResist = file->nextLE<Sint8>();
	pPlayer->_pLghtResist = file->nextLE<Sint8>();
	pPlayer->_pGold = file->nextLE<Sint32>();

	pPlayer->_pInfraFlag = file->nextBool32();
	pPlayer->_pVar1 = file->nextLE<Sint32>();
	pPlayer->_pVar2 = file->nextLE<Sint32>();
	pPlayer->_pVar3 = (direction)file->nextLE<Sint32>();
	pPlayer->_pVar4 = file->nextLE<Sint32>();
	pPlayer->_pVar5 = file->nextLE<Sint32>();
	pPlayer->_pVar6 = file->nextLE<Sint32>();
	pPlayer->_pVar7 = file->nextLE<Sint32>();
	pPlayer->_pVar8 = file->nextLE<Sint32>();
	for (int i = 0; i < giNumberOfLevels; i++)
		pPlayer->_pLvlVisited[i] = file->nextBool8();
	for (int i = 0; i < giNumberOfLevels; i++)
		pPlayer->_pSLvlVisited[i] = file->nextBool8();

	file->skip(2); // Alignment

	pPlayer->_pGFXLoad = file->nextLE<Sint32>();
	file->skip(4 * 8); // Skip pointers _pNAnim
	pPlayer->_pNFrames = file->nextLE<Sint32>();
	pPlayer->_pNWidth = file->nextLE<Sint32>();
	file->skip(4 * 8); // Skip pointers _pWAnim
	pPlayer->_pWFrames = file->nextLE<Sint32>();
	pPlayer->_pWWidth = file->nextLE<Sint32>();
	file->skip(4 * 8); // Skip pointers _pAAnim
	pPlayer->_pAFrames = file->nextLE<Sint32>();
	pPlayer->_pAWidth = file->nextLE<Sint32>();
	pPlayer->_pAFNum = file->nextLE<Sint32>();
	file->skip(4 * 8); // Skip pointers _pLAnim
	file->skip(4 * 8); // Skip pointers _pFAnim
	file->skip(4 * 8); // Skip pointers _pTAnim
	pPlayer->_pSFrames = file->nextLE<Sint32>();
	pPlayer->_pSWidth = file->nextLE<Sint32>();
	pPlayer->_pSFNum = file->nextLE<Sint32>();
	file->skip(4 * 8); // Skip pointers _pHAnim
	pPlayer->_pHFrames = file->nextLE<Sint32>();
	pPlayer->_pHWidth = file->nextLE<Sint32>();
	file->skip(4 * 8); // Skip pointers _pDAnim
	pPlayer->_pDFrames = file->nextLE<Sint32>();
	pPlayer->_pDWidth = file->nextLE<Sint32>();
	file->skip(4 * 8); // Skip pointers _pBAnim
	pPlayer->_pBFrames = file->nextLE<Sint32>();
	pPlayer->_pBWidth = file->nextLE<Sint32>();

	LoadItems(file, NUM_INVLOC, pPlayer->InvBody);
	LoadItems(file, NUM_INV_GRID_ELEM, pPlayer->InvList);
	pPlayer->_pNumInv = file->nextLE<Sint32>();
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++)
		pPlayer->InvGrid[i] = file->nextLE<Sint8>();
	LoadItems(file, MAXBELTITEMS, pPlayer->SpdList);
	LoadItemData(file, &pPlayer->HoldItem);

	pPlayer->_pIMinDam = file->nextLE<Sint32>();
	pPlayer->_pIMaxDam = file->nextLE<Sint32>();
	pPlayer->_pIAC = file->nextLE<Sint32>();
	pPlayer->_pIBonusDam = file->nextLE<Sint32>();
	pPlayer->_pIBonusToHit = file->nextLE<Sint32>();
	pPlayer->_pIBonusAC = file->nextLE<Sint32>();
	pPlayer->_pIBonusDamMod = file->nextLE<Sint32>();
	file->skip(4); // Alignment

	pPlayer->_pISpells = file->nextLE<Uint64>();
	pPlayer->_pIFlags = file->nextLE<Sint32>();
	pPlayer->_pIGetHit = file->nextLE<Sint32>();
	pPlayer->_pISplLvlAdd = file->nextLE<Sint8>();
	file->skip(1); // Unused
	file->skip(2); // Alignment
	pPlayer->_pISplDur = file->nextLE<Sint32>();
	pPlayer->_pIEnAc = file->nextLE<Sint32>();
	pPlayer->_pIFMinDam = file->nextLE<Sint32>();
	pPlayer->_pIFMaxDam = file->nextLE<Sint32>();
	pPlayer->_pILMinDam = file->nextLE<Sint32>();
	pPlayer->_pILMaxDam = file->nextLE<Sint32>();
	pPlayer->_pOilType = (item_misc_id)file->nextLE<Sint32>();
	pPlayer->pTownWarps = file->nextLE<Uint8>();
	pPlayer->pDungMsgs = file->nextLE<Uint8>();
	pPlayer->pLvlLoad = file->nextLE<Uint8>();

	if (gbIsHellfireSaveGame) {
		pPlayer->pDungMsgs2 = file->nextLE<Uint8>();
		pPlayer->pBattleNet = false;
	} else {
		pPlayer->pDungMsgs2 = 0;
		pPlayer->pBattleNet = file->nextBool8();
	}
	pPlayer->pManaShield = file->nextBool8();
	if (gbIsHellfireSaveGame) {
		pPlayer->pOriginalCathedral = file->nextBool8();
	} else {
		file->skip(1);
		pPlayer->pOriginalCathedral = true;
	}
	file->skip(2); // Available bytes
	pPlayer->wReflections = file->nextLE<Uint16>();
	file->skip(14); // Available bytes

	pPlayer->pDiabloKillLevel = file->nextLE<Uint32>();
	pPlayer->pDifficulty = (_difficulty)file->nextLE<Uint32>();
	pPlayer->pDamAcFlags = file->nextLE<Uint32>();
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

	pMonster->_mMTidx = file->nextLE<Sint32>();
	pMonster->_mmode = (MON_MODE)file->nextLE<Sint32>();
	pMonster->_mgoal = (monster_goal)file->nextLE<Uint8>();
	file->skip(3); // Alignment
	pMonster->_mgoalvar1 = file->nextLE<Sint32>();
	pMonster->_mgoalvar2 = file->nextLE<Sint32>();
	pMonster->_mgoalvar3 = file->nextLE<Sint32>();
	file->skip(4); // Unused
	pMonster->_pathcount = file->nextLE<Uint8>();
	file->skip(3); // Alignment
	pMonster->_mx = file->nextLE<Sint32>();
	pMonster->_my = file->nextLE<Sint32>();
	pMonster->_mfutx = file->nextLE<Sint32>();
	pMonster->_mfuty = file->nextLE<Sint32>();
	pMonster->_moldx = file->nextLE<Sint32>();
	pMonster->_moldy = file->nextLE<Sint32>();
	pMonster->_mxoff = file->nextLE<Sint32>();
	pMonster->_myoff = file->nextLE<Sint32>();
	pMonster->_mxvel = file->nextLE<Sint32>();
	pMonster->_myvel = file->nextLE<Sint32>();
	pMonster->_mdir = file->nextLE<Sint32>();
	pMonster->_menemy = file->nextLE<Sint32>();
	pMonster->_menemyx = file->nextLE<Uint8>();
	pMonster->_menemyy = file->nextLE<Uint8>();
	file->skip(2); // Unused

	file->skip(4); // Skip pointer _mAnimData
	pMonster->_mAnimDelay = file->nextLE<Sint32>();
	pMonster->_mAnimCnt = file->nextLE<Sint32>();
	pMonster->_mAnimLen = file->nextLE<Sint32>();
	pMonster->_mAnimFrame = file->nextLE<Sint32>();
	file->skip(4); // Skip _meflag
	pMonster->_mDelFlag = file->nextBool32();
	pMonster->_mVar1 = file->nextLE<Sint32>();
	pMonster->_mVar2 = file->nextLE<Sint32>();
	pMonster->_mVar3 = file->nextLE<Sint32>();
	pMonster->_mVar4 = file->nextLE<Sint32>();
	pMonster->_mVar5 = file->nextLE<Sint32>();
	pMonster->_mVar6 = file->nextLE<Sint32>();
	pMonster->_mVar7 = file->nextLE<Sint32>();
	pMonster->_mVar8 = file->nextLE<Sint32>();
	pMonster->_mmaxhp = file->nextLE<Sint32>();
	pMonster->_mhitpoints = file->nextLE<Sint32>();

	pMonster->_mAi = (_mai_id)file->nextLE<Uint8>();
	pMonster->_mint = file->nextLE<Uint8>();
	file->skip(2); // Alignment
	pMonster->_mFlags = file->nextLE<Uint32>();
	pMonster->_msquelch = file->nextLE<Uint8>();
	file->skip(3); // Alignment
	file->skip(4); // Unused
	pMonster->_lastx = file->nextLE<Sint32>();
	pMonster->_lasty = file->nextLE<Sint32>();
	pMonster->_mRndSeed = file->nextLE<Sint32>();
	pMonster->_mAISeed = file->nextLE<Sint32>();
	file->skip(4); // Unused

	pMonster->_uniqtype = file->nextLE<Uint8>();
	pMonster->_uniqtrans = file->nextLE<Uint8>();
	pMonster->_udeadval = file->nextLE<Sint8>();

	pMonster->mWhoHit = file->nextLE<Sint8>();
	pMonster->mLevel = file->nextLE<Sint8>();
	file->skip(1); // Alignment
	pMonster->mExp = file->nextLE<Uint16>();

	file->skip(1); // Skip mHit as it's already initialized
	pMonster->mMinDamage = file->nextLE<Uint8>();
	pMonster->mMaxDamage = file->nextLE<Uint8>();
	file->skip(1); // Skip mHit2 as it's already initialized
	pMonster->mMinDamage2 = file->nextLE<Uint8>();
	pMonster->mMaxDamage2 = file->nextLE<Uint8>();
	pMonster->mArmorClass = file->nextLE<Uint8>();
	file->skip(1); // Alignment
	pMonster->mMagicRes = file->nextLE<Uint16>();
	file->skip(2); // Alignment

	pMonster->mtalkmsg = file->nextLE<Sint32>();
	pMonster->leader = file->nextLE<Uint8>();
	pMonster->leaderflag = file->nextLE<Uint8>();
	pMonster->packsize = file->nextLE<Uint8>();
	pMonster->mlid = file->nextLE<Sint8>();
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

	pMissile->_mitype = file->nextLE<Sint32>();
	pMissile->_mix = file->nextLE<Sint32>();
	pMissile->_miy = file->nextLE<Sint32>();
	pMissile->_mixoff = file->nextLE<Sint32>();
	pMissile->_miyoff = file->nextLE<Sint32>();
	pMissile->_mixvel = file->nextLE<Sint32>();
	pMissile->_miyvel = file->nextLE<Sint32>();
	pMissile->_misx = file->nextLE<Sint32>();
	pMissile->_misy = file->nextLE<Sint32>();
	pMissile->_mitxoff = file->nextLE<Sint32>();
	pMissile->_mityoff = file->nextLE<Sint32>();
	pMissile->_mimfnum = file->nextLE<Sint32>();
	pMissile->_mispllvl = file->nextLE<Sint32>();
	pMissile->_miDelFlag = file->nextBool32();
	pMissile->_miAnimType = file->nextLE<Uint8>();
	file->skip(3); // Alignment
	pMissile->_miAnimFlags = file->nextLE<Sint32>();
	file->skip(4); // Skip pointer _miAnimData
	pMissile->_miAnimDelay = file->nextLE<Sint32>();
	pMissile->_miAnimLen = file->nextLE<Sint32>();
	pMissile->_miAnimWidth = file->nextLE<Sint32>();
	pMissile->_miAnimWidth2 = file->nextLE<Sint32>();
	pMissile->_miAnimCnt = file->nextLE<Sint32>();
	pMissile->_miAnimAdd = file->nextLE<Sint32>();
	pMissile->_miAnimFrame = file->nextLE<Sint32>();
	pMissile->_miDrawFlag = file->nextBool32();
	pMissile->_miLightFlag = file->nextBool32();
	pMissile->_miPreFlag = file->nextBool32();
	pMissile->_miUniqTrans = file->nextLE<Uint32>();
	pMissile->_mirange = file->nextLE<Sint32>();
	pMissile->_misource = file->nextLE<Sint32>();
	pMissile->_micaster = file->nextLE<Sint32>();
	pMissile->_midam = file->nextLE<Sint32>();
	pMissile->_miHitFlag = file->nextBool32();
	pMissile->_midist = file->nextLE<Sint32>();
	pMissile->_mlid = file->nextLE<Sint32>();
	pMissile->_mirnd = file->nextLE<Sint32>();
	pMissile->_miVar1 = file->nextLE<Sint32>();
	pMissile->_miVar2 = file->nextLE<Sint32>();
	pMissile->_miVar3 = file->nextLE<Sint32>();
	pMissile->_miVar4 = file->nextLE<Sint32>();
	pMissile->_miVar5 = file->nextLE<Sint32>();
	pMissile->_miVar6 = file->nextLE<Sint32>();
	pMissile->_miVar7 = file->nextLE<Sint32>();
	pMissile->_miVar8 = file->nextLE<Sint32>();
}

static void LoadObject(LoadHelper *file, int i)
{
	ObjectStruct *pObject = &object[i];

	pObject->_otype = (_object_id)file->nextLE<Sint32>();
	pObject->_ox = file->nextLE<Sint32>();
	pObject->_oy = file->nextLE<Sint32>();
	pObject->_oLight = file->nextBool32();
	pObject->_oAnimFlag = file->nextLE<Uint32>();
	file->skip(4); // Skip pointer _oAnimData
	pObject->_oAnimDelay = file->nextLE<Sint32>();
	pObject->_oAnimCnt = file->nextLE<Sint32>();
	pObject->_oAnimLen = file->nextLE<Sint32>();
	pObject->_oAnimFrame = file->nextLE<Sint32>();
	pObject->_oAnimWidth = file->nextLE<Sint32>();
	pObject->_oAnimWidth2 = file->nextLE<Sint32>();
	pObject->_oDelFlag = file->nextBool32();
	pObject->_oBreak = file->nextLE<Sint8>();
	file->skip(3); // Alignment
	pObject->_oSolidFlag = file->nextBool32();
	pObject->_oMissFlag = file->nextBool32();

	pObject->_oSelFlag = file->nextLE<Sint8>();
	file->skip(3); // Alignment
	pObject->_oPreFlag = file->nextBool32();
	pObject->_oTrapFlag = file->nextBool32();
	pObject->_oDoorFlag = file->nextBool32();
	pObject->_olid = file->nextLE<Sint32>();
	pObject->_oRndSeed = file->nextLE<Sint32>();
	pObject->_oVar1 = file->nextLE<Sint32>();
	pObject->_oVar2 = file->nextLE<Sint32>();
	pObject->_oVar3 = file->nextLE<Sint32>();
	pObject->_oVar4 = file->nextLE<Sint32>();
	pObject->_oVar5 = file->nextLE<Sint32>();
	pObject->_oVar6 = file->nextLE<Sint32>();
	pObject->_oVar7 = (_speech_id)file->nextLE<Sint32>();
	pObject->_oVar8 = file->nextLE<Sint32>();
}

static void LoadItem(LoadHelper *file, int i)
{
	LoadItemData(file, &items[i]);
	GetItemFrm(i);
}

static void LoadPremium(LoadHelper *file, int i)
{
	LoadItemData(file, &premiumitem[i]);
}

static void LoadQuest(LoadHelper *file, int i)
{
	QuestStruct *pQuest = &quests[i];

	pQuest->_qlevel = file->nextLE<Uint8>();
	pQuest->_qtype = file->nextLE<Uint8>();
	pQuest->_qactive = (quest_state)file->nextLE<Uint8>();
	pQuest->_qlvltype = (dungeon_type)file->nextLE<Uint8>();
	pQuest->_qtx = file->nextLE<Sint32>();
	pQuest->_qty = file->nextLE<Sint32>();
	pQuest->_qslvl = (_setlevels)file->nextLE<Uint8>();
	pQuest->_qidx = file->nextLE<Uint8>();
	if (gbIsHellfireSaveGame) {
		file->skip(2); // Alignment
		pQuest->_qmsg = file->nextLE<Sint32>();
	} else {
		pQuest->_qmsg = file->nextLE<Uint8>();
	}
	pQuest->_qvar1 = file->nextLE<Uint8>();
	pQuest->_qvar2 = file->nextLE<Uint8>();
	file->skip(2); // Alignment
	if (!gbIsHellfireSaveGame)
		file->skip(1); // Alignment
	pQuest->_qlog = file->nextBool32();

	ReturnLvlX = file->nextBE<Sint32>();
	ReturnLvlY = file->nextBE<Sint32>();
	ReturnLvl = file->nextBE<Sint32>();
	ReturnLvlT = (dungeon_type)file->nextBE<Sint32>();
	DoomQuestState = file->nextBE<Sint32>();
}

static void LoadLighting(LoadHelper *file, LightListStruct *pLight)
{
	pLight->_lx = file->nextLE<Sint32>();
	pLight->_ly = file->nextLE<Sint32>();
	pLight->_lradius = file->nextLE<Sint32>();
	pLight->_lid = file->nextLE<Sint32>();
	pLight->_ldel = file->nextBool32();
	pLight->_lunflag = file->nextBool32();
	file->skip(4); // Unused
	pLight->_lunx = file->nextLE<Sint32>();
	pLight->_luny = file->nextLE<Sint32>();
	pLight->_lunr = file->nextLE<Sint32>();
	pLight->_xoff = file->nextLE<Sint32>();
	pLight->_yoff = file->nextLE<Sint32>();
	pLight->_lflags = file->nextBool32();
}

static void LoadPortal(LoadHelper *file, int i)
{
	PortalStruct *pPortal = &portal[i];

	pPortal->open = file->nextBool32();
	pPortal->x = file->nextLE<Sint32>();
	pPortal->y = file->nextLE<Sint32>();
	pPortal->level = file->nextLE<Sint32>();
	pPortal->ltype = (dungeon_type)file->nextLE<Sint32>();
	pPortal->setlvl = file->nextBool32();
}

int RemapItemIdxFromDiablo(int i)
{
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
}

int RemapItemIdxToDiablo(int i)
{
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
}

bool IsHeaderValid(Uint32 magicNumber)
{
	gbIsHellfireSaveGame = false;
	if (magicNumber == LOAD_LE32("SHAR")) {
		return true;
	} else if (magicNumber == LOAD_LE32("SHLF")) {
		gbIsHellfireSaveGame = true;
		return true;
	} else if (!gbIsSpawn && magicNumber == LOAD_LE32("RETL")) {
		return true;
	} else if (!gbIsSpawn && magicNumber == LOAD_LE32("HELF")) {
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
	for (int i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_NOTAVAIL) {
			continue;
		}

		leveltype = quests[i]._qlvltype;
		if (leveltype == DTYPE_NONE) {
			continue;
		}

		setlvlnum = quests[i]._qslvl;
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

	const size_t nHotkeyTypes = sizeof(plr[myplr]._pSplHotKey) / sizeof(plr[myplr]._pSplHotKey[0]);
	const size_t nHotkeySpells = sizeof(plr[myplr]._pSplTHotKey) / sizeof(plr[myplr]._pSplTHotKey[0]);

	for (size_t i = 0; i < nHotkeyTypes; i++) {
		plr[myplr]._pSplHotKey[i] = (spell_id)file.nextLE<Sint32>();
	}
	for (size_t i = 0; i < nHotkeySpells; i++) {
		plr[myplr]._pSplTHotKey[i] = (spell_type)file.nextLE<Sint8>();
	}
	plr[myplr]._pRSpell = (spell_id)file.nextLE<Sint32>();
	plr[myplr]._pRSplType = (spell_type)file.nextLE<Sint8>();
}

void SaveHotkeys()
{
	const size_t nHotkeyTypes = sizeof(plr[myplr]._pSplHotKey) / sizeof(plr[myplr]._pSplHotKey[0]);
	const size_t nHotkeySpells = sizeof(plr[myplr]._pSplTHotKey) / sizeof(plr[myplr]._pSplTHotKey[0]);

	SaveHelper file("hotkeys", (nHotkeyTypes * 4) + nHotkeySpells + 4 + 1);

	for (size_t i = 0; i < nHotkeyTypes; i++) {
		file.writeLE<Sint32>(plr[myplr]._pSplHotKey[i]);
	}
	for (size_t i = 0; i < nHotkeySpells; i++) {
		file.writeLE<Uint8>(plr[myplr]._pSplTHotKey[i]);
	}
	file.writeLE<Sint32>(plr[myplr]._pRSpell);
	file.writeLE<Uint8>(plr[myplr]._pRSplType);
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

void LoadHeroItems(PlayerStruct *pPlayer)
{
	LoadHelper file("heroitems");
	if (!file.isValid())
		return;

	gbIsHellfireSaveGame = file.nextBool8();

	LoadMatchingItems(&file, NUM_INVLOC, pPlayer->InvBody);
	LoadMatchingItems(&file, NUM_INV_GRID_ELEM, pPlayer->InvList);
	LoadMatchingItems(&file, MAXBELTITEMS, pPlayer->SpdList);

	gbIsHellfireSaveGame = gbIsHellfire;
}

void RemoveEmptyInventory(int pnum)
{
	for (int i = NUM_INV_GRID_ELEM; i > 0; i--) {
		int idx = plr[pnum].InvGrid[i - 1];
		if (idx > 0 && plr[pnum].InvList[idx - 1].isEmpty()) {
			RemoveInvItem(pnum, idx - 1);
		}
	};
}

void RemoveEmptyLevelItems()
{
	for (int i = numitems; i > 0; i--) {
		int ii = itemactive[i];
		if (items[ii].isEmpty()) {
			dItem[items[ii]._ix][items[ii]._iy] = 0;
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
		app_fatal("Unable to open save file archive");

	if (!IsHeaderValid(file.nextLE<Uint32>()))
		app_fatal("Invalid save file");

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
	setlvlnum = (_setlevels)file.nextBE<Uint32>();
	currlevel = file.nextBE<Uint32>();
	leveltype = (dungeon_type)file.nextBE<Uint32>();
	if (!setlevel)
		leveltype = gnLevelTypeTbl[currlevel];
	int _ViewX = file.nextBE<Sint32>();
	int _ViewY = file.nextBE<Sint32>();
	invflag = file.nextBool8();
	chrflag = file.nextBool8();
	int _nummonsters = file.nextBE<Sint32>();
	int _numitems = file.nextBE<Sint32>();
	int _nummissiles = file.nextBE<Sint32>();
	int _nobjects = file.nextBE<Sint32>();

	if (!gbIsHellfire && currlevel > 17)
		app_fatal("Player is on a Hellfire only level");

	for (int i = 0; i < giNumberOfLevels; i++) {
		glSeedTbl[i] = file.nextBE<Uint32>();
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
		RemoveEmptyInventory(myplr);
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

	for (int i = 0; i < MAXMONSTERS; i++)
		monstkills[i] = file.nextBE<Sint32>();

	if (leveltype != DTYPE_TOWN) {
		for (int i = 0; i < MAXMONSTERS; i++)
			monstactive[i] = file.nextBE<Sint32>();
		for (int i = 0; i < nummonsters; i++)
			LoadMonster(&file, monstactive[i]);
		for (int i = 0; i < MAXMISSILES; i++)
			missileactive[i] = file.nextLE<Sint8>();
		for (int i = 0; i < MAXMISSILES; i++)
			missileavail[i] = file.nextLE<Sint8>();
		for (int i = 0; i < nummissiles; i++)
			LoadMissile(&file, missileactive[i]);
		for (int i = 0; i < MAXOBJECTS; i++)
			objectactive[i] = file.nextLE<Sint8>();
		for (int i = 0; i < MAXOBJECTS; i++)
			objectavail[i] = file.nextLE<Sint8>();
		for (int i = 0; i < nobjects; i++)
			LoadObject(&file, objectactive[i]);
		for (int i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);

		numlights = file.nextBE<Sint32>();

		for (int i = 0; i < MAXLIGHTS; i++)
			lightactive[i] = file.nextLE<Uint8>();
		for (int i = 0; i < numlights; i++)
			LoadLighting(&file, &LightList[lightactive[i]]);

		visionid = file.nextBE<Sint32>();
		numvision = file.nextBE<Sint32>();

		for (int i = 0; i < numvision; i++)
			LoadLighting(&file, &VisionList[i]);
	}

	for (int i = 0; i < MAXITEMS; i++)
		itemactive[i] = file.nextLE<Sint8>();
	for (int i = 0; i < MAXITEMS; i++)
		itemavail[i] = file.nextLE<Sint8>();
	for (int i = 0; i < numitems; i++)
		LoadItem(&file, itemactive[i]);
	for (int i = 0; i < 128; i++)
		UniqueItemFlag[i] = file.nextBool8();

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dLight[i][j] = file.nextLE<Sint8>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dFlags[i][j] = file.nextLE<Sint8>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dPlayer[i][j] = file.nextLE<Sint8>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dItem[i][j] = file.nextLE<Sint8>();
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMonster[i][j] = file.nextBE<Sint32>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dDead[i][j] = file.nextLE<Sint8>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dObject[i][j] = file.nextLE<Sint8>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dLight[i][j] = file.nextLE<Sint8>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dPreLight[i][j] = file.nextLE<Sint8>();
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				automapview[i][j] = file.nextBool8();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMissile[i][j] = file.nextLE<Sint8>();
		}
	}

	numpremium = file.nextBE<Sint32>();
	premiumlevel = file.nextBE<Sint32>();

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		LoadPremium(&file, i);
	if (gbIsHellfire && !gbIsHellfireSaveGame)
		SpawnPremium(myplr);

	automapflag = file.nextBool8();
	AutoMapScale = file.nextBE<Sint32>();
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
	int idx = pItem->IDidx;
	if (!gbIsHellfire)
		idx = RemapItemIdxToDiablo(idx);
	int iType = pItem->_itype;
	if (idx == -1) {
		idx = 0;
		iType = ITYPE_NONE;
	}

	file->writeLE<Sint32>(pItem->_iSeed);
	file->writeLE<Sint16>(pItem->_iCreateInfo);
	file->skip(2); // Alignment
	file->writeLE<Sint32>(iType);
	file->writeLE<Sint32>(pItem->_ix);
	file->writeLE<Sint32>(pItem->_iy);
	file->writeLE<Uint32>(pItem->_iAnimFlag);
	file->skip(4); // Skip pointer _iAnimData
	file->writeLE<Sint32>(pItem->_iAnimLen);
	file->writeLE<Sint32>(pItem->_iAnimFrame);
	file->writeLE<Sint32>(pItem->_iAnimWidth);
	file->writeLE<Sint32>(pItem->_iAnimWidth2);
	file->skip(4); // Unused since 1.02
	file->writeLE<Uint8>(pItem->_iSelFlag);
	file->skip(3); // Alignment
	file->writeLE<Uint32>(pItem->_iPostDraw);
	file->writeLE<Uint32>(pItem->_iIdentified);
	file->writeLE<Sint8>(pItem->_iMagical);
	file->writeBytes(pItem->_iName, 64);
	file->writeBytes(pItem->_iIName, 64);
	file->writeLE<Sint8>(pItem->_iLoc);
	file->writeLE<Uint8>(pItem->_iClass);
	file->skip(1); // Alignment
	file->writeLE<Sint32>(pItem->_iCurs);
	file->writeLE<Sint32>(pItem->_ivalue);
	file->writeLE<Sint32>(pItem->_iIvalue);
	file->writeLE<Sint32>(pItem->_iMinDam);
	file->writeLE<Sint32>(pItem->_iMaxDam);
	file->writeLE<Sint32>(pItem->_iAC);
	file->writeLE<Sint32>(pItem->_iFlags);
	file->writeLE<Sint32>(pItem->_iMiscId);
	file->writeLE<Sint32>(pItem->_iSpell);
	file->writeLE<Sint32>(pItem->_iCharges);
	file->writeLE<Sint32>(pItem->_iMaxCharges);
	file->writeLE<Sint32>(pItem->_iDurability);
	file->writeLE<Sint32>(pItem->_iMaxDur);
	file->writeLE<Sint32>(pItem->_iPLDam);
	file->writeLE<Sint32>(pItem->_iPLToHit);
	file->writeLE<Sint32>(pItem->_iPLAC);
	file->writeLE<Sint32>(pItem->_iPLStr);
	file->writeLE<Sint32>(pItem->_iPLMag);
	file->writeLE<Sint32>(pItem->_iPLDex);
	file->writeLE<Sint32>(pItem->_iPLVit);
	file->writeLE<Sint32>(pItem->_iPLFR);
	file->writeLE<Sint32>(pItem->_iPLLR);
	file->writeLE<Sint32>(pItem->_iPLMR);
	file->writeLE<Sint32>(pItem->_iPLMana);
	file->writeLE<Sint32>(pItem->_iPLHP);
	file->writeLE<Sint32>(pItem->_iPLDamMod);
	file->writeLE<Sint32>(pItem->_iPLGetHit);
	file->writeLE<Sint32>(pItem->_iPLLight);
	file->writeLE<Sint8>(pItem->_iSplLvlAdd);
	file->writeLE<Sint8>(pItem->_iRequest);
	file->skip(2); // Alignment
	file->writeLE<Sint32>(pItem->_iUid);
	file->writeLE<Sint32>(pItem->_iFMinDam);
	file->writeLE<Sint32>(pItem->_iFMaxDam);
	file->writeLE<Sint32>(pItem->_iLMinDam);
	file->writeLE<Sint32>(pItem->_iLMaxDam);
	file->writeLE<Sint32>(pItem->_iPLEnAc);
	file->writeLE<Sint8>(pItem->_iPrePower);
	file->writeLE<Sint8>(pItem->_iSufPower);
	file->skip(2); // Alignment
	file->writeLE<Sint32>(pItem->_iVAdd1);
	file->writeLE<Sint32>(pItem->_iVMult1);
	file->writeLE<Sint32>(pItem->_iVAdd2);
	file->writeLE<Sint32>(pItem->_iVMult2);
	file->writeLE<Sint8>(pItem->_iMinStr);
	file->writeLE<Uint8>(pItem->_iMinMag);
	file->writeLE<Sint8>(pItem->_iMinDex);
	file->skip(1); // Alignment
	file->writeLE<Uint32>(pItem->_iStatFlag);
	file->writeLE<Sint32>(idx);
	file->writeLE<Uint32>(pItem->dwBuff);
	if (gbIsHellfire)
		file->writeLE<Uint32>(pItem->_iDamAcFlags);
}

static void SaveItems(SaveHelper *file, ItemStruct *pItem, const int n)
{
	for (int i = 0; i < n; i++) {
		SaveItem(file, &pItem[i]);
	}
}

static void SavePlayer(SaveHelper *file, int p)
{
	PlayerStruct *pPlayer = &plr[p];

	file->writeLE<Sint32>(pPlayer->_pmode);
	for (int i = 0; i < MAX_PATH_LENGTH; i++)
		file->writeLE<Sint8>(pPlayer->walkpath[i]);
	file->writeLE<Uint8>(pPlayer->plractive);
	file->skip(2); // Alignment
	file->writeLE<Sint32>(pPlayer->destAction);
	file->writeLE<Sint32>(pPlayer->destParam1);
	file->writeLE<Sint32>(pPlayer->destParam2);
	file->writeLE<Sint32>(pPlayer->destParam3);
	file->writeLE<Sint32>(pPlayer->destParam4);
	file->writeLE<Sint32>(pPlayer->plrlevel);
	file->writeLE<Sint32>(pPlayer->_px);
	file->writeLE<Sint32>(pPlayer->_py);
	file->writeLE<Sint32>(pPlayer->_pfutx);
	file->writeLE<Sint32>(pPlayer->_pfuty);
	file->writeLE<Sint32>(pPlayer->_ptargx);
	file->writeLE<Sint32>(pPlayer->_ptargy);
	file->writeLE<Sint32>(pPlayer->_pownerx);
	file->writeLE<Sint32>(pPlayer->_pownery);
	file->writeLE<Sint32>(pPlayer->_poldx);
	file->writeLE<Sint32>(pPlayer->_poldy);
	file->writeLE<Sint32>(pPlayer->_pxoff);
	file->writeLE<Sint32>(pPlayer->_pyoff);
	file->writeLE<Sint32>(pPlayer->_pxvel);
	file->writeLE<Sint32>(pPlayer->_pyvel);
	file->writeLE<Sint32>(pPlayer->_pdir);
	file->skip(4); // Unused
	file->writeLE<Sint32>(pPlayer->_pgfxnum);
	file->skip(4); // Skip pointer _pAnimData
	file->writeLE<Sint32>(pPlayer->_pAnimDelay);
	file->writeLE<Sint32>(pPlayer->_pAnimCnt);
	file->writeLE<Sint32>(pPlayer->_pAnimLen);
	file->writeLE<Sint32>(pPlayer->_pAnimFrame);
	file->writeLE<Sint32>(pPlayer->_pAnimWidth);
	file->writeLE<Sint32>(pPlayer->_pAnimWidth2);
	file->skip(4); // Skip _peflag
	file->writeLE<Sint32>(pPlayer->_plid);
	file->writeLE<Sint32>(pPlayer->_pvid);

	file->writeLE<Sint32>(pPlayer->_pSpell);
	file->writeLE<Sint8>(pPlayer->_pSplType);
	file->writeLE<Sint8>(pPlayer->_pSplFrom);
	file->skip(2); // Alignment
	file->writeLE<Sint32>(pPlayer->_pTSpell);
	file->writeLE<Sint8>(pPlayer->_pTSplType);
	file->skip(3); // Alignment
	file->writeLE<Sint32>(pPlayer->_pRSpell);
	file->writeLE<Sint8>(pPlayer->_pRSplType);
	file->skip(3); // Alignment
	file->writeLE<Sint32>(pPlayer->_pSBkSpell);
	file->writeLE<Sint8>(pPlayer->_pSBkSplType);
	for (int i = 0; i < 64; i++)
		file->writeLE<Sint8>(pPlayer->_pSplLvl[i]);
	file->skip(7); // Alignment
	file->writeLE<Uint64>(pPlayer->_pMemSpells);
	file->writeLE<Uint64>(pPlayer->_pAblSpells);
	file->writeLE<Uint64>(pPlayer->_pScrlSpells);
	file->writeLE<Uint8>(pPlayer->_pSpellFlags);
	file->skip(3); // Alignment
	for (int i = 0; i < 4; i++)
		file->writeLE<Sint32>(pPlayer->_pSplHotKey[i]);
	for (int i = 0; i < 4; i++)
		file->writeLE<Sint8>(pPlayer->_pSplTHotKey[i]);

	file->writeLE<Sint32>(pPlayer->_pwtype);
	file->writeLE<Uint8>(pPlayer->_pBlockFlag);
	file->writeLE<Uint8>(pPlayer->_pInvincible);
	file->writeLE<Sint8>(pPlayer->_pLightRad);
	file->writeLE<Uint8>(pPlayer->_pLvlChanging);

	file->writeBytes(pPlayer->_pName, PLR_NAME_LEN);
	file->writeLE<Sint8>(pPlayer->_pClass);
	file->skip(3); // Alignment
	file->writeLE<Sint32>(pPlayer->_pStrength);
	file->writeLE<Sint32>(pPlayer->_pBaseStr);
	file->writeLE<Sint32>(pPlayer->_pMagic);
	file->writeLE<Sint32>(pPlayer->_pBaseMag);
	file->writeLE<Sint32>(pPlayer->_pDexterity);
	file->writeLE<Sint32>(pPlayer->_pBaseDex);
	file->writeLE<Sint32>(pPlayer->_pVitality);
	file->writeLE<Sint32>(pPlayer->_pBaseVit);
	file->writeLE<Sint32>(pPlayer->_pStatPts);
	file->writeLE<Sint32>(pPlayer->_pDamageMod);

	file->writeLE<Sint32>(pPlayer->_pBaseToBlk);
	file->writeLE<Sint32>(pPlayer->_pHPBase);
	file->writeLE<Sint32>(pPlayer->_pMaxHPBase);
	file->writeLE<Sint32>(pPlayer->_pHitPoints);
	file->writeLE<Sint32>(pPlayer->_pMaxHP);
	file->writeLE<Sint32>(pPlayer->_pHPPer);
	file->writeLE<Sint32>(pPlayer->_pManaBase);
	file->writeLE<Sint32>(pPlayer->_pMaxManaBase);
	file->writeLE<Sint32>(pPlayer->_pMana);
	file->writeLE<Sint32>(pPlayer->_pMaxMana);
	file->writeLE<Sint32>(pPlayer->_pManaPer);
	file->writeLE<Sint8>(pPlayer->_pLevel);
	file->writeLE<Sint8>(pPlayer->_pMaxLvl);
	file->skip(2); // Alignment
	file->writeLE<Sint32>(pPlayer->_pExperience);
	file->writeLE<Sint32>(pPlayer->_pMaxExp);
	file->writeLE<Sint32>(pPlayer->_pNextExper);
	file->writeLE<Sint8>(pPlayer->_pArmorClass);
	file->writeLE<Sint8>(pPlayer->_pMagResist);
	file->writeLE<Sint8>(pPlayer->_pFireResist);
	file->writeLE<Sint8>(pPlayer->_pLghtResist);
	file->writeLE<Sint32>(pPlayer->_pGold);

	file->writeLE<Uint32>(pPlayer->_pInfraFlag);
	file->writeLE<Sint32>(pPlayer->_pVar1);
	file->writeLE<Sint32>(pPlayer->_pVar2);
	file->writeLE<Sint32>(pPlayer->_pVar3);
	file->writeLE<Sint32>(pPlayer->_pVar4);
	file->writeLE<Sint32>(pPlayer->_pVar5);
	file->writeLE<Sint32>(pPlayer->_pVar6);
	file->writeLE<Sint32>(pPlayer->_pVar7);
	file->writeLE<Sint32>(pPlayer->_pVar8);
	for (int i = 0; i < giNumberOfLevels; i++)
		file->writeLE<Uint8>(pPlayer->_pLvlVisited[i]);
	for (int i = 0; i < giNumberOfLevels; i++)
		file->writeLE<Uint8>(pPlayer->_pSLvlVisited[i]); // only 10 used

	file->skip(2); // Alignment

	file->writeLE<Sint32>(pPlayer->_pGFXLoad);
	file->skip(4 * 8); // Skip pointers _pNAnim
	file->writeLE<Sint32>(pPlayer->_pNFrames);
	file->writeLE<Sint32>(pPlayer->_pNWidth);
	file->skip(4 * 8); // Skip pointers _pWAnim
	file->writeLE<Sint32>(pPlayer->_pWFrames);
	file->writeLE<Sint32>(pPlayer->_pWWidth);
	file->skip(4 * 8); // Skip pointers _pAAnim
	file->writeLE<Sint32>(pPlayer->_pAFrames);
	file->writeLE<Sint32>(pPlayer->_pAWidth);
	file->writeLE<Sint32>(pPlayer->_pAFNum);
	file->skip(4 * 8); // Skip pointers _pLAnim
	file->skip(4 * 8); // Skip pointers _pFAnim
	file->skip(4 * 8); // Skip pointers _pTAnim
	file->writeLE<Sint32>(pPlayer->_pSFrames);
	file->writeLE<Sint32>(pPlayer->_pSWidth);
	file->writeLE<Sint32>(pPlayer->_pSFNum);
	file->skip(4 * 8); // Skip pointers _pHAnim
	file->writeLE<Sint32>(pPlayer->_pHFrames);
	file->writeLE<Sint32>(pPlayer->_pHWidth);
	file->skip(4 * 8); // Skip pointers _pDAnim
	file->writeLE<Sint32>(pPlayer->_pDFrames);
	file->writeLE<Sint32>(pPlayer->_pDWidth);
	file->skip(4 * 8); // Skip pointers _pBAnim
	file->writeLE<Sint32>(pPlayer->_pBFrames);
	file->writeLE<Sint32>(pPlayer->_pBWidth);

	SaveItems(file, pPlayer->InvBody, NUM_INVLOC);
	SaveItems(file, pPlayer->InvList, NUM_INV_GRID_ELEM);
	file->writeLE<Sint32>(pPlayer->_pNumInv);
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++)
		file->writeLE<Sint8>(pPlayer->InvGrid[i]);
	SaveItems(file, pPlayer->SpdList, MAXBELTITEMS);
	SaveItem(file, &pPlayer->HoldItem);

	file->writeLE<Sint32>(pPlayer->_pIMinDam);
	file->writeLE<Sint32>(pPlayer->_pIMaxDam);
	file->writeLE<Sint32>(pPlayer->_pIAC);
	file->writeLE<Sint32>(pPlayer->_pIBonusDam);
	file->writeLE<Sint32>(pPlayer->_pIBonusToHit);
	file->writeLE<Sint32>(pPlayer->_pIBonusAC);
	file->writeLE<Sint32>(pPlayer->_pIBonusDamMod);
	file->skip(4); // Alignment

	file->writeLE<Uint64>(pPlayer->_pISpells);
	file->writeLE<Sint32>(pPlayer->_pIFlags);
	file->writeLE<Sint32>(pPlayer->_pIGetHit);

	file->writeLE<Sint8>(pPlayer->_pISplLvlAdd);
	file->skip(1); // Unused
	file->skip(2); // Alignment
	file->writeLE<Sint32>(pPlayer->_pISplDur);
	file->writeLE<Sint32>(pPlayer->_pIEnAc);
	file->writeLE<Sint32>(pPlayer->_pIFMinDam);
	file->writeLE<Sint32>(pPlayer->_pIFMaxDam);
	file->writeLE<Sint32>(pPlayer->_pILMinDam);
	file->writeLE<Sint32>(pPlayer->_pILMaxDam);
	file->writeLE<Sint32>(pPlayer->_pOilType);
	file->writeLE<Uint8>(pPlayer->pTownWarps);
	file->writeLE<Uint8>(pPlayer->pDungMsgs);
	file->writeLE<Uint8>(pPlayer->pLvlLoad);
	if (gbIsHellfire)
		file->writeLE<Uint8>(pPlayer->pDungMsgs2);
	else
		file->writeLE<Uint8>(pPlayer->pBattleNet);
	file->writeLE<Uint8>(pPlayer->pManaShield);
	file->writeLE<Uint8>(pPlayer->pOriginalCathedral);
	file->skip(2); // Available bytes
	file->writeLE<Uint16>(pPlayer->wReflections);
	file->skip(14); // Available bytes

	file->writeLE<Uint32>(pPlayer->pDiabloKillLevel);
	file->writeLE<Uint32>(pPlayer->pDifficulty);
	file->writeLE<Uint32>(pPlayer->pDamAcFlags);
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

	file->writeLE<Sint32>(pMonster->_mMTidx);
	file->writeLE<Sint32>(pMonster->_mmode);
	file->writeLE<Uint8>(pMonster->_mgoal);
	file->skip(3); // Alignment
	file->writeLE<Sint32>(pMonster->_mgoalvar1);
	file->writeLE<Sint32>(pMonster->_mgoalvar2);
	file->writeLE<Sint32>(pMonster->_mgoalvar3);
	file->skip(4); // Unused
	file->writeLE<Uint8>(pMonster->_pathcount);
	file->skip(3); // Alignment
	file->writeLE<Sint32>(pMonster->_mx);
	file->writeLE<Sint32>(pMonster->_my);
	file->writeLE<Sint32>(pMonster->_mfutx);
	file->writeLE<Sint32>(pMonster->_mfuty);
	file->writeLE<Sint32>(pMonster->_moldx);
	file->writeLE<Sint32>(pMonster->_moldy);
	file->writeLE<Sint32>(pMonster->_mxoff);
	file->writeLE<Sint32>(pMonster->_myoff);
	file->writeLE<Sint32>(pMonster->_mxvel);
	file->writeLE<Sint32>(pMonster->_myvel);
	file->writeLE<Sint32>(pMonster->_mdir);
	file->writeLE<Sint32>(pMonster->_menemy);
	file->writeLE<Uint8>(pMonster->_menemyx);
	file->writeLE<Uint8>(pMonster->_menemyy);
	file->skip(2); // Unused

	file->skip(4); // Skip pointer _mAnimData
	file->writeLE<Sint32>(pMonster->_mAnimDelay);
	file->writeLE<Sint32>(pMonster->_mAnimCnt);
	file->writeLE<Sint32>(pMonster->_mAnimLen);
	file->writeLE<Sint32>(pMonster->_mAnimFrame);
	file->skip(4); // Skip _meflag
	file->writeLE<Uint32>(pMonster->_mDelFlag);
	file->writeLE<Sint32>(pMonster->_mVar1);
	file->writeLE<Sint32>(pMonster->_mVar2);
	file->writeLE<Sint32>(pMonster->_mVar3);
	file->writeLE<Sint32>(pMonster->_mVar4);
	file->writeLE<Sint32>(pMonster->_mVar5);
	file->writeLE<Sint32>(pMonster->_mVar6);
	file->writeLE<Sint32>(pMonster->_mVar7);
	file->writeLE<Sint32>(pMonster->_mVar8);
	file->writeLE<Sint32>(pMonster->_mmaxhp);
	file->writeLE<Sint32>(pMonster->_mhitpoints);

	file->writeLE<Uint8>(pMonster->_mAi);
	file->writeLE<Uint8>(pMonster->_mint);
	file->skip(2); // Alignment
	file->writeLE<Uint32>(pMonster->_mFlags);
	file->writeLE<Uint8>(pMonster->_msquelch);
	file->skip(3); // Alignment
	file->skip(4); // Unused
	file->writeLE<Sint32>(pMonster->_lastx);
	file->writeLE<Sint32>(pMonster->_lasty);
	file->writeLE<Sint32>(pMonster->_mRndSeed);
	file->writeLE<Sint32>(pMonster->_mAISeed);
	file->skip(4); // Unused

	file->writeLE<Uint8>(pMonster->_uniqtype);
	file->writeLE<Uint8>(pMonster->_uniqtrans);
	file->writeLE<Sint8>(pMonster->_udeadval);

	file->writeLE<Sint8>(pMonster->mWhoHit);
	file->writeLE<Sint8>(pMonster->mLevel);
	file->skip(1); // Alignment
	file->writeLE<Uint16>(pMonster->mExp);

	file->writeLE<Uint8>(pMonster->mHit < SCHAR_MAX ? pMonster->mHit : SCHAR_MAX); // For backwards compatibility
	file->writeLE<Uint8>(pMonster->mMinDamage);
	file->writeLE<Uint8>(pMonster->mMaxDamage);
	file->writeLE<Uint8>(pMonster->mHit2 < SCHAR_MAX ? pMonster->mHit2 : SCHAR_MAX); // For backwards compatibility
	file->writeLE<Uint8>(pMonster->mMinDamage2);
	file->writeLE<Uint8>(pMonster->mMaxDamage2);
	file->writeLE<Uint8>(pMonster->mArmorClass);
	file->skip(1); // Alignment
	file->writeLE<Uint16>(pMonster->mMagicRes);
	file->skip(2); // Alignment

	file->writeLE<Sint32>(pMonster->mtalkmsg);
	file->writeLE<Uint8>(pMonster->leader);
	file->writeLE<Uint8>(pMonster->leaderflag);
	file->writeLE<Uint8>(pMonster->packsize);
	file->writeLE<Sint8>(pMonster->mlid);

	// Omit pointer mName;
	// Omit pointer MType;
	// Omit pointer MData;
}

static void SaveMissile(SaveHelper *file, int i)
{
	MissileStruct *pMissile = &missile[i];

	file->writeLE<Sint32>(pMissile->_mitype);
	file->writeLE<Sint32>(pMissile->_mix);
	file->writeLE<Sint32>(pMissile->_miy);
	file->writeLE<Sint32>(pMissile->_mixoff);
	file->writeLE<Sint32>(pMissile->_miyoff);
	file->writeLE<Sint32>(pMissile->_mixvel);
	file->writeLE<Sint32>(pMissile->_miyvel);
	file->writeLE<Sint32>(pMissile->_misx);
	file->writeLE<Sint32>(pMissile->_misy);
	file->writeLE<Sint32>(pMissile->_mitxoff);
	file->writeLE<Sint32>(pMissile->_mityoff);
	file->writeLE<Sint32>(pMissile->_mimfnum);
	file->writeLE<Sint32>(pMissile->_mispllvl);
	file->writeLE<Uint32>(pMissile->_miDelFlag);
	file->writeLE<Uint8>(pMissile->_miAnimType);
	file->skip(3); // Alignment
	file->writeLE<Sint32>(pMissile->_miAnimFlags);
	file->skip(4); // Skip pointer _miAnimData
	file->writeLE<Sint32>(pMissile->_miAnimDelay);
	file->writeLE<Sint32>(pMissile->_miAnimLen);
	file->writeLE<Sint32>(pMissile->_miAnimWidth);
	file->writeLE<Sint32>(pMissile->_miAnimWidth2);
	file->writeLE<Sint32>(pMissile->_miAnimCnt);
	file->writeLE<Sint32>(pMissile->_miAnimAdd);
	file->writeLE<Sint32>(pMissile->_miAnimFrame);
	file->writeLE<Uint32>(pMissile->_miDrawFlag);
	file->writeLE<Uint32>(pMissile->_miLightFlag);
	file->writeLE<Uint32>(pMissile->_miPreFlag);
	file->writeLE<Uint32>(pMissile->_miUniqTrans);
	file->writeLE<Sint32>(pMissile->_mirange);
	file->writeLE<Sint32>(pMissile->_misource);
	file->writeLE<Sint32>(pMissile->_micaster);
	file->writeLE<Sint32>(pMissile->_midam);
	file->writeLE<Uint32>(pMissile->_miHitFlag);
	file->writeLE<Sint32>(pMissile->_midist);
	file->writeLE<Sint32>(pMissile->_mlid);
	file->writeLE<Sint32>(pMissile->_mirnd);
	file->writeLE<Sint32>(pMissile->_miVar1);
	file->writeLE<Sint32>(pMissile->_miVar2);
	file->writeLE<Sint32>(pMissile->_miVar3);
	file->writeLE<Sint32>(pMissile->_miVar4);
	file->writeLE<Sint32>(pMissile->_miVar5);
	file->writeLE<Sint32>(pMissile->_miVar6);
	file->writeLE<Sint32>(pMissile->_miVar7);
	file->writeLE<Sint32>(pMissile->_miVar8);
}

static void SaveObject(SaveHelper *file, int i)
{
	ObjectStruct *pObject = &object[i];

	file->writeLE<Sint32>(pObject->_otype);
	file->writeLE<Sint32>(pObject->_ox);
	file->writeLE<Sint32>(pObject->_oy);
	file->writeLE<Uint32>(pObject->_oLight);
	file->writeLE<Uint32>(pObject->_oAnimFlag);
	file->skip(4); // Skip pointer _oAnimData
	file->writeLE<Sint32>(pObject->_oAnimDelay);
	file->writeLE<Sint32>(pObject->_oAnimCnt);
	file->writeLE<Sint32>(pObject->_oAnimLen);
	file->writeLE<Sint32>(pObject->_oAnimFrame);
	file->writeLE<Sint32>(pObject->_oAnimWidth);
	file->writeLE<Sint32>(pObject->_oAnimWidth2);
	file->writeLE<Uint32>(pObject->_oDelFlag);
	file->writeLE<Sint8>(pObject->_oBreak);
	file->skip(3); // Alignment
	file->writeLE<Uint32>(pObject->_oSolidFlag);
	file->writeLE<Uint32>(pObject->_oMissFlag);

	file->writeLE<Sint8>(pObject->_oSelFlag);
	file->skip(3); // Alignment
	file->writeLE<Uint32>(pObject->_oPreFlag);
	file->writeLE<Uint32>(pObject->_oTrapFlag);
	file->writeLE<Uint32>(pObject->_oDoorFlag);
	file->writeLE<Sint32>(pObject->_olid);
	file->writeLE<Sint32>(pObject->_oRndSeed);
	file->writeLE<Sint32>(pObject->_oVar1);
	file->writeLE<Sint32>(pObject->_oVar2);
	file->writeLE<Sint32>(pObject->_oVar3);
	file->writeLE<Sint32>(pObject->_oVar4);
	file->writeLE<Sint32>(pObject->_oVar5);
	file->writeLE<Sint32>(pObject->_oVar6);
	file->writeLE<Sint32>(pObject->_oVar7);
	file->writeLE<Sint32>(pObject->_oVar8);
}

static void SavePremium(SaveHelper *file, int i)
{
	SaveItem(file, &premiumitem[i]);
}

static void SaveQuest(SaveHelper *file, int i)
{
	QuestStruct *pQuest = &quests[i];

	file->writeLE<Uint8>(pQuest->_qlevel);
	file->writeLE<Uint8>(pQuest->_qtype);
	file->writeLE<Uint8>(pQuest->_qactive);
	file->writeLE<Uint8>(pQuest->_qlvltype);
	file->writeLE<Sint32>(pQuest->_qtx);
	file->writeLE<Sint32>(pQuest->_qty);
	file->writeLE<Uint8>(pQuest->_qslvl);
	file->writeLE<Uint8>(pQuest->_qidx);
	if (gbIsHellfire) {
		file->skip(2); // Alignment
		file->writeLE<Sint32>(pQuest->_qmsg);
	} else {
		file->writeLE<Uint8>(pQuest->_qmsg);
	}
	file->writeLE<Uint8>(pQuest->_qvar1);
	file->writeLE<Uint8>(pQuest->_qvar2);
	file->skip(2); // Alignment
	if (!gbIsHellfire)
		file->skip(1); // Alignment
	file->writeLE<Uint32>(pQuest->_qlog);

	file->writeBE<Sint32>(ReturnLvlX);
	file->writeBE<Sint32>(ReturnLvlY);
	file->writeBE<Sint32>(ReturnLvl);
	file->writeBE<Sint32>(ReturnLvlT);
	file->writeBE<Sint32>(DoomQuestState);
}

static void SaveLighting(SaveHelper *file, LightListStruct *pLight)
{
	file->writeLE<Sint32>(pLight->_lx);
	file->writeLE<Sint32>(pLight->_ly);
	file->writeLE<Sint32>(pLight->_lradius);
	file->writeLE<Sint32>(pLight->_lid);
	file->writeLE<Uint32>(pLight->_ldel);
	file->writeLE<Uint32>(pLight->_lunflag);
	file->skip(4); // Unused
	file->writeLE<Sint32>(pLight->_lunx);
	file->writeLE<Sint32>(pLight->_luny);
	file->writeLE<Sint32>(pLight->_lunr);
	file->writeLE<Sint32>(pLight->_xoff);
	file->writeLE<Sint32>(pLight->_yoff);
	file->writeLE<Uint32>(pLight->_lflags);
}

static void SavePortal(SaveHelper *file, int i)
{
	PortalStruct *pPortal = &portal[i];

	file->writeLE<Uint32>(pPortal->open);
	file->writeLE<Sint32>(pPortal->x);
	file->writeLE<Sint32>(pPortal->y);
	file->writeLE<Sint32>(pPortal->level);
	file->writeLE<Sint32>(pPortal->ltype);
	file->writeLE<Uint32>(pPortal->setlvl);
}

const int DiabloItemSaveSize = 368;
const int HellfireItemSaveSize = 372;

void SaveHeroItems(PlayerStruct *pPlayer)
{
	size_t items = NUM_INVLOC + NUM_INV_GRID_ELEM + MAXBELTITEMS;
	SaveHelper file("heroitems", items * (gbIsHellfire ? HellfireItemSaveSize : DiabloItemSaveSize) + sizeof(Uint8));

	file.writeLE<Uint8>(gbIsHellfire);

	SaveItems(&file, pPlayer->InvBody, NUM_INVLOC);
	SaveItems(&file, pPlayer->InvList, NUM_INV_GRID_ELEM);
	SaveItems(&file, pPlayer->SpdList, MAXBELTITEMS);
}

void SaveGameData()
{
	SaveHelper file("game", FILEBUFF);

	if (gbIsSpawn && !gbIsHellfire)
		file.writeLE<Uint32>(LOAD_LE32("SHAR"));
	else if (gbIsSpawn && gbIsHellfire)
		file.writeLE<Uint32>(LOAD_LE32("SHLF"));
	else if (!gbIsSpawn && gbIsHellfire)
		file.writeLE<Uint32>(LOAD_LE32("HELF"));
	else if (!gbIsSpawn && !gbIsHellfire)
		file.writeLE<Uint32>(LOAD_LE32("RETL"));
	else
		app_fatal("Invalid game state");

	if (gbIsHellfire) {
		giNumberOfLevels = 25;
		giNumberQuests = 24;
		giNumberOfSmithPremiumItems = 15;
	} else {
		giNumberOfLevels = 17;
		giNumberQuests = 16;
		giNumberOfSmithPremiumItems = 6;
	}

	file.writeLE<Uint8>(setlevel);
	file.writeBE<Uint32>(setlvlnum);
	file.writeBE<Uint32>(currlevel);
	file.writeBE<Uint32>(leveltype);
	file.writeBE<Sint32>(ViewX);
	file.writeBE<Sint32>(ViewY);
	file.writeLE<Uint8>(invflag);
	file.writeLE<Uint8>(chrflag);
	file.writeBE<Sint32>(nummonsters);
	file.writeBE<Sint32>(numitems);
	file.writeBE<Sint32>(nummissiles);
	file.writeBE<Sint32>(nobjects);

	for (int i = 0; i < giNumberOfLevels; i++) {
		file.writeBE<Uint32>(glSeedTbl[i]);
		file.writeBE<Sint32>(gnLevelTypeTbl[i]);
	}

	plr[myplr].pDifficulty = sgGameInitInfo.nDifficulty;
	SavePlayer(&file, myplr);

	for (int i = 0; i < giNumberQuests; i++)
		SaveQuest(&file, i);
	for (int i = 0; i < MAXPORTAL; i++)
		SavePortal(&file, i);
	for (int i = 0; i < MAXMONSTERS; i++)
		file.writeBE<Sint32>(monstkills[i]);

	if (leveltype != DTYPE_TOWN) {
		for (int i = 0; i < MAXMONSTERS; i++)
			file.writeBE<Sint32>(monstactive[i]);
		for (int i = 0; i < nummonsters; i++)
			SaveMonster(&file, monstactive[i]);
		for (int i = 0; i < MAXMISSILES; i++)
			file.writeLE<Sint8>(missileactive[i]);
		for (int i = 0; i < MAXMISSILES; i++)
			file.writeLE<Sint8>(missileavail[i]);
		for (int i = 0; i < nummissiles; i++)
			SaveMissile(&file, missileactive[i]);
		for (int i = 0; i < MAXOBJECTS; i++)
			file.writeLE<Sint8>(objectactive[i]);
		for (int i = 0; i < MAXOBJECTS; i++)
			file.writeLE<Sint8>(objectavail[i]);
		for (int i = 0; i < nobjects; i++)
			SaveObject(&file, objectactive[i]);

		file.writeBE<Sint32>(numlights);

		for (int i = 0; i < MAXLIGHTS; i++)
			file.writeLE<Uint8>(lightactive[i]);
		for (int i = 0; i < numlights; i++)
			SaveLighting(&file, &LightList[lightactive[i]]);

		file.writeBE<Sint32>(visionid);
		file.writeBE<Sint32>(numvision);

		for (int i = 0; i < numvision; i++)
			SaveLighting(&file, &VisionList[i]);
	}

	for (int i = 0; i < MAXITEMS; i++)
		file.writeLE<Sint8>(itemactive[i]);
	for (int i = 0; i < MAXITEMS; i++)
		file.writeLE<Sint8>(itemavail[i]);
	for (int i = 0; i < numitems; i++)
		SaveItem(&file, &items[itemactive[i]]);
	for (int i = 0; i < 128; i++)
		file.writeLE<Sint8>(UniqueItemFlag[i]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<Sint8>(dLight[i][j]);
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<Sint8>(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<Sint8>(dPlayer[i][j]);
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<Sint8>(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeBE<Sint32>(dMonster[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dDead[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dObject[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dLight[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dPreLight[i][j]);
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				file.writeLE<Uint8>(automapview[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dMissile[i][j]);
		}
	}

	file.writeBE<Sint32>(numpremium);
	file.writeBE<Sint32>(premiumlevel);

	for (int i = 0; i < giNumberOfSmithPremiumItems; i++)
		SavePremium(&file, i);

	file.writeLE<Uint8>(automapflag);
	file.writeBE<Sint32>(AutoMapScale);

}

void SaveGame() {
	gbValidSaveFile = true;
	pfile_write_hero(/*save_game_data=*/true);
}

void SaveLevel()
{
	PFileScopedArchiveWriter scoped_writer;

	DoUnVision(plr[myplr]._px, plr[myplr]._py, plr[myplr]._pLightRad); // fix for vision staying on the level

	if (currlevel == 0)
		glSeedTbl[0] = AdvanceRndSeed();

	char szName[MAX_PATH];
	GetTempLevelNames(szName);
	SaveHelper file(szName, FILEBUFF);

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dDead[i][j]);
		}
	}

	file.writeBE<Sint32>(nummonsters);
	file.writeBE<Sint32>(numitems);
	file.writeBE<Sint32>(nobjects);

	if (leveltype != DTYPE_TOWN) {
		for (int i = 0; i < MAXMONSTERS; i++)
			file.writeBE<Sint32>(monstactive[i]);
		for (int i = 0; i < nummonsters; i++)
			SaveMonster(&file, monstactive[i]);
		for (int i = 0; i < MAXOBJECTS; i++)
			file.writeLE<Sint8>(objectactive[i]);
		for (int i = 0; i < MAXOBJECTS; i++)
			file.writeLE<Sint8>(objectavail[i]);
		for (int i = 0; i < nobjects; i++)
			SaveObject(&file, objectactive[i]);
	}

	for (int i = 0; i < MAXITEMS; i++)
		file.writeLE<Sint8>(itemactive[i]);
	for (int i = 0; i < MAXITEMS; i++)
		file.writeLE<Sint8>(itemavail[i]);

	for (int i = 0; i < numitems; i++)
		SaveItem(&file, &items[itemactive[i]]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<Sint8>(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			file.writeLE<Sint8>(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeBE<Sint32>(dMonster[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dObject[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dLight[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dPreLight[i][j]);
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				file.writeLE<Uint8>(automapview[i][j]);
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				file.writeLE<Sint8>(dMissile[i][j]);
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
		app_fatal("Unable to open save file archive");

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dDead[i][j] = file.nextLE<Sint8>();
		}
		SetDead();
	}

	nummonsters = file.nextBE<Sint32>();
	numitems = file.nextBE<Sint32>();
	nobjects = file.nextBE<Sint32>();

	if (leveltype != DTYPE_TOWN) {
		for (int i = 0; i < MAXMONSTERS; i++)
			monstactive[i] = file.nextBE<Sint32>();
		for (int i = 0; i < nummonsters; i++)
			LoadMonster(&file, monstactive[i]);
		for (int i = 0; i < MAXOBJECTS; i++)
			objectactive[i] = file.nextLE<Sint8>();
		for (int i = 0; i < MAXOBJECTS; i++)
			objectavail[i] = file.nextLE<Sint8>();
		for (int i = 0; i < nobjects; i++)
			LoadObject(&file, objectactive[i]);
		if (!gbSkipSync) {
			for (int i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
		}
	}

	for (int i = 0; i < MAXITEMS; i++)
		itemactive[i] = file.nextLE<Sint8>();
	for (int i = 0; i < MAXITEMS; i++)
		itemavail[i] = file.nextLE<Sint8>();
	for (int i = 0; i < numitems; i++)
		LoadItem(&file, itemactive[i]);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dFlags[i][j] = file.nextLE<Sint8>();
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++)
			dItem[i][j] = file.nextLE<Sint8>();
	}

	if (leveltype != DTYPE_TOWN) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMonster[i][j] = file.nextBE<Sint32>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dObject[i][j] = file.nextLE<Sint8>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dLight[i][j] = file.nextLE<Sint8>();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dPreLight[i][j] = file.nextLE<Sint8>();
		}
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++)
				automapview[i][j] = file.nextBool8();
		}
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++)
				dMissile[i][j] = 0; /// BUGFIX: supposed to load saved missiles with "file.nextLE<Sint8>()"?
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

	for (int i = 0; i < MAX_PLRS; i++) {
		if (plr[i].plractive && currlevel == plr[i].plrlevel)
			LightList[plr[i]._plid]._lunflag = true;
	}
}

} // namespace devilution

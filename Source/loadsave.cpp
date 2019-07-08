#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE *tbuff;

void LoadGame(BOOL firstflag)
{
	int i, j;
	DWORD dwLen;
	char szName[MAX_PATH];
	BYTE *LoadBuff;
	int _ViewX, _ViewY, _nummonsters, _numitems, _nummissiles, _nobjects;

	FreeGameMem();
	pfile_remove_temp_files();
	pfile_get_game_name(szName);
	LoadBuff = pfile_read(szName, &dwLen);
	tbuff = LoadBuff;

	if (ILoad() != 'RETL')
		app_fatal("Invalid save file");

	setlevel = OLoad();
	setlvlnum = WLoad();
	currlevel = WLoad();
	leveltype = WLoad();
	_ViewX = WLoad();
	_ViewY = WLoad();
	invflag = OLoad();
	chrflag = OLoad();
	_nummonsters = WLoad();
	_numitems = WLoad();
	_nummissiles = WLoad();
	_nobjects = WLoad();

	for (i = 0; i < NUMLEVELS; i++) {
		glSeedTbl[i] = ILoad();
		gnLevelTypeTbl[i] = WLoad();
	}

	LoadPlayer(myplr);

	for (i = 0; i < MAXQUESTS; i++)
		LoadQuest(i);
	for (i = 0; i < MAXPORTAL; i++)
		LoadPortal(i);

	LoadGameLevel(firstflag, 4);
	SyncInitPlr(myplr);
	SyncPlrAnim(myplr);

	ViewX = _ViewX;
	ViewY = _ViewY;
	nummonsters = _nummonsters;
	numitems = _numitems;
	nummissiles = _nummissiles;
	nobjects = _nobjects;

	for (i = 0; i < MAXMONSTERS; i++)
		monstkills[i] = ILoad();

	if (leveltype != DTYPE_TOWN) {
		for (i = 0; i < MAXMONSTERS; i++)
			monstactive[i] = WLoad();
		for (i = 0; i < nummonsters; i++)
			LoadMonster(monstactive[i]);
		for (i = 0; i < MAXMISSILES; i++)
			missileactive[i] = BLoad();
		for (i = 0; i < MAXMISSILES; i++)
			missileavail[i] = BLoad();
		for (i = 0; i < nummissiles; i++)
			LoadMissile(missileactive[i]);
		for (i = 0; i < MAXOBJECTS; i++)
			objectactive[i] = BLoad();
		for (i = 0; i < MAXOBJECTS; i++)
			objectavail[i] = BLoad();
		for (i = 0; i < nobjects; i++)
			LoadObject(objectactive[i]);
		for (i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);

		numlights = WLoad();

		for (i = 0; i < MAXLIGHTS; i++)
			lightactive[i] = BLoad();
		for (i = 0; i < numlights; i++)
			LoadLighting(lightactive[i]);

		visionid = WLoad();
		numvision = WLoad();

		for (i = 0; i < numvision; i++)
			LoadVision(i);
	}

	for (i = 0; i < MAXITEMS; i++)
		itemactive[i] = BLoad();
	for (i = 0; i < MAXITEMS; i++)
		itemavail[i] = BLoad();
	for (i = 0; i < numitems; i++)
		LoadItem(itemactive[i]);
	for (i = 0; i < 128; i++)
		UniqueItemFlag[i] = OLoad();

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			dLight[i][j] = BLoad();
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			dFlags[i][j] = BLoad();
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			dPlayer[i][j] = BLoad();
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			dItem[i][j] = BLoad();
	}

	if (leveltype != DTYPE_TOWN) {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dMonster[i][j] = WLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dDead[i][j] = BLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dObject[i][j] = BLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dLight[i][j] = BLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dPreLight[i][j] = BLoad();
		}
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++)
				automapview[i][j] = OLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dMissile[i][j] = BLoad();
		}
	}

	numpremium = WLoad();
	premiumlevel = WLoad();

	for (i = 0; i < 6; i++)
		LoadPremium(i);

	automapflag = OLoad();
	AutoMapScale = WLoad();
	mem_free_dbg(LoadBuff);
	AutomapZoomReset();
	ResyncQuests();

	if (leveltype != DTYPE_TOWN)
		ProcessLightList();

	RedoPlayerVision();
	ProcessVisionList();
	missiles_process_charge();
	ResetPal();
	SetCursor_(CURSOR_HAND);
	gbProcessPlayers = TRUE;
}

// Load a byte-size char from the buffer
char BLoad()
{
	return *tbuff++;
}

// Redundant?
int WLoad()
{
	int rv = *tbuff++ << 24;
	rv |= *tbuff++ << 16;
	rv |= *tbuff++ << 8;
	rv |= *tbuff++;

	return rv;
}

// Load a 32 bit integer from the buffer
int ILoad()
{
	int rv = *tbuff++ << 24;
	rv |= *tbuff++ << 16;
	rv |= *tbuff++ << 8;
	rv |= *tbuff++;

	return rv;
}

BOOL OLoad()
{
	if (*tbuff++ == TRUE)
		return TRUE;
	else
		return FALSE;
}

// Consume an int from tbuff with memcpy - copies directly, doesn't change byte order
void CopyInt(int & dst)
{
	memcpy(&dst, tbuff, 4);
	tbuff += 4;
}

void CopyInt(unsigned int & dst)
{
	memcpy(&dst, tbuff, 4);
	tbuff += 4;
}

void CopyShort(unsigned short & dst)
{
	memcpy(&dst, tbuff, 2);
	tbuff += 2;
}

void CopyShort(short & dst)
{
	memcpy(&dst, tbuff, 2);
	tbuff += 2;
}

void CopyShorts(const int n, unsigned short * dst)
{
	memcpy(dst, tbuff, 2 * n);
	tbuff += 2 * n;
}

// Copy an int array of size n
void CopyInts(const int n, int* dst)
{
	memcpy(dst, tbuff, 4 * n);
	tbuff += 4 * n;
}

void CopyBytes(const int n, unsigned char *dst)
{
	memcpy(dst, tbuff, n);
	tbuff += n;
}

void CopyChar(unsigned char & dst)
{
	memcpy(&dst, tbuff, 1);
	tbuff += 1;
}

void CopyChar(char & dst)
{
	memcpy(&dst, tbuff, 1);
	tbuff += 1;
}

void CopyInt64(long long unsigned int & dst)
{
	memcpy(&dst, tbuff, 8);
	tbuff += 8;
}

void LoadPlayer(int i)
{
	PlayerStruct *pPlayer = &plr[i];	
	CopyInt(pPlayer->_pmode);
	CopyBytes(25, pPlayer->walkpath);
	CopyBytes(1, &pPlayer->plractive);
	tbuff += 2; // Some padding here?
	CopyInt(pPlayer->destAction);
	CopyInt(pPlayer->destParam1);
	CopyInt(pPlayer->destParam2);
	CopyInt(pPlayer->destParam3);
	CopyInt(pPlayer->destParam4);
	CopyInt(pPlayer->plrlevel);
	CopyInt(pPlayer->WorldX);
	CopyInt(pPlayer->WorldY);
	CopyInt(pPlayer->_px);
	CopyInt(pPlayer->_py);
	CopyInt(pPlayer->_ptargx);
	CopyInt(pPlayer->_ptargy);
	CopyInt(pPlayer->_pownerx);
	CopyInt(pPlayer->_pownery);
	CopyInt(pPlayer->_poldx);
	CopyInt(pPlayer->_poldy);
	CopyInt(pPlayer->_pxoff);
	CopyInt(pPlayer->_pyoff);
	CopyInt(pPlayer->_pxvel);
	CopyInt(pPlayer->_pyvel);
	CopyInt(pPlayer->_pdir);
	CopyInt(pPlayer->_nextdir);
	CopyInt(pPlayer->_pgfxnum);
	tbuff += 4; // Skip pointers
	CopyInt(pPlayer->_pAnimDelay);
	CopyInt(pPlayer->_pAnimCnt);
	CopyInt(pPlayer->_pAnimLen);
	CopyInt(pPlayer->_pAnimFrame);
	CopyInt(pPlayer->_pAnimWidth);
	CopyInt(pPlayer->_pAnimWidth2);
	CopyInt(pPlayer->_peflag);
	CopyInt(pPlayer->_plid);
	CopyInt(pPlayer->_pvid);

	CopyInt(pPlayer->_pSpell);
	CopyChar(pPlayer->_pSplType);
	CopyChar(pPlayer->_pSplFrom);
	CopyInt(pPlayer->_pTSpell);
	CopyChar(pPlayer->_pTSplType);
	tbuff += 5;
	CopyInt(pPlayer->_pRSpell);
	CopyChar(pPlayer->_pRSplType);
	tbuff += 3;
	CopyInt(pPlayer->_pSBkSpell);
	CopyChar(pPlayer->_pSBkSplType);

	CopyBytes(64, pPlayer->_pSplLvl);
	tbuff += 7;
	CopyInt64(pPlayer->_pMemSpells);
	CopyInt64(pPlayer->_pAblSpells);
	CopyInt64(pPlayer->_pScrlSpells);
	CopyChar(pPlayer->_pSpellFlags);
 	tbuff += 3;
	CopyInts(4, pPlayer->_pSplHotKey);
	CopyBytes(4, pPlayer->_pSplTHotKey);
	CopyInt(pPlayer->_pwtype);
	CopyChar(pPlayer->_pBlockFlag);
	CopyChar(pPlayer->_pInvincible);
	CopyChar(pPlayer->_pLightRad);
	CopyChar(pPlayer->_pLvlChanging);
	CopyBytes(PLR_NAME_LEN, pPlayer->_pName);
	CopyChar(pPlayer->_pClass);
	tbuff += 3;
	CopyInt(pPlayer->_pStrength);
	CopyInt(pPlayer->_pBaseStr);
	CopyInt(pPlayer->_pMagic);
	CopyInt(pPlayer->_pBaseMag);
	CopyInt(pPlayer->_pDexterity);
	CopyInt(pPlayer->_pBaseDex);
	CopyInt(pPlayer->_pVitality);
	CopyInt(pPlayer->_pBaseVit);
	CopyInt(pPlayer->_pStatPts);
	CopyInt(pPlayer->_pDamageMod);
	CopyInt(pPlayer->_pBaseToBlk);
	CopyInt(pPlayer->_pHPBase);
	CopyInt(pPlayer->_pMaxHPBase);
	CopyInt(pPlayer->_pHitPoints);
	CopyInt(pPlayer->_pMaxHP);
	CopyInt(pPlayer->_pHPPer);
	CopyInt(pPlayer->_pManaBase);
	CopyInt(pPlayer->_pMaxManaBase);
	CopyInt(pPlayer->_pMana);
	CopyInt(pPlayer->_pMaxMana);
	CopyInt(pPlayer->_pManaPer);
	CopyChar(pPlayer->_pLevel);
	CopyChar(pPlayer->_pMaxLvl);
	tbuff += 2;
	CopyInt(pPlayer->_pExperience);
	CopyInt(pPlayer->_pMaxExp);
	CopyInt(pPlayer->_pNextExper);
	CopyChar(pPlayer->_pArmorClass);
	CopyChar(pPlayer->_pMagResist);
	CopyChar(pPlayer->_pFireResist);
	CopyChar(pPlayer->_pLghtResist);
	CopyInt(pPlayer->_pGold);
	CopyInt(pPlayer->_pInfraFlag);
	CopyInt(pPlayer->_pVar1);
	CopyInt(pPlayer->_pVar2);
	CopyInt(pPlayer->_pVar3);
	CopyInt(pPlayer->_pVar4);
	CopyInt(pPlayer->_pVar5);
	CopyInt(pPlayer->_pVar6);
	CopyInt(pPlayer->_pVar7);
	CopyInt(pPlayer->_pVar8);
	CopyBytes(NUMLEVELS, pPlayer->_pLvlVisited);
	CopyBytes(NUMLEVELS, pPlayer->_pSLvlVisited); // only 10 used
	tbuff += 2;
	CopyInt(pPlayer->_pGFXLoad);

	tbuff += sizeof(__uint32_t) * 8; // Skip 8 pointers
	CopyInt(pPlayer->_pNFrames);
	CopyInt(pPlayer->_pNWidth);

	tbuff += sizeof(__uint32_t) * 8; // Skip 8 pointers

	CopyInt(pPlayer->_pWFrames);
	CopyInt(pPlayer->_pWWidth);

	tbuff += sizeof(__uint32_t) * 8; // Skip 8 pointers

	CopyInt(pPlayer->_pAFrames);
	CopyInt(pPlayer->_pAWidth);
	CopyInt(pPlayer->_pAFNum);

	tbuff += sizeof(__uint32_t) * 24; // Skip 24 pointers

	CopyInt(pPlayer->_pSFrames);
	CopyInt(pPlayer->_pSWidth);
	CopyInt(pPlayer->_pSFNum);

	tbuff += sizeof(__uint32_t) * 8; // Skip 8 pointers

	CopyInt(pPlayer->_pHFrames);
	CopyInt(pPlayer->_pHWidth);

	tbuff += sizeof(__uint32_t) * 8; // Skip 8 pointers

	CopyInt(pPlayer->_pDFrames);
	CopyInt(pPlayer->_pDWidth);

	tbuff += sizeof(__uint32_t) * 8; // Skip 8 pointers

	CopyInt(pPlayer->_pBFrames);
	CopyInt(pPlayer->_pBWidth);

	CopyItems(NUM_INVLOC, pPlayer->InvBody);
	CopyItems(NUM_INV_GRID_ELEM, pPlayer->InvList);

	CopyInt(pPlayer->_pNumInv);
	CopyBytes(NUM_INV_GRID_ELEM, pPlayer->InvGrid);
	CopyItems(MAXBELTITEMS, pPlayer->SpdList);
	CopyItem(&pPlayer->HoldItem);
	CopyInt(pPlayer->_pIMinDam);
	CopyInt(pPlayer->_pIMaxDam);
	CopyInt(pPlayer->_pIAC);
	CopyInt(pPlayer->_pIBonusDam);
	CopyInt(pPlayer->_pIBonusToHit);
	CopyInt(pPlayer->_pIBonusAC);
	CopyInt(pPlayer->_pIBonusDamMod);
	tbuff += 4;
	CopyInt64(pPlayer->_pISpells);
	CopyInt(pPlayer->_pIFlags);
	CopyInt(pPlayer->_pIGetHit);
	CopyChar(pPlayer->_pISplLvlAdd);
	CopyChar(pPlayer->_pISplCost);
	tbuff += 2;
	CopyInt(pPlayer->_pISplDur);
	CopyInt(pPlayer->_pIEnAc);
	CopyInt(pPlayer->_pIFMinDam);
	CopyInt(pPlayer->_pIFMaxDam);
	CopyInt(pPlayer->_pILMinDam);
	CopyInt(pPlayer->_pILMaxDam);
	CopyInt(pPlayer->_pOilType);
	CopyChar(pPlayer->pTownWarps);
	CopyChar(pPlayer->pDungMsgs);
	CopyChar(pPlayer->pLvlLoad);
	CopyChar(pPlayer->pBattleNet);
	CopyChar(pPlayer->pManaShield);
	CopyBytes(3, pPlayer->bReserved);
	CopyShorts(8, pPlayer->wReserved);
	CopyInt(pPlayer->pDiabloKillLevel);
	CopyInts(7, pPlayer->dwReserved);

	// Omit 10 pointers
	
}

void LoadMonster(int i)
{
	MonsterStruct *pMonster = &monster[i];

	CopyInt(pMonster->_mMTidx);
	CopyInt(pMonster->_mmode);

	CopyChar(pMonster->_mgoal);
	tbuff += 3;

	CopyInt(pMonster->_mgoalvar1);
	CopyInt(pMonster->_mgoalvar2);
	CopyInt(pMonster->_mgoalvar3);
	CopyInt(pMonster->field_18);

	CopyChar(pMonster->_pathcount);
	tbuff += 3;

	CopyInt(pMonster->_mx);
	CopyInt(pMonster->_my);
	CopyInt(pMonster->_mfutx);
	CopyInt(pMonster->_mfuty);
	CopyInt(pMonster->_moldx);
	CopyInt(pMonster->_moldy);
	CopyInt(pMonster->_mxoff);
	CopyInt(pMonster->_myoff);
	CopyInt(pMonster->_mxvel);
	CopyInt(pMonster->_myvel);
	CopyInt(pMonster->_mdir);
	CopyInt(pMonster->_menemy);

	CopyChar(pMonster->_menemyx);
	CopyChar(pMonster->_menemyy);
	CopyShort(pMonster->falign_52);

	tbuff += 4; // Skip pointer
	CopyInt(pMonster->_mAnimDelay);
	CopyInt(pMonster->_mAnimCnt);
	CopyInt(pMonster->_mAnimLen);
	CopyInt(pMonster->_mAnimFrame);
	CopyInt(pMonster->_meflag);
	CopyInt(pMonster->_mDelFlag);
	CopyInt(pMonster->_mVar1);
	CopyInt(pMonster->_mVar2);
	CopyInt(pMonster->_mVar3);
	CopyInt(pMonster->_mVar4);
	CopyInt(pMonster->_mVar5);
	CopyInt(pMonster->_mVar6);
	CopyInt(pMonster->_mVar7);
	CopyInt(pMonster->_mVar8);
	CopyInt(pMonster->_mmaxhp);
	CopyInt(pMonster->_mhitpoints);

	CopyChar(pMonster->_mAi);
	CopyChar(pMonster->_mint);
	CopyShort(pMonster->falign_9A);

	CopyInt(pMonster->_mFlags);

	CopyChar(pMonster->_msquelch);
	tbuff += 3;

	CopyInt(pMonster->falign_A4);
	CopyInt(pMonster->_lastx);
	CopyInt(pMonster->_lasty);
	CopyInt(pMonster->_mRndSeed);
	CopyInt(pMonster->_mAISeed);
	CopyInt(pMonster->falign_B8);

	CopyChar(pMonster->_uniqtype);
	CopyChar(pMonster->_uniqtrans);
	CopyChar(pMonster->_udeadval);
	CopyChar(pMonster->mWhoHit);

	CopyChar(pMonster->mLevel);
	CopyShort(pMonster->mExp);
	tbuff += 1;
	
	CopyChar(pMonster->mHit);
	CopyChar(pMonster->mMinDamage);
	CopyChar(pMonster->mMaxDamage);
	CopyChar(pMonster->mHit2);
	
	CopyChar(pMonster->mMinDamage2);
	CopyChar(pMonster->mMaxDamage2);
	CopyChar(pMonster->mArmorClass);
	CopyChar(pMonster->falign_CB);
	
	CopyShort(pMonster->mMagicRes);
	tbuff += 2;

	CopyInt(pMonster->mtalkmsg);

	CopyChar(pMonster->leader);
	CopyChar(pMonster->leaderflag);
	CopyChar(pMonster->packsize);
	CopyChar(pMonster->mlid);
	
	SyncMonsterAnim(i);
}

void LoadMissile(int i)
{
	MissileStruct *pMissile = &missile[i];

	CopyInt(pMissile->_mitype);
	CopyInt(pMissile->_mix);
	CopyInt(pMissile->_miy);
	CopyInt(pMissile->_mixoff);
	CopyInt(pMissile->_miyoff);
	CopyInt(pMissile->_mixvel);
	CopyInt(pMissile->_miyvel);
	CopyInt(pMissile->_misx);
	CopyInt(pMissile->_misy);
	CopyInt(pMissile->_mitxoff);
	CopyInt(pMissile->_mityoff);
	CopyInt(pMissile->_mimfnum);
	CopyInt(pMissile->_mispllvl);
	CopyInt(pMissile->_miDelFlag);
	CopyChar(pMissile->_miAnimType);
	tbuff += 3;
	
	CopyInt(pMissile->_miAnimFlags);
	tbuff += 4;
	CopyInt(pMissile->_miAnimDelay);
	CopyInt(pMissile->_miAnimLen);
	CopyInt(pMissile->_miAnimWidth);
	CopyInt(pMissile->_miAnimWidth2);
	CopyInt(pMissile->_miAnimCnt);
	CopyInt(pMissile->_miAnimAdd);
	CopyInt(pMissile->_miAnimFrame);
	CopyInt(pMissile->_miDrawFlag);
	CopyInt(pMissile->_miLightFlag);
	CopyInt(pMissile->_miPreFlag);
	CopyInt(pMissile->_miUniqTrans);
	CopyInt(pMissile->_mirange);
	CopyInt(pMissile->_misource);
	CopyInt(pMissile->_micaster);
	CopyInt(pMissile->_midam);
	CopyInt(pMissile->_miHitFlag);
	CopyInt(pMissile->_midist);
	CopyInt(pMissile->_mlid);
	CopyInt(pMissile->_mirnd);
	CopyInt(pMissile->_miVar1);
	CopyInt(pMissile->_miVar2);
	CopyInt(pMissile->_miVar3);
	CopyInt(pMissile->_miVar4);
	CopyInt(pMissile->_miVar5);
	CopyInt(pMissile->_miVar6);
	CopyInt(pMissile->_miVar7);
	CopyInt(pMissile->_miVar8);
}

void LoadObject(int i)
{
	ObjectStruct *pObject = &object[i];
	CopyInt(pObject->_otype);
	CopyInt(pObject->_ox);
	CopyInt(pObject->_oy);
	CopyInt(pObject->_oLight);
	CopyInt(pObject->_oAnimFlag);
	tbuff += 4;
	CopyInt(pObject->_oAnimDelay);
	CopyInt(pObject->_oAnimCnt);
	CopyInt(pObject->_oAnimLen);
	CopyInt(pObject->_oAnimFrame);
	CopyInt(pObject->_oAnimWidth);
	CopyInt(pObject->_oAnimWidth2);
	CopyInt(pObject->_oDelFlag);

	CopyChar(pObject->_oBreak);
	tbuff += 3;

	CopyInt(pObject->_oSolidFlag);
	CopyInt(pObject->_oMissFlag);

	CopyChar(pObject->_oSelFlag); 
	tbuff += 3;

	CopyInt(pObject->_oPreFlag);
	CopyInt(pObject->_oTrapFlag);
	CopyInt(pObject->_oDoorFlag);
	CopyInt(pObject->_olid);
	CopyInt(pObject->_oRndSeed);
	CopyInt(pObject->_oVar1);
	CopyInt(pObject->_oVar2);
	CopyInt(pObject->_oVar3);
	CopyInt(pObject->_oVar4);
	CopyInt(pObject->_oVar5);
	CopyInt(pObject->_oVar6);
	CopyInt(pObject->_oVar7);
	CopyInt(pObject->_oVar8);
}

void LoadItem(int i)
{
	CopyItem(&item[i]);
	GetItemFrm(i);
}

void CopyItem(ItemStruct *pItem)
{
	CopyInt(pItem->_iSeed);
	CopyShort(pItem->_iCreateInfo);
	tbuff += 2;
	CopyInt(pItem->_itype);
	CopyInt(pItem->_ix);
	CopyInt(pItem->_iy);
	CopyInt(pItem->_iAnimFlag);
	tbuff += 4; // Skip pointer _iAnimData
	CopyInt(pItem->_iAnimLen);
	CopyInt(pItem->_iAnimFrame);
	CopyInt(pItem->_iAnimWidth);
	CopyInt(pItem->_iAnimWidth2); // width 2?
	CopyInt(pItem->_isin);        // set when item is flagged for deletion, deprecated in 1.02
	CopyChar(pItem->_iSelFlag);
	tbuff += 3;
	CopyInt(pItem->_iPostDraw);
	CopyInt(pItem->_iIdentified);
	CopyChar(pItem->_iMagical);
	CopyBytes(64, pItem->_iName);
	CopyBytes(64, pItem->_iIName);
	CopyChar(pItem->_iLoc);
	CopyChar(pItem->_iClass);
	tbuff += 1;
	CopyInt(pItem->_iCurs);
	CopyInt(pItem->_ivalue);
	CopyInt(pItem->_iIvalue);
	CopyInt(pItem->_iMinDam);
	CopyInt(pItem->_iMaxDam);
	CopyInt(pItem->_iAC);
	CopyInt(pItem->_iFlags);
	CopyInt(pItem->_iMiscId);
	CopyInt(pItem->_iSpell);
	CopyInt(pItem->_iCharges);
	CopyInt(pItem->_iMaxCharges);
	CopyInt(pItem->_iDurability);
	CopyInt(pItem->_iMaxDur);
	CopyInt(pItem->_iPLDam);
	CopyInt(pItem->_iPLToHit);
	CopyInt(pItem->_iPLAC);
	CopyInt(pItem->_iPLStr);
	CopyInt(pItem->_iPLMag);
	CopyInt(pItem->_iPLDex);
	CopyInt(pItem->_iPLVit);
	CopyInt(pItem->_iPLFR);
	CopyInt(pItem->_iPLLR);
	CopyInt(pItem->_iPLMR);
	CopyInt(pItem->_iPLMana);
	CopyInt(pItem->_iPLHP);
	CopyInt(pItem->_iPLDamMod);
	CopyInt(pItem->_iPLGetHit);
	CopyInt(pItem->_iPLLight);
	CopyChar(pItem->_iSplLvlAdd);
	CopyChar(pItem->_iRequest);
	tbuff += 2;
	CopyInt(pItem->_iUid);
	CopyInt(pItem->_iFMinDam);
	CopyInt(pItem->_iFMaxDam);
	CopyInt(pItem->_iLMinDam);
	CopyInt(pItem->_iLMaxDam);
	CopyInt(pItem->_iPLEnAc);
	CopyChar(pItem->_iPrePower);
	CopyChar(pItem->_iSufPower);
	tbuff += 2;
	CopyInt(pItem->_iVAdd1);
	CopyInt(pItem->_iVMult1);
	CopyInt(pItem->_iVAdd2);
	CopyInt(pItem->_iVMult2);
	CopyChar(pItem->_iMinStr);
	CopyChar(pItem->_iMinMag);
	CopyChar(pItem->_iMinDex);
	tbuff += 1;
	CopyInt(pItem->_iStatFlag);
	CopyInt(pItem->IDidx);
	CopyInt(pItem->offs016C);
}

void CopyItems(const int n, ItemStruct *pItem)
{
	for (int i = 0; i < n; i++)
	{
		CopyItem(&pItem[i]);
	}
}

void LoadPremium(int i)
{
	memcpy(&premiumitem[i], tbuff, sizeof(*premiumitem));
	tbuff += sizeof(*premiumitem);
}

void LoadQuest(int i)
{
	memcpy(&quests[i], tbuff, sizeof(*quests));
	tbuff += sizeof(*quests);
	ReturnLvlX = WLoad();
	ReturnLvlY = WLoad();
	ReturnLvl = WLoad();
	ReturnLvlT = WLoad();
	DoomQuestState = WLoad();
}

void LoadLighting(int i)
{
	memcpy(&LightList[i], tbuff, sizeof(*LightList));
	tbuff += sizeof(*LightList);
}

void LoadVision(int i)
{
	memcpy(&VisionList[i], tbuff, sizeof(*VisionList));
	tbuff += sizeof(*VisionList);
}

void LoadPortal(int i)
{
	memcpy(&portal[i], tbuff, sizeof(*portal));
	tbuff += sizeof(*portal);
}

void SaveGame()
{
	int i, j;
	char szName[MAX_PATH];

	DWORD dwLen = codec_get_encoded_len(FILEBUFF);
	BYTE *SaveBuff = DiabloAllocPtr(dwLen);
	tbuff = SaveBuff;

	ISave('RETL');
	OSave((unsigned char)setlevel);
	WSave(setlvlnum);
	WSave(currlevel);
	WSave((unsigned char)leveltype);
	WSave(ViewX);
	WSave(ViewY);
	OSave(invflag);
	OSave(chrflag);
	WSave(nummonsters);
	WSave(numitems);
	WSave(nummissiles);
	WSave(nobjects);

	for (i = 0; i < NUMLEVELS; i++) {
		ISave(glSeedTbl[i]);
		WSave(gnLevelTypeTbl[i]);
	}

	SavePlayer(myplr);

	for (i = 0; i < MAXQUESTS; i++)
		SaveQuest(i);
	for (i = 0; i < MAXPORTAL; i++)
		SavePortal(i);
	for (i = 0; i < MAXMONSTERS; i++)
		ISave(monstkills[i]);

	if (leveltype != DTYPE_TOWN) {
		for (i = 0; i < MAXMONSTERS; i++)
			WSave(monstactive[i]);
		for (i = 0; i < nummonsters; i++)
			SaveMonster(monstactive[i]);
		for (i = 0; i < MAXMISSILES; i++)
			BSave(missileactive[i]);
		for (i = 0; i < MAXMISSILES; i++)
			BSave(missileavail[i]);
		for (i = 0; i < nummissiles; i++)
			SaveMissile(missileactive[i]);
		for (i = 0; i < MAXOBJECTS; i++)
			BSave(objectactive[i]);
		for (i = 0; i < MAXOBJECTS; i++)
			BSave(objectavail[i]);
		for (i = 0; i < nobjects; i++)
			SaveObject(objectactive[i]);

		WSave(numlights);

		for (i = 0; i < MAXLIGHTS; i++)
			BSave(lightactive[i]);
		for (i = 0; i < numlights; i++)
			SaveLighting(lightactive[i]);

		WSave(visionid);
		WSave(numvision);

		for (i = 0; i < numvision; i++)
			SaveVision(i);
	}

	for (i = 0; i < MAXITEMS; i++)
		BSave(itemactive[i]);
	for (i = 0; i < MAXITEMS; i++)
		BSave(itemavail[i]);
	for (i = 0; i < numitems; i++)
		SaveItem(itemactive[i]);
	for (i = 0; i < 128; i++)
		OSave(UniqueItemFlag[i]);

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			BSave(dLight[i][j]);
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			BSave(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			BSave(dPlayer[i][j]);
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			BSave(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				WSave(dMonster[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dDead[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dObject[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dLight[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dPreLight[i][j]);
		}
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++)
				OSave(automapview[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dMissile[i][j]);
		}
	}

	WSave(numpremium);
	WSave(premiumlevel);

	for (i = 0; i < 6; i++)
		SavePremium(i);

	OSave(automapflag);
	WSave(AutoMapScale);
	pfile_get_game_name(szName);
	dwLen = codec_get_encoded_len(tbuff - SaveBuff);
	pfile_write_save_file(szName, SaveBuff, tbuff - SaveBuff, dwLen);
	mem_free_dbg(SaveBuff);
	gbValidSaveFile = TRUE;
	pfile_rename_temp_to_perm();
	pfile_write_hero();
}

void BSave(char v)
{
	*tbuff++ = v;
}

void WSave(int v)
{
	*tbuff++ = v >> 24;
	*tbuff++ = v >> 16;
	*tbuff++ = v >> 8;
	*tbuff++ = v;
}

void ISave(int v)
{
	*tbuff++ = v >> 24;
	*tbuff++ = v >> 16;
	*tbuff++ = v >> 8;
	*tbuff++ = v;
}

void OSave(BOOL v)
{
	if (v != FALSE)
		*tbuff++ = TRUE;
	else
		*tbuff++ = FALSE;
}

void SavePlayer(int i)
{
	memcpy(tbuff, &plr[i], sizeof(*plr) - (10 * sizeof(void *)));
	tbuff += sizeof(*plr) - (10 * sizeof(void *)); // omit last 10 pointers
}

void SaveMonster(int i)
{
	memcpy(tbuff, &monster[i], sizeof(*monster) - (3 * sizeof(void *)));
	tbuff += sizeof(*monster) - (3 * sizeof(void *)); // omit last 3 pointers
}

void SaveMissile(int i)
{
	memcpy(tbuff, &missile[i], sizeof(*missile));
	tbuff += sizeof(*missile);
}

void SaveObject(int i)
{
	memcpy(tbuff, &object[i], sizeof(*object));
	tbuff += sizeof(*object);
}

void SaveItem(int i)
{
	memcpy(tbuff, &item[i], sizeof(*item));
	tbuff += sizeof(*item);
}

void SavePremium(int i)
{
	memcpy(tbuff, &premiumitem[i], sizeof(*premiumitem));
	tbuff += sizeof(*premiumitem);
}

void SaveQuest(int i)
{
	memcpy(tbuff, &quests[i], sizeof(*quests));
	tbuff += sizeof(*quests);
	WSave(ReturnLvlX);
	WSave(ReturnLvlY);
	WSave(ReturnLvl);
	WSave(ReturnLvlT);
	WSave(DoomQuestState);
}

void SaveLighting(int i)
{
	memcpy(tbuff, &LightList[i], sizeof(*LightList));
	tbuff += sizeof(*LightList);
}

void SaveVision(int i)
{
	memcpy(tbuff, &VisionList[i], sizeof(*VisionList));
	tbuff += sizeof(*VisionList);
}

void SavePortal(int i)
{
	memcpy(tbuff, &portal[i], sizeof(*portal));
	tbuff += sizeof(*portal);
}

void SaveLevel()
{
	int i, j;
	char szName[MAX_PATH];
	int dwLen;
	unsigned char *SaveBuff;

	if (!currlevel)
		glSeedTbl[0] = GetRndSeed();

	dwLen = codec_get_encoded_len(FILEBUFF);
	SaveBuff = DiabloAllocPtr(dwLen);
	tbuff = SaveBuff;

	if (leveltype != DTYPE_TOWN) {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dDead[i][j]);
		}
	}

	WSave(nummonsters);
	WSave(numitems);
	WSave(nobjects);

	if (leveltype != DTYPE_TOWN) {
		for (i = 0; i < MAXMONSTERS; i++)
			WSave(monstactive[i]);
		for (i = 0; i < nummonsters; i++)
			SaveMonster(monstactive[i]);
		for (i = 0; i < MAXOBJECTS; i++)
			BSave(objectactive[i]);
		for (i = 0; i < MAXOBJECTS; i++)
			BSave(objectavail[i]);
		for (i = 0; i < nobjects; i++)
			SaveObject(objectactive[i]);
	}

	for (i = 0; i < MAXITEMS; i++)
		BSave(itemactive[i]);
	for (i = 0; i < MAXITEMS; i++)
		BSave(itemavail[i]);
	for (i = 0; i < numitems; i++)
		SaveItem(itemactive[i]);

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			BSave(dFlags[i][j] & ~(BFLAG_MISSILE | BFLAG_VISIBLE | BFLAG_DEAD_PLAYER));
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			BSave(dItem[i][j]);
	}

	if (leveltype != DTYPE_TOWN) {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				WSave(dMonster[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dObject[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dLight[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dPreLight[i][j]);
		}
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++)
				OSave(automapview[i][j]);
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				BSave(dMissile[i][j]);
		}
	}

	GetTempLevelNames(szName);
	dwLen = codec_get_encoded_len(tbuff - SaveBuff);
	pfile_write_save_file(szName, SaveBuff, tbuff - SaveBuff, dwLen);
	mem_free_dbg(SaveBuff);

	if (setlevel == 0)
		plr[myplr]._pLvlVisited[currlevel] = 1;
	else
		plr[myplr]._pSLvlVisited[setlvlnum] = 1;
}

void LoadLevel()
{
	int i, j;
	DWORD dwLen;
	char szName[MAX_PATH];
	BYTE *LoadBuff;

	GetPermLevelNames(szName);
	LoadBuff = pfile_read(szName, &dwLen);
	tbuff = LoadBuff;

	if (leveltype != DTYPE_TOWN) {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dDead[i][j] = BLoad();
		}
		SetDead();
	}

	nummonsters = WLoad();
	numitems = WLoad();
	nobjects = WLoad();

	if (leveltype != DTYPE_TOWN) {
		for (i = 0; i < MAXMONSTERS; i++)
			monstactive[i] = WLoad();
		for (i = 0; i < nummonsters; i++)
			LoadMonster(monstactive[i]);
		for (i = 0; i < MAXOBJECTS; i++)
			objectactive[i] = BLoad();
		for (i = 0; i < MAXOBJECTS; i++)
			objectavail[i] = BLoad();
		for (i = 0; i < nobjects; i++)
			LoadObject(objectactive[i]);
		for (i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);
	}

	for (i = 0; i < MAXITEMS; i++)
		itemactive[i] = BLoad();
	for (i = 0; i < MAXITEMS; i++)
		itemavail[i] = BLoad();
	for (i = 0; i < numitems; i++)
		LoadItem(itemactive[i]);

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			dFlags[i][j] = BLoad();
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++)
			dItem[i][j] = BLoad();
	}

	if (leveltype != DTYPE_TOWN) {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dMonster[i][j] = WLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dObject[i][j] = BLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dLight[i][j] = BLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dPreLight[i][j] = BLoad();
		}
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++)
				automapview[i][j] = OLoad();
		}
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++)
				dMissile[i][j] = 0; /// BUGFIX: supposed to load saved missiles with "BLoad()"?
		}
	}

	AutomapZoomReset();
	ResyncQuests();
	SyncPortals();
	dolighting = 1;

	for (i = 0; i < MAX_PLRS; i++) {
		if (plr[i].plractive && currlevel == plr[i].plrlevel)
			LightList[plr[i]._plid]._lunflag = 1;
	}

	mem_free_dbg(LoadBuff);
}

DEVILUTION_END_NAMESPACE

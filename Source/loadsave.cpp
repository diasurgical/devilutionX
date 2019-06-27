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

char BLoad()
{
	return *tbuff++;
}

int WLoad()
{
	int rv = *tbuff++ << 24;
	rv |= *tbuff++ << 16;
	rv |= *tbuff++ << 8;
	rv |= *tbuff++;

	return rv;
}

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

void LoadPlayer(int i)
{
	memcpy(&plr[i], tbuff, sizeof(*plr) - (10 * sizeof(void *)));
	tbuff += sizeof(*plr) - (10 * sizeof(void *)); // omit last 10 pointers
}

void LoadMonster(int i)
{
	memcpy(&monster[i], tbuff, sizeof(*monster) - (3 * sizeof(void *)));
	tbuff += sizeof(*monster) - (3 * sizeof(void *)); // omit last 3 pointers
	SyncMonsterAnim(i);
}

void LoadMissile(int i)
{
	memcpy(&missile[i], tbuff, sizeof(*missile));
	tbuff += sizeof(*missile);
}

void LoadObject(int i)
{
	memcpy(&object[i], tbuff, sizeof(*object));
	tbuff += sizeof(*object);
}

void LoadItem(int i)
{
	memcpy(&item[i], tbuff, sizeof(*item));
	tbuff += sizeof(*item);
	GetItemFrm(i);
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

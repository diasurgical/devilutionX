/**
 * @file error.cpp
 *
 * Implementation of in-game message functions.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

char msgtable[MAX_SEND_STR_LEN];
DWORD msgdelay;
char msgflag;
char msgcnt;

/** Maps from error_id to error message. */
const char *const MsgStrings[] = {
	"",
	"B FOPODE KAPTA HEDOCTYZHA",
	"B DEMO BEPCNN MY/LTNZ/EEP HEDOCTYZEH",
	"OUN#KA BLIBODA 3BYKA",
	"HEDOCTYZHO B GTO& BEPCNN",
	"HEDOCTATO4HO MECTA, 4TO#LI COXPAHNTLCR",
	"B FOPODE ZAY3A HE PA#OTAET",
	"PEKOMEHDYETCR CKOZNPOBATL NFPY HA )KSCTKNN DNCK",
	"ZPO#/EMA B CNHXPOHN3AQNN",
	"B MY/LTNZ/EEPE ZAY3A HE PA#OTAET",
	"3AFPY3KA...",
	"COXPAHEHNE...",
	"HEKOTOPLIE OC/A#HYT, 4TO#LI ODNH YCN/N/CR",
	"HOBAR CN/A KYETCR 4EPE3 PA3PYUEHNE",
	"TOT KTO 3AVNVAETCR, PEDKO HAZADAET",
	"ME4 ZPABOCYDNR #LICTP N OCTP",
	"ZOKA DYX #ODPCTBYET TE/O PACQBETAET",
	"MOVL MAFNN HAZPABNTCR TYDA FDE ES HET",
	"BPEMR HE ZODB/ACTHO KPEZKOCTN CTA/N",
	"MAFNR HE BCEFDA TO, 4EM KA)KETCR",
	"4TO #LI/O OTKPLITLIM, 3AKPLITO TEZEPL",
	"COCPEDOTO4NE ZPNXODNT QEHO& MYDPOCTN",
	"TA&HOE 3HAHNE HECST PA3PYUEHNE",
	"TO, 4TO HE/L3R YDEP)KATL, HE/L3R ZOBPEDNTL",
	"KAPMNH N /A3YPL CTAHYT KAK CO/HQE",
	"3HAHNE N MYDPOCTL - QEHA CAMOFO CE#R",
	"BLIZE& N OCBE)KNCL",
	"KYDA NDSTE, TAM N ZPE#YDETE",
	"GHEPFNR ZPNXODNT QEHO& MYDPOCTN",
	"#OFATCBO ZPNXODNT HE)KDAHHO",
	"FDE A/4HOCTL ZADET, TAM TEPZEHNE HA&DET HAFPADY",
	"#/AFOC/OB/EHLI DO#PLIM TOBAPNVEM!",
	"BAUN PYKN ZPABRT BAUE& CYDL#O&",
	"CN/Y YKPEZ/RET #O)KECTBEHHAR BEPA",
	"CYTL )KN3HN NCXODNT N3HYTPN",
	"CBEPXY BAU ZYTL CTAHET RCHE&",
	"CZACEHNE ZPNXODNT QEHO& MYDPOCTN",
	"BCS MNCTN4ECKOE PACKPOETCR B CBETE BAUEFO PA3YMA",
	"ZOC/EDHNE EVE MOFYT #LITL ZEPBLIMN",
	"HN ODHO DO#POE DE/O HE OCTAETCR #E3HAKA3AHHLIM",
	"HEO#XODNM ZO KPA&HE& MEPE 8 YPOBEHL.",
	"HEO#XODNM ZO KPA&HE& MEPE 13 YPOBEHL.",
	"HEO#XODNM ZO KPA&HE& MEPE 17 YPOBEHL.",
	"ZO/Y4EHLI TA&HLIE 3HAHNR!",
	"TO, 4TO HE Y#NBAET TE#R...",
	"3HAHNE - CN/A.",
	"DAWVEMY, DA BO3DACTCR!",
	"NHOFDA HY)KHO ZOTPOFATL, 4TO#LI ZOHRTL.",
	"B FOCTRX XOPOUO, A DOMA /Y4UE.",
	"DYXOBHAR GHEPFNR BOCCTAHOB/EHA.",
	"BLI 4YBCTBYETE CE#R #O/EE /OBKNM.",
	"BLI 4YBCTBYETE CE#R CN/LHEE.",
	"BLI 4YBCTBYETE CE#R MYDPEE.",
	"BLI 4YBCTBYETE CE#R OCBE)KSHHLIM.",
	"GTO MO)KET C/OMNTL BO/W.",
};

void InitDiabloMsg(char e)
{
	int i;

	if (msgcnt >= sizeof(msgtable))
		return;

	for (i = 0; i < msgcnt; i++) {
		if (msgtable[i] == e)
			return;
	}

	msgtable[msgcnt] = e; // BUGFIX: missing out-of-bounds check (fixed)
	msgcnt++;

	msgflag = msgtable[0];
	msgdelay = SDL_GetTicks();
}

void ClrDiabloMsg()
{
	int i;

	for (i = 0; i < sizeof(msgtable); i++)
		msgtable[i] = 0;

	msgflag = 0;
	msgcnt = 0;
}

void DrawDiabloMsg()
{
	int i, len, width, sx, sy;
	BYTE c;

	CelDraw(PANEL_X + 101, DIALOG_Y, pSTextSlidCels, 1, 12);
	CelDraw(PANEL_X + 527, DIALOG_Y, pSTextSlidCels, 4, 12);
	CelDraw(PANEL_X + 101, DIALOG_Y + 48, pSTextSlidCels, 2, 12);
	CelDraw(PANEL_X + 527, DIALOG_Y + 48, pSTextSlidCels, 3, 12);

	sx = PANEL_X + 109;
	for (i = 0; i < 35; i++) {
		CelDraw(sx, DIALOG_Y, pSTextSlidCels, 5, 12);
		CelDraw(sx, DIALOG_Y + 48, pSTextSlidCels, 7, 12);
		sx += 12;
	}
	sy = DIALOG_Y + 12;
	for (i = 0; i < 3; i++) {
		CelDraw(PANEL_X + 101, sy, pSTextSlidCels, 6, 12);
		CelDraw(PANEL_X + 527, sy, pSTextSlidCels, 8, 12);
		sy += 12;
	}

	assert(gpBuffer);

	trans_rect(PANEL_LEFT + 104, DIALOG_TOP - 8, 432, 54);

	strcpy(tempstr, MsgStrings[msgflag]);
	sx = PANEL_X + 101;
	sy = DIALOG_Y + 24;
	len = strlen(tempstr);
	width = 0;

	for (i = 0; i < len; i++) {
		width += fontkern[fontframe[gbFontTransTbl[(BYTE)tempstr[i]]]] + 1;
	}

	if (width < 442) {
		sx += (442 - width) >> 1;
	}

	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(BYTE)tempstr[i]]];
		if (c != '\0') {
			PrintChar(sx, sy, c, COL_GOLD);
		}
		sx += fontkern[c] + 1;
	}

	if (msgdelay > 0 && msgdelay <= SDL_GetTicks() - 3500) {
		msgdelay = 0;
	}
	if (msgdelay == 0) {
		msgcnt--;
		if (msgcnt == 0) {
			msgflag = 0;
		} else {
			msgflag = msgtable[msgcnt];
			msgdelay = SDL_GetTicks();
		}
	}
}

DEVILUTION_END_NAMESPACE

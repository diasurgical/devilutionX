/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int help_select_line;
int unused_help;
BOOL helpflag;
int displayinghelp[22]; /* check, does nothing? */
int HelpTop;

const char gszSpawnHelpText[] = {
	"O3HAKOMNTE/LHAR BEPCNR NFPLI|"
	"|"
	"$HACTPO&KN K/ABNU:|"
	"B NFPY MO)KHO NFPATL NCK/W4NTE/LHO C ZOMOVLW MLIUN. "
	"ODHAKO BLI MO)KETE NCZO/L3OBATL HEKOTOPLIE DE&CTBNR "
	"C K/ABNATYPLI. GTN KOMAHDLI ZEPE4NC/EHLI HN)KE:|"
	"|"
	"(P1:    OKHO ZOMOVN|"
	"ECK:   F/ABHOE MEHW|"
	"TA#:   KAPTA|"
	"LI: BLI#OP DE&CTBN&|"
	"N: KHNFA 3AK/NHAHN&|"
	"U: NHBEHTAPL|"
	"C: OKHO ZEPCOHA)KA|"
	"&: 3ADAHNR|"
	"A: YMEHLUNTL RPKOCTL GKPAHA|"
	"Z: YBE/N4NTL RPKOCTL GKPAHA|"
	"R: MACUTA#NPOBAHNE GKPAHA|"
	"+ / -: MACUTA# KAPTLI|"
	"1 - 8: NCZO/L3OBATL ZPEDMETLI HA ZORCE|"
	"(P5, (P6, (P7, (P8:     HA3HA4NTL K/ABNUN D/R HABLIKOB"
	" N 3AK/NHAHN&|"
	"UN(PT + /KM: ATAKA #E3 DBN)KEHNR|"
	"ZPO#E/:  3AKPLITL BCE OKHA|"
	"&"
};

const char gszHelpText[] = {
	"$HACTPO&KN K/ABNU:|"
	"(P1:    OKHO ZOMOVN|"
	"ECK:   F/ABHOE MEHW|"
	"TA#:   KAPTA|"
	"LI: BLI#OP DE&CTBN&|"
	"N: KHNFA 3AK/NHAHN&|"
	"U: NHBEHTAPL|"
	"C: OKHO ZEPCOHA)KA|"
	"&: 3ADAHNR|"
	"A: YMEHLUNTL RPKOCTL GKPAHA|"
	"Z: YBE/N4NTL RPKOCTL GKPAHA|"
	"R: MACUTA#NPOBAHNE GKPAHA|"
	"+ / -: MACUTA# KAPTLI|"
	"1 - 8: NCZO/L3OBATL ZPEDMETLI HA ZORCE|"
	"(P5, (P6, (P7, (P8:     HA3HA4NTL K/ABNUN D/R HABLIKOB"
	" N 3AK/NHAHN&|"
	"UN(PT + /KM: ATAKA #E3 DBN)KEHNR|"
	"ZPO#E/:  3AKPLITL BCE OKHA|"
	"|"
	"$ZEPEDBN)KEHNE:|"
	"EC/N YDEP)KNBATL /KM, ZEPCOHA)K #YDET DBNFATLCR B HAZPAB/EHNN "
	"YKA3ATE/R.|"
	"|"
	"$CPA)KEHNE:|"
	"BLI MO)KETE ATAKOBATL HE ZEPEMEVARCL, YDEP)KNBAR K/ABNUY UN(PT "
	"ZPN ATAKE.|"
	"|"
	"$KAPTA:|"
	"HA)KMNTE KHOZKY KAPTA HA ZAHE/N N/N K/ABNUY TA#. "
	"MACUTA#NPOBAHNE KAPTLI OCYVECTB/RETCR C ZOMOVLW K/ABNU + N -. "
	"ZEPEMEVATL KAPTY MO)KHO C ZOMOVLW CTPE/OK KYPCOPA.|"
	"|"
	"$ZODHRTNE ZPEDMETOB:|"
	"ZO/E3HLIE ZPEDMETLI HE#O/LUOFO PA3MEPA, TAKNE KAK 3E/LR N/N CBNTKN "
	"ABTOMATN4ECKN ZOMEVAWTCR HA BAU ZORC, NX KO/N4ECTBO OTO#PA)KAETCR "
	"4NC/OM. ZPEDMETLI C ZORCA MO)KHO NCZO/L3OBATL, HA)KNMAR K/ABNUN 1-9,"
	"/N#O HA)KAB ZKM HA ZPEDMETE.|"
	"|"
	"$3O/OTO|"
	"BLI MO)KETE BLI#PATL OZPEDE/EHHOE KO/N4ECTBO MOHET HA)KATNEM ZKM N "
	"BLI/O)KNTL NX N3 NHBEHTAPR.|"
	"|"
	"$HABLIKN N MAFNR:|"
	"HA)KAB HA K/ABNUY BLI#OPA DE&CTBN& BLI MO)KETE ZPOCMOTPETL CZNCOK "
	"BAUNX HABLIKOB, BLIY4EHHLIX 3AK/NHAHN&, CBNTKOB N 3AK/NHAHN& B "
	"ZOCOXAX. HA)KATNE /KM ZPNBEDET BLI#PAHHLI& HABLIK N/N 3AK/NHAHNE B "
	"FOTOBHOCTL. 4TO#LI ZPON3HECTN FOTOBOE 3AK/NHAHNE N/N NCZO/L3OBATL "
	"HABLIK, DOCTATO4HO ZPOCTO VE/KHYTL ZKM HA QE/L.|"
	"|"
	"$NCZO/L3OBAHNE DE&CTBN&|"
	"HA)KATNE /KM ZPNBEDET BLI#PAHHLI& HABLIK N/N 3AK/NHAHNE B FOTOBHOCTL. "
	"4TO#LI ZPON3HECTN FOTOBOE 3AK/NHAHNE N/N NCZO/L3OBATL HABLIK, "
	"DOCTATO4HO ZPOCTO VE/KHYTL ZKM HA QE/L.|"
	"|"
	"$HACTPO&KA FOPR4NX K/ABNU|"
	"BLI MO)KETE HA3HA4NTL FOPR4NE K/ABNUN D/R HABLIKOB N/N 3AK/NHAHN&. "
	"OTKPO&TE CZNCOK BLI#OPA DE&CTBN&, BLI#EPETE HY)KHOE N HA)KMNTE ODHY "
	"N3 K/ABNU (P5, (P6, (P7 N/N (P8 4TO#LI 3AKPEZNTL DE&CTBNE 3A HE&.|"
	"|"
	"$KHNFN MAFNN|"
	"4TEHNE KHNF YBE/N4NBAET BAUN ZO3HAHNR B O#/ACTN TOFO N/N NHOFO "
	"3AK/NHAHNR, ZO3BO/RR NCZO/L3OBATL EFO #O/EE G(P(PEKTNBHO.|"
	"&"
};

void InitHelp()
{
	helpflag = FALSE;
	unused_help = 0;
	displayinghelp[0] = 0;
}

static void DrawHelpLine(int x, int y, char *text, char color)
{
	int sx, sy, width;
	BYTE c;

	width = 0;
	sx = x + 32 + PANEL_X;
	sy = y * 12 + 44 + SCREEN_Y + UI_OFFSET_Y;
	while (*text) {
		c = gbFontTransTbl[(BYTE)*text];
		text++;
		c = fontframe[c];
		width += fontkern[c] + 1;
		if (c) {
			if (width <= 577)
				PrintChar(sx, sy, c, color);
		}
		sx += fontkern[c] + 1;
	}
}

void DrawHelp()
{
	int i, c, w;
	char col;
	const char *s;

	DrawSTextHelp();
	DrawQTextBack();
	if (gbIsHellfire)
		PrintSString(0, 2, TRUE, "ZOMOVL ZO NFPE", COL_GOLD, 0);
	else
		PrintSString(0, 2, TRUE, "ZOMOVL ZO NFPE", COL_GOLD, 0);
	DrawSLine(5);

	s = &gszHelpText[0];
	if (gbIsSpawn)
		s = &gszSpawnHelpText[0];

	for (i = 0; i < help_select_line; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		if (*s == '$') {
			s++;
		}
		if (*s == '&') {
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			w += fontkern[fontframe[gbFontTransTbl[(BYTE)tempstr[c]]]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (*s == '|') {
			s++;
		}
	}
	for (i = 7; i < 22; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		if (*s == '$') {
			s++;
			col = COL_RED;
		} else {
			col = COL_WHITE;
		}
		if (*s == '&') {
			HelpTop = help_select_line;
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			BYTE tc = gbFontTransTbl[(BYTE)tempstr[c]];
			w += fontkern[fontframe[tc]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (c != 0) {
			tempstr[c] = '\0';
			DrawHelpLine(0, i, tempstr, col);
		}
		if (*s == '|') {
			s++;
		}
	}

	PrintSString(0, 23, TRUE, "NCZO/L3Y&TE KYPCOP D/R ZPOKPYTKN, ECK D/R BLIXODA", COL_GOLD, 0);
}

void DisplayHelp()
{
	help_select_line = 0;
	helpflag = TRUE;
	HelpTop = 5000;
}

void HelpScrollUp()
{
	if (help_select_line > 0)
		help_select_line--;
}

void HelpScrollDown()
{
	if (help_select_line < HelpTop)
		help_select_line++;
}

DEVILUTION_END_NAMESPACE

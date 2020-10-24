/**
 * @file automap.cpp
 *
 * Implementation of the in-game map overlay.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

/**
 * Maps from tile_id to automap type.
 * BUGFIX: only the first 256 elements are ever read
 */
WORD automaptype[512];
static int AutoMapX;
static int AutoMapY;
/** Specifies whether the automap is enabled. */
BOOL automapflag;
char AmShiftTab[32];
/** Tracks the explored areas of the map. */
BOOLEAN automapview[DMAXX][DMAXY];
/** Specifies the scale of the automap. */
int AutoMapScale;
int AutoMapXOfs;
int AutoMapYOfs;
int AmLine64;
int AmLine32;
int AmLine16;
int AmLine8;
int AmLine4;

/** color used to draw the player's arrow */
#define COLOR_PLAYER (PAL8_ORANGE + 1)
/** color for bright map lines (doors, stairs etc.) */
#define COLOR_BRIGHT PAL8_YELLOW
/** color for dim map lines/dots */
#define COLOR_DIM (PAL16_YELLOW + 8)
#ifdef HELLFIRE
// color for items on automap
#define COLOR_ITEM (PAL8_BLUE + 1)
#endif

#define MAPFLAG_TYPE 0x000F
/** these are in the second byte */
#define MAPFLAG_VERTDOOR 0x01
#define MAPFLAG_HORZDOOR 0x02
#define MAPFLAG_VERTARCH 0x04
#define MAPFLAG_HORZARCH 0x08
#define MAPFLAG_VERTGRATE 0x10
#define MAPFLAG_HORZGRATE 0x20
#define MAPFLAG_DIRT 0x40
#define MAPFLAG_STAIRS 0x80

/**
 * @brief Initializes the automap.
 */
void InitAutomapOnce()
{
	automapflag = FALSE;
	AutoMapScale = 50;
	AmLine64 = 32;
	AmLine32 = 16;
	AmLine16 = 8;
	AmLine8 = 4;
	AmLine4 = 2;
}

/**
 * @brief Loads the mapping between tile IDs and automap shapes.
 */
void InitAutomap()
{
	BYTE b1, b2;
	DWORD dwTiles;
	int x, y;
	BYTE *pAFile, *pTmp;
	int i;

	memset(automaptype, 0, sizeof(automaptype));

	switch (leveltype) {
	case DTYPE_CATHEDRAL:
#ifdef HELLFIRE
		if (currlevel < 21)
			pAFile = LoadFileInMem("Levels\\L1Data\\L1.AMP", &dwTiles);
		else
			pAFile = LoadFileInMem("NLevels\\L5Data\\L5.AMP", &dwTiles);
#else
		pAFile = LoadFileInMem("Levels\\L1Data\\L1.AMP", &dwTiles);
#endif
		break;
	case DTYPE_CATACOMBS:
		pAFile = LoadFileInMem("Levels\\L2Data\\L2.AMP", &dwTiles);
		break;
	case DTYPE_CAVES:
#ifdef HELLFIRE
		if (currlevel < 17)
			pAFile = LoadFileInMem("Levels\\L3Data\\L3.AMP", &dwTiles);
		else
			pAFile = LoadFileInMem("NLevels\\L6Data\\L6.AMP", &dwTiles);
#else
		pAFile = LoadFileInMem("Levels\\L3Data\\L3.AMP", &dwTiles);
#endif
		break;
	case DTYPE_HELL:
		pAFile = LoadFileInMem("Levels\\L4Data\\L4.AMP", &dwTiles);
		break;
	default:
		return;
	}

	dwTiles /= 2;
	pTmp = pAFile;

	for (i = 1; i <= dwTiles; i++) {
		b1 = *pTmp++;
		b2 = *pTmp++;
		automaptype[i] = b1 + (b2 << 8);
	}

	mem_free_dbg(pAFile);
	memset(automapview, 0, sizeof(automapview));

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++)
			dFlags[x][y] &= ~BFLAG_EXPLORED;
	}
}

/**
 * @brief Displays the automap.
 */
void StartAutomap()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	automapflag = TRUE;
}

/**
 * @brief Scrolls the automap upwards.
 */
void AutomapUp()
{
	AutoMapXOfs--;
	AutoMapYOfs--;
}

/**
 * @brief Scrolls the automap downwards.
 */
void AutomapDown()
{
	AutoMapXOfs++;
	AutoMapYOfs++;
}

/**
 * @brief Scrolls the automap leftwards.
 */
void AutomapLeft()
{
	AutoMapXOfs--;
	AutoMapYOfs++;
}

/**
 * @brief Scrolls the automap rightwards.
 */
void AutomapRight()
{
	AutoMapXOfs++;
	AutoMapYOfs--;
}

/**
 * @brief Increases the zoom level of the automap.
 */
void AutomapZoomIn()
{
	if (AutoMapScale < 200) {
		AutoMapScale += 5;
		AmLine64 = (AutoMapScale << 6) / 100;
		AmLine32 = AmLine64 >> 1;
		AmLine16 = AmLine32 >> 1;
		AmLine8 = AmLine16 >> 1;
		AmLine4 = AmLine8 >> 1;
	}
}

/**
 * @brief Decreases the zoom level of the automap.
 */
void AutomapZoomOut()
{
	if (AutoMapScale > 50) {
		AutoMapScale -= 5;
		AmLine64 = (AutoMapScale << 6) / 100;
		AmLine32 = AmLine64 >> 1;
		AmLine16 = AmLine32 >> 1;
		AmLine8 = AmLine16 >> 1;
		AmLine4 = AmLine8 >> 1;
	}
}

/**
 * @brief Renders the automap on screen.
 */
void DrawAutomap()
{
	int cells;
	int sx, sy;
	int i, j, d;
	int mapx, mapy;

	if (leveltype == DTYPE_TOWN) {
		DrawAutomapText();
		return;
	}

	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (SCREEN_Y + VIEWPORT_HEIGHT)];

	AutoMapX = (ViewX - 16) >> 1;
	while (AutoMapX + AutoMapXOfs < 0)
		AutoMapXOfs++;
	while (AutoMapX + AutoMapXOfs >= DMAXX)
		AutoMapXOfs--;
	AutoMapX += AutoMapXOfs;

	AutoMapY = (ViewY - 16) >> 1;
	while (AutoMapY + AutoMapYOfs < 0)
		AutoMapYOfs++;
	while (AutoMapY + AutoMapYOfs >= DMAXY)
		AutoMapYOfs--;
	AutoMapY += AutoMapYOfs;

	d = (AutoMapScale << 6) / 100;
	cells = 2 * (SCREEN_WIDTH / 2 / d) + 1;
	if ((SCREEN_WIDTH / 2) % d)
		cells++;
	if ((SCREEN_WIDTH / 2) % d >= (AutoMapScale << 5) / 100)
		cells++;

	if (ScrollInfo._sxoff + ScrollInfo._syoff)
		cells++;
	mapx = AutoMapX - cells;
	mapy = AutoMapY - 1;

	if (cells & 1) {
		sx = SCREEN_WIDTH / 2 + SCREEN_X - AmLine64 * ((cells - 1) >> 1);
		sy = (SCREEN_HEIGHT - PANEL_HEIGHT) / 2 + SCREEN_Y - AmLine32 * ((cells + 1) >> 1);
	} else {
		sx = SCREEN_WIDTH / 2 + SCREEN_X - AmLine64 * (cells >> 1) + AmLine32;
		sy = (SCREEN_HEIGHT - PANEL_HEIGHT) / 2 + SCREEN_Y - AmLine32 * (cells >> 1) - AmLine16;
	}
	if (ViewX & 1) {
		sx -= AmLine16;
		sy -= AmLine8;
	}
	if (ViewY & 1) {
		sx += AmLine16;
		sy -= AmLine8;
	}

	sx += AutoMapScale * ScrollInfo._sxoff / 100 >> 1;
	sy += AutoMapScale * ScrollInfo._syoff / 100 >> 1;
	if (PANELS_COVER) {
		if (invflag || sbookflag) {
			sx -= SCREEN_WIDTH / 4;
		}
		if (chrflag || questlog) {
			sx += SCREEN_WIDTH / 4;
		}
	}

	for (i = 0; i <= cells + 1; i++) {
		int x = sx;
		int y;

		for (j = 0; j < cells; j++) {
			WORD maptype = GetAutomapType(mapx + j, mapy - j, TRUE);
			if (maptype != 0)
				DrawAutomapTile(x, sy, maptype);
			x += AmLine64;
		}
		mapy++;
		x = sx - AmLine32;
		y = sy + AmLine16;
		for (j = 0; j <= cells; j++) {
			WORD maptype = GetAutomapType(mapx + j, mapy - j, TRUE);
			if (maptype != 0)
				DrawAutomapTile(x, y, maptype);
			x += AmLine64;
		}
		mapx++;
		sy += AmLine32;
	}
	DrawAutomapPlr();
#ifdef HELLFIRE
	if (AutoMapShowItems)
		SearchAutomapItem();
#endif
	DrawAutomapText();
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (SCREEN_Y + SCREEN_HEIGHT)];
}

int GetTextWidth(char *s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[*s++]]] + 1;
	}
	return l;
}

void local_DvlIntSetting(const char *valuename, int *value)
{
	if (!SRegLoadValue("devilutionx", valuename, 0, value)) {
		SRegSaveValue("devilutionx", valuename, 0, *value);
	}
}

void DrawMonsterHealthBar(int monsterID)
{
	BOOL enabled = FALSE;
	local_DvlIntSetting("monster health bar", &enabled);
	if (!enabled)
		return;
	if (currlevel == 0)
		return;
	MonsterStruct *mon = &monster[monsterID];
	bool specialMonster = mon->_uniqtype != 0;
	int currentLife = mon->_mhitpoints;
	int maxLife = mon->_mmaxhp;

	if (currentLife > maxLife) 
		maxLife = currentLife;
	
	float FilledPercent = (float)currentLife / (float)maxLife;
	const int yPos = 180;
	const int width = 250;
	const int xPos = (SCREEN_WIDTH) / 2 - BORDER_LEFT;
	const int height = 25;
	const int xOffset = 0;
	const int yOffset = 1;
	int borderWidth = 2;
	if (specialMonster) 
		borderWidth = 2;
	int borderColors[] = { 242 /*undead*/, 232 /*demon*/, 182 /*beast*/ };
	int borderColor = borderColors[mon->MData->mMonstClass]; //200; // pure golden, unique item style
	int filledColor = 142; // optimum balance in bright red between dark and light
	bool fillCorners = true;
	int square = 10;
	char *immuText = "IMMU: ";
	char *resText = "RES: ";
	char *vulnText = ":VULN";
	int resSize = 3;
	int resistColors[] = { 148, 140, 129 }; // { 170,140,129,148,242 };// {168, 216, 200, 242, 142 }; // arcane // fire // lightning // acid
	WORD immunes[] = { IMUNE_MAGIC, IMUNE_FIRE, IMUNE_LIGHTNING };
	WORD resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };
	WORD mres = mon->mMagicRes;

	int resOffset = 0 + GetTextWidth(resText);
	for (int k = 0; k < resSize; ++k) {
		if (!(mres & resists[k]))
			continue;
		for (int j = 0; j < square; j++) {
			for (int i = 0; i < square; i++) {
				ENG_set_pixel(xPos + i + resOffset, yPos + height + j + yOffset + borderWidth + 2, resistColors[k]);
			}
		}
		resOffset += square + 2;
	}

	int vulOffset = width - square - GetTextWidth(vulnText) - 4;
	for (int k = 0; k < resSize; ++k) {
		if (mres & resists[k] || mres & immunes[k])
			continue;
		for (int j = 0; j < square; j++) {
			for (int i = 0; i < square; i++) {
				ENG_set_pixel(xPos + i + vulOffset, yPos + height + j + yOffset + borderWidth + 2, resistColors[k]);
			}
		}
		vulOffset -= square + 2;
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < (width * FilledPercent); i++) {
			int tmpColor = filledColor;
			ENG_set_pixel(xPos + i, yPos + j, tmpColor);
		}
	}

	for (int j = 0; j < borderWidth; j++) {
		for (int i = -xOffset - (fillCorners ? borderWidth : 0); i < width + xOffset + (fillCorners ? borderWidth : 0); i++) {
			ENG_set_pixel(xPos + i, yPos + j - yOffset - borderWidth, borderColor);
		}
		for (int i = -xOffset; i < width + xOffset + (fillCorners ? borderWidth : 0); i++) {
			ENG_set_pixel(xPos + i, yPos + j + yOffset + height, borderColor);
		}
	}

	for (int j = -yOffset; j < height + yOffset + (fillCorners ? borderWidth : 0); j++) {
		for (int i = 0; i < borderWidth; i++) {
			ENG_set_pixel(xPos + i - xOffset - borderWidth, yPos + j, borderColor);
			ENG_set_pixel(xPos + i + xOffset + width, yPos + j, borderColor);
		}
	}

	bool drawImmu = false;
	int immuOffset = 0 + GetTextWidth(immuText) - 5;
	for (int k = 0; k < resSize; ++k) {
		if (mres & immunes[k]) {
			drawImmu = true;
			for (int j = 0; j < square; j++) {
				for (int i = 0; i < square; i++) {
					ENG_set_pixel(xPos + i + immuOffset, yPos + height + j + yOffset + borderWidth + 2 - 15, resistColors[k]);
				}
			}
			immuOffset += square + 2;
		}
	}

	int newX = xPos + BORDER_LEFT;
	int newY = yPos + height - 3;

	char text[166];
	strcpy(text, mon->mName);
	if (mon->leader > 0) 
		strcat(text, " (minion)");
	
	int namecolor = COL_WHITE;
	if (specialMonster) 
		namecolor = COL_GOLD;
	PrintGameStr(newX - GetTextWidth(text) / 2, 30, text, namecolor);
	PrintGameStr(newX - GetTextWidth("/") / 2, 43, "/", COL_WHITE);
	
	sprintf(text, "%d", (maxLife >> 6));
	PrintGameStr(newX + GetTextWidth("/"), 43, text, COL_WHITE);

	sprintf(text, "%d", (currentLife >> 6));
	PrintGameStr(newX - GetTextWidth(text) - GetTextWidth("/"), 43, text, COL_WHITE);

	PrintGameStr(newX - width / 2, 59, resText, COL_GOLD);
	
	sprintf(text, "kills: %d", monstkills[mon->MType->mtype]);
	PrintGameStr(newX - GetTextWidth("kills:") / 2 - 30, 59, text, COL_WHITE);

	if (drawImmu) 
		PrintGameStr(newX - width / 2, 46, immuText, COL_GOLD);
	
	PrintGameStr(newX + width / 2 - GetTextWidth(vulnText), 59, vulnText, COL_RED);
}

void DrawXPBar()
{
	BOOL enabled = FALSE;
	local_DvlIntSetting("xp bar", &enabled);
	if (!enabled)
		return;
	int barSize = 306; // *ScreenWidth / 640;
	int offset = 3;
	int barRows = 3; // *ScreenHeight / 480;
	int dividerHeight = 3;
	int numDividers = 10;
	int barColor = 242; /*242white, 142red, 200yellow, 182blue*/
	int emptyBarColor = 0;
	int frameColor = 242;
	int yPos = BORDER_TOP + SCREEN_HEIGHT - 8;

	PrintGameStr(145 + (SCREEN_WIDTH - 640) / 2, SCREEN_HEIGHT - 4, "XP", COL_WHITE);
	int charLevel = plr[myplr]._pLevel;
	if (charLevel != MAXCHARLEVEL - 1) {
		int curXp = ExpLvlsTbl[charLevel];
		int prevXp = ExpLvlsTbl[charLevel - 1];
		int prevXpDelta = curXp - prevXp;
		int prevXpDelta_1 = plr[myplr]._pExperience - prevXp;
		if (plr[myplr]._pExperience >= prevXp) {
			int visibleBar = barSize * (unsigned __int64)prevXpDelta_1 / prevXpDelta;

			for (int i = 0; i < visibleBar; ++i) {
				for (int j = 0; j < barRows; ++j) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos - barRows / 2 + j, barColor);
				}
			}

			for (int i = visibleBar; i < barSize; ++i) {
				for (int j = 0; j < barRows; ++j) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos - barRows / 2 + j, emptyBarColor);
				}
			}
			//draw frame
			//horizontal
			for (int i = -1; i <= barSize; ++i) {
				ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos - barRows / 2 - 1, frameColor);
				ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos + barRows / 2 + 1, frameColor);
			}
			//vertical
			for (int i = -dividerHeight; i < barRows + dividerHeight; ++i) {
				if (i >= 0 && i < barRows) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 - 1 + offset, yPos - barRows / 2 + i, frameColor);
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + barSize + offset, yPos - barRows / 2 + i, frameColor);
				}
				for (int j = 1; j < numDividers; ++j) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 - 1 + offset + (barSize * j / numDividers), yPos - barRows / 2 + i, frameColor);
				}
			}
			//draw frame
		}
	}
}

class drawingQueue {
public:
	int ItemID;
	int Row;
	int Col;
	int x;
	int y;
	int width;
	int height;
	int color;
	char text[64];
	drawingQueue(int x2, int y2, int width2, int height2, int Row2, int Col2, int ItemID2, int q2, char* text2)
	{
		x = x2;
		y = y2;
		Row = Row2;
		Col = Col2;
		ItemID = ItemID2;
		width = width2;
		height = height2;
		color = q2;
		strcpy(text, text2);
	}
};

std::vector<drawingQueue> drawQ;

void AddItemToDrawQueue(int x, int y, int id)
{
	BOOL enabled = FALSE;
	local_DvlIntSetting("highlight items", &enabled);
	if (!enabled)
		return;
	ItemStruct *it = &item[id];
	bool error = false;

	char textOnGround[64];
	if (it->_itype == ITYPE_GOLD) {
		sprintf(textOnGround, "%i gold", it->_ivalue);
	} else {
		sprintf(textOnGround, "%s", it->_iIdentified ? it->_iIName : it->_iName);
	}


	int centerXOffset = GetTextWidth((char *)textOnGround);

	x -= centerXOffset / 2 + 20;
	y -= 193;
	drawQ.push_back(drawingQueue(x, y, GetTextWidth((char *)textOnGround), 13, it->_ix, it->_iy, id, 66, textOnGround));
}

void DrawBackground(int xPos, int yPos, int width, int height, int borderX, int borderY, BYTE backgroundColor, BYTE borderColor)
{
	xPos += BORDER_LEFT;
	yPos += BORDER_TOP;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x < borderX || x + borderX >= width || y < borderY || y + borderY >= height)
				continue;
			int val = ((yPos - height) + y) * BUFFER_WIDTH + xPos + x;
			gpBuffer[val] = backgroundColor;
		}
	}
}

void HighlightItemsNameOnMap()
{
	BOOL enabled = FALSE;
	local_DvlIntSetting("highlight items", &enabled);
	if (!enabled)
		return;
	const int borderX = 5;
	for (unsigned int i = 0; i < drawQ.size(); ++i) {
		std::map<int, bool> backtrace;

		bool canShow;
		do {
			canShow = true;
			for (unsigned int j = 0; j < i; ++j) {
				if (abs(drawQ[j].y - drawQ[i].y) < drawQ[i].height + 2) {
					int newpos = drawQ[j].x;
					if (drawQ[j].x >= drawQ[i].x && drawQ[j].x - drawQ[i].x < drawQ[i].width + borderX) {
						newpos -= drawQ[i].width + borderX;
						if (backtrace.find(newpos) != backtrace.end()) 
							newpos = drawQ[j].x + drawQ[j].width + borderX;
					} else if (drawQ[j].x < drawQ[i].x && drawQ[i].x - drawQ[j].x < drawQ[j].width + borderX) {
						newpos += drawQ[j].width + borderX;
						if (backtrace.find(newpos) != backtrace.end()) 
							newpos = drawQ[j].x - drawQ[i].width - borderX;
					} else 
						continue;
					canShow = false;
					drawQ[i].x = newpos;
					backtrace[newpos] = true;
				}
			}
		} while (!canShow);
	}

	for (unsigned int i = 0; i < drawQ.size(); ++i) {
		drawingQueue t = drawQ[i];

		int sx = t.x;
		int sy = t.y;

		int sx2 = sx;
		int sy2 = sy + 1;

		if (sx < 0 || sx >= SCREEN_WIDTH || sy < 0 || sy >= SCREEN_HEIGHT) {
			continue;
		}
		if (sx2 < 0 || sx2 >= SCREEN_WIDTH || sy2 < 0 || sy2 >= SCREEN_HEIGHT) {
			continue;
		}

		int bgcolor = 0;
		int CursorX = MouseX;
		int CursorY = MouseY + t.height;

		if (CursorX >= sx && CursorX <= sx + t.width + 1 && CursorY >= sy && CursorY <= sy + t.height) {
			bgcolor = 134;
			cursmx = t.Row;
			cursmy = t.Col;
			pcursitem = t.ItemID;
		}
		DrawBackground(sx2, sy2, t.width + 1, t.height, 0, 0, bgcolor, bgcolor);
		PrintGameStr(sx, sy, &t.text[0u], t.color);
	}
	drawQ.clear();
}

/**
 * @brief Renders the given automap shape at the specified screen coordinates.
 */
void DrawAutomapTile(int sx, int sy, WORD automap_type)
{
	BOOL do_vert;
	BOOL do_horz;
	BOOL do_cave_horz;
	BOOL do_cave_vert;
	int x1, y1, x2, y2;

	BYTE flags = automap_type >> 8;

	if (flags & MAPFLAG_DIRT) {
		ENG_set_pixel(sx, sy, COLOR_DIM);
		ENG_set_pixel(sx - AmLine8, sy - AmLine4, COLOR_DIM);
		ENG_set_pixel(sx - AmLine8, sy + AmLine4, COLOR_DIM);
		ENG_set_pixel(sx + AmLine8, sy - AmLine4, COLOR_DIM);
		ENG_set_pixel(sx + AmLine8, sy + AmLine4, COLOR_DIM);
		ENG_set_pixel(sx - AmLine16, sy, COLOR_DIM);
		ENG_set_pixel(sx + AmLine16, sy, COLOR_DIM);
		ENG_set_pixel(sx, sy - AmLine8, COLOR_DIM);
		ENG_set_pixel(sx, sy + AmLine8, COLOR_DIM);
		ENG_set_pixel(sx + AmLine8 - AmLine32, sy + AmLine4, COLOR_DIM);
		ENG_set_pixel(sx - AmLine8 + AmLine32, sy + AmLine4, COLOR_DIM);
		ENG_set_pixel(sx - AmLine16, sy + AmLine8, COLOR_DIM);
		ENG_set_pixel(sx + AmLine16, sy + AmLine8, COLOR_DIM);
		ENG_set_pixel(sx - AmLine8, sy + AmLine16 - AmLine4, COLOR_DIM);
		ENG_set_pixel(sx + AmLine8, sy + AmLine16 - AmLine4, COLOR_DIM);
		ENG_set_pixel(sx, sy + AmLine16, COLOR_DIM);
	}

	if (flags & MAPFLAG_STAIRS) {
		DrawLine(sx - AmLine8, sy - AmLine8 - AmLine4, sx + AmLine8 + AmLine16, sy + AmLine4, COLOR_BRIGHT);
		DrawLine(sx - AmLine16, sy - AmLine8, sx + AmLine16, sy + AmLine8, COLOR_BRIGHT);
		DrawLine(sx - AmLine16 - AmLine8, sy - AmLine4, sx + AmLine8, sy + AmLine8 + AmLine4, COLOR_BRIGHT);
		DrawLine(sx - AmLine32, sy, sx, sy + AmLine16, COLOR_BRIGHT);
	}

	do_vert = FALSE;
	do_horz = FALSE;
	do_cave_horz = FALSE;
	do_cave_vert = FALSE;
	switch (automap_type & MAPFLAG_TYPE) {
	case 1: // stand-alone column or other unpassable object
		x1 = sx - AmLine16;
		y1 = sy - AmLine16;
		x2 = x1 + AmLine32;
		y2 = sy - AmLine8;
		DrawLine(sx, y1, x1, y2, COLOR_DIM);
		DrawLine(sx, y1, x2, y2, COLOR_DIM);
		DrawLine(sx, sy, x1, y2, COLOR_DIM);
		DrawLine(sx, sy, x2, y2, COLOR_DIM);
		break;
	case 2:
	case 5:
		do_vert = TRUE;
		break;
	case 3:
	case 6:
		do_horz = TRUE;
		break;
	case 4:
		do_vert = TRUE;
		do_horz = TRUE;
		break;
	case 8:
		do_vert = TRUE;
		do_cave_horz = TRUE;
		break;
	case 9:
		do_horz = TRUE;
		do_cave_vert = TRUE;
		break;
	case 10:
		do_cave_horz = TRUE;
		break;
	case 11:
		do_cave_vert = TRUE;
		break;
	case 12:
		do_cave_horz = TRUE;
		do_cave_vert = TRUE;
		break;
	}

	if (do_vert) {                      // right-facing obstacle
		if (flags & MAPFLAG_VERTDOOR) { // two wall segments with a door in the middle
			x1 = sx - AmLine32;
			x2 = sx - AmLine16;
			y1 = sy - AmLine16;
			y2 = sy - AmLine8;

			DrawLine(sx, y1, sx - AmLine8, y1 + AmLine4, COLOR_DIM);
			DrawLine(x1, sy, x1 + AmLine8, sy - AmLine4, COLOR_DIM);
			DrawLine(x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, sx, y2, COLOR_BRIGHT);
		}
		if (flags & MAPFLAG_VERTGRATE) { // right-facing half-wall
			DrawLine(sx - AmLine16, sy - AmLine8, sx - AmLine32, sy, COLOR_DIM);
			flags |= MAPFLAG_VERTARCH;
		}
		if (flags & MAPFLAG_VERTARCH) { // window or passable column
			x1 = sx - AmLine16;
			y1 = sy - AmLine16;
			x2 = x1 + AmLine32;
			y2 = sy - AmLine8;

			DrawLine(sx, y1, x1, y2, COLOR_DIM);
			DrawLine(sx, y1, x2, y2, COLOR_DIM);
			DrawLine(sx, sy, x1, y2, COLOR_DIM);
			DrawLine(sx, sy, x2, y2, COLOR_DIM);
		}
		if ((flags & (MAPFLAG_VERTDOOR | MAPFLAG_VERTGRATE | MAPFLAG_VERTARCH)) == 0)
			DrawLine(sx, sy - AmLine16, sx - AmLine32, sy, COLOR_DIM);
	}

	if (do_horz) { // left-facing obstacle
		if (flags & MAPFLAG_HORZDOOR) {
			x1 = sx + AmLine16;
			x2 = sx + AmLine32;
			y1 = sy - AmLine16;
			y2 = sy - AmLine8;

			DrawLine(sx, y1, sx + AmLine8, y1 + AmLine4, COLOR_DIM);
			DrawLine(x2, sy, x2 - AmLine8, sy - AmLine4, COLOR_DIM);
			DrawLine(x1, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, x2, y2, COLOR_BRIGHT);
		}
		if (flags & MAPFLAG_HORZGRATE) {
			DrawLine(sx + AmLine16, sy - AmLine8, sx + AmLine32, sy, COLOR_DIM);
			flags |= MAPFLAG_HORZARCH;
		}
		if (flags & MAPFLAG_HORZARCH) {
			x1 = sx - AmLine16;
			y1 = sy - AmLine16;
			x2 = x1 + AmLine32;
			y2 = sy - AmLine8;

			DrawLine(sx, y1, x1, y2, COLOR_DIM);
			DrawLine(sx, y1, x2, y2, COLOR_DIM);
			DrawLine(sx, sy, x1, y2, COLOR_DIM);
			DrawLine(sx, sy, x2, y2, COLOR_DIM);
		}
		if ((flags & (MAPFLAG_HORZDOOR | MAPFLAG_HORZGRATE | MAPFLAG_HORZARCH)) == 0)
			DrawLine(sx, sy - AmLine16, sx + AmLine32, sy, COLOR_DIM);
	}

	// for caves the horz/vert flags are switched
	if (do_cave_horz) {
		if (flags & MAPFLAG_VERTDOOR) {
			x1 = sx - AmLine32;
			x2 = sx - AmLine16;
			y1 = sy + AmLine16;
			y2 = sy + AmLine8;

			DrawLine(sx, y1, sx - AmLine8, y1 - AmLine4, COLOR_DIM);
			DrawLine(x1, sy, x1 + AmLine8, sy + AmLine4, COLOR_DIM);
			DrawLine(x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, sx, y2, COLOR_BRIGHT);
		} else
			DrawLine(sx, sy + AmLine16, sx - AmLine32, sy, COLOR_DIM);
	}

	if (do_cave_vert) {
		if (flags & MAPFLAG_HORZDOOR) {
			x1 = sx + AmLine16;
			x2 = sx + AmLine32;
			y1 = sy + AmLine16;
			y2 = sy + AmLine8;

			DrawLine(sx, y1, sx + AmLine8, y1 - AmLine4, COLOR_DIM);
			DrawLine(x2, sy, x2 - AmLine8, sy + AmLine4, COLOR_DIM);
			DrawLine(x1, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, x2, y2, COLOR_BRIGHT);
		} else
			DrawLine(sx, sy + AmLine16, sx + AmLine32, sy, COLOR_DIM);
	}
}
#ifdef HELLFIRE

void SearchAutomapItem()
{
	int x, y;
	int x1, y1, x2, y2;
	int px, py;
	int i, j;

	if (plr[myplr]._pmode == PM_WALK3) {
		x = plr[myplr]._pfutx;
		y = plr[myplr]._pfuty;
		if (plr[myplr]._pdir == DIR_W)
			x++;
		else
			y++;
	} else {
		x = plr[myplr]._px;
		y = plr[myplr]._py;
	}

	x1 = x - 8;
	if (x1 < 0)
		x1 = 0;
	else if (x1 > MAXDUNX)
		x1 = MAXDUNX;

	y1 = y - 8;
	if (y1 < 0)
		y1 = 0;
	else if (y1 > MAXDUNY)
		y1 = MAXDUNY;

	x2 = x + 8;
	if (x2 < 0)
		x2 = 0;
	else if (x2 > MAXDUNX)
		x2 = MAXDUNX;

	y2 = y + 8;
	if (y2 < 0)
		y2 = 0;
	else if (y2 > MAXDUNY)
		y2 = MAXDUNY;

	for (i = x1; i < x2; i++) {
		for (j = y1; j < y2; j++) {
			if (dItem[i][j] != 0){
				px = i - 2 * AutoMapXOfs - ViewX;
				py = j - 2 * AutoMapYOfs - ViewY;

				x = (ScrollInfo._sxoff * AutoMapScale / 100 >> 1) + (px - py) * AmLine16 + SCREEN_WIDTH / 2 + SCREEN_X;
				y = (ScrollInfo._syoff * AutoMapScale / 100 >> 1) + (px + py) * AmLine8 + (SCREEN_HEIGHT - PANEL_HEIGHT) / 2 + SCREEN_Y;

				if (PANELS_COVER) {
					if (invflag || sbookflag)
						x -= 160;
					if (chrflag || questlog)
						x += 160;
				}
				y -= AmLine8;
				DrawAutomapItem(x, y, COLOR_ITEM);
			}
		}
	}
}

void DrawAutomapItem(int x, int y, BYTE color)
{
	int x1, y1, x2, y2;

	x1 = x - AmLine32 / 2;
	y1 = y - AmLine16 / 2;
	x2 = x1 + AmLine64 / 2;
	y2 = y1 + AmLine32 / 2;
	DrawLine(x, y1, x1, y, color);
	DrawLine(x, y1, x2, y, color);
	DrawLine(x, y2, x1, y, color);
	DrawLine(x, y2, x2, y, color);
}
#endif

/**
 * @brief Renders an arrow on the automap, centered on and facing the direction of the player.
 */
void DrawAutomapPlr()
{
	int px, py;
	int x, y;

	if (plr[myplr]._pmode == PM_WALK3) {
		x = plr[myplr]._pfutx;
		y = plr[myplr]._pfuty;
		if (plr[myplr]._pdir == DIR_W)
			x++;
		else
			y++;
	} else {
		x = plr[myplr]._px;
		y = plr[myplr]._py;
	}
	px = x - 2 * AutoMapXOfs - ViewX;
	py = y - 2 * AutoMapYOfs - ViewY;

	x = (plr[myplr]._pxoff * AutoMapScale / 100 >> 1) + (ScrollInfo._sxoff * AutoMapScale / 100 >> 1) + (px - py) * AmLine16 + SCREEN_WIDTH / 2 + SCREEN_X;
	y = (plr[myplr]._pyoff * AutoMapScale / 100 >> 1) + (ScrollInfo._syoff * AutoMapScale / 100 >> 1) + (px + py) * AmLine8 + (SCREEN_HEIGHT - PANEL_HEIGHT) / 2 + SCREEN_Y;

	if (PANELS_COVER) {
		if (invflag || sbookflag)
			x -= SCREEN_WIDTH / 4;
		if (chrflag || questlog)
			x += SCREEN_WIDTH / 4;
	}
	y -= AmLine8;

	switch (plr[myplr]._pdir) {
	case DIR_N:
		DrawLine(x, y, x, y - AmLine16, COLOR_PLAYER);
		DrawLine(x, y - AmLine16, x - AmLine4, y - AmLine8, COLOR_PLAYER);
		DrawLine(x, y - AmLine16, x + AmLine4, y - AmLine8, COLOR_PLAYER);
		break;
	case DIR_NE:
		DrawLine(x, y, x + AmLine16, y - AmLine8, COLOR_PLAYER);
		DrawLine(x + AmLine16, y - AmLine8, x + AmLine8, y - AmLine8, COLOR_PLAYER);
		DrawLine(x + AmLine16, y - AmLine8, x + AmLine8 + AmLine4, y, COLOR_PLAYER);
		break;
	case DIR_E:
		DrawLine(x, y, x + AmLine16, y, COLOR_PLAYER);
		DrawLine(x + AmLine16, y, x + AmLine8, y - AmLine4, COLOR_PLAYER);
		DrawLine(x + AmLine16, y, x + AmLine8, y + AmLine4, COLOR_PLAYER);
		break;
	case DIR_SE:
		DrawLine(x, y, x + AmLine16, y + AmLine8, COLOR_PLAYER);
		DrawLine(x + AmLine16, y + AmLine8, x + AmLine8 + AmLine4, y, COLOR_PLAYER);
		DrawLine(x + AmLine16, y + AmLine8, x + AmLine8, y + AmLine8, COLOR_PLAYER);
		break;
	case DIR_S:
		DrawLine(x, y, x, y + AmLine16, COLOR_PLAYER);
		DrawLine(x, y + AmLine16, x + AmLine4, y + AmLine8, COLOR_PLAYER);
		DrawLine(x, y + AmLine16, x - AmLine4, y + AmLine8, COLOR_PLAYER);
		break;
	case DIR_SW:
		DrawLine(x, y, x - AmLine16, y + AmLine8, COLOR_PLAYER);
		DrawLine(x - AmLine16, y + AmLine8, x - AmLine4 - AmLine8, y, COLOR_PLAYER);
		DrawLine(x - AmLine16, y + AmLine8, x - AmLine8, y + AmLine8, COLOR_PLAYER);
		break;
	case DIR_W:
		DrawLine(x, y, x - AmLine16, y, COLOR_PLAYER);
		DrawLine(x - AmLine16, y, x - AmLine8, y - AmLine4, COLOR_PLAYER);
		DrawLine(x - AmLine16, y, x - AmLine8, y + AmLine4, COLOR_PLAYER);
		break;
	case DIR_NW:
		DrawLine(x, y, x - AmLine16, y - AmLine8, COLOR_PLAYER);
		DrawLine(x - AmLine16, y - AmLine8, x - AmLine8, y - AmLine8, COLOR_PLAYER);
		DrawLine(x - AmLine16, y - AmLine8, x - AmLine4 - AmLine8, y, COLOR_PLAYER);
		break;
	}
}

/**
 * @brief Returns the automap shape at the given coordinate.
 */
WORD GetAutomapType(int x, int y, BOOL view)
{
	WORD rv;

	if (view && x == -1 && y >= 0 && y < DMAXY && automapview[0][y]) {
		if (GetAutomapType(0, y, FALSE) & (MAPFLAG_DIRT << 8)) {
			return 0;
		} else {
			return MAPFLAG_DIRT << 8;
		}
	}

	if (view && y == -1 && x >= 0 && x < DMAXY && automapview[x][0]) {
		if (GetAutomapType(x, 0, FALSE) & (MAPFLAG_DIRT << 8)) {
			return 0;
		} else {
			return MAPFLAG_DIRT << 8;
		}
	}

	if (x < 0 || x >= DMAXX) {
		return 0;
	}
	if (y < 0 || y >= DMAXX) {
		return 0;
	}
	if (!automapview[x][y] && view) {
		return 0;
	}

	rv = automaptype[(BYTE)dungeon[x][y]];
	if (rv == 7) {
#ifdef HELLFIRE
		if ((BYTE)(GetAutomapType(x - 1, y, FALSE) >> 8) & MAPFLAG_HORZARCH) {
			if ((BYTE)(GetAutomapType(x, y - 1, FALSE) >> 8) & MAPFLAG_VERTARCH) {
#else
		if ((GetAutomapType(x - 1, y, FALSE) >> 8) & MAPFLAG_HORZARCH) {
			if ((GetAutomapType(x, y - 1, FALSE) >> 8) & MAPFLAG_VERTARCH) {
#endif
				rv = 1;
			}
		}
	}
	return rv;
}

/**
 * @brief Renders game info, such as the name of the current level, and in multi player the name of the game and the game password.
 */
void DrawAutomapText()
{
	char desc[256];
	int nextline = 20;

	if (gbMaxPlayers > 1) {
		strcat(strcpy(desc, "game: "), szPlayerName);
		PrintGameStr(8, 20, desc, COL_GOLD);
		nextline = 35;
		if (szPlayerDescript[0]) {
			strcat(strcpy(desc, "password: "), szPlayerDescript);
			PrintGameStr(8, 35, desc, COL_GOLD);
			nextline = 50;
		}
	}
	if (setlevel) {
		PrintGameStr(8, nextline, quest_level_names[(BYTE)setlvlnum], COL_GOLD);
	} else if (currlevel != 0) {
#ifdef HELLFIRE
		if (currlevel < 17 || currlevel > 20) {
			if (currlevel < 21 || currlevel > 24)
				sprintf(desc, "Level: %i", currlevel);
			else
				sprintf(desc, "Level: Crypt %i", currlevel - 20);
		} else {
			sprintf(desc, "Level: Nest %i", currlevel - 16);
		}
#else
		sprintf(desc, "Level: %i", currlevel);
#endif
		PrintGameStr(8, nextline, desc, COL_GOLD);
	}
}

/**
 * @brief Marks the given coordinate as within view on the automap.
 */
void SetAutomapView(int x, int y)
{
	WORD maptype, solid;
	int xx, yy;

	xx = (x - 16) >> 1;
	yy = (y - 16) >> 1;

	if (xx < 0 || xx >= DMAXX || yy < 0 || yy >= DMAXY) {
		return;
	}

	automapview[xx][yy] = TRUE;

	maptype = GetAutomapType(xx, yy, FALSE);
	solid = maptype & 0x4000;

	switch (maptype & MAPFLAG_TYPE) {
	case 2:
		if (solid) {
			if (GetAutomapType(xx, yy + 1, FALSE) == 0x4007)
				automapview[xx][yy + 1] = TRUE;
		} else if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000) {
			automapview[xx - 1][yy] = TRUE;
		}
		break;
	case 3:
		if (solid) {
			if (GetAutomapType(xx + 1, yy, FALSE) == 0x4007)
				automapview[xx + 1][yy] = TRUE;
		} else if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000) {
			automapview[xx][yy - 1] = TRUE;
		}
		break;
	case 4:
		if (solid) {
			if (GetAutomapType(xx, yy + 1, FALSE) == 0x4007)
				automapview[xx][yy + 1] = TRUE;
			if (GetAutomapType(xx + 1, yy, FALSE) == 0x4007)
				automapview[xx + 1][yy] = TRUE;
		} else {
			if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000)
				automapview[xx - 1][yy] = TRUE;
			if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000)
				automapview[xx][yy - 1] = TRUE;
			if (GetAutomapType(xx - 1, yy - 1, FALSE) & 0x4000)
				automapview[xx - 1][yy - 1] = TRUE;
		}
		break;
	case 5:
		if (solid) {
			if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000)
				automapview[xx][yy - 1] = TRUE;
			if (GetAutomapType(xx, yy + 1, FALSE) == 0x4007)
				automapview[xx][yy + 1] = TRUE;
		} else if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000) {
			automapview[xx - 1][yy] = TRUE;
		}
		break;
	case 6:
		if (solid) {
			if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000)
				automapview[xx - 1][yy] = TRUE;
			if (GetAutomapType(xx + 1, yy, FALSE) == 0x4007)
				automapview[xx + 1][yy] = TRUE;
		} else if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000) {
			automapview[xx][yy - 1] = TRUE;
		}
		break;
	}
}

/**
 * @brief Resets the zoom level of the automap.
 */
void AutomapZoomReset()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	AmLine64 = (AutoMapScale << 6) / 100;
	AmLine32 = AmLine64 >> 1;
	AmLine16 = AmLine32 >> 1;
	AmLine8 = AmLine16 >> 1;
	AmLine4 = AmLine8 >> 1;
}

DEVILUTION_END_NAMESPACE

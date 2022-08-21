#include <stack>

#include "levels/gendung.h"

#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "init.h"
#include "levels/drlg_l1.h"
#include "levels/drlg_l2.h"
#include "levels/drlg_l3.h"
#include "levels/drlg_l4.h"
#include "levels/town.h"
#include "lighting.h"
#include "options.h"

namespace devilution {

Bitset2d<DMAXX, DMAXY> DungeonMask;
uint8_t dungeon[DMAXX][DMAXY];
uint8_t pdungeon[DMAXX][DMAXY];
Bitset2d<DMAXX, DMAXY> Protected;
Rectangle SetPieceRoom;
Rectangle SetPiece;
std::unique_ptr<uint16_t[]> pSetPiece;
OptionalOwnedClxSpriteList pSpecialCels;
std::unique_ptr<MegaTile[]> pMegaTiles;
std::unique_ptr<byte[]> pDungeonCels;
std::array<TileProperties, MAXTILES> SOLData;
Point dminPosition;
Point dmaxPosition;
dungeon_type leveltype;
uint8_t currlevel;
bool setlevel;
_setlevels setlvlnum;
dungeon_type setlvltype;
Point ViewPosition;
int MicroTileLen;
char TransVal;
bool TransList[256];
uint16_t dPiece[MAXDUNX][MAXDUNY];
MICROS DPieceMicros[MAXTILES];
int8_t dTransVal[MAXDUNX][MAXDUNY];
char dLight[MAXDUNX][MAXDUNY];
char dPreLight[MAXDUNX][MAXDUNY];
DungeonFlag dFlags[MAXDUNX][MAXDUNY];
int8_t dPlayer[MAXDUNX][MAXDUNY];
int16_t dMonster[MAXDUNX][MAXDUNY];
int8_t dCorpse[MAXDUNX][MAXDUNY];
int8_t dObject[MAXDUNX][MAXDUNY];
char dSpecial[MAXDUNX][MAXDUNY];
int themeCount;
THEME_LOC themeLoc[MAXTHEMES];

namespace {

std::unique_ptr<uint16_t[]> LoadMinData(size_t &tileCount)
{
	switch (leveltype) {
	case DTYPE_TOWN:
		if (gbIsHellfire)
			return LoadFileInMem<uint16_t>("nlevels\\towndata\\town.min", &tileCount);
		return LoadFileInMem<uint16_t>("levels\\towndata\\town.min", &tileCount);
	case DTYPE_CATHEDRAL:
		return LoadFileInMem<uint16_t>("levels\\l1data\\l1.min", &tileCount);
	case DTYPE_CATACOMBS:
		return LoadFileInMem<uint16_t>("levels\\l2data\\l2.min", &tileCount);
	case DTYPE_CAVES:
		return LoadFileInMem<uint16_t>("levels\\l3data\\l3.min", &tileCount);
	case DTYPE_HELL:
		return LoadFileInMem<uint16_t>("levels\\l4data\\l4.min", &tileCount);
	case DTYPE_NEST:
		return LoadFileInMem<uint16_t>("nlevels\\l6data\\l6.min", &tileCount);
	case DTYPE_CRYPT:
		return LoadFileInMem<uint16_t>("nlevels\\l5data\\l5.min", &tileCount);
	default:
		app_fatal("LoadMinData");
	}
}

/**
 * @brief Starting from the origin point determine how much floor space is available with the given bounds
 *
 * Essentially looks for the widest/tallest rectangular area of at least the minimum size, but due to a weird/buggy
 * bounds check can return an area smaller than the available width/height.
 *
 * @param floor what value defines floor tiles within a dungeon
 * @param origin starting point for the search
 * @param minSize minimum allowable value for both dimensions
 * @param maxSize maximum allowable value for both dimensions
 * @return how much width/height is available for a theme room or an empty optional if there's not enough space
 */
std::optional<Size> GetSizeForThemeRoom(int floor, Point origin, int minSize, int maxSize)
{
	if (origin.x + maxSize > DMAXX && origin.y + maxSize > DMAXY) {
		return {}; // Original broken bounds check, avoids lower right corner
	}
	if (IsNearThemeRoom(origin)) {
		return {};
	}

	const int maxWidth = std::min(maxSize, DMAXX - origin.x);
	const int maxHeight = std::min(maxSize, DMAXY - origin.y);

	Size room { maxWidth, maxHeight };

	for (int i = 0; i < maxSize; i++) {
		int width = i < room.height ? i : 0;
		if (i < maxHeight) {
			while (width < room.width) {
				if (dungeon[origin.x + width][origin.y + i] != floor)
					break;

				width++;
			}
		}

		int height = i < room.width ? i : 0;
		if (i < maxWidth) {
			while (height < room.height) {
				if (dungeon[origin.x + i][origin.y + height] != floor)
					break;

				height++;
			}
		}

		if (width < minSize || height < minSize) {
			if (i < minSize)
				return {};
			break;
		}

		room = { std::min(room.width, width), std::min(room.height, height) };
	}

	return room - 2;
}

void CreateThemeRoom(int themeIndex)
{
	const int lx = themeLoc[themeIndex].room.position.x;
	const int ly = themeLoc[themeIndex].room.position.y;
	const int hx = lx + themeLoc[themeIndex].room.size.width;
	const int hy = ly + themeLoc[themeIndex].room.size.height;

	for (int yy = ly; yy < hy; yy++) {
		for (int xx = lx; xx < hx; xx++) {
			if (leveltype == DTYPE_CATACOMBS) {
				if (yy == ly || yy == hy - 1) {
					dungeon[xx][yy] = 2;
				} else if (xx == lx || xx == hx - 1) {
					dungeon[xx][yy] = 1;
				} else {
					dungeon[xx][yy] = 3;
				}
			}
			if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST)) {
				if (yy == ly || yy == hy - 1) {
					dungeon[xx][yy] = 134;
				} else if (xx == lx || xx == hx - 1) {
					dungeon[xx][yy] = 137;
				} else {
					dungeon[xx][yy] = 7;
				}
			}
			if (leveltype == DTYPE_HELL) {
				if (yy == ly || yy == hy - 1) {
					dungeon[xx][yy] = 2;
				} else if (xx == lx || xx == hx - 1) {
					dungeon[xx][yy] = 1;
				} else {
					dungeon[xx][yy] = 6;
				}
			}
		}
	}

	if (leveltype == DTYPE_CATACOMBS) {
		dungeon[lx][ly] = 8;
		dungeon[hx - 1][ly] = 7;
		dungeon[lx][hy - 1] = 9;
		dungeon[hx - 1][hy - 1] = 6;
	}
	if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST)) {
		dungeon[lx][ly] = 150;
		dungeon[hx - 1][ly] = 151;
		dungeon[lx][hy - 1] = 152;
		dungeon[hx - 1][hy - 1] = 138;
	}
	if (leveltype == DTYPE_HELL) {
		dungeon[lx][ly] = 9;
		dungeon[hx - 1][ly] = 16;
		dungeon[lx][hy - 1] = 15;
		dungeon[hx - 1][hy - 1] = 12;
	}

	if (leveltype == DTYPE_CATACOMBS) {
		if (FlipCoin())
			dungeon[hx - 1][(ly + hy) / 2] = 4;
		else
			dungeon[(lx + hx) / 2][hy - 1] = 5;
	}
	if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST)) {
		if (FlipCoin())
			dungeon[hx - 1][(ly + hy) / 2] = 147;
		else
			dungeon[(lx + hx) / 2][hy - 1] = 146;
	}
	if (leveltype == DTYPE_HELL) {
		if (FlipCoin()) {
			int yy = (ly + hy) / 2;
			dungeon[hx - 1][yy - 1] = 53;
			dungeon[hx - 1][yy] = 6;
			dungeon[hx - 1][yy + 1] = 52;
			dungeon[hx - 2][yy - 1] = 54;
		} else {
			int xx = (lx + hx) / 2;
			dungeon[xx - 1][hy - 1] = 57;
			dungeon[xx][hy - 1] = 6;
			dungeon[xx + 1][hy - 1] = 56;
			dungeon[xx][hy - 2] = 59;
			dungeon[xx - 1][hy - 2] = 58;
		}
	}
}

bool IsFloor(Point p, uint8_t floorID)
{
	int i = (p.x - 16) / 2;
	int j = (p.y - 16) / 2;
	if (i < 0 || i >= DMAXX)
		return false;
	if (j < 0 || j >= DMAXY)
		return false;
	return dungeon[i][j] == floorID;
}

void FillTransparencyValues(Point floor, uint8_t floorID)
{
	Direction allDirections[] = {
		Direction::North,
		Direction::South,
		Direction::East,
		Direction::West,
		Direction::NorthEast,
		Direction::NorthWest,
		Direction::SouthEast,
		Direction::SouthWest,
	};

	// We only fill in the surrounding tiles if they are not floor tiles
	// because they would otherwise not be visited by the span filling algorithm
	for (Direction dir : allDirections) {
		Point adjacent = floor + dir;
		if (!IsFloor(adjacent, floorID))
			dTransVal[adjacent.x][adjacent.y] = TransVal;
	}

	dTransVal[floor.x][floor.y] = TransVal;
}

void FindTransparencyValues(Point floor, uint8_t floorID)
{
	// Algorithm adapted from https://en.wikipedia.org/wiki/Flood_fill#Span_Filling
	// Modified to include diagonally adjacent tiles that would otherwise not be visited
	// Also, Wikipedia's selection for the initial seed is incorrect
	using Seed = std::tuple<int, int, int, int>;
	std::stack<Seed> seedStack;
	seedStack.push(std::make_tuple(floor.x, floor.x + 1, floor.y, 1));

	const auto isInside = [&](int x, int y) {
		if (dTransVal[x][y] != 0)
			return false;
		return IsFloor({ x, y }, floorID);
	};

	const auto set = [&](int x, int y) {
		FillTransparencyValues({ x, y }, floorID);
	};

	const Displacement left = { -1, 0 };
	const Displacement right = { 1, 0 };
	const auto checkDiagonals = [&](Point p, Displacement direction) {
		Point up = p + Displacement { 0, -1 };
		Point upOver = up + direction;
		if (!isInside(up.x, up.y) && isInside(upOver.x, upOver.y))
			seedStack.push(std::make_tuple(upOver.x, upOver.x + 1, upOver.y, -1));

		Point down = p + Displacement { 0, 1 };
		Point downOver = down + direction;
		if (!isInside(down.x, down.y) && isInside(downOver.x, downOver.y))
			seedStack.push(std::make_tuple(downOver.x, downOver.x + 1, downOver.y, 1));
	};

	while (!seedStack.empty()) {
		int scanStart, scanEnd, y, dy;
		std::tie(scanStart, scanEnd, y, dy) = seedStack.top();
		seedStack.pop();

		int scanLeft = scanStart;
		if (isInside(scanLeft, y)) {
			while (isInside(scanLeft - 1, y)) {
				set(scanLeft - 1, y);
				scanLeft--;
			}
			checkDiagonals({ scanLeft, y }, left);
		}
		if (scanLeft < scanStart)
			seedStack.push(std::make_tuple(scanLeft, scanStart - 1, y - dy, -dy));

		int scanRight = scanStart;
		while (scanRight < scanEnd) {
			while (isInside(scanRight, y)) {
				set(scanRight, y);
				scanRight++;
			}
			seedStack.push(std::make_tuple(scanLeft, scanRight - 1, y + dy, dy));
			if (scanRight - 1 > scanEnd)
				seedStack.push(std::make_tuple(scanEnd + 1, scanRight - 1, y - dy, -dy));
			if (scanLeft < scanRight)
				checkDiagonals({ scanRight - 1, y }, right);

			while (scanRight < scanEnd && !isInside(scanRight, y))
				scanRight++;
			scanLeft = scanRight;
			if (scanLeft < scanEnd)
				checkDiagonals({ scanLeft, y }, left);
		}
	}
}

void InitGlobals()
{
	memset(dFlags, 0, sizeof(dFlags));
	memset(dPlayer, 0, sizeof(dPlayer));
	memset(dMonster, 0, sizeof(dMonster));
	memset(dCorpse, 0, sizeof(dCorpse));
	memset(dItem, 0, sizeof(dItem));
	memset(dObject, 0, sizeof(dObject));
	memset(dSpecial, 0, sizeof(dSpecial));
	memset(dLight, DisableLighting || leveltype == DTYPE_TOWN ? 0 : 15, sizeof(dLight));

	DRLG_InitTrans();

	dminPosition = Point(0, 0).megaToWorld();
	dmaxPosition = Point(40, 40).megaToWorld();
	SetPieceRoom = { { -1, -1 }, { -1, -1 } };
	SetPiece = { { 0, 0 }, { 0, 0 } };
}

} // namespace

#ifdef BUILD_TESTING
std::optional<Size> GetSizeForThemeRoom()
{
	return GetSizeForThemeRoom(0, { 0, 0 }, 5, 10);
}
#endif

dungeon_type GetLevelType(int level)
{
	if (level == 0)
		return DTYPE_TOWN;
	if (level <= 4)
		return DTYPE_CATHEDRAL;
	if (level <= 8)
		return DTYPE_CATACOMBS;
	if (level <= 12)
		return DTYPE_CAVES;
	if (level <= 16)
		return DTYPE_HELL;
	if (level <= 20)
		return DTYPE_NEST;
	if (level <= 24)
		return DTYPE_CRYPT;

	return DTYPE_NONE;
}

void CreateDungeon(uint32_t rseed, lvl_entry entry)
{
	InitGlobals();

	switch (leveltype) {
	case DTYPE_TOWN:
		CreateTown(entry);
		break;
	case DTYPE_CATHEDRAL:
	case DTYPE_CRYPT:
		CreateL5Dungeon(rseed, entry);
		break;
	case DTYPE_CATACOMBS:
		CreateL2Dungeon(rseed, entry);
		break;
	case DTYPE_CAVES:
	case DTYPE_NEST:
		CreateL3Dungeon(rseed, entry);
		break;
	case DTYPE_HELL:
		CreateL4Dungeon(rseed, entry);
		break;
	default:
		app_fatal("Invalid level type");
	}

	Make_SetPC(SetPiece);
}

bool TileHasAny(int tileId, TileProperties property)
{
	return HasAnyOf(SOLData[tileId], property);
}

void LoadLevelSOLData()
{
	switch (leveltype) {
	case DTYPE_TOWN:
		if (gbIsHellfire)
			LoadFileInMem("nlevels\\towndata\\town.sol", SOLData);
		else
			LoadFileInMem("levels\\towndata\\town.sol", SOLData);
		break;
	case DTYPE_CATHEDRAL:
		LoadFileInMem("levels\\l1data\\l1.sol", SOLData);
		SOLData[27] |= TileProperties::BlockMissile; // Tile is incorrectly marked
		break;
	case DTYPE_CATACOMBS:
		LoadFileInMem("levels\\l2data\\l2.sol", SOLData);
		break;
	case DTYPE_CAVES:
		LoadFileInMem("levels\\l3data\\l3.sol", SOLData);
		break;
	case DTYPE_HELL:
		LoadFileInMem("levels\\l4data\\l4.sol", SOLData);
		SOLData[210] = TileProperties::None; // Tile is incorrectly marked as being solid
		break;
	case DTYPE_NEST:
		LoadFileInMem("nlevels\\l6data\\l6.sol", SOLData);
		break;
	case DTYPE_CRYPT:
		LoadFileInMem("nlevels\\l5data\\l5.sol", SOLData);
		break;
	default:
		app_fatal("LoadLevelSOLData");
	}
}

void SetDungeonMicros()
{
	MicroTileLen = 10;
	size_t blocks = 10;

	if (leveltype == DTYPE_TOWN) {
		MicroTileLen = 16;
		blocks = 16;
	} else if (leveltype == DTYPE_HELL) {
		MicroTileLen = 12;
		blocks = 16;
	}

	size_t tileCount;
	std::unique_ptr<uint16_t[]> levelPieces = LoadMinData(tileCount);

	for (size_t i = 0; i < tileCount / blocks; i++) {
		uint16_t *pieces = &levelPieces[blocks * i];
		for (size_t block = 0; block < blocks; block++) {
			DPieceMicros[i].mt[block] = SDL_SwapLE16(pieces[blocks - 2 + (block & 1) - (block & 0xE)]);
		}
	}
}

void DRLG_InitTrans()
{
	memset(dTransVal, 0, sizeof(dTransVal));
	memset(TransList, 0, sizeof(TransList));
	TransVal = 1;
}

void DRLG_RectTrans(Rectangle area)
{
	Point position = area.position;
	Size size = area.size;

	for (int j = position.y; j <= position.y + size.height; j++) {
		for (int i = position.x; i <= position.x + size.width; i++) {
			dTransVal[i][j] = TransVal;
		}
	}

	TransVal++;
}

void DRLG_MRectTrans(Rectangle area)
{
	Point position = area.position.megaToWorld();
	Size size = area.size * 2;

	DRLG_RectTrans({ position + Displacement { 1, 1 }, { size.width - 1, size.height - 1 } });
}

void DRLG_MRectTrans(Point origin, Point extent)
{
	DRLG_MRectTrans({ origin, { extent.x - origin.x, extent.y - origin.y } });
}

void DRLG_CopyTrans(int sx, int sy, int dx, int dy)
{
	dTransVal[dx][dy] = dTransVal[sx][sy];
}

void LoadTransparency(const uint16_t *dunData)
{
	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	int layer2Offset = 2 + width * height;

	// The rest of the layers are at dPiece scale
	width *= 2;
	height *= 2;

	const uint16_t *transparentLayer = &dunData[layer2Offset + width * height * 3];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			dTransVal[16 + i][16 + j] = SDL_SwapLE16(*transparentLayer);
			transparentLayer++;
		}
	}
}

void LoadDungeonBase(const char *path, Point spawn, int floorId, int dirtId)
{
	ViewPosition = spawn;

	InitGlobals();

	memset(dungeon, dirtId, sizeof(dungeon));

	auto dunData = LoadFileInMem<uint16_t>(path);
	PlaceDunTiles(dunData.get(), { 0, 0 }, floorId);
	LoadTransparency(dunData.get());

	SetMapMonsters(dunData.get(), Point(0, 0).megaToWorld());
	SetMapObjects(dunData.get(), 0, 0);
}

void Make_SetPC(Rectangle area)
{
	Point position = area.position.megaToWorld();
	Size size = area.size * 2;

	for (int j = 0; j < size.height; j++) {
		for (int i = 0; i < size.width; i++) {
			dFlags[position.x + i][position.y + j] |= DungeonFlag::Populated;
		}
	}
}

std::optional<Point> PlaceMiniSet(const Miniset &miniset, int tries, bool drlg1Quirk)
{
	int sw = miniset.size.width;
	int sh = miniset.size.height;
	Point position { GenerateRnd(DMAXX - sw), GenerateRnd(DMAXY - sh) };

	for (int i = 0; i < tries; i++, position.x++) {
		if (position.x == DMAXX - sw) {
			position.x = 0;
			position.y++;
			if (position.y == DMAXY - sh) {
				position.y = 0;
			}
		}

		// Limit the position of SetPieces for compatibility with Diablo bug
		if (drlg1Quirk) {
			bool valid = true;
			if (position.x <= 12) {
				position.x++;
				valid = false;
			}
			if (position.y <= 12) {
				position.y++;
				valid = false;
			}
			if (!valid) {
				continue;
			}
		}

		if (SetPieceRoom.contains(position))
			continue;
		if (!miniset.matches(position))
			continue;

		miniset.place(position);

		return position;
	}

	return {};
}

void PlaceDunTiles(const uint16_t *dunData, Point position, int floorId)
{
	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			auto tileId = static_cast<uint8_t>(SDL_SwapLE16(tileLayer[j * width + i]));
			if (tileId != 0) {
				dungeon[position.x + i][position.y + j] = tileId;
				Protected.set(position.x + i, position.y + j);
			} else if (floorId != 0) {
				dungeon[position.x + i][position.y + j] = floorId;
			}
		}
	}
}

void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, bool rndSize)
{
	themeCount = 0;
	memset(themeLoc, 0, sizeof(*themeLoc));
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == floor && FlipCoin(freq)) {
				std::optional<Size> themeSize = GetSizeForThemeRoom(floor, { i, j }, minSize, maxSize);

				if (!themeSize)
					continue;

				if (rndSize) {
					int min = minSize - 2;
					int max = maxSize - 2;
					themeSize->width = min + GenerateRnd(GenerateRnd(themeSize->width - min + 1));
					if (themeSize->width < min || themeSize->width > max)
						themeSize->width = min;
					themeSize->height = min + GenerateRnd(GenerateRnd(themeSize->height - min + 1));
					if (themeSize->height < min || themeSize->height > max)
						themeSize->height = min;
				}

				THEME_LOC &theme = themeLoc[themeCount];
				theme.room = { Point { i, j } + Direction::South, *themeSize };
				if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST)) {
					DRLG_RectTrans({ (theme.room.position + Direction::South).megaToWorld(), theme.room.size * 2 - 5 });
				} else {
					DRLG_MRectTrans({ theme.room.position, theme.room.size - 1 });
				}
				theme.ttval = TransVal - 1;
				CreateThemeRoom(themeCount);
				themeCount++;
			}
		}
	}
} // namespace

void DRLG_HoldThemeRooms()
{
	for (int i = 0; i < themeCount; i++) {
		for (int y = themeLoc[i].room.position.y; y < themeLoc[i].room.position.y + themeLoc[i].room.size.height - 1; y++) {
			for (int x = themeLoc[i].room.position.x; x < themeLoc[i].room.position.x + themeLoc[i].room.size.width - 1; x++) {
				int xx = 2 * x + 16;
				int yy = 2 * y + 16;
				dFlags[xx][yy] |= DungeonFlag::Populated;
				dFlags[xx + 1][yy] |= DungeonFlag::Populated;
				dFlags[xx][yy + 1] |= DungeonFlag::Populated;
				dFlags[xx + 1][yy + 1] |= DungeonFlag::Populated;
			}
		}
	}
}

void SetSetPieceRoom(Point position, int floorId)
{
	if (pSetPiece == nullptr)
		return;

	PlaceDunTiles(pSetPiece.get(), position, floorId);
	SetPiece = { position, { SDL_SwapLE16(pSetPiece[0]), SDL_SwapLE16(pSetPiece[1]) } };
}

void FreeQuestSetPieces()
{
	pSetPiece = nullptr;
}

void DRLG_LPass3(int lv)
{
	{
		MegaTile mega = pMegaTiles[lv];
		int v1 = SDL_SwapLE16(mega.micro1);
		int v2 = SDL_SwapLE16(mega.micro2);
		int v3 = SDL_SwapLE16(mega.micro3);
		int v4 = SDL_SwapLE16(mega.micro4);

		for (int j = 0; j < MAXDUNY; j += 2) {
			for (int i = 0; i < MAXDUNX; i += 2) {
				dPiece[i + 0][j + 0] = v1;
				dPiece[i + 1][j + 0] = v2;
				dPiece[i + 0][j + 1] = v3;
				dPiece[i + 1][j + 1] = v4;
			}
		}
	}

	int yy = 16;
	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			int tileId = dungeon[i][j] - 1;
			MegaTile mega = pMegaTiles[tileId];
			dPiece[xx + 0][yy + 0] = SDL_SwapLE16(mega.micro1);
			dPiece[xx + 1][yy + 0] = SDL_SwapLE16(mega.micro2);
			dPiece[xx + 0][yy + 1] = SDL_SwapLE16(mega.micro3);
			dPiece[xx + 1][yy + 1] = SDL_SwapLE16(mega.micro4);
			xx += 2;
		}
		yy += 2;
	}
}

bool IsNearThemeRoom(Point testPosition)
{
	for (int i = 0; i < themeCount; i++) {
		if (Rectangle(themeLoc[i].room.position - Displacement { 2 }, themeLoc[i].room.size + 5).contains(testPosition))
			return true;
	}

	return false;
}

void InitLevels()
{
	currlevel = 0;
	leveltype = DTYPE_TOWN;
	setlevel = false;
}

void FloodTransparencyValues(uint8_t floorID)
{
	int yy = 16;
	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == floorID && dTransVal[xx][yy] == 0) {
				FindTransparencyValues({ xx, yy }, floorID);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

} // namespace devilution

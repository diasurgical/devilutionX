#include "test_utils.h"

#include "../types.h"

void test_init_common()
{
	init_archives();

	InitHash();
	diablo_init_screen();

	InitLightTable();

	gdwNormalMsgSize = 512;

	gbMaxPlayers = 1;

	light4flag = 0; // VALGRIND
}

void test_set_seeds(int baseSeed)
{
	SetRndSeed(baseSeed);
	for (int i = 0; i < 17; i++) {
		glSeedTbl[i] = GetRndSeed();
		gnLevelTypeTbl[i] = InitNewSeed(i);
	}
}

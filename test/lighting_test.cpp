#include <gtest/gtest.h>

#include "control.h"
#include "lighting.h"

using namespace devilution;

TEST(Lighting, CrawlTables)
{
	bool added[40][40];
	memset(added, 0, sizeof(added));

	for (int j = 0; j < (int)CrawlTable.size(); j++) {
		int x = 20;
		int y = 20;
		for (unsigned i = 0; i < CrawlTable[j].size(); i++) {
			int dx = x + CrawlTable[j][i].deltaX;
			int dy = y + CrawlTable[j][i].deltaY;
			EXPECT_EQ(added[dx][dy], false) << "location " << i << ":" << j << " added twice";
			added[dx][dy] = true;
		}
	}

	for (int i = -18; i <= 18; i++) {
		for (int j = -18; j <= 18; j++) {
			if (added[i + 20][j + 20])
				continue;
			if ((i == -18 && j == -18) || (i == -18 && j == 18) || (i == 18 && j == -18) || (i == 18 && j == 18))
				continue; // Limit of the crawl table rage
			EXPECT_EQ(false, true) << "while checking location " << i << ":" << j;
		}
	}
}

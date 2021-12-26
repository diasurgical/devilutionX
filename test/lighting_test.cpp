#include <gtest/gtest.h>

#include "control.h"
#include "lighting.h"

using namespace devilution;

TEST(Lighting, CrawlTables)
{
	bool added[40][40];
	memset(added, 0, sizeof(added));

	for (int j = 0; j < 19; j++) {
		int x = 20;
		int y = 20;
		int cr = CrawlNum[j] + 1;
		for (unsigned i = (uint8_t)CrawlTable[cr - 1]; i > 0; i--, cr += 2) {
			int dx = x + CrawlTable[cr];
			int dy = y + CrawlTable[cr + 1];
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

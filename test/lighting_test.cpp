#include <gtest/gtest.h>

#include "control.h"
#include "lighting.h"

using namespace devilution;

TEST(Lighting, CrawlTables)
{
	bool added[40][40];
	memset(added, 0, sizeof(added));

	int x = 20;
	int y = 20;

	Crawl(0, MaxCrawlRadius, [&](Displacement displacement) {
		int dx = x + displacement.deltaX;
		int dy = y + displacement.deltaY;
		EXPECT_EQ(added[dx][dy], false) << "displacement " << displacement.deltaX << ":" << displacement.deltaY << " added twice";
		added[dx][dy] = true;
		return false;
	});

	for (int i = -MaxCrawlRadius; i <= MaxCrawlRadius; i++) {
		for (int j = -MaxCrawlRadius; j <= MaxCrawlRadius; j++) {
			if (added[i + 20][j + 20])
				continue;
			if (abs(i) == MaxCrawlRadius && abs(j) == MaxCrawlRadius)
				continue; // Limit of the crawl table rage
			EXPECT_EQ(false, true) << "while checking location " << i << ":" << j;
		}
	}
}

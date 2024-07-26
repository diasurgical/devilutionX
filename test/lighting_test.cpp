#include <cmath>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "control.h"
#include "lighting.h"

namespace devilution {
namespace {
using ::testing::ElementsAre;

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
			if (std::abs(i) == MaxCrawlRadius && std::abs(j) == MaxCrawlRadius)
				continue; // Limit of the crawl table rage
			EXPECT_EQ(false, true) << "while checking location " << i << ":" << j;
		}
	}
}

TEST(Lighting, CrawlTablesVisitationOrder)
{
	std::vector<Displacement> order;
	Crawl(0, 2, [&](Displacement displacement) {
		order.push_back(displacement);
		return false;
	});
	EXPECT_THAT(
	    order,
	    ElementsAre(
	        Displacement(0, 0),
	        Displacement(0, 1), Displacement(0, -1),
	        Displacement(-1, 0), Displacement(1, 0),
	        Displacement(0, 2), Displacement(0, -2),
	        Displacement(-1, 2), Displacement(1, 2), Displacement(-1, -2), Displacement(1, -2),
	        Displacement(-1, 1), Displacement(1, 1), Displacement(-1, -1), Displacement(1, -1),
	        Displacement(-2, 0), Displacement(2, 0), Displacement(-2, 1),
	        Displacement(2, 1), Displacement(-2, -1), Displacement(2, -1)));
}

} // namespace
} // namespace devilution

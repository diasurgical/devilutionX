#include <gtest/gtest.h>

#include "automap.h"

using namespace devilution;

TEST(Automap, InitAutomap)
{
	InitAutomapOnce();
	EXPECT_EQ(AutomapActive, false);
	EXPECT_EQ(AutoMapScale, 50);
	EXPECT_EQ(AmLine64, 32);
	EXPECT_EQ(AmLine32, 16);
	EXPECT_EQ(AmLine16, 8);
	EXPECT_EQ(AmLine8, 4);
	EXPECT_EQ(AmLine4, 2);
}

TEST(Automap, StartAutomap)
{
	StartAutomap();
	EXPECT_EQ(AutomapOffset.x, 0);
	EXPECT_EQ(AutomapOffset.y, 0);
	EXPECT_EQ(AutomapActive, true);
}

TEST(Automap, AutomapUp)
{
	AutomapOffset.x = 1;
	AutomapOffset.y = 1;
	AutomapUp();
	EXPECT_EQ(AutomapOffset.x, 0);
	EXPECT_EQ(AutomapOffset.y, 0);
}

TEST(Automap, AutomapDown)
{
	AutomapOffset.x = 1;
	AutomapOffset.y = 1;
	AutomapDown();
	EXPECT_EQ(AutomapOffset.x, 2);
	EXPECT_EQ(AutomapOffset.y, 2);
}

TEST(Automap, AutomapLeft)
{
	AutomapOffset.x = 1;
	AutomapOffset.y = 1;
	AutomapLeft();
	EXPECT_EQ(AutomapOffset.x, 0);
	EXPECT_EQ(AutomapOffset.y, 2);
}

TEST(Automap, AutomapRight)
{
	AutomapOffset.x = 1;
	AutomapOffset.y = 1;
	AutomapRight();
	EXPECT_EQ(AutomapOffset.x, 2);
	EXPECT_EQ(AutomapOffset.y, 0);
}

TEST(Automap, AutomapZoomIn)
{
	AutoMapScale = 50;
	AutomapZoomIn();
	EXPECT_EQ(AutoMapScale, 55);
	EXPECT_EQ(AmLine64, 35);
	EXPECT_EQ(AmLine32, 17);
	EXPECT_EQ(AmLine16, 8);
	EXPECT_EQ(AmLine8, 4);
	EXPECT_EQ(AmLine4, 2);
}

TEST(Automap, AutomapZoomIn_Max)
{
	AutoMapScale = 195;
	AutomapZoomIn();
	AutomapZoomIn();
	EXPECT_EQ(AutoMapScale, 200);
	EXPECT_EQ(AmLine64, 128);
	EXPECT_EQ(AmLine32, 64);
	EXPECT_EQ(AmLine16, 32);
	EXPECT_EQ(AmLine8, 16);
	EXPECT_EQ(AmLine4, 8);
}

TEST(Automap, AutomapZoomOut)
{
	AutoMapScale = 200;
	AutomapZoomOut();
	EXPECT_EQ(AutoMapScale, 195);
	EXPECT_EQ(AmLine64, 124);
	EXPECT_EQ(AmLine32, 62);
	EXPECT_EQ(AmLine16, 31);
	EXPECT_EQ(AmLine8, 15);
	EXPECT_EQ(AmLine4, 7);
}

TEST(Automap, AutomapZoomOut_Min)
{
	AutoMapScale = 55;
	AutomapZoomOut();
	AutomapZoomOut();
	EXPECT_EQ(AutoMapScale, 50);
	EXPECT_EQ(AmLine64, 32);
	EXPECT_EQ(AmLine32, 16);
	EXPECT_EQ(AmLine16, 8);
	EXPECT_EQ(AmLine8, 4);
	EXPECT_EQ(AmLine4, 2);
}

TEST(Automap, AutomapZoomReset)
{
	AutoMapScale = 50;
	AutomapOffset.x = 1;
	AutomapOffset.y = 1;
	AutomapZoomReset();
	EXPECT_EQ(AutomapOffset.x, 0);
	EXPECT_EQ(AutomapOffset.y, 0);
	EXPECT_EQ(AutoMapScale, 50);
	EXPECT_EQ(AmLine64, 32);
	EXPECT_EQ(AmLine32, 16);
	EXPECT_EQ(AmLine16, 8);
	EXPECT_EQ(AmLine8, 4);
	EXPECT_EQ(AmLine4, 2);
}

#include <gtest/gtest.h>

#include "automap.h"

using namespace devilution;

TEST(Automap, InitAutomap)
{
	InitAutomapOnce();
	EXPECT_EQ(AutomapActive, false);
	EXPECT_EQ(AutoMapScale, 50);
	EXPECT_EQ(AmLine(64), 32);
	EXPECT_EQ(AmLine(32), 16);
	EXPECT_EQ(AmLine(16), 8);
	EXPECT_EQ(AmLine(8), 4);
	EXPECT_EQ(AmLine(4), 2);
}

TEST(Automap, StartAutomap)
{
	StartAutomap();
	EXPECT_EQ(AutomapOffset.deltaX, 0);
	EXPECT_EQ(AutomapOffset.deltaY, 0);
	EXPECT_EQ(AutomapActive, true);
}

TEST(Automap, AutomapUp)
{
	AutomapOffset.deltaX = 1;
	AutomapOffset.deltaY = 1;
	AutomapUp();
	EXPECT_EQ(AutomapOffset.deltaX, 0);
	EXPECT_EQ(AutomapOffset.deltaY, 0);
}

TEST(Automap, AutomapDown)
{
	AutomapOffset.deltaX = 1;
	AutomapOffset.deltaY = 1;
	AutomapDown();
	EXPECT_EQ(AutomapOffset.deltaX, 2);
	EXPECT_EQ(AutomapOffset.deltaY, 2);
}

TEST(Automap, AutomapLeft)
{
	AutomapOffset.deltaX = 1;
	AutomapOffset.deltaY = 1;
	AutomapLeft();
	EXPECT_EQ(AutomapOffset.deltaX, 0);
	EXPECT_EQ(AutomapOffset.deltaY, 2);
}

TEST(Automap, AutomapRight)
{
	AutomapOffset.deltaX = 1;
	AutomapOffset.deltaY = 1;
	AutomapRight();
	EXPECT_EQ(AutomapOffset.deltaX, 2);
	EXPECT_EQ(AutomapOffset.deltaY, 0);
}

TEST(Automap, AutomapZoomIn)
{
	AutoMapScale = 50;
	AutomapZoomIn();
	EXPECT_EQ(AutoMapScale, 75);
	EXPECT_EQ(AmLine(64), 48);
	EXPECT_EQ(AmLine(32), 24);
	EXPECT_EQ(AmLine(16), 12);
	EXPECT_EQ(AmLine(8), 6);
	EXPECT_EQ(AmLine(4), 3);
}

TEST(Automap, AutomapZoomIn_Max)
{
	AutoMapScale = 175;
	AutomapZoomIn();
	AutomapZoomIn();
	EXPECT_EQ(AutoMapScale, 200);
	EXPECT_EQ(AmLine(64), 128);
	EXPECT_EQ(AmLine(32), 64);
	EXPECT_EQ(AmLine(16), 32);
	EXPECT_EQ(AmLine(8), 16);
	EXPECT_EQ(AmLine(4), 8);
}

TEST(Automap, AutomapZoomOut)
{
	AutoMapScale = 200;
	AutomapZoomOut();
	EXPECT_EQ(AutoMapScale, 175);
	EXPECT_EQ(AmLine(64), 112);
	EXPECT_EQ(AmLine(32), 56);
	EXPECT_EQ(AmLine(16), 28);
	EXPECT_EQ(AmLine(8), 14);
	EXPECT_EQ(AmLine(4), 7);
}

TEST(Automap, AutomapZoomOut_Min)
{
	AutoMapScale = 50;
	AutomapZoomOut();
	AutomapZoomOut();
	EXPECT_EQ(AutoMapScale, 25);
	EXPECT_EQ(AmLine(64), 16);
	EXPECT_EQ(AmLine(32), 8);
	EXPECT_EQ(AmLine(16), 4);
	EXPECT_EQ(AmLine(8), 2);
	EXPECT_EQ(AmLine(4), 1);
}

TEST(Automap, AutomapZoomReset)
{
	AutoMapScale = 50;
	AutomapOffset.deltaX = 1;
	AutomapOffset.deltaY = 1;
	AutomapZoomReset();
	EXPECT_EQ(AutomapOffset.deltaX, 0);
	EXPECT_EQ(AutomapOffset.deltaY, 0);
	EXPECT_EQ(AutoMapScale, 50);
	EXPECT_EQ(AmLine(64), 32);
	EXPECT_EQ(AmLine(32), 16);
	EXPECT_EQ(AmLine(16), 8);
	EXPECT_EQ(AmLine(8), 4);
	EXPECT_EQ(AmLine(4), 2);
}

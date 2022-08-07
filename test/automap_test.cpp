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
	EXPECT_EQ(AutoMapScale, 55);
	EXPECT_EQ(AmLine(64), 35);
	EXPECT_EQ(AmLine(32), 17);
	EXPECT_EQ(AmLine(16), 8);
	EXPECT_EQ(AmLine(8), 4);
	EXPECT_EQ(AmLine(4), 2);
}

TEST(Automap, AutomapZoomIn_Max)
{
	AutoMapScale = 195;
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
	EXPECT_EQ(AutoMapScale, 195);
	EXPECT_EQ(AmLine(64), 124);
	EXPECT_EQ(AmLine(32), 62);
	EXPECT_EQ(AmLine(16), 31);
	EXPECT_EQ(AmLine(8), 15);
	EXPECT_EQ(AmLine(4), 7);
}

TEST(Automap, AutomapZoomOut_Min)
{
	AutoMapScale = 55;
	AutomapZoomOut();
	AutomapZoomOut();
	EXPECT_EQ(AutoMapScale, 50);
	EXPECT_EQ(AmLine(64), 32);
	EXPECT_EQ(AmLine(32), 16);
	EXPECT_EQ(AmLine(16), 8);
	EXPECT_EQ(AmLine(8), 4);
	EXPECT_EQ(AmLine(4), 2);
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

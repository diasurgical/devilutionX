#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Automap, InitAutomap)
{
	InitAutomapOnce();
	EXPECT_EQ(automapflag, false);
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
	EXPECT_EQ(AutoMapXOfs, 0);
	EXPECT_EQ(AutoMapYOfs, 0);
	EXPECT_EQ(automapflag, true);
}

TEST(Automap, AutomapUp)
{
	AutoMapXOfs = 1;
	AutoMapYOfs = 1;
	AutomapUp();
	EXPECT_EQ(AutoMapXOfs, 0);
	EXPECT_EQ(AutoMapYOfs, 0);
}

TEST(Automap, AutomapDown)
{
	AutoMapXOfs = 1;
	AutoMapYOfs = 1;
	AutomapDown();
	EXPECT_EQ(AutoMapXOfs, 2);
	EXPECT_EQ(AutoMapYOfs, 2);
}

TEST(Automap, AutomapLeft)
{
	AutoMapXOfs = 1;
	AutoMapYOfs = 1;
	AutomapLeft();
	EXPECT_EQ(AutoMapXOfs, 0);
	EXPECT_EQ(AutoMapYOfs, 2);
}

TEST(Automap, AutomapRight)
{
	AutoMapXOfs = 1;
	AutoMapYOfs = 1;
	AutomapRight();
	EXPECT_EQ(AutoMapXOfs, 2);
	EXPECT_EQ(AutoMapYOfs, 0);
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
	AutoMapXOfs = 1;
	AutoMapYOfs = 1;
	AutomapZoomReset();
	EXPECT_EQ(AutoMapXOfs, 0);
	EXPECT_EQ(AutoMapYOfs, 0);
	EXPECT_EQ(AutoMapScale, 50);
	EXPECT_EQ(AmLine64, 32);
	EXPECT_EQ(AmLine32, 16);
	EXPECT_EQ(AmLine16, 8);
	EXPECT_EQ(AmLine8, 4);
	EXPECT_EQ(AmLine4, 2);
}

#include <gtest/gtest.h>
#include "all.h"

TEST(Automap, InitAutomap) {
	dvl::InitAutomapOnce();
	EXPECT_EQ(dvl::automapflag, false);
	EXPECT_EQ(dvl::AutoMapScale, 50);
	EXPECT_EQ(dvl::AmLine64, 32);
	EXPECT_EQ(dvl::AmLine32, 16);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}

TEST(Automap, StartAutomap) {
	dvl::StartAutomap();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
	EXPECT_EQ(dvl::automapflag, true);
}

TEST(Automap, AutomapUp) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapUp();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
}

TEST(Automap, AutomapDown) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapDown();
	EXPECT_EQ(dvl::AutoMapXOfs, 2);
	EXPECT_EQ(dvl::AutoMapYOfs, 2);
}

TEST(Automap, AutomapLeft) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapLeft();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 2);
}

TEST(Automap, AutomapRight) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapRight();
	EXPECT_EQ(dvl::AutoMapXOfs, 2);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
}

TEST(Automap, AutomapZoomIn) {
	dvl::AutoMapScale = 50;
	dvl::AutomapZoomIn();
	EXPECT_EQ(dvl::AutoMapScale, 55);
	EXPECT_EQ(dvl::AmLine64, 35);
	EXPECT_EQ(dvl::AmLine32, 17);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}

TEST(Automap, AutomapZoomIn_Max) {
	dvl::AutoMapScale = 195;
	dvl::AutomapZoomIn();
	dvl::AutomapZoomIn();
	EXPECT_EQ(dvl::AutoMapScale, 200);
	EXPECT_EQ(dvl::AmLine64, 128);
	EXPECT_EQ(dvl::AmLine32, 64);
	EXPECT_EQ(dvl::AmLine16, 32);
	EXPECT_EQ(dvl::AmLine8, 16);
	EXPECT_EQ(dvl::AmLine4, 8);
}

TEST(Automap, AutomapZoomOut) {
	dvl::AutoMapScale = 200;
	dvl::AutomapZoomOut();
	EXPECT_EQ(dvl::AutoMapScale, 195);
	EXPECT_EQ(dvl::AmLine64, 124);
	EXPECT_EQ(dvl::AmLine32, 62);
	EXPECT_EQ(dvl::AmLine16, 31);
	EXPECT_EQ(dvl::AmLine8, 15);
	EXPECT_EQ(dvl::AmLine4, 7);
}

TEST(Automap, AutomapZoomOut_Min) {
	dvl::AutoMapScale = 55;
	dvl::AutomapZoomOut();
	dvl::AutomapZoomOut();
	EXPECT_EQ(dvl::AutoMapScale, 50);
	EXPECT_EQ(dvl::AmLine64, 32);
	EXPECT_EQ(dvl::AmLine32, 16);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}

TEST(Automap, AutomapZoomReset) {
	dvl::AutoMapScale = 50;
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapZoomReset();
	EXPECT_EQ(dvl::AutoMapXOfs, 0);
	EXPECT_EQ(dvl::AutoMapYOfs, 0);
	EXPECT_EQ(dvl::AutoMapScale, 50);
	EXPECT_EQ(dvl::AmLine64, 32);
	EXPECT_EQ(dvl::AmLine32, 16);
	EXPECT_EQ(dvl::AmLine16, 8);
	EXPECT_EQ(dvl::AmLine8, 4);
	EXPECT_EQ(dvl::AmLine4, 2);
}

#include <gtest/gtest.h>
#include "devilution.h"

TEST(Automap, InitAutomap) {
	dvl::InitAutomapOnce();
	ASSERT_EQ(dvl::automapflag, false);
	ASSERT_EQ(dvl::AutoMapScale, 50);
	ASSERT_EQ(dvl::AutoMapPosBits, 32);
	ASSERT_EQ(dvl::AutoMapXPos, 16);
	ASSERT_EQ(dvl::AutoMapYPos, 8);
	ASSERT_EQ(dvl::AMPlayerX, 4);
	ASSERT_EQ(dvl::AMPlayerY, 2);
}

TEST(Automap, StartAutomap) {
	dvl::StartAutomap();
	ASSERT_EQ(dvl::AutoMapXOfs, 0);
	ASSERT_EQ(dvl::AutoMapYOfs, 0);
	ASSERT_EQ(dvl::automapflag, true);
}

TEST(Automap, AutomapUp) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapUp();
	ASSERT_EQ(dvl::AutoMapXOfs, 0);
	ASSERT_EQ(dvl::AutoMapYOfs, 0);
}

TEST(Automap, AutomapDown) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapDown();
	ASSERT_EQ(dvl::AutoMapXOfs, 2);
	ASSERT_EQ(dvl::AutoMapYOfs, 2);
}

TEST(Automap, AutomapLeft) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapLeft();
	ASSERT_EQ(dvl::AutoMapXOfs, 0);
	ASSERT_EQ(dvl::AutoMapYOfs, 2);
}

TEST(Automap, AutomapRight) {
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapRight();
	ASSERT_EQ(dvl::AutoMapXOfs, 2);
	ASSERT_EQ(dvl::AutoMapYOfs, 0);
}

TEST(Automap, AutomapZoomIn) {
	dvl::AutoMapScale = 50;
	dvl::AutomapZoomIn();
	ASSERT_EQ(dvl::AutoMapScale, 55);
	ASSERT_EQ(dvl::AutoMapPosBits, 35);
	ASSERT_EQ(dvl::AutoMapXPos, 17);
	ASSERT_EQ(dvl::AutoMapYPos, 8);
	ASSERT_EQ(dvl::AMPlayerX, 4);
	ASSERT_EQ(dvl::AMPlayerY, 2);
}

TEST(Automap, AutomapZoomIn_Max) {
	dvl::AutoMapScale = 195;
	dvl::AutomapZoomIn();
	dvl::AutomapZoomIn();
	ASSERT_EQ(dvl::AutoMapScale, 200);
	ASSERT_EQ(dvl::AutoMapPosBits, 128);
	ASSERT_EQ(dvl::AutoMapXPos, 64);
	ASSERT_EQ(dvl::AutoMapYPos, 32);
	ASSERT_EQ(dvl::AMPlayerX, 16);
	ASSERT_EQ(dvl::AMPlayerY, 8);
}

TEST(Automap, AutomapZoomOut) {
	dvl::AutoMapScale = 200;
	dvl::AutomapZoomOut();
	ASSERT_EQ(dvl::AutoMapScale, 195);
	ASSERT_EQ(dvl::AutoMapPosBits, 124);
	ASSERT_EQ(dvl::AutoMapXPos, 62);
	ASSERT_EQ(dvl::AutoMapYPos, 31);
	ASSERT_EQ(dvl::AMPlayerX, 15);
	ASSERT_EQ(dvl::AMPlayerY, 7);
}

TEST(Automap, AutomapZoomOut_Min) {
	dvl::AutoMapScale = 55;
	dvl::AutomapZoomOut();
	dvl::AutomapZoomOut();
	ASSERT_EQ(dvl::AutoMapScale, 50);
	ASSERT_EQ(dvl::AutoMapPosBits, 32);
	ASSERT_EQ(dvl::AutoMapXPos, 16);
	ASSERT_EQ(dvl::AutoMapYPos, 8);
	ASSERT_EQ(dvl::AMPlayerX, 4);
	ASSERT_EQ(dvl::AMPlayerY, 2);
}

TEST(Automap, AutomapZoomReset) {
	dvl::AutoMapScale = 50;
	dvl::AutoMapXOfs = 1;
	dvl::AutoMapYOfs = 1;
	dvl::AutomapZoomReset();
	ASSERT_EQ(dvl::AutoMapXOfs, 0);
	ASSERT_EQ(dvl::AutoMapYOfs, 0);
	ASSERT_EQ(dvl::AutoMapScale, 50);
	ASSERT_EQ(dvl::AutoMapPosBits, 32);
	ASSERT_EQ(dvl::AutoMapXPos, 16);
	ASSERT_EQ(dvl::AutoMapYPos, 8);
	ASSERT_EQ(dvl::AMPlayerX, 4);
	ASSERT_EQ(dvl::AMPlayerY, 2);
}

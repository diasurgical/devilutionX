#include <gtest/gtest.h>
#include "all.h"
#include "ui_fwd.h"

// TilesInView

TEST(Scrool_rt, calc_tiles_in_view_original)
{
	dvl::gnScreenWidth = 640;
	dvl::gnScreenHeight = 480;
	dvl::gnViewportHeight = dvl::gnScreenHeight - 128;
	dvl::zoomflag = true;
	int columns = 0;
	int rows = 0;
	dvl::TilesInView(&columns, &rows);
	EXPECT_EQ(columns, 10);
	EXPECT_EQ(rows, 11);
}

TEST(Scrool_rt, calc_tiles_in_view_original_zoom)
{
	dvl::gnScreenWidth = 640;
	dvl::gnScreenHeight = 480;
	dvl::gnViewportHeight = dvl::gnScreenHeight - 128;
	dvl::zoomflag = false;
	int columns = 0;
	int rows = 0;
	dvl::TilesInView(&columns, &rows);
	EXPECT_EQ(columns, 5);
	EXPECT_EQ(rows, 6);
}

TEST(Scrool_rt, calc_tiles_in_view_960_540)
{
	dvl::gnScreenWidth = 960;
	dvl::gnScreenHeight = 540;
	dvl::gnViewportHeight = dvl::gnScreenHeight;
	dvl::zoomflag = true;
	int columns = 0;
	int rows = 0;
	dvl::TilesInView(&columns, &rows);
	EXPECT_EQ(columns, 15);
	EXPECT_EQ(rows, 17);
}

TEST(Scrool_rt, calc_tiles_in_view_640_512)
{
	dvl::gnScreenWidth = 640;
	dvl::gnScreenHeight = 512;
	dvl::gnViewportHeight = dvl::gnScreenHeight - 128;
	dvl::zoomflag = true;
	int columns = 0;
	int rows = 0;
	dvl::TilesInView(&columns, &rows);
	EXPECT_EQ(columns, 10);
	EXPECT_EQ(rows, 12);
}

TEST(Scrool_rt, calc_tiles_in_view_768_480_zoom)
{
	dvl::gnScreenWidth = 768;
	dvl::gnScreenHeight = 480;
	dvl::gnViewportHeight = dvl::gnScreenHeight;
	dvl::zoomflag = false;
	int columns = 0;
	int rows = 0;
	dvl::TilesInView(&columns, &rows);
	EXPECT_EQ(columns, 6);
	EXPECT_EQ(rows, 8);
}

// CalcTileOffset

TEST(Scrool_rt, calc_tile_offset_original)
{
	dvl::gnScreenWidth = 640;
	dvl::gnScreenHeight = 480;
	dvl::gnViewportHeight = dvl::gnScreenHeight - 128;
	dvl::zoomflag = true;
	int x = 0;
	int y = 0;
	dvl::CalcTileOffset(&x, &y);
	EXPECT_EQ(x, 0);
	EXPECT_EQ(y, 0);
}

TEST(Scrool_rt, calc_tile_offset_original_zoom)
{
	dvl::gnScreenWidth = 640;
	dvl::gnScreenHeight = 480;
	dvl::gnViewportHeight = dvl::gnScreenHeight - 128;
	dvl::zoomflag = false;
	int x = 0;
	int y = 0;
	dvl::CalcTileOffset(&x, &y);
	EXPECT_EQ(x, 0);
	EXPECT_EQ(y, 8);
}

TEST(Scrool_rt, calc_tile_offset_960_540)
{
	dvl::gnScreenWidth = 960;
	dvl::gnScreenHeight = 540;
	dvl::gnViewportHeight = dvl::gnScreenHeight;
	dvl::zoomflag = true;
	int x = 0;
	int y = 0;
	dvl::CalcTileOffset(&x, &y);
	EXPECT_EQ(x, 0);
	EXPECT_EQ(y, 2);
}

TEST(Scrool_rt, calc_tile_offset_853_480)
{
	dvl::gnScreenWidth = 853;
	dvl::gnScreenHeight = 480;
	dvl::gnViewportHeight = dvl::gnScreenHeight;
	dvl::zoomflag = true;
	int x = 0;
	int y = 0;
	dvl::CalcTileOffset(&x, &y);
	EXPECT_EQ(x, 21);
	EXPECT_EQ(y, 0);
}

TEST(Scrool_rt, calc_tile_offset_768_480_zoom)
{
	dvl::gnScreenWidth = 768;
	dvl::gnScreenHeight = 480;
	dvl::gnViewportHeight = dvl::gnScreenHeight;
	dvl::zoomflag = false;
	int x = 0;
	int y = 0;
	dvl::CalcTileOffset(&x, &y);
	EXPECT_EQ(x, 0);
	EXPECT_EQ(y, 8);
}

// RowsCoveredByPanel

TEST(Scrool_rt, calc_tiles_covered_by_panel_original)
{
	dvl::gnScreenWidth = 640;
	dvl::zoomflag = true;
	EXPECT_EQ(dvl::RowsCoveredByPanel(), 0);
}

TEST(Scrool_rt, calc_tiles_covered_by_panel_960)
{
	dvl::gnScreenWidth = 960;
	dvl::zoomflag = true;
	EXPECT_EQ(dvl::RowsCoveredByPanel(), 4);
}

TEST(Scrool_rt, calc_tiles_covered_by_panel_960_zoom)
{
	dvl::gnScreenWidth = 960;
	dvl::zoomflag = false;
	EXPECT_EQ(dvl::RowsCoveredByPanel(), 2);
}

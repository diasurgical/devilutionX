#include <gtest/gtest.h>

#include "appfat.h"
#include "diablo.h"

using namespace devilution;

TEST(AppfatTest, app_fatal)
{
	EXPECT_EXIT(app_fatal("test"), ::testing::ExitedWithCode(1), "test");
}

TEST(AppfatTest, ErrDlg)
{
	EXPECT_EXIT(ErrDlg("Title", "Unknown error", "appfat.cpp", 7), ::testing::ExitedWithCode(1), "Unknown error\n\nThe error occurred at: appfat.cpp line 7");
}

TEST(AppfatTest, InsertCDDlg)
{
	EXPECT_EXIT(InsertCDDlg("diabdat.mpq"), ::testing::ExitedWithCode(1), "diabdat.mpq");
}

TEST(AppfatTest, DirErrorDlg)
{
	EXPECT_EXIT(DirErrorDlg("/"), ::testing::ExitedWithCode(1), "Unable to write to location:\n/");
}

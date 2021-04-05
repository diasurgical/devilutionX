#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Appfat, app_fatal)
{
	EXPECT_EXIT(app_fatal("test"), ::testing::ExitedWithCode(1), "test");
}

TEST(Appfat, ErrDlg)
{
	EXPECT_EXIT(ErrDlg("Title", "Unknown error", "appfat.cpp", 7), ::testing::ExitedWithCode(1), "Unknown error\n\nThe error occurred at: appfat.cpp line 7");
}

TEST(Appfat, FileErrDlg)
{
	EXPECT_EXIT(FileErrDlg("devilution/image.cl2"), ::testing::ExitedWithCode(1), "devilution/image.cl2");
}

TEST(Appfat, InsertCDDlg)
{
	EXPECT_EXIT(InsertCDDlg(), ::testing::ExitedWithCode(1), "diabdat.mpq");
}

TEST(Appfat, DirErrorDlg)
{
	EXPECT_EXIT(DirErrorDlg("/"), ::testing::ExitedWithCode(1), "Unable to write to location:\n/");
}

#include <gtest/gtest.h>
#include "devilution.h"

TEST(Appfat, app_fatal) {
    ASSERT_EXIT(dvl::app_fatal("test"), ::testing::ExitedWithCode(1), "test");
}

TEST(Appfat, ErrDlg) {
    ASSERT_EXIT(dvl::ErrDlg("Title", "Unknown error", "appfat.cpp", 7), ::testing::ExitedWithCode(1), "Unknown error\n\nThe error occurred at: appfat.cpp line 7");
}

TEST(Appfat, FileErrDlg) {
    ASSERT_EXIT(dvl::FileErrDlg("devilution/image.cl2"), ::testing::ExitedWithCode(1), "devilution/image.cl2");
}

TEST(Appfat, InsertCDDlg) {
    ASSERT_EXIT(dvl::InsertCDDlg("devilution/image.cl2"), ::testing::ExitedWithCode(1), "devilution/image.cl2");
}

TEST(Appfat, DirErrorDlg) {
    ASSERT_EXIT(dvl::DirErrorDlg("/"), ::testing::ExitedWithCode(1), "Unable to write to location:\n/");
}

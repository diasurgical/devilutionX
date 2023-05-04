#include <gtest/gtest.h>

#include <cstdio>

#include "utils/file_util.h"

using namespace devilution;

namespace {

void WriteDummyFile(const char *name, std::uintmax_t size)
{
	std::printf("Writing test file %s\n", name);
	FILE *test_file = std::fopen(name, "wb");
	ASSERT_TRUE(test_file != nullptr);
	const char c = '\0';
	for (std::uintmax_t i = 0; i < size; ++i) {
		std::fwrite(&c, sizeof(c), 1, test_file);
		ASSERT_EQ(std::ferror(test_file), 0);
	}
	std::fclose(test_file);
}

std::string GetTmpPathName(const char *suffix = ".tmp")
{
	const auto *current_test = ::testing::UnitTest::GetInstance()->current_test_info();
	std::string result = "Test_";
	result.append(current_test->test_case_name());
	result += '_';
	result.append(current_test->name());
	result.append(suffix);
	return result;
}

TEST(FileUtil, GetFileSize)
{
	const std::string path = GetTmpPathName();
	WriteDummyFile(path.c_str(), 42);
	std::uintmax_t result;
	ASSERT_TRUE(GetFileSize(path.c_str(), &result));
	EXPECT_EQ(result, 42);
}

TEST(FileUtil, FileExists)
{
	EXPECT_FALSE(FileExists("this-file-should-not-exist"));
	const std::string path = GetTmpPathName();
	WriteDummyFile(path.c_str(), 42);
	EXPECT_TRUE(FileExists(path.c_str()));
}

TEST(FileUtil, ResizeFile)
{
	const std::string path = GetTmpPathName();
	WriteDummyFile(path.c_str(), 42);
	std::uintmax_t size;
	ASSERT_TRUE(GetFileSize(path.c_str(), &size));
	EXPECT_EQ(size, 42);
	ASSERT_TRUE(ResizeFile(path.c_str(), 30));
	ASSERT_TRUE(GetFileSize(path.c_str(), &size));
	EXPECT_EQ(size, 30);
}

TEST(FileUtil, Dirname)
{
	EXPECT_EQ(Dirname(""), ".");
	EXPECT_EQ(Dirname("abc"), ".");
	EXPECT_EQ(Dirname(DIRECTORY_SEPARATOR_STR), DIRECTORY_SEPARATOR_STR);
	EXPECT_EQ(Dirname(DIRECTORY_SEPARATOR_STR DIRECTORY_SEPARATOR_STR), DIRECTORY_SEPARATOR_STR);
	EXPECT_EQ(Dirname("a" DIRECTORY_SEPARATOR_STR "b"), "a");
	EXPECT_EQ(Dirname("a" DIRECTORY_SEPARATOR_STR "b" DIRECTORY_SEPARATOR_STR "c"),
	    "a" DIRECTORY_SEPARATOR_STR "b");
	EXPECT_EQ(Dirname("a" DIRECTORY_SEPARATOR_STR "b" DIRECTORY_SEPARATOR_STR "c"),
	    "a" DIRECTORY_SEPARATOR_STR "b");
}

TEST(FileUtil, RecursivelyCreateDir)
{
	const std::string path = GetTmpPathName(/*suffix=*/"") + DIRECTORY_SEPARATOR_STR
	    + "a" + DIRECTORY_SEPARATOR_STR + "b" + DIRECTORY_SEPARATOR_STR + "c";
	RecursivelyCreateDir(path.c_str());
	EXPECT_TRUE(DirectoryExists(path.c_str()));
}

} // namespace

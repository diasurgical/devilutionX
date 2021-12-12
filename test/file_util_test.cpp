#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

#include "utils/file_util.h"

using namespace devilution;

namespace {

void WriteDummyFile(const char *name, std::uintmax_t size)
{
	std::ofstream test_file(name, std::ios::out | std::ios::trunc | std::ios::binary);
	ASSERT_FALSE(test_file.fail());
	const char c = '\0';
	for (std::uintmax_t i = 0; i < size; ++i) {
		test_file.write(&c, 1);
		ASSERT_FALSE(test_file.fail());
	}
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
	std::cout << path << std::endl;
	WriteDummyFile(path.c_str(), 42);
	std::uintmax_t result;
	ASSERT_TRUE(GetFileSize(path.c_str(), &result));
	EXPECT_EQ(result, 42);
}

TEST(FileUtil, FileExists)
{
	EXPECT_FALSE(FileExists("this-file-should-not-exist"));
	const std::string path = GetTmpPathName();
	std::cout << path << std::endl;
	WriteDummyFile(path.c_str(), 42);
	EXPECT_TRUE(FileExists(path.c_str()));
}

TEST(FileUtil, ResizeFile)
{
	const std::string path = GetTmpPathName();
	std::cout << path << std::endl;
	WriteDummyFile(path.c_str(), 42);
	std::uintmax_t size;
	ASSERT_TRUE(GetFileSize(path.c_str(), &size));
	EXPECT_EQ(size, 42);
	ASSERT_TRUE(ResizeFile(path.c_str(), 30));
	ASSERT_TRUE(GetFileSize(path.c_str(), &size));
	EXPECT_EQ(size, 30);
}

} // namespace

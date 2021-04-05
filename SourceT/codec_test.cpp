#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Codec, codec_get_encoded_len)
{
	EXPECT_EQ(codec_get_encoded_len(50), 72);
}

TEST(Codec, codec_get_encoded_len_eq)
{
	EXPECT_EQ(codec_get_encoded_len(128), 136);
}

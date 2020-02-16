#include <gtest/gtest.h>
#include "all.h"

TEST(Codec, codec_get_encoded_len) {
	EXPECT_EQ(dvl::codec_get_encoded_len(50), 72);
}

TEST(Codec, codec_get_encoded_len_eq) {
	EXPECT_EQ(dvl::codec_get_encoded_len(128), 136);
}

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "engine/dynamic_array.hpp"

namespace devilution {
TEST(DynamicArrayTest, AddRemove)
{
	dynamic_array<int, 20> container;

	EXPECT_EQ(container.empty(), true);
	EXPECT_EQ(container.size(), 0);

	EXPECT_EQ(container.capacity(), 20);
	EXPECT_EQ(container.max_size(), 20);
	EXPECT_EQ(container.full(), false);

	container.push_back(3);
	EXPECT_EQ(container.size(), 1);
	EXPECT_EQ(container[0], 3);

	container.push_back(5);
	container.push_back(1);
	EXPECT_EQ(container.size(), 3);

	container.erase(container.begin());
	EXPECT_EQ(container.size(), 2);
	EXPECT_NE(container[0], 3);

	// Need to fix the definition of dynamic_array::end() before the following compiles
	//EXPECT_THAT(container, ::testing::UnorderedElementsAre(5, 1)) << "erasing an item does not guarantee order is preserved";

	for (int i = 0; i < 18; i++)
		container.push_back(3 * i);

	EXPECT_EQ(container.full(), true);

	// not implemented yet
	/* container.erase(container.begin() + 3, container.begin() + 8);
	EXPECT_EQ(container.size(), 15);
	EXPECT_EQ(container[2], 0);
	EXPECT_EQ(container[3], 3 * 5); */
}

TEST(DynamicArrayTest, Iterators)
{
	dynamic_array<int, 20> container { 2, 3, 1, 600 };

	size_t index = 0;
	for (auto item : container) {
		EXPECT_EQ(item, container[index]);
		index++;
	}
	EXPECT_EQ(container.size(), index);
}
} // namespace devilution

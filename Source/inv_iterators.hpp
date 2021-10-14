#pragma once

#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

#include "inv.h"
#include "stash.h"
#include "items.h"
#include "player.h"

namespace devilution {

/**
 * @brief A range over non-empty items in a container.
 */
class ItemsContainerRange {
public:
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = int;
		using value_type = Item;
		using pointer = value_type *;
		using reference = value_type &;

		Iterator() = default;

		Iterator(Item *items, std::size_t count, std::size_t index)
		    : items_(items)
		    , count_(count)
		    , index_(index)
		{
			AdvancePastEmpty();
		}

		pointer operator->() const
		{
			return &items_[index_];
		}

		reference operator*() const
		{
			return items_[index_];
		}

		Iterator &operator++()
		{
			++index_;
			AdvancePastEmpty();
			return *this;
		}

		Iterator operator++(int)
		{
			auto copy = *this;
			++(*this);
			return copy;
		}

		bool operator==(const Iterator &other) const
		{
			return index_ == other.index_;
		}

		bool operator!=(const Iterator &other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] bool AtEnd() const
		{
			return index_ == count_;
		}

	private:
		void AdvancePastEmpty()
		{
			while (index_ < count_ && items_[index_].isEmpty()) {
				++index_;
			}
		}

		Item *items_ = nullptr;
		std::size_t count_ = 0;
		std::size_t index_ = 0;
	};

	ItemsContainerRange(Item *items, std::size_t count)
	    : items_(items)
	    , count_(count)
	{
	}

	[[nodiscard]] Iterator begin() const // NOLINT(readability-identifier-naming)
	{
		return Iterator { items_, count_, 0 };
	}

	[[nodiscard]] Iterator end() const // NOLINT(readability-identifier-naming)
	{
		return Iterator { nullptr, count_, count_ };
	}

private:
	Item *items_;
	std::size_t count_;
};

/**
 * @brief A range over non-empty items in a list of containers.
 */
class ItemsContainerListRange {
public:
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = int;
		using value_type = Item;
		using pointer = value_type *;
		using reference = value_type &;

		Iterator() = default;

		explicit Iterator(std::vector<ItemsContainerRange::Iterator> iterators)
		    : iterators_(std::move(iterators))
		{
			AdvancePastEmpty();
		}

		pointer operator->() const
		{
			return iterators_[current_].operator->();
		}

		reference operator*() const
		{
			return iterators_[current_].operator*();
		}

		Iterator &operator++()
		{
			++iterators_[current_];
			AdvancePastEmpty();
			return *this;
		}

		Iterator operator++(int)
		{
			auto copy = *this;
			++(*this);
			return copy;
		}

		bool operator==(const Iterator &other) const
		{
			return current_ == other.current_ && iterators_[current_] == other.iterators_[current_];
		}
		bool operator!=(const Iterator &other) const
		{
			return !(*this == other);
		}

	private:
		void AdvancePastEmpty()
		{
			while (current_ + 1 < iterators_.size() && iterators_[current_].AtEnd()) {
				++current_;
			}
		}

		std::vector<ItemsContainerRange::Iterator> iterators_;
		std::size_t current_ = 0;
	};
};

/**
 * @brief A range over equipped player items.
 */
class EquippedPlayerItemsRange {
public:
	explicit EquippedPlayerItemsRange(Player &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] ItemsContainerRange::Iterator begin() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerRange::Iterator { &player_->InvBody[0], ContainerSize(), 0 };
	}

	[[nodiscard]] ItemsContainerRange::Iterator end() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerRange::Iterator { nullptr, ContainerSize(), ContainerSize() };
	}

private:
	[[nodiscard]] std::size_t ContainerSize() const
	{
		return sizeof(player_->InvBody) / sizeof(player_->InvBody[0]);
	}

	Player *player_;
};

/**
 * @brief A range over non-equipped inventory player items.
 */
class InventoryPlayerItemsRange {
public:
	explicit InventoryPlayerItemsRange(Player &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] ItemsContainerRange::Iterator begin() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerRange::Iterator { &player_->InvList[0], ContainerSize(), 0 };
	}

	[[nodiscard]] ItemsContainerRange::Iterator end() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerRange::Iterator { nullptr, ContainerSize(), ContainerSize() };
	}

private:
	[[nodiscard]] std::size_t ContainerSize() const
	{
		return static_cast<std::size_t>(player_->_pNumInv);
	}

	Player *player_;
};

/**
 * @brief A range over belt player items.
 */
class BeltPlayerItemsRange {
public:
	explicit BeltPlayerItemsRange(Player &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] ItemsContainerRange::Iterator begin() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerRange::Iterator { &player_->SpdList[0], ContainerSize(), 0 };
	}

	[[nodiscard]] ItemsContainerRange::Iterator end() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerRange::Iterator { nullptr, ContainerSize(), ContainerSize() };
	}

private:
	[[nodiscard]] std::size_t ContainerSize() const
	{
		return sizeof(player_->SpdList) / sizeof(player_->SpdList[0]);
	}

	Player *player_;
};

/**
 * @brief A range over non-equipped player items in the following order: Inventory, Belt.
 */
class InventoryAndBeltPlayerItemsRange {
public:
	explicit InventoryAndBeltPlayerItemsRange(Player &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] ItemsContainerListRange::Iterator begin() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerListRange::Iterator({
		    InventoryPlayerItemsRange(*player_).begin(),
		    BeltPlayerItemsRange(*player_).begin(),
		});
	}

	[[nodiscard]] ItemsContainerListRange::Iterator end() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerListRange::Iterator({
		    InventoryPlayerItemsRange(*player_).end(),
		    BeltPlayerItemsRange(*player_).end(),
		});
	}

private:
	Player *player_;
};

/**
 * @brief A range over non-empty player items in the following order: Equipped, Inventory, Belt.
 */
class PlayerItemsRange {
public:
	explicit PlayerItemsRange(Player &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] ItemsContainerListRange::Iterator begin() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerListRange::Iterator({
		    EquippedPlayerItemsRange(*player_).begin(),
		    InventoryPlayerItemsRange(*player_).begin(),
		    BeltPlayerItemsRange(*player_).begin(),
		});
	}

	[[nodiscard]] ItemsContainerListRange::Iterator end() const // NOLINT(readability-identifier-naming)
	{
		return ItemsContainerListRange::Iterator({
		    EquippedPlayerItemsRange(*player_).end(),
		    InventoryPlayerItemsRange(*player_).end(),
		    BeltPlayerItemsRange(*player_).end(),
		});
	}

private:
	Player *player_;
};

} // namespace devilution

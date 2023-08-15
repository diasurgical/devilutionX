#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

#include "items.h"
#include "player.h"

namespace devilution {

/**
 * @brief A range over non-empty items in a container.
 */
template <typename ItemT>
class ItemsContainerRange {
	static_assert(std::is_same_v<ItemT, Item> || std::is_same_v<ItemT, const Item>,
	    "The template argument must be `Item` or `const Item`");

public:
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = int;
		using value_type = ItemT;
		using pointer = value_type *;
		using reference = value_type &;

		Iterator() = default;

		Iterator(ItemT *items, std::size_t count, std::size_t index)
		    : items_(items)
		    , count_(count)
		    , index_(index)
		{
			advancePastEmpty();
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
			advancePastEmpty();
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

		[[nodiscard]] bool atEnd() const
		{
			return index_ == count_;
		}

		[[nodiscard]] std::size_t index() const
		{
			return index_;
		}

	private:
		void advancePastEmpty()
		{
			while (index_ < count_ && items_[index_].isEmpty()) {
				++index_;
			}
		}

		ItemT *items_ = nullptr;
		std::size_t count_ = 0;
		std::size_t index_ = 0;
	};

	ItemsContainerRange(ItemT *items, std::size_t count)
	    : items_(items)
	    , count_(count)
	{
	}

	[[nodiscard]] Iterator begin() const
	{
		return Iterator { items_, count_, 0 };
	}

	[[nodiscard]] Iterator end() const
	{
		return Iterator { nullptr, count_, count_ };
	}

private:
	ItemT *items_;
	std::size_t count_;
};

/**
 * @brief A range over non-empty items in a list of containers.
 */
template <typename ItemT>
class ItemsContainerListRange {
	static_assert(std::is_same_v<ItemT, Item> || std::is_same_v<ItemT, const Item>,
	    "The template argument must be `Item` or `const Item`");

public:
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = int;
		using value_type = ItemT;
		using pointer = value_type *;
		using reference = value_type &;

		Iterator() = default;

		explicit Iterator(std::vector<typename ItemsContainerRange<ItemT>::Iterator> iterators)
		    : iterators_(std::move(iterators))
		{
			advancePastEmpty();
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
			advancePastEmpty();
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
		void advancePastEmpty()
		{
			while (current_ + 1 < iterators_.size() && iterators_[current_].atEnd()) {
				++current_;
			}
		}

		std::vector<typename ItemsContainerRange<ItemT>::Iterator> iterators_;
		std::size_t current_ = 0;
	};
};

/**
 * @brief A range over equipped player items.
 */
template <typename PlayerT>
class EquippedPlayerItemsRange {
	static_assert(std::is_same_v<PlayerT, Player> || std::is_same_v<PlayerT, const Player>,
	    "The template argument must be `Player` or `const Player`");
	using ItemT = std::conditional_t<std::is_const_v<PlayerT>, const Item, Item>;
	using Iterator = typename ItemsContainerRange<ItemT>::Iterator;

public:
	explicit EquippedPlayerItemsRange(PlayerT &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] Iterator begin() const
	{
		return Iterator { &player_->InvBody[0], containerSize(), 0 };
	}

	[[nodiscard]] Iterator end() const
	{
		return Iterator { nullptr, containerSize(), containerSize() };
	}

private:
	[[nodiscard]] std::size_t containerSize() const
	{
		return sizeof(player_->InvBody) / sizeof(player_->InvBody[0]);
	}

	PlayerT *player_;
};

/**
 * @brief A range over non-equipped inventory player items.
 */
template <typename PlayerT>
class InventoryPlayerItemsRange {
	static_assert(std::is_same_v<PlayerT, Player> || std::is_same_v<PlayerT, const Player>,
	    "The template argument must be `Player` or `const Player`");
	using ItemT = std::conditional_t<std::is_const_v<PlayerT>, const Item, Item>;
	using Iterator = typename ItemsContainerRange<ItemT>::Iterator;

public:
	explicit InventoryPlayerItemsRange(PlayerT &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] Iterator begin() const
	{
		return Iterator { &player_->InvList[0], containerSize(), 0 };
	}

	[[nodiscard]] Iterator end() const
	{
		return Iterator { nullptr, containerSize(), containerSize() };
	}

private:
	[[nodiscard]] std::size_t containerSize() const
	{
		return static_cast<std::size_t>(player_->_pNumInv);
	}

	PlayerT *player_;
};

/**
 * @brief A range over belt player items.
 */
template <typename PlayerT>
class BeltPlayerItemsRange {
	static_assert(std::is_same_v<PlayerT, Player> || std::is_same_v<PlayerT, const Player>,
	    "The template argument must be `Player` or `const Player`");
	using ItemT = std::conditional_t<std::is_const_v<PlayerT>, const Item, Item>;
	using Iterator = typename ItemsContainerRange<ItemT>::Iterator;

public:
	explicit BeltPlayerItemsRange(PlayerT &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] Iterator begin() const
	{
		return Iterator { &player_->SpdList[0], containerSize(), 0 };
	}

	[[nodiscard]] Iterator end() const
	{
		return Iterator { nullptr, containerSize(), containerSize() };
	}

private:
	[[nodiscard]] std::size_t containerSize() const
	{
		return sizeof(player_->SpdList) / sizeof(player_->SpdList[0]);
	}

	PlayerT *player_;
};

/**
 * @brief A range over non-equipped player items in the following order: Inventory, Belt.
 */
template <typename PlayerT>
class InventoryAndBeltPlayerItemsRange {
	static_assert(std::is_same_v<PlayerT, Player> || std::is_same_v<PlayerT, const Player>,
	    "The template argument must be `Player` or `const Player`");
	using ItemT = std::conditional_t<std::is_const_v<PlayerT>, const Item, Item>;
	using Iterator = typename ItemsContainerListRange<ItemT>::Iterator;

public:
	explicit InventoryAndBeltPlayerItemsRange(PlayerT &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] Iterator begin() const
	{
		return Iterator({
		    InventoryPlayerItemsRange(*player_).begin(),
		    BeltPlayerItemsRange(*player_).begin(),
		});
	}

	[[nodiscard]] Iterator end() const
	{
		return Iterator({
		    InventoryPlayerItemsRange(*player_).end(),
		    BeltPlayerItemsRange(*player_).end(),
		});
	}

private:
	PlayerT *player_;
};

/**
 * @brief A range over non-empty player items in the following order: Equipped, Inventory, Belt.
 */
template <typename PlayerT>
class PlayerItemsRange {
	static_assert(std::is_same_v<PlayerT, Player> || std::is_same_v<PlayerT, const Player>,
	    "The template argument must be `Player` or `const Player`");
	using ItemT = std::conditional_t<std::is_const_v<PlayerT>, const Item, Item>;
	using Iterator = typename ItemsContainerListRange<ItemT>::Iterator;

public:
	explicit PlayerItemsRange(PlayerT &player)
	    : player_(&player)
	{
	}

	[[nodiscard]] Iterator begin() const
	{
		return Iterator({
		    EquippedPlayerItemsRange(*player_).begin(),
		    InventoryPlayerItemsRange(*player_).begin(),
		    BeltPlayerItemsRange(*player_).begin(),
		});
	}

	[[nodiscard]] Iterator end() const
	{
		return Iterator({
		    EquippedPlayerItemsRange(*player_).end(),
		    InventoryPlayerItemsRange(*player_).end(),
		    BeltPlayerItemsRange(*player_).end(),
		});
	}

private:
	PlayerT *player_;
};

} // namespace devilution

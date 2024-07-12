#pragma once

#include <cstddef>
#include <forward_list>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include <ankerl/unordered_dense.h>

#include "utils/string_view_hash.hpp"

namespace devilution {

class ResourceStore;
class OwnedClxSpriteListOrSheet;
class OwnedClxSpriteList;
class OwnedClxSpriteSheet;

/**
 * @brief An entry stored in the resource store.
 * Contains the pointer to the underlying resource and variant name.
 */
struct ResourceStoreEntry {
	// Resources must inherit from `OwnedResource`.
	using PtrVariant = std::variant<
	    OwnedClxSpriteListOrSheet *,
	    OwnedClxSpriteList *,
	    OwnedClxSpriteSheet *>;

	PtrVariant ptr;
	std::string variant;

	[[nodiscard]] std::string_view name() const;
	[[nodiscard]] size_t dataSize() const;

private:
	void updateHandleIterator(const std::forward_list<ResourceStoreEntry>::iterator &newBefore);

	friend class ResourceStore;
};

/**
 * @returns The global resource store.
 */
ResourceStore &GetResourceStore();

/**
 * @brief A handle to a resource in the store.
 */
class ResourceStoreHandle {
public:
	ResourceStoreHandle() = default;

	// NOLINTNEXTLINE(readability-identifier-naming): match std::optional API.
	[[nodiscard]] bool has_value() const { return !name_.empty(); }
	[[nodiscard]] const ResourceStoreEntry &value() const;
	[[nodiscard]] ResourceStoreEntry &value();
	[[nodiscard]] std::string_view name() const { return name_; }

private:
	explicit ResourceStoreHandle(std::string_view name)
	    : name_(name)
	{
	}

	// Handles to the front of the list have no iterator and must resolved by name.
	// They have no iterator because it would be unstable wrt map modifications.
	[[nodiscard]] bool isFront() const
	{
		return it_before_ == std::forward_list<ResourceStoreEntry>::iterator {};
	}

	std::string name_;
	std::forward_list<ResourceStoreEntry>::iterator it_before_;

	friend class ResourceStore;
	friend class ResourceStoreEntry;
};

template <typename Self>
class OwnedResource;

/**
 * @brief Keeps track of all the currently loaded resources.
 */
class ResourceStore {
public:
	using Map = ankerl::unordered_dense::segmented_map</*name*/ std::string, std::forward_list<ResourceStoreEntry>, StringViewHash, StringViewEquals>;

	[[nodiscard]] const ResourceStoreEntry *get(std::string_view name, std::string_view variant = {}) const;

	template <typename T>
	[[nodiscard]] ResourceStoreHandle registerResource(std::string_view name, T &resource, std::string &&variant)
	{
		return registerResource(name, ResourceStoreEntry { ResourceStoreEntry::PtrVariant { &resource }, std::move(variant) });
	}

	template <typename T>
	[[nodiscard]] ResourceStoreHandle registerResource(std::string_view name, T &resource, std::string_view variant)
	{
		return registerResource(name, ResourceStoreEntry { ResourceStoreEntry::PtrVariant { &resource }, std::string(variant) });
	}

	template <typename T>
	ResourceStoreHandle replaceResource(T &resource, ResourceStoreHandle &&handle)
	{
		ResourceStoreHandle result = std::move(handle);
		if (result.has_value()) {
			result.value().ptr = &resource;
		}
		handle = {};
		return result;
	}

	void unregisterResource(const ResourceStoreHandle &handle);

	[[nodiscard]] const Map &getAll() const { return map_; }

private:
	[[nodiscard]] ResourceStoreHandle registerResource(std::string_view name, ResourceStoreEntry &&ref);

	// For internal use by OwnedResource
	[[nodiscard]] const ResourceStoreEntry &resolveHandle(const ResourceStoreHandle &handle) const;
	[[nodiscard]] ResourceStoreEntry &resolveHandle(ResourceStoreHandle &handle);

	Map map_;

	friend class OwnedResource<OwnedClxSpriteListOrSheet>;
	friend class OwnedResource<OwnedClxSpriteList>;
	friend class OwnedResource<OwnedClxSpriteSheet>;
	friend class ResourceStoreHandle;
};

/**
 * @brief Base class for the owning instance of a resource.
 */
template <typename Self>
class OwnedResource {
public:
	~OwnedResource()
	{
		if (handle_.has_value()) GetResourceStore().unregisterResource(handle_);
	}

	[[nodiscard]] std::string_view resourceName() const { return handle_.name(); }
	[[nodiscard]] std::string_view resourceVariant() const { return storeEntry().variant; }

	void setVariant(std::string_view variant)
	{
		storeEntry().variant = variant;
	}

protected:
	OwnedResource(std::string_view name, std::string_view variant)
	    : handle_(GetResourceStore().registerResource(name, *static_cast<Self *>(this), variant))
	{
	}

	OwnedResource(std::string_view name, std::string &&variant)
	    : handle_(GetResourceStore().registerResource(name, *static_cast<Self *>(this), std::move(variant)))
	{
	}

	explicit OwnedResource(ResourceStoreHandle &&handle)
	    : handle_(GetResourceStore().replaceResource(*static_cast<Self *>(this), std::move(handle)))
	{
	}

	OwnedResource(OwnedResource<Self> &&other) noexcept
	    : handle_(GetResourceStore().replaceResource(*static_cast<Self *>(this), std::move(other.handle_)))
	{
	}

	OwnedResource &operator=(OwnedResource<Self> &&other) noexcept
	{
		if (handle_.has_value()) GetResourceStore().unregisterResource(handle_);
		handle_ = GetResourceStore().replaceResource(*static_cast<Self *>(this), std::move(other.handle_));
		return *this;
	}

	[[nodiscard]] const ResourceStoreEntry &storeEntry() const { return handle_.value(); }
	[[nodiscard]] ResourceStoreEntry &storeEntry() { return handle_.value(); }

	// For OptionalOwned* types
	OwnedResource() = default;

	ResourceStoreHandle handle_;
	friend class ResourceStore;
	friend class ResourceStoreEntry;
};

} // namespace devilution

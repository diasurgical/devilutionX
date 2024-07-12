#include "engine/resource_store.hpp"

#include <cstddef>
#include <forward_list>
#include <string_view>
#include <variant>

#include "appfat.h"
// NOLINTNEXTLINE(misc-include-cleaner): Used via templated lambdas passed to `std::visit`.
#include "engine/clx_sprite.hpp"
#include "utils/str_cat.hpp"

namespace devilution {

ResourceStore &GetResourceStore()
{
	static auto *instance = new ResourceStore();
	return *instance;
}

std::string_view ResourceStoreEntry::name() const
{
	return std::visit([](const auto *p) { return p->resourceName(); }, ptr);
}

size_t ResourceStoreEntry::dataSize() const
{
	return std::visit([](const auto *p) { return p->dataSize(); }, ptr);
}

void ResourceStoreEntry::updateHandleIterator(const std::forward_list<ResourceStoreEntry>::iterator &newBefore)
{
	std::visit([newBefore](auto &p) { p->handle_.it_before_ = newBefore; }, ptr);
}

const ResourceStoreEntry &ResourceStoreHandle::value() const { return GetResourceStore().resolveHandle(*this); }
ResourceStoreEntry &ResourceStoreHandle::value() { return GetResourceStore().resolveHandle(*this); }

const ResourceStoreEntry *ResourceStore::get(std::string_view name, std::string_view variant) const
{
	const auto it = map_.find(name);
	if (it == map_.end()) return nullptr;
	for (const ResourceStoreEntry &ref : it->second) {
		if (ref.variant == variant) return &ref;
	}
	return nullptr;
}

const ResourceStoreEntry &ResourceStore::resolveHandle(const ResourceStoreHandle &handle) const
{
	if (!handle.isFront()) return *std::next(handle.it_before_);
	return *map_.at(handle.name_).begin();
}

ResourceStoreEntry &ResourceStore::resolveHandle(ResourceStoreHandle &handle)
{
	if (!handle.isFront()) return *std::next(handle.it_before_);
	return *map_.at(handle.name_).begin();
}

ResourceStoreHandle ResourceStore::registerResource(std::string_view name, ResourceStoreEntry &&ref)
{
	auto [it, inserted] = map_.try_emplace(name);
	if (inserted) {
		it->second.push_front(std::move(ref));
	} else {
		std::forward_list<ResourceStoreEntry> &list = it->second;
		ResourceStoreEntry &next = *list.begin();
		list.push_front(std::move(ref));
		next.updateHandleIterator(list.begin());
	}

	// We return the name-based handle instead without a `list.before_begin()` iterator
	// `list.before_begin()` can move when the `map_` changes.
	return ResourceStoreHandle { name };
}

void ResourceStore::unregisterResource(const ResourceStoreHandle &handle)
{
	const std::string_view name = handle.name_;
	const auto it = map_.find(name);
	if (it == map_.end() || it->second.empty()) {
		app_fatal(StrCat("ResourceStore: Invalid unregister request for [", name, "]: list is empty"));
	}

	std::forward_list<ResourceStoreEntry> &list = it->second;

	if (handle.isFront()) {
		list.erase_after(list.before_begin());
		if (!list.empty()) {
			// Iterators for front-of-the-list handles must be empty because
			// `list.before_begin()` is unstable wrt to map changes.
			list.begin()->updateHandleIterator({});
		}
	} else {
		list.erase_after(handle.it_before_);
		auto it = handle.it_before_;
		++it;
		if (it != std::forward_list<ResourceStoreEntry>::iterator {}) {
			it->updateHandleIterator(handle.it_before_);
		}
	}
	if (list.empty()) {
		map_.erase(it);
	}
}

} // namespace devilution

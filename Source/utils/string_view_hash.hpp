#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <ankerl/unordered_dense.h>

namespace devilution {

// A hash functor that enables heterogenous lookup for `unordered_set/map`.
struct StringViewHash {
	using is_transparent = void;
	using is_avalanching = void;

	[[nodiscard]] uint64_t operator()(std::string_view str) const noexcept
	{
		return ankerl::unordered_dense::hash<std::string_view> {}(str);
	}

	[[nodiscard]] uint64_t operator()(const char *str) const noexcept
	{
		return (*this)(std::string_view { str });
	}

	[[nodiscard]] uint64_t operator()(const std::string &str) const noexcept
	{
		return (*this)(std::string_view { str });
	}
};

// Usually we'd use `std::equal_to<>` instead but the latter
// does not link on the libcxx that comes with Xbox NXDK as of Aug 2024,
struct StringViewEquals {
	using is_transparent = void;

	[[nodiscard]] bool operator()(std::string_view a, std::string_view b) const { return a == b; }
	[[nodiscard]] bool operator()(std::string_view a, const std::string &b) const { return a == b; }
	[[nodiscard]] bool operator()(const std::string &a, std::string_view &b) const { return a == b; }
	[[nodiscard]] bool operator()(const char *a, const std::string &b) const { return a == b; }
	[[nodiscard]] bool operator()(const std::string &a, const char *b) const { return a == b; }
	[[nodiscard]] bool operator()(std::string_view a, const char *b) const { return a == b; }
	[[nodiscard]] bool operator()(const char *a, std::string_view b) const { return a == b; }
	[[nodiscard]] bool operator()(const char *a, const char *b) const { return std::string_view { a } == b; }
};

} // namespace devilution

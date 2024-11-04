#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace devilution {

class StringOrView {
public:
	StringOrView()
	    : rep_ { std::string_view {} }
	{
	}

	StringOrView(const StringOrView &) = default;
	StringOrView(StringOrView &&) noexcept = default;

	StringOrView(std::string &&str)
	    : rep_ { std::move(str) }
	{
	}

	StringOrView(std::string_view str)
	    : rep_ { str }
	{
	}

	StringOrView &operator=(StringOrView &&) noexcept = default;

	StringOrView &operator=(std::string &&value) noexcept
	{
		rep_ = std::move(value);
		return *this;
	}

	StringOrView &operator=(std::string_view value) noexcept
	{
		rep_ = value;
		return *this;
	}

	bool empty() const
	{
		return std::visit([](auto &&val) -> bool { return val.empty(); }, rep_);
	}

	std::string_view str() const
	{
		return std::visit([](auto &&val) -> std::string_view { return val; }, rep_);
	}

	operator std::string_view() const
	{
		return str();
	}

private:
	std::variant<std::string, std::string_view> rep_;
};

} // namespace devilution

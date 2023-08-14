#pragma once

#include <string>
#include <string_view>
#include <utility>

namespace devilution {

class StringOrView {
public:
	StringOrView()
	    : owned_(false)
	    , view_()
	{
	}

	StringOrView(std::string &&str)
	    : owned_(true)
	    , str_(std::move(str))
	{
	}

	StringOrView(std::string_view str)
	    : owned_(false)
	    , view_(str)
	{
	}

	StringOrView(StringOrView &&other) noexcept
	    : owned_(other.owned_)
	{
		if (other.owned_) {
			new (&str_) std::string(std::move(other.str_));
		} else {
			new (&view_) std::string_view(other.view_);
		}
	}

	StringOrView &operator=(StringOrView &&other) noexcept
	{
		if (owned_) {
			if (other.owned_) {
				str_ = std::move(other.str_);
			} else {
				str_.~basic_string();
				owned_ = false;
				new (&view_) std::string_view(other.view_);
			}
		} else {
			if (other.owned_) {
				view_.~basic_string_view();
				owned_ = true;
				new (&str_) std::string(std::move(other.str_));
			} else {
				view_ = other.view_;
			}
		}
		return *this;
	}

	~StringOrView()
	{
		if (owned_) {
			str_.~basic_string();
		} else {
			view_.~basic_string_view();
		}
	}

	bool empty() const
	{
		return owned_ ? str_.empty() : view_.empty();
	}

	std::string_view str() const
	{
		return owned_ ? str_ : view_;
	}

	operator std::string_view() const
	{
		return str();
	}

private:
	bool owned_;
	union {
		std::string str_;
		std::string_view view_;
	};
};

} // namespace devilution

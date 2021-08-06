#pragma once

#include <string>
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

class StringVariant
{
	std::string str;
	string_view view;

public:
	StringVariant()
	{
	}

	explicit StringVariant(string_view view)
	    : view(view)
	{
	}

	explicit StringVariant(std::string str)
	    : str(std::move(str))
	{
	}

	explicit StringVariant(const char *cstr)
	    : view(cstr)
	{
	}

	explicit StringVariant(char *cstr)
	    : str(cstr)
	{
	}

	StringVariant &operator =(const char *cstr)
	{
		*this = StringVariant(cstr);
		return *this;
	}

	StringVariant &operator =(char *cstr)
	{
		*this = StringVariant(cstr);
		return *this;
	}

	StringVariant &operator =(const string_view &view)
	{
		*this = StringVariant(view);
		return *this;
	}

	StringVariant &operator =(std::string &str)
	{
		*this = StringVariant(std::move(str));
		return *this;
	}

	size_t length() const
	{
		if (str.length() > 0)
			return str.length();
		return view.length();
	}

	operator string_view() const
	{
		if (str.length() > 0)
			return str;
		return view;
	}
};

} // namespace devilution

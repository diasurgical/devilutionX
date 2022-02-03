#pragma once

#include <cstring>
#include <initializer_list>

#include <fmt/format.h>

#include "utils/stdcompat/string_view.hpp"

namespace devilution {

/**
 * @brief Generates file names from prefixes, a suffix, and an index.
 *
 * @example FileNameGenerator f({"a/", "b"}, ".txt", 1);
 *     f()  // "a/b.txt"
 *     f(0) // "a/b1.txt"
 *     f(1) // "a/b2.txt"
 */
class FileNameGenerator {
public:
	FileNameGenerator(std::initializer_list<string_view> prefixes, string_view suffix, unsigned min = 1)
	    : suffix_(suffix)
	    , min_(min)
	    , prefixEnd_(Append(buf_, prefixes))
	{
	}

	const char *operator()() const
	{
		*Append(prefixEnd_, suffix_) = '\0';
		return buf_;
	}

	const char *operator()(size_t i) const
	{
		*Append(fmt::format_to(prefixEnd_, "{}", static_cast<unsigned>(min_ + i)), suffix_) = '\0';
		return buf_;
	}

private:
	static char *Append(char *buf, std::initializer_list<string_view> strings)
	{
		for (string_view str : strings)
			buf = Append(buf, str);
		return buf;
	}

	static char *Append(char *buf, string_view str)
	{
		memcpy(buf, str.data(), str.size());
		return buf + str.size();
	}

	string_view suffix_;
	unsigned min_;
	char *prefixEnd_;
	char buf_[256];
};

} // namespace devilution

#pragma once

#include <cstring>
#include <initializer_list>

#include <fmt/format.h>

#include "utils/stdcompat/string_view.hpp"

namespace devilution {

class BaseFileNameGenerator {
public:
	BaseFileNameGenerator(std::initializer_list<string_view> prefixes, string_view suffix)
	    : suffix_(suffix)
	    , prefixEnd_(Append(buf_, prefixes))
	{
	}

	const char *operator()() const
	{
		*Append(prefixEnd_, suffix_) = '\0';
		return buf_;
	}

protected:
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

	[[nodiscard]] string_view Suffix() const
	{
		return suffix_;
	}
	[[nodiscard]] char *PrefixEnd() const
	{
		return prefixEnd_;
	}
	[[nodiscard]] const char *Buffer() const
	{
		return buf_;
	}

private:
	string_view suffix_;
	char *prefixEnd_;
	char buf_[256];
};

/**
 * @brief Generates file names from prefixes, a suffix, and an index.
 *
 * @example FileNameGenerator f({"a/", "b"}, ".txt", 1);
 *     f()  // "a/b.txt"
 *     f(0) // "a/b1.txt"
 *     f(1) // "a/b2.txt"
 */
class FileNameGenerator : public BaseFileNameGenerator {
public:
	FileNameGenerator(std::initializer_list<string_view> prefixes, string_view suffix, unsigned min = 1)
	    : BaseFileNameGenerator(prefixes, suffix)
	    , min_(min)
	{
	}

	using BaseFileNameGenerator::operator();

	const char *operator()(size_t i) const
	{
		*Append(fmt::format_to(PrefixEnd(), "{}", static_cast<unsigned>(min_ + i)), Suffix()) = '\0';
		return Buffer();
	}

private:
	unsigned min_;
};

/**
 * @brief Generates file names from prefixes, a suffix, a char array and an index into it.
 *
 * @example FileNameWithCharAffixGenerator f({"a/", "b"}, ".txt", "ABC");
 *     f(0) // "a/bA.txt"
 *     f(1) // "a/bB.txt"
 */
class FileNameWithCharAffixGenerator : public BaseFileNameGenerator {
public:
	FileNameWithCharAffixGenerator(std::initializer_list<string_view> prefixes, string_view suffix, const char *chars)
	    : BaseFileNameGenerator(prefixes, suffix)
	    , chars_(chars)
	{
		*Append(PrefixEnd() + 1, Suffix()) = '\0';
	}

	const char *operator()(size_t i) const
	{
		PrefixEnd()[0] = chars_[i];
		return Buffer();
	}

private:
	const char *chars_;
};

} // namespace devilution

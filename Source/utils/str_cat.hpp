#pragma once

#include <cstring>
#include <string>
#include <type_traits>

#include "utils/stdcompat/string_view.hpp"

namespace devilution {

/**
 * @brief Writes the integer to the given buffer.
 * @return char* end of the buffer
 */
char *BufCopy(char *out, int value);

/**
 * @brief Appends the integer to the given string.
 */
void StrAppend(std::string &out, int value);

/**
 * @brief Copies the given string_view to the given buffer.
 */
inline char *BufCopy(char *out, string_view value)
{
	std::memcpy(out, value.data(), value.size());
	return out + value.size();
}

/**
 * @brief Copies the given string_view to the given string.
 */
inline void StrAppend(std::string &out, string_view value)
{
	out.append(value.data(), value.size());
}

/**
 * @brief Appends the given C string to the given buffer.
 *
 * `str` must be a null-terminated C string, `out` will not be null terminated.
 */
inline char *BufCopy(char *out, const char *str)
{
	return BufCopy(out, string_view(str != nullptr ? str : "(nullptr)"));
}

/**
 * @brief Appends the given C string to the given string.
 */
inline void StrAppend(std::string &out, const char *str)
{
	if (str == nullptr)
		out.append("(nullptr)");
	out.append(str, string_view(str).size());
}

template <typename Arg, typename... Args>
inline typename std::enable_if<(sizeof...(Args) > 0), char *>::type
BufCopy(char *out, Arg &&arg, Args &&...args)
{
	return BufCopy(BufCopy(out, std::forward<Arg>(arg)), std::forward<Args>(args)...);
}

template <typename Arg, typename... Args>
inline typename std::enable_if<(sizeof...(Args) > 0), void>::type
StrAppend(std::string &out, Arg &&arg, Args &&...args)
{
	StrAppend(out, std::forward<Arg>(arg));
	StrAppend(out, std::forward<Args>(args)...);
}

template <typename... Args>
std::string StrCat(Args &&...args)
{
	std::string result;
	StrAppend(result, std::forward<Args>(args)...);
	return result;
}

} // namespace devilution

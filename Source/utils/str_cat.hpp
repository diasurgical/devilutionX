#pragma once

#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>

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
 * @brief Copies the given std::string_view to the given buffer.
 */
inline char *BufCopy(char *out, std::string_view value)
{
	std::memcpy(out, value.data(), value.size());
	return out + value.size();
}

/**
 * @brief Copies the given std::string_view to the given string.
 */
inline void StrAppend(std::string &out, std::string_view value)
{
	out.append(value);
}

/**
 * @brief Appends the given C string to the given buffer.
 *
 * `str` must be a null-terminated C string, `out` will not be null terminated.
 */
inline char *BufCopy(char *out, const char *str)
{
	return BufCopy(out, std::string_view(str != nullptr ? str : "(nullptr)"));
}

/**
 * @brief Appends the given C string to the given string.
 */
inline void StrAppend(std::string &out, const char *str)
{
	out.append(std::string_view(str != nullptr ? str : "(nullptr)"));
}

template <typename... Args>
typename std::enable_if<(sizeof...(Args) > 1), char *>::type
BufCopy(char *out, Args &&...args)
{
	return ((out = BufCopy(out, std::forward<Args>(args))), ...);
}

template <typename... Args>
typename std::enable_if<(sizeof...(Args) > 1), void>::type
StrAppend(std::string &out, Args &&...args)
{
	(StrAppend(out, std::forward<Args>(args)), ...);
}

template <typename... Args>
std::string StrCat(Args &&...args)
{
	std::string result;
	StrAppend(result, std::forward<Args>(args)...);
	return result;
}

} // namespace devilution

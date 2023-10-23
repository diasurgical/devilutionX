#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string_view>

#include <SDL.h>

#include "utils/utf8.hpp"

namespace devilution {

/**
 * @brief Manages state for a single-line text input with a cursor.
 *
 * The text value and the cursor position are stored externally.
 */
class TextInputState {
	/**
	 * @brief Manages an unowned fixed size char array.
	 */
	struct Buffer {
		Buffer(char *begin, size_t maxLength)
		    : buf_(begin)
		    , maxLength_(maxLength)
		{
			std::string_view str(begin);
			str = TruncateUtf8(str, maxLength);
			len_ = str.size();
			buf_[len_] = '\0';
		}

		[[nodiscard]] size_t size() const
		{
			return len_;
		}

		[[nodiscard]] bool empty() const
		{
			return len_ == 0;
		}

		Buffer &operator=(std::string_view value)
		{
			value = TruncateUtf8(value, maxLength_);
			CopyUtf8(buf_, value, maxLength_);
			len_ = value.size();
			return *this;
		}

		void insert(size_t pos, std::string_view value)
		{
			value = truncateForInsertion(value);
			std::memmove(&buf_[pos + value.size()], &buf_[pos], len_ - pos);
			std::memcpy(&buf_[pos], value.data(), value.size());
			len_ += value.size();
			buf_[len_] = '\0';
		}

		void erase(size_t pos, size_t len)
		{
			std::memmove(&buf_[pos], &buf_[pos + len], len);
			len_ -= len;
			buf_[len_] = '\0';
		}

		explicit operator std::string_view() const
		{
			return { buf_, len_ };
		}

	private:
		/**
		 * @brief Truncates `text` so that it would fit when inserted,
		 * respecting UTF-8 code point boundaries.
		 */
		[[nodiscard]] std::string_view truncateForInsertion(std::string_view text) const
		{
			const size_t newLength = len_ + text.size();
			if (newLength > maxLength_) {
				return TruncateUtf8(text, newLength - maxLength_);
			}
			return text;
		}

		char *buf_; // unowned
		size_t maxLength_;
		size_t len_;
	};

public:
	struct Options {
		char *value;            // unowned
		size_t *cursorPosition; // unowned
		size_t maxLength = 0;
	};
	TextInputState(const Options &options)
	    : value_(options.value, options.maxLength)
	    , cursorPosition_(options.cursorPosition)
	{
		*cursorPosition_ = value_.size();
	}

	[[nodiscard]] std::string_view value() const
	{
		return std::string_view(value_);
	}

	[[nodiscard]] bool empty() const
	{
		return value_.empty();
	}

	/**
	 * @brief Overwrites the value with the given text and moves cursor to the end.
	 */
	void assign(std::string_view text)
	{
		value_ = text;
		*cursorPosition_ = value_.size();
	}

	/**
	 * @brief Truncate to precisely `length` bytes.
	 */
	void truncate(size_t length)
	{
		if (length >= value().size())
			return;
		value_ = value().substr(0, length);
		*cursorPosition_ = std::min(*cursorPosition_, value_.size());
	}

	/**
	 * @brief Inserts the text at the current cursor position.
	 */
	void type(std::string_view text)
	{
		const size_t prevSize = value_.size();
		value_.insert(*cursorPosition_, text);
		*cursorPosition_ += value_.size() - prevSize;
	}

	void backspace()
	{
		if (*cursorPosition_ == 0)
			return;
		const size_t toRemove = *cursorPosition_ - FindLastUtf8Symbols(beforeCursor());
		*cursorPosition_ -= toRemove;
		value_.erase(*cursorPosition_, toRemove);
	}

	void del()
	{
		if (*cursorPosition_ == value_.size())
			return;
		value_.erase(*cursorPosition_, Utf8CodePointLen(afterCursor().data()));
	}

	void setCursorToStart()
	{
		*cursorPosition_ = 0;
	}

	void setCursorToEnd()
	{
		*cursorPosition_ = std::string_view(value_).size();
	}

	void moveCursorLeft()
	{
		if (*cursorPosition_ == 0)
			return;
		--*cursorPosition_;
	}

	void moveCursorRight()
	{
		if (*cursorPosition_ == value_.size())
			return;
		++*cursorPosition_;
	}

private:
	[[nodiscard]] std::string_view beforeCursor() const
	{
		return std::string_view(value_).substr(0, *cursorPosition_);
	}

	[[nodiscard]] std::string_view afterCursor() const
	{
		return std::string_view(value_).substr(*cursorPosition_);
	}

	Buffer value_;
	size_t *cursorPosition_; // unowned
};

bool HandleTextInputEvent(const SDL_Event &event, TextInputState &state);

} // namespace devilution

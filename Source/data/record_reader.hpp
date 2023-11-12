#pragma once

#include <string_view>
#include <type_traits>

#include <expected.hpp>
#include <function_ref.hpp>

#include "data/file.hpp"
#include "data/iterators.hpp"

namespace devilution {

/**
 * @brief A record reader that treats every error as fatal.
 */
class RecordReader {
public:
	RecordReader(DataFileRecord &record, std::string_view filename)
	    : it_(record.begin())
	    , end_(record.end())
	    , filename_(filename)
	{
	}

	template <typename T>
	typename std::enable_if_t<std::is_integral_v<T>, void>
	readInt(std::string_view name, T &out)
	{
		advance();
		DataFileField field = *it_;
		if (tl::expected<void, DataFileField::Error> result = field.parseInt(out); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename_, name, field);
		}
	}

	template <typename T>
	typename std::enable_if_t<std::is_integral_v<T>, void>
	readOptionalInt(std::string_view name, T &out)
	{
		advance();
		DataFileField field = *it_;
		if (field.value().empty()) return;
		if (tl::expected<void, DataFileField::Error> result = field.parseInt(out); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename_, name, field);
		}
	}

	template <typename T, size_t N>
	void readIntArray(std::string_view name, T (&out)[N])
	{
		advance();
		DataFileField field = *it_;
		if (tl::expected<void, DataFileField::Error> result = field.parseIntArray(out); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename_, name, field);
		}
	}

	template <typename T>
	typename std::enable_if_t<std::is_integral_v<T>, void>
	readFixed6(std::string_view name, T &out)
	{
		advance();
		DataFileField field = *it_;
		if (tl::expected<void, DataFileField::Error> result = field.parseFixed6(out); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename_, name, field);
		}
	}

	void readBool(std::string_view name, bool &out)
	{
		advance();
		DataFileField field = *it_;
		if (tl::expected<void, DataFileField::Error> result = field.parseBool(out); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename_, name, field);
		}
	}

	void readString(std::string_view name, std::string &out)
	{
		advance();
		out = (*it_).value();
	}

	template <typename T, typename F>
	void read(std::string_view name, T &out, F &&parseFn)
	{
		advance();
		DataFileField field = *it_;
		if (tl::expected<T, std::string> result = parseFn(field.value()); result.has_value()) {
			out = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename_, name, field, result.error());
		}
	}

	template <typename T, typename F>
	void readEnumList(std::string_view name, T &out, F &&parseFn)
	{
		advance();
		DataFileField field = *it_;
		if (tl::expected<void, std::string> result = field.parseEnumList(out, std::forward<F>(parseFn)); !result.has_value()) {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename_, name, field, result.error());
		}
	}

	std::string_view value()
	{
		advance();
		needsIncrement_ = false;
		return (*it_).value();
	}

	void advance();

private:
	FieldIterator it_;
	const FieldIterator end_;
	std::string_view filename_;
	bool needsIncrement_ = false;
};

} // namespace devilution

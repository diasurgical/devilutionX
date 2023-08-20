#pragma once

#include <memory>
#include <string_view>

#include <expected.hpp>

#include "iterators.hpp"

namespace devilution {
/**
 * @brief Container for a tab-delimited file following the TSV-like format described in txtdata/Readme.md
 */
class DataFile {
	std::unique_ptr<char[]> data_;
	std::string_view content_;

	DataFile() = delete;

	/**
	 * @brief Creates a view over a sequence of utf8 code units, skipping over the BOM if present
	 * @param data pointer to the raw data backing the view (this container will take ownership to ensure the lifetime of the view)
	 * @param size total number of bytes/code units including the BOM if present
	 */
	DataFile(std::unique_ptr<char[]> &&data, size_t size)
	    : data_(std::move(data))
	    , content_(data_.get(), size)
	{
		constexpr std::string_view utf8BOM = "\xef\xbb\xbf";
		if (this->content_.starts_with(utf8BOM))
			this->content_.remove_prefix(utf8BOM.size());
	}

public:
	enum class Error {
		NotFound,
		ReadError
	};

	/**
	 * @brief Attempts to load a data file (using the same mechanism as other runtime assets)
	 *
	 * @param path file to load including the /txtdata/ prefix
	 * @return an object containing an owned pointer to an in-memory copy of the file
	 *         or an error code describing the reason for failure.
	 */
	static tl::expected<DataFile, Error> load(std::string_view path);

	[[nodiscard]] RecordsRange records() const
	{
		return content_;
	}

	[[nodiscard]] const char *begin() const
	{
		return content_.data();
	}

	[[nodiscard]] const char *end() const
	{
		return content_.data() + content_.size();
	}

	[[nodiscard]] size_t size() const
	{
		return content_.size();
	}
};
} // namespace devilution

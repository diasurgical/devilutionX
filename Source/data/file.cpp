#include "file.hpp"

#include <fmt/format.h>

#include "engine/assets.hpp"
#include "utils/language.h"

namespace devilution {
tl::expected<DataFile, DataFile::Error> DataFile::load(std::string_view path)
{
	AssetRef ref = FindAsset(path);
	if (!ref.ok())
		return tl::unexpected { Error::NotFound };
	const size_t size = ref.size();
	// TODO: It should be possible to stream the data file contents instead of copying the whole thing into memory
	std::unique_ptr<char[]> data { new char[size] };
	{
		AssetHandle handle = OpenAsset(std::move(ref));
		if (!handle.ok())
			return tl::unexpected { Error::OpenFailed };
		if (size > 0 && !handle.read(data.get(), size))
			return tl::unexpected { Error::BadRead };
	}
	return DataFile { std::move(data), size };
}

void DataFile::reportFatalError(Error code, std::string_view fileName)
{
	switch (code) {
	case Error::NotFound:
	case Error::OpenFailed:
	case Error::BadRead:
		app_fatal(fmt::format(fmt::runtime(_(
		                          /* TRANSLATORS: Error message when a data file is missing or corrupt. Arguments are {file name} */
		                          "Unable to load data from file {0}")),
		    fileName));
	case Error::NoContent:
		app_fatal(fmt::format(fmt::runtime(_(
		                          /* TRANSLATORS: Error message when a data file is empty or only contains the header row. Arguments are {file name} */
		                          "{0} is incomplete, please check the file contents.")),
		    fileName));
	case Error::NotEnoughColumns:
		app_fatal(fmt::format(fmt::runtime(_(
		                          /* TRANSLATORS: Error message when a data file doesn't contain the expected columns. Arguments are {file name} */
		                          "Your {0} file doesn't have the expected columns, please make sure it matches the documented format.")),
		    fileName));
	}
}

void DataFile::reportFatalFieldError(DataFileField::Error code, std::string_view fileName, std::string_view fieldName, const DataFileField &field)
{
	switch (code) {
	case DataFileField::Error::NotANumber:
		app_fatal(fmt::format(fmt::runtime(_(
		                          /* TRANSLATORS: Error message when parsing a data file and a text value is encountered when a number is expected. Arguments are {found value}, {column heading}, {file name}, {row/record number}, {column/field number} */
		                          "Non-numeric value {0} for {1} in {2} at row {3} and column {4}")),
		    field.currentValue(), fieldName, fileName, field.row(), field.column()));
	case DataFileField::Error::OutOfRange:
		app_fatal(fmt::format(fmt::runtime(_(
		                          /* TRANSLATORS: Error message when parsing a data file and we find a number larger than expected. Arguments are {found value}, {column heading}, {file name}, {row/record number}, {column/field number} */
		                          "Out of range value {0} for {1} in {2} at row {3} and column {4}")),
		    field.currentValue(), fieldName, fileName, field.row(), field.column()));
	case DataFileField::Error::InvalidValue:
		app_fatal(fmt::format(fmt::runtime(_(
		                          /* TRANSLATORS: Error message when we find an unrecognised value in a key column. Arguments are {found value}, {column heading}, {file name}, {row/record number}, {column/field number} */
		                          "Invalid value {0} for {1} in {2} at row {3} and column {4}")),
		    field.currentValue(), fieldName, fileName, field.row(), field.column()));
	}
}

tl::expected<void, DataFile::Error> DataFile::parseHeader(ColumnDefinition *begin, ColumnDefinition *end, tl::function_ref<tl::expected<uint8_t, ColumnDefinition::Error>(std::string_view)> mapper)
{
	std::bitset<std::numeric_limits<uint8_t>::max()> seenColumns;
	unsigned lastColumn = 0;

	RecordIterator firstRecord { data(), data() + size(), false };
	for (DataFileField field : *firstRecord) {
		if (begin == end) {
			// All key columns have been identified
			break;
		}

		auto mapResult = mapper(*field);
		if (!mapResult.has_value()) {
			// not a key column
			continue;
		}

		uint8_t columnType = mapResult.value();
		if (seenColumns.test(columnType)) {
			// Repeated column? unusual, maybe this should be an error
			continue;
		}
		seenColumns.set(columnType);

		unsigned skipColumns = 0;
		if (field.column() > lastColumn)
			skipColumns = field.column() - lastColumn - 1;
		lastColumn = field.column();

		*begin = { columnType, skipColumns };
		++begin;
	}

	// Incrementing the iterator causes it to read to the end of the record in case we broke early (maybe there were extra columns)
	++firstRecord;
	if (firstRecord == this->end()) {
		return tl::unexpected { Error::NoContent };
	}

	body_ = firstRecord.data();

	if (begin != end) {
		return tl::unexpected { Error::NotEnoughColumns };
	}
	return {};
}

tl::expected<void, DataFile::Error> DataFile::skipHeader()
{
	RecordIterator it { data(), data() + size(), false };
	++it;
	if (it == this->end()) {
		return tl::unexpected { Error::NoContent };
	}
	body_ = it.data();
	return {};
}

} // namespace devilution

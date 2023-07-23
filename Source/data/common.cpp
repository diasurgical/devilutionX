#include "common.hpp"

#include "engine/assets.hpp"

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
		if (!handle.ok() || !handle.read(data.get(), size))
			return tl::unexpected { Error::ReadError };
	}
	return DataFile { std::move(data), size };
}

GetFieldResult HandleRecordSeparator(const char *begin, const char *end)
{
	if (begin == end) {
		return { end, GetFieldResult::Status::EndOfFile };
	}

	if (*begin == '\r') {
		++begin;
		if (begin == end) {
			return { end, GetFieldResult::Status::FileTruncated };
		}
		// carriage returns should be followed by a newline, so let's let the following checks handle it
	}
	if (*begin == '\n') {
		return { begin + 1, GetFieldResult::Status::EndOfRecord };
	}

	return { begin, GetFieldResult::Status::BadRecordSeparator };
}

GetFieldResult DiscardMultipleFields(const char *begin, const char *end, unsigned skipLength)
{
	GetFieldResult result { begin };
	while (skipLength > 0) {
		result = DiscardField(result.next, end);
		if (result.endOfRecord()) {
			// Found the end of record early, we can reuse the error code so just return it
			return result;
		}
		--skipLength;
	}
	return result;
}
} // namespace devilution

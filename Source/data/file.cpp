#include "file.hpp"

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
} // namespace devilution

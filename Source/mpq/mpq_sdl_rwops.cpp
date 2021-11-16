#include "mpq/mpq_sdl_rwops.hpp"

#include <cstring>
#include <memory>
#include <vector>

namespace devilution {

namespace {

struct Data {
	// File information:
	std::optional<MpqArchive> ownedArchive;
	MpqArchive *mpqArchive;
	uint32_t fileNumber;
	uint32_t blockSize;
	uint32_t lastBlockSize;
	uint32_t numBlocks;
	uint32_t size;

	// State:
	uint32_t position;
	bool blockRead;
	std::unique_ptr<uint8_t[]> blockData;
};

Data *GetData(struct SDL_RWops *context)
{
	return reinterpret_cast<Data *>(context->hidden.unknown.data1);
}

void SetData(struct SDL_RWops *context, Data *data)
{
	context->hidden.unknown.data1 = data;
}

#ifndef USE_SDL1
using OffsetType = Sint64;
using SizeType = size_t;
#else
using OffsetType = int;
using SizeType = int;
#endif

extern "C" {

#ifndef USE_SDL1
static Sint64 MpqFileRwSize(struct SDL_RWops *context)
{
	return GetData(context)->size;
}
#endif

static OffsetType MpqFileRwSeek(struct SDL_RWops *context, OffsetType offset, int whence)
{
	Data &data = *GetData(context);
	OffsetType newPosition;
	switch (whence) {
	case RW_SEEK_SET:
		newPosition = offset;
		break;
	case RW_SEEK_CUR:
		newPosition = data.position + offset;
		break;
	case RW_SEEK_END:
		newPosition = data.size + offset;
		break;
	default:
		return -1;
	}

	if (newPosition == data.position)
		return newPosition;

	if (newPosition > data.size) {
		SDL_SetError("MpqFileRwSeek beyond EOF (%d > %u)", static_cast<int>(newPosition), data.size);
		return -1;
	}

	if (newPosition < 0) {
		SDL_SetError("MpqFileRwSeek beyond BOF (%d < 0)", static_cast<int>(newPosition));
		return -1;
	}

	if (data.position / data.blockSize != newPosition / data.blockSize)
		data.blockRead = false;

	data.position = newPosition;

	return newPosition;
}

static SizeType MpqFileRwRead(struct SDL_RWops *context, void *ptr, SizeType size, SizeType maxnum)
{
	Data &data = *GetData(context);
	const SizeType totalSize = size * maxnum;
	SizeType remainingSize = totalSize;

	auto *out = static_cast<uint8_t *>(ptr);

	if (data.blockData == nullptr) {
		data.blockData = std::unique_ptr<uint8_t[]> { new uint8_t[data.blockSize] };
	}

	uint32_t blockNumber = data.position / data.blockSize;
	while (remainingSize > 0) {
		if (data.position == data.size) {
			SDL_SetError("MpqFileRwRead beyond EOF by %u bytes", static_cast<unsigned>(remainingSize));
			break;
		}

		const uint32_t currentBlockSize = blockNumber + 1 == data.numBlocks ? data.lastBlockSize : data.blockSize;

		if (!data.blockRead) {
			const int32_t error = data.mpqArchive->ReadBlock(data.fileNumber, blockNumber, data.blockData.get(), currentBlockSize);
			if (error != 0) {
				SDL_SetError("MpqFileRwRead ReadBlock: %s", MpqArchive::ErrorMessage(error));
				return 0;
			}
			data.blockRead = true;
		}

		const uint32_t blockPosition = data.position - blockNumber * data.blockSize;
		const uint32_t remainingBlockSize = currentBlockSize - blockPosition;

		if (remainingSize < remainingBlockSize) {
			std::memcpy(out, data.blockData.get() + blockPosition, remainingSize);
			data.position += remainingSize;
			return maxnum;
		}

		std::memcpy(out, data.blockData.get() + blockPosition, remainingBlockSize);
		out += remainingBlockSize;
		data.position += remainingBlockSize;
		remainingSize -= remainingBlockSize;
		++blockNumber;
		data.blockRead = false;
	}

	return (totalSize - remainingSize) / size;
}

static int MpqFileRwClose(struct SDL_RWops *context)
{
	Data *data = GetData(context);
	data->mpqArchive->CloseBlockOffsetTable(data->fileNumber);
	delete data;
	delete context;
	return 0;
}

} // extern "C"

} // namespace

SDL_RWops *SDL_RWops_FromMpqFile(MpqArchive &mpqArchive, uint32_t fileNumber, const char *filename, bool threadsafe)
{
	auto result = std::make_unique<SDL_RWops>();
	std::memset(result.get(), 0, sizeof(*result));

#ifndef USE_SDL1
	result->size = &MpqFileRwSize;
	result->type = SDL_RWOPS_UNKNOWN;
#else
	result->type = 0;
#endif

	result->seek = &MpqFileRwSeek;
	result->read = &MpqFileRwRead;
	result->write = nullptr;
	result->close = &MpqFileRwClose;

	auto data = std::make_unique<Data>();
	int32_t error = 0;

	if (threadsafe) {
		data->ownedArchive = mpqArchive.Clone(error);
		if (error != 0) {
			SDL_SetError("MpqFileRwRead Clone: %s", MpqArchive::ErrorMessage(error));
			return nullptr;
		}
		data->mpqArchive = &*data->ownedArchive;
	} else {
		data->mpqArchive = &mpqArchive;
	}
	data->fileNumber = fileNumber;
	MpqArchive &archive = *data->mpqArchive;

	error = archive.OpenBlockOffsetTable(fileNumber, filename);
	if (error != 0) {
		SDL_SetError("MpqFileRwRead OpenBlockOffsetTable: %s", MpqArchive::ErrorMessage(error));
		return nullptr;
	}

	data->size = archive.GetUnpackedFileSize(fileNumber, error);
	if (error != 0) {
		SDL_SetError("MpqFileRwRead GetUnpackedFileSize: %s", MpqArchive::ErrorMessage(error));
		return nullptr;
	}

	const std::uint32_t numBlocks = archive.GetNumBlocks(fileNumber, error);
	if (error != 0) {
		SDL_SetError("MpqFileRwRead GetNumBlocks: %s", MpqArchive::ErrorMessage(error));
		return nullptr;
	}
	data->numBlocks = numBlocks;

	const std::uint32_t blockSize = archive.GetBlockSize(fileNumber, 0, error);
	if (error != 0) {
		SDL_SetError("MpqFileRwRead GetBlockSize: %s", MpqArchive::ErrorMessage(error));
		return nullptr;
	}
	data->blockSize = blockSize;

	if (numBlocks > 1) {
		data->lastBlockSize = archive.GetBlockSize(fileNumber, numBlocks - 1, error);
		if (error != 0) {
			SDL_SetError("MpqFileRwRead GetBlockSize: %s", MpqArchive::ErrorMessage(error));
			return nullptr;
		}
	} else {
		data->lastBlockSize = blockSize;
	}

	data->position = 0;
	data->blockRead = false;

	SetData(result.get(), data.release());
	return result.release();
}

} // namespace devilution

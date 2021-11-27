#include "mpq/mpq_reader.hpp"

#include <libmpq/mpq.h>

#include "utils/stdcompat/optional.hpp"

namespace devilution {

std::optional<MpqArchive> MpqArchive::Open(const char *path, int32_t &error)
{
	mpq_archive_s *archive;
	error = libmpq__archive_open(&archive, path, -1);
	if (error != 0) {
		if (error == LIBMPQ_ERROR_EXIST)
			error = 0;
		return std::nullopt;
	}
	return MpqArchive { std::string(path), archive };
}

std::optional<MpqArchive> MpqArchive::Clone(int32_t &error)
{
	mpq_archive_s *copy;
	error = libmpq__archive_dup(archive_, path_.c_str(), &copy);
	if (error != 0)
		return std::nullopt;
	return MpqArchive { path_, copy };
}

const char *MpqArchive::ErrorMessage(int32_t errorCode)
{
	return libmpq__strerror(errorCode);
}

MpqArchive::FileHash MpqArchive::CalculateFileHash(const char *filename)
{
	FileHash fileHash;
	libmpq__file_hash(filename, &fileHash[0], &fileHash[1], &fileHash[2]);
	return fileHash;
}

MpqArchive &MpqArchive::operator=(MpqArchive &&other) noexcept
{
	path_ = std::move(other.path_);
	if (archive_ != nullptr)
		libmpq__archive_close(archive_);
	archive_ = other.archive_;
	tmp_buf_ = std::move(other.tmp_buf_);
	return *this;
}

MpqArchive::~MpqArchive()
{
	if (archive_ != nullptr)
		libmpq__archive_close(archive_);
}

bool MpqArchive::GetFileNumber(MpqArchive::FileHash fileHash, uint32_t &fileNumber)
{
	return libmpq__file_number_from_hash(archive_, fileHash[0], fileHash[1], fileHash[2], &fileNumber) == 0;
}

std::unique_ptr<byte[]> MpqArchive::ReadFile(const char *filename, std::size_t &fileSize, int32_t &error)
{
	std::unique_ptr<byte[]> result;
	std::uint32_t fileNumber;
	error = libmpq__file_number(archive_, filename, &fileNumber);
	if (error != 0)
		return result;

	libmpq__off_t unpackedSize;
	error = libmpq__file_size_unpacked(archive_, fileNumber, &unpackedSize);
	if (error != 0)
		return result;

	error = OpenBlockOffsetTable(fileNumber, filename);
	if (error != 0)
		return result;

	result = std::make_unique<byte[]>(unpackedSize);

	const std::size_t blockSize = GetBlockSize(fileNumber, 0, error);
	if (error != 0)
		return result;

	std::vector<std::uint8_t> &tmp = GetTemporaryBuffer(blockSize);
	if (error != 0)
		return result;

	error = libmpq__file_read_with_filename_and_temporary_buffer(
	    archive_, fileNumber, filename, reinterpret_cast<std::uint8_t *>(result.get()), unpackedSize,
	    tmp.data(), static_cast<libmpq__off_t>(blockSize), nullptr);
	if (error != 0) {
		result = nullptr;
		CloseBlockOffsetTable(fileNumber);
		return result;
	}
	CloseBlockOffsetTable(fileNumber);

	fileSize = unpackedSize;
	return result;
}

int32_t MpqArchive::ReadBlock(uint32_t fileNumber, uint32_t blockNumber, uint8_t *out, uint32_t outSize)
{
	std::vector<std::uint8_t> &tmpBuf = GetTemporaryBuffer(outSize);
	return libmpq__block_read_with_temporary_buffer(
	    archive_, fileNumber, blockNumber, out, static_cast<libmpq__off_t>(outSize),
	    tmpBuf.data(), outSize,
	    /*transferred=*/nullptr);
}

std::size_t MpqArchive::GetUnpackedFileSize(uint32_t fileNumber, int32_t &error)
{
	libmpq__off_t unpackedSize;
	error = libmpq__file_size_unpacked(archive_, fileNumber, &unpackedSize);
	return unpackedSize;
}

uint32_t MpqArchive::GetNumBlocks(uint32_t fileNumber, int32_t &error)
{
	uint32_t numBlocks;
	error = libmpq__file_blocks(archive_, fileNumber, &numBlocks);
	return numBlocks;
}

int32_t MpqArchive::OpenBlockOffsetTable(uint32_t fileNumber, const char *filename)
{
	return libmpq__block_open_offset_with_filename(archive_, fileNumber, filename);
}

int32_t MpqArchive::CloseBlockOffsetTable(uint32_t fileNumber)
{
	return libmpq__block_close_offset(archive_, fileNumber);
}

// Requires the block offset table to be open
std::size_t MpqArchive::GetBlockSize(uint32_t fileNumber, uint32_t blockNumber, int32_t &error)
{
	libmpq__off_t blockSize;
	error = libmpq__block_size_unpacked(archive_, fileNumber, blockNumber, &blockSize);
	return blockSize;
}

} // namespace devilution

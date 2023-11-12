#include "data/record_reader.hpp"

namespace devilution {

void RecordReader::advance()
{
	if (needsIncrement_) {
		++it_;
	} else {
		needsIncrement_ = true;
	}
	if (it_ == end_) {
		DataFile::reportFatalError(DataFile::Error::NotEnoughColumns, filename_);
	}
}

} // namespace devilution

#include "utils/logged_fstream.hpp"

namespace devilution {

const char *LoggedFStream::DirToString(int dir)
{
	switch (dir) {
	case SEEK_SET:
		return "SEEK_SET";
	case SEEK_END:
		return "SEEK_END";
	case SEEK_CUR:
		return "SEEK_CUR";
	default:
		return "invalid";
	}
}

} // namespace devilution

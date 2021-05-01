#pragma once

#include <vector>
#include <tuple>

#include "../miniwin/miniwin.h"

namespace devilution {

class Recorder final {
public:
	enum class Status {
		Stopped,
		Recording,
		Replaying,
	};

	Recorder() = default;

	void onMessage(const tagMSG &msg, std::uint64_t frameCount);
	void setStatus(Status status);
	Status getStatus() const
	{
		return status;
	}
	bool replay(tagMSG &msg, std::uint64_t frameCount);

private:
	std::vector<std::tuple<std::uint64_t, tagMSG>> record;
	Status status {};
	std::uint64_t startFrame {};
	std::size_t recordIndex {};
};

} // namespace devilution

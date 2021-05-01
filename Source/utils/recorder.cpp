#include "utils/recorder.hpp"

#include <SDL_timer.h>

#include "utils/log.hpp"

namespace devilution {

void Recorder::onMessage(const tagMSG &msg, std::uint64_t frameCount)
{
	if (status != Status::Recording)
		return;

	if (!startFrame)
		startFrame = frameCount;

	record.emplace_back(frameCount - startFrame, msg);
}

void Recorder::setStatus(Status status)
{
	if (this->status == status)
		return;

	this->status = status;

	switch (status) {
	case Status::Replaying:
		Log("Replaying {} messages", record.size());
		recordIndex = 0;
		break;
	case Status::Recording:
		record.clear();
		break;
	case Status::Stopped:
		startFrame = 0;
		break;
	}
}

bool Recorder::replay(tagMSG &msg, std::uint64_t frameCount)
{
	if (status != Status::Replaying)
		return false;

	if (!startFrame)
		startFrame = frameCount;

	if (recordIndex >= record.size()) {
		Log("Replaying finished");
		setStatus(Status::Stopped);
		return false;
	}

	auto time = frameCount - startFrame;
	const auto &rec = record[recordIndex];

	auto recTime = std::get<std::uint64_t>(rec);
	if (time >= recTime) {
		msg = std::get<tagMSG>(rec);
		++recordIndex;
		return true;
	}

	return false;
}

} // namespace devilution

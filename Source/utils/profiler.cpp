#include <utils/profiler.h>

#include <algorithm>
#include <unordered_map>

#include <windows.h>
#include <SDL.h>

std::unordered_map<std::string, TimingInfo> timings;

FunctionProfiler::FunctionProfiler(std::string name)
    : _name(name)
{
	LARGE_INTEGER frequency;
	if (QueryPerformanceFrequency(&frequency)) {
		_frequency = frequency.QuadPart;
	} else {
		_frequency = 1000;
	}

	LARGE_INTEGER ticks;
	if (QueryPerformanceCounter(&ticks)) {
		_start = ticks.QuadPart;
	} else {
		_start = SDL_GetTicks() * _frequency / 1000;
	}
}

FunctionProfiler::~FunctionProfiler()
{
	LARGE_INTEGER ticks;
	uint64_t now;
	if (QueryPerformanceCounter(&ticks)) {
		now = ticks.QuadPart;
	} else {
		now = SDL_GetTicks() * _frequency / 1000;
	}
	double ms = (double)(now - _start) * 1000.0 / (double)_frequency;

	TimingInfo &timing = timings[_name];
	timing.ms += ms;
	timing.count++;
}

static bool CompareTiming(const TimingInfo &a, const TimingInfo &b)
{
	return a.ms > b.ms;
}

std::vector<TimingInfo> FunctionProfiler::Dump()
{
	std::vector<TimingInfo> timingList;
	for (auto it = timings.begin(); it != timings.end(); ++it) {
		it->second.name = it->first;
		timingList.push_back(it->second);
	}
	std::sort(timingList.begin(), timingList.end(), CompareTiming);
	return timingList;
}

#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct TimingInfo {
	std::string name;
	double ms;
	int count;
};

class FunctionProfiler {
public:
    FunctionProfiler(std::string name);
    ~FunctionProfiler();
    static std::vector<TimingInfo> Dump();
private:
	std::string _name;
	uint64_t _frequency;
	uint64_t _start;
};

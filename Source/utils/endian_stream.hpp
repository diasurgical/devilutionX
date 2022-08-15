#pragma once

#include <cstring>

#include <fstream>

#include "utils/endian.hpp"

namespace devilution {

template <typename T = uint8_t>
T ReadByte(std::ifstream &stream)
{
	static_assert(sizeof(T) == 1, "invalid argument");
	char buf;
	stream.read(&buf, 1);
	return static_cast<T>(buf);
}

template <typename T = uint16_t>
T ReadLE16(std::ifstream &stream)
{
	static_assert(sizeof(T) == 2, "invalid argument");
	char buf[2];
	stream.read(buf, 2);
	return static_cast<T>(LoadLE16(buf));
}

template <typename T = uint32_t>
T ReadLE32(std::ifstream &stream)
{
	static_assert(sizeof(T) == 4, "invalid argument");
	char buf[4];
	stream.read(buf, 4);
	return static_cast<T>(LoadLE32(buf));
}

inline float ReadLEFloat(std::ifstream &stream)
{
	static_assert(sizeof(float) == sizeof(uint32_t), "invalid float size");
	const uint32_t val = ReadLE32(stream);
	float result;
	memcpy(&result, &val, sizeof(float));
	return result;
}

inline void WriteByte(std::ofstream &out, uint8_t val)
{
	out.write(reinterpret_cast<const char *>(&val), 1);
}

inline void WriteLE16(std::ofstream &out, uint16_t val)
{
	char data[2];
	WriteLE16(data, val);
	out.write(data, 2);
}

inline void WriteLE32(std::ofstream &out, uint32_t val)
{
	char data[4];
	WriteLE32(data, val);
	out.write(data, 4);
}

inline void WriteLEFloat(std::ofstream &out, float val)
{
	static_assert(sizeof(float) == sizeof(uint32_t), "invalid float size");
	uint32_t intVal;
	memcpy(&intVal, &val, sizeof(uint32_t));
	char data[4];
	WriteLE32(data, intVal);
	out.write(data, 4);
}

} // namespace devilution

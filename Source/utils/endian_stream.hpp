#pragma once

#include <cstdio>
#include <cstring>

#include "utils/endian.hpp"

namespace devilution {

template <typename T = uint8_t>
T ReadByte(FILE *stream)
{
	static_assert(sizeof(T) == 1, "invalid argument");
	char buf;
	std::fread(&buf, sizeof(buf), 1, stream);
	return static_cast<T>(buf);
}

template <typename T = uint16_t>
T ReadLE16(FILE *stream)
{
	static_assert(sizeof(T) == 2, "invalid argument");
	char buf[2];
	std::fread(buf, sizeof(buf), 1, stream);
	return static_cast<T>(LoadLE16(buf));
}

template <typename T = uint32_t>
T ReadLE32(FILE *stream)
{
	static_assert(sizeof(T) == 4, "invalid argument");
	char buf[4];
	std::fread(buf, sizeof(buf), 1, stream);
	return static_cast<T>(LoadLE32(buf));
}

inline float ReadLEFloat(FILE *stream)
{
	static_assert(sizeof(float) == sizeof(uint32_t), "invalid float size");
	const uint32_t val = ReadLE32(stream);
	float result;
	memcpy(&result, &val, sizeof(float));
	return result;
}

inline void WriteByte(FILE *out, uint8_t val)
{
	std::fwrite(&val, sizeof(val), 1, out);
}

inline void WriteLE16(FILE *out, uint16_t val)
{
	char data[2];
	WriteLE16(data, val);
	std::fwrite(data, sizeof(data), 1, out);
}

inline void WriteLE32(FILE *out, uint32_t val)
{
	char data[4];
	WriteLE32(data, val);
	std::fwrite(data, sizeof(data), 1, out);
}

inline void WriteLEFloat(FILE *out, float val)
{
	static_assert(sizeof(float) == sizeof(uint32_t), "invalid float size");
	uint32_t intVal;
	memcpy(&intVal, &val, sizeof(uint32_t));
	char data[4];
	WriteLE32(data, intVal);
	std::fwrite(data, sizeof(data), 1, out);
}

} // namespace devilution

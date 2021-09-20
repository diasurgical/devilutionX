#pragma once

#include "engine/random.hpp"

namespace devilution {

struct Damage {
	int minValue;
	int maxValue;

	Damage() = default;

	constexpr Damage(int fixedValue)
	    : Damage(fixedValue, fixedValue)
	{
	}

	constexpr Damage(int minValue, int maxValue)
	    : minValue(minValue)
	    , maxValue(maxValue)
	{
	}

	[[nodiscard]] bool IsFixed() const
	{
		return minValue == maxValue;
	}

	int GetValue() const
	{
		return IsFixed()
		    ? minValue
		    : minValue + GenerateRnd(maxValue - minValue + 1);
	}

	constexpr bool operator==(const Damage &other) const
	{
		return minValue == other.minValue && maxValue == other.maxValue;
	}

	constexpr bool operator!=(const Damage &other) const
	{
		return !(*this == other);
	}

	constexpr Damage operator/=(const int factor)
	{
		minValue /= factor;
		maxValue /= factor;
		return *this;
	}

	constexpr friend Damage operator/(Damage damage, const int factor)
	{
		damage /= factor;
		return damage;
	}
};

} // namespace devilution

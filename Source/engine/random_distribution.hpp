/**
 * @file random_distribution.hpp
 *
 * Definition for the random number distribution function objects used in Diablo dungeon/item generation functions
 */
#pragma once

#include <istream>
#include <limits>
#include <ostream>

#include "utils/stdcompat/abs.hpp"

namespace devilution {

/**
 * @brief This type of distribution attempts to mask the MSB off a value assumed to come from a LCG using abs()
 *
 * If the generator yields a value that casts to INT_MIN we can't reliably take the absolute value. In this case
 * return either 0 or INT_MIN (a large negative value) based on the params used when creating the distribution
 */
struct AbsoluteDistribution {
	using result_type = int32_t;

	struct param_type {
		using distribution_type = AbsoluteDistribution;

		param_type()
		    : allowIntMin(false)
		{
		}

		explicit param_type(bool allowIntMin)
		    : allowIntMin(allowIntMin)
		{
		}

		bool allowIntMin;

		[[nodiscard]] friend bool operator==(const param_type &left, const param_type &right)
		{
			return left.allowIntMin == right.allowIntMin;
		}
	};

	AbsoluteDistribution()
	    : param_()
	{
	}

	explicit AbsoluteDistribution(bool allowIntMin)
	    : param_(allowIntMin)
	{
	}

	void reset()
	{
	}

	[[nodiscard]] param_type param() const
	{
		return param_;
	}

	void param(param_type &param)
	{
		param_ = param;
	}

	template <class Generator>
	result_type operator()(Generator &generator, const param_type &param = {})
	{
		result_type value = static_cast<result_type>(generator());
		// in an attempt to mask off the MSB the returned value is converted to a positive value
		// however since abs(INT_MIN) is undefined behavior, need to handle this value specially
		return value == std::numeric_limits<int32_t>::min() ? min() : abs(value);
	}

	[[nodiscard]] result_type min() const
	{
		return param_.allowIntMin ? std::numeric_limits<result_type>::min() : 0;
	}

	[[nodiscard]] result_type max() const
	{
		return std::numeric_limits<result_type>::max();
	}

	[[nodiscard]] friend bool operator==(const AbsoluteDistribution &left, const AbsoluteDistribution &right)
	{
		return left.param() == right.param();
	}

	template <class CharT, class Traits>
	friend std::basic_ostream<CharT, Traits> &operator<<(std::basic_istream<CharT, Traits> &stream, const AbsoluteDistribution &distribution)
	{
		return stream << distribution.param_.allowIntMin;
	}

	template <class CharT, class Traits>
	friend std::basic_istream<CharT, Traits> &operator>>(std::basic_istream<CharT, Traits> &stream, AbsoluteDistribution &distribution)
	{
		AbsoluteDistribution::param_type params;
		stream >> params.allowIntMin;
		distribution.param(params);
		return stream;
	}

private:
	param_type param_;
};

/**
 * @brief This type of distribution uses the provided generator to get a random number within [min, max) in at most a single call to g()
 *
 * The distribution is slightly biased towards lower values unless the difference between min and max is a power of 2.
 * This distribution function assumes the generator is a LCG engine and tries to compensate for biases in the sequence given by that type of
 * engine. It attempts to discard the MSB by casting the unsigned value yielded by the engine to a signed value and taking the absolute value
 * of the result. This can fail if the cast results in INT_MIN, so depending on the parameters provided we use either 0 or keep it unchanged.
 * If the difference between min and max is not a power of 2 and you've set the allowIntMin parameter to true then this will return a value
 * less than the min parameter.
 */
class DiabloDistribution {
public:
	using result_type = int32_t;

	struct param_type {
		using distribution_type = DiabloDistribution;

		param_type()
		    : min(0)
		    , max(std::numeric_limits<result_type>::max())
		    , allowIntMin(false)
		{
		}

		explicit param_type(bool allowIntMin)
		    : min(0)
		    , max(std::numeric_limits<result_type>::max())
		    , allowIntMin(allowIntMin)
		{
		}

		explicit param_type(result_type max, bool allowIntMin = false)
		    : min(0)
		    , max(max)
		    , allowIntMin(allowIntMin)
		{
		}

		explicit param_type(result_type min, result_type max)
		    : min(min)
		    , max(max)
		    , allowIntMin(false)
		{
		}

		result_type min;
		result_type max;
		bool allowIntMin;

		[[nodiscard]] friend bool operator==(const param_type &left, const param_type &right)
		{
			return left.min == right.min && left.max == right.max && left.allowIntMin == right.allowIntMin;
		}
	};

	DiabloDistribution()
	    : param_()
	{
	}

	explicit DiabloDistribution(result_type max, bool allowIntMin = false)
	    : param_(max, allowIntMin)
	{
	}

	explicit DiabloDistribution(result_type min, result_type max)
	    : param_(min, max)
	{
	}

	void reset()
	{
	}

	[[nodiscard]] param_type param() const
	{
		return param_;
	}

	void param(param_type &param)
	{
		param_ = param;
	}

	template <class Generator>
	result_type operator()(Generator &generator)
	{
		return this->operator()(generator, param());
	}

	template <class Generator>
	result_type operator()(Generator &generator, const param_type &param)
	{
		if (param.max <= param.min)
			return 0;

		result_type value = static_cast<result_type>(generator());

		if (value == std::numeric_limits<int32_t>::min())
			// in an attempt to mask off the MSB the returned value is converted to a positive value
			// however since abs(INT_MIN) is undefined behavior, need to handle this value specially
			value = param_.allowIntMin ? std::numeric_limits<result_type>::min() : 0;
		else
			value = abs(value);

		if (param.max - param.min <= 0x7FFF) // This distribution assumes it is called with a LCG, for smaller values it uses the high bits to correct for LCG bias
			value >>= 16;

		return param.min + value % (param.max - param.min);
	}

	[[nodiscard]] result_type min() const
	{
		return param_.allowIntMin ? param_.min + std::numeric_limits<result_type>::min() % (param_.max - param_.min) : param_.min;
	}

	[[nodiscard]] result_type max() const
	{
		return param_.max - 1;
	}

	[[nodiscard]] friend bool operator==(const DiabloDistribution &left, const DiabloDistribution &right)
	{
		return left.param() == right.param();
	}

	template <class CharT, class Traits>
	friend std::basic_ostream<CharT, Traits> &operator<<(std::basic_istream<CharT, Traits> &stream, const DiabloDistribution &distribution)
	{
		return stream << distribution.param_.min << ' ' << distribution.param_.max << ' ' << distribution.param_.allowIntMin;
	}

	template <class CharT, class Traits>
	friend std::basic_istream<CharT, Traits> &operator>>(std::basic_istream<CharT, Traits> &stream, DiabloDistribution &distribution)
	{
		DiabloDistribution::param_type params;
		stream >> params.min >> params.max >> params.allowIntMin;
		distribution.param(params);
		return stream;
	}

private:
	param_type param_;
};

} // namespace devilution

// Copyright Dmitro bjornus Szewczuk 2017 under zlib license

#pragma once

#include <string>

#include "Named.hpp"

namespace radon
{
	class Key final
		: public Named
	{
	public:

		Key() = default;

		Key(const std::string & name, const std::string & value);

		Key(const std::string & name, float value);

		std::string getStringValue() const;

		float getFloatValue() const;

		void setValue(float value);

		void setValue(const std::string & value);

	private:
		std::string value;
	};
}
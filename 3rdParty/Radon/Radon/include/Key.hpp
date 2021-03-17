// Copyright Dmitro bjornus Szewczuk 2017 under zlib license

#pragma once

#include <string>

#include "Named.hpp"

namespace radon
{
	class Key
		: public Named
	{
	public:

		Key();

		Key(const std::string & name, const std::string & value);

		Key(const std::string & name, float value);

		std::string getStringValue();

		float getFloatValue();

		void setValue(float value);

		void setValue(std::string & value);

	private:
		std::string value;

		friend class File;
	};
}
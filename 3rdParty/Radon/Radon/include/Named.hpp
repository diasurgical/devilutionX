// Copyright Dmitro bjornus Szewczuk 2017 under zlib license

#pragma once

#include <string>

namespace radon
{
	class Named
	{
	public:

		Named();

		Named(const std::string & name);

		void setName(const std::string & name);

		std::string getName();

	protected:
		std::string name;
	};
}
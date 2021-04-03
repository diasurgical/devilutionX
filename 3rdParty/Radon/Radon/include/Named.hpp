// Copyright Dmitro bjornus Szewczuk 2017 under zlib license

#pragma once

#include <string>

namespace radon
{
	class Named
	{
	public:

		Named() = default;

		Named(const std::string & name);

		virtual ~Named() = default;

		void setName(const std::string & name);

		const std::string & getName() const;

	protected:
		std::string name{"You need to set a name!"};
	};
}
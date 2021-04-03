// Copyright Dmitro bjornus Szewczuk 2017 under zlib license

#pragma once

#include "Section.hpp"

#include <string>
#include <vector>

namespace radon
{
	class File final
	{
	public:

		File(const std::string & path, bool reading = true);

		Section* getSection(const std::string & name);
		
		const Section* getSection(const std::string & name) const;

		void addSection(const std::string & name);

		void saveToFile() const;

	private:
		std::vector<Section> sections;
		std::string path;
	};
}
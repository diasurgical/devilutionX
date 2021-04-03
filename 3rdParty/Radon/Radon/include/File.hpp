// Copyright Dmitro bjornus Szewczuk 2017 under zlib license

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace radon
{
	class Section;

	class File
	{
	public:

		File(const std::string & path, bool reading = true);

		Section* getSection(const std::string & name);

		void addSection(const std::string & name);

		void saveToFile();

	private:
		std::vector<Section> sections;
		std::string path;
	};
}
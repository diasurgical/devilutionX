// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/File.hpp"
#include "../include/Key.hpp"

#include <fstream>
#include <algorithm>
#include <iostream>
namespace radon
{
	File::File(const std::string & path, bool read)
		: path(path)
	{
		if (!read)
			return;
	
		std::ifstream stream(path);
		if (!stream.is_open())
			return;

		std::string buffer;
		std::string nameOfCurrent;

		while (std::getline(stream, buffer))
		{
			if (buffer[0] == ';' || buffer[0] == '#')
				continue;
			if (buffer[0] == '[')
			{
				nameOfCurrent = buffer.substr(buffer.find("[") + 1, buffer.find("]") - 1);
				sections.emplace_back(nameOfCurrent);
			}
			else
			{
				int equalsPosition = buffer.find('=');

				std::string nameOfElement = buffer.substr(0, equalsPosition);
				std::string valueOfElement = buffer.substr(equalsPosition + 1, buffer.size());

				sections.back().addKey({nameOfElement, valueOfElement});
			}
		}
	}


	Section *File::getSection(const std::string & name)
	{
		for (auto &section: sections)
		{
			if (section.getName() == name)
			{
				return &section;
			}
		}

		return nullptr;
	}


	const Section *File::getSection(const std::string & name) const
	{
		for (const auto &section: sections)
		{
			if (section.getName() == name)
			{
				return &section;
			}
		}

		return nullptr;
	}


	void File::addSection(const std::string & name)
	{
		sections.emplace_back(name);
	}


	void File::saveToFile() const
	{
		std::ofstream stream(path, std::ios::out | std::ios::trunc);

		if (!stream.is_open())
			return;

		for (const auto &section: sections)
		{
			stream << "[" << section.getName() << "] \n";
			for (const auto &key: section.getKeys())
			{
				stream << key.getName() << "=" << key.getStringValue() << "\n";
			}
		}
	}
}
// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Radon.hpp"

#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <assert.h>

namespace radon
{
	File::File(const std::string & path, bool reading)
	{
		this->path = path;
		if (reading)
		{
			std::ifstream stream(path);

			if (stream.is_open())
			{
				std::string buffer;
				std::string nameOfCurrent = "";

				while (std::getline(stream, buffer))
				{
					if (buffer[0] == ';' || buffer[0] == '#') continue;
					if (buffer[0] == '[')
					{
						nameOfCurrent = buffer.substr(buffer.find("[") + 1, buffer.find("]") - 1);
						sections.push_back(Section(nameOfCurrent));
					}
					else
					{
						int equalsPosition = buffer.find('=');

						std::string nameOfElement = buffer.substr(0, equalsPosition);
						std::string valueOfElement = buffer.substr(equalsPosition + 1, buffer.size());

						sections.back().addKey(Key(nameOfElement, valueOfElement));
					}
				}
			}
		}
	}


	Section *File::getSection(const std::string & name)
	{
		for (auto & section : sections)
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
		sections.push_back(Section(name));
	}


	void File::saveToFile()
	{
		std::ofstream file(path.data(), std::ios::out | std::ios::trunc);

		for (auto & section : sections)
		{
			file << "[" << section.getName() << "] \n";
			for (auto & key : section.keys)
			{
				file << key.getName() << "=" << key.getStringValue() << "\n";
			}
		}
		file.close();
	}
}

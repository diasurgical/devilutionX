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
			std::ifstream stream(path.c_str());

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
		for (int i = 0; i < (int)sections.size(); i++)
		{
			if (sections[i].getName() == name)
			{
				return &sections[i];
			}
		}

		return NULL;
	}


	void File::addSection(const std::string & name)
	{
		sections.push_back(Section(name));
	}


	void File::saveToFile()
	{
		std::ofstream file(path.data(), std::ios::out | std::ios::trunc);

		for (int i = 0; i < (int)sections.size(); i++)
		{
			file << "[" << sections[i].getName() << "] \n";
			for(int j = 0; j < (int)sections[i].keys.size(); j++)
			{
				file << sections[i].keys[j].getName() << "=" << sections[i].keys[j].getStringValue() << "\n";
			}
		}
		file.close();
	}
}
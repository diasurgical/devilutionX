// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Radon.hpp"

#include <assert.h>

namespace radon
{
	Section::Section()
		: Named()
	{
	}


	Section::Section(const std::string & name)
		: Named(name)
	{
	}


	Key *Section::getKey(const std::string & name)
	{
		for (int i = 0; i < (int)keys.size(); i++)
		{
			if (keys[i].getName() == name)
				return &keys[i];
		}

		return NULL;
	}


	void Section::addKey(Key key)
	{
		keys.push_back(key);
	}
}
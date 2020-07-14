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
		for (auto & key : keys)
		{
			if (key.getName() == name)
				return &key;
		}

		return nullptr;
	}

	void Section::addKey(Key key)
	{
		keys.push_back(key);
	}
}

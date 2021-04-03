// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Section.hpp"
#include "../include/Key.hpp"

namespace radon
{
	Section::Section(const std::string & name)
		: Named(name)
	{
	}


	Key *Section::getKey(const std::string & name)
	{
		for(auto &key: keys)
		{
			if (key.getName() == name)
				return &key;
		}

		return nullptr;
	}


	const Key *Section::getKey(const std::string & name) const
	{
		for(const auto &key: keys)
		{
			if (key.getName() == name)
				return &key;
		}

		return nullptr;
	}


	const std::vector<Key> &Section::getKeys() const
	{
		return keys;
	}


	void Section::addKey(const Key & key)
	{
		keys.emplace_back(key);
	}
}
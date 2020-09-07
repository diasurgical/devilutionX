// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Radon.hpp"

namespace radon
{
	Key::Key()
		: Named()
	{
	}


	Key::Key(const std::string & name, const std::string & value)
		: Named(name), value(value)
	{
	}


	std::string Key::getStringValue()
	{
		return value;
	}


	void Key::setValue(std::string & value)
	{
		this->value = value;
	}
}

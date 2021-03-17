// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Radon.hpp"
#include <sstream>

std::string Float2String(float fVal)
{
	std::ostringstream ss;
	ss << fVal;
	std::string s(ss.str());
	return s;
}

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


	Key::Key(const std::string & name, float value)
		: Named(name), value(Float2String(value))
	{
	}


	std::string Key::getStringValue()
	{
		return value;
	}


	float Key::getFloatValue()
	{
		return (float)(atof(value.data()));
	}


	void Key::setValue(float value)
	{
		this->value = Float2String(value);
	}


	void Key::setValue(std::string & value)
	{
		this->value = value;
	}
}
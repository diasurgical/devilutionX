// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Key.hpp"

namespace radon
{
	Key::Key(const std::string & name, const std::string & value)
		: Named(name), value(value)
	{
	}


	Key::Key(const std::string & name, float value)
		: Named(name), value(std::to_string(value))
	{
	}


	std::string Key::getStringValue() const
	{
		return value;
	}


	float Key::getFloatValue() const
	{
		return std::stof(value);
	}


	void Key::setValue(float value)
	{
		this->value = std::to_string(value);
	}


	void Key::setValue(const std::string & value)
	{
		this->value = value;
	}
}
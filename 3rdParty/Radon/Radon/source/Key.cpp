// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Radon.hpp"
#include <stdlib.h>
#include <string>
#include <sstream>

namespace std
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#include <iostream>

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


	Key::Key(const std::string & name, const float & value)
		: Named(name), value(std::to_string(value))
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


	void Key::setValue(float & value)
	{
		this->value = std::to_string(value);
	}


	void Key::setValue(std::string & value)
	{
		this->value = value;
	}
}

// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Named.hpp"

namespace radon
{
	Named::Named(const std::string & name)
	{
		setName(name);
	}


	void Named::setName(const std::string & name)
	{
		this->name = name;
	}


	const std::string & Named::getName() const
	{
		return name;
	}
}
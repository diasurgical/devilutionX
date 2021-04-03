// Copyright Dmitro bjornus Szewczuk 2017

#include "../include/Radon.hpp"

namespace radon
{
	Named::Named()
	{
		setName("You need to set name!");
	}


	Named::Named(const std::string & name)
	{
		setName(name);
	}


	void Named::setName(const std::string & name)
	{
		this->name = name;
	}


	std::string Named::getName()
	{
		return name;
	}
}
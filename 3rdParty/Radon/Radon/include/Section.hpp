// Copyright Dmitro bjornus Szewczuk 2017 under zlib license

#pragma once

#include <string>
#include <vector>

#include "Named.hpp"

namespace radon
{
	class Key;

	class Section final
		: public Named
	{
	public:

		Section() = default;

		Section(const std::string & name);

		Key *getKey(const std::string & name);
		
		const Key *getKey(const std::string & name) const;

		const std::vector<Key> &getKeys() const;

		void addKey(const Key & key);

	private:
		std::vector<Key> keys;
	};
}
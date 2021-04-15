/**
 * @file enum_traits.h
 *
 * Base template for 'enum_traits' which allow us to access static information about an enum.
 */
#pragma once

#include <cstddef>

namespace devilution {

template <typename T>
struct enum_size {
	constexpr static const std::size_t value = static_cast<std::size_t>(T::LAST) + 1;
};

template <typename T>
class enum_values {
public:
	class Iterator {
		typename std::underlying_type<T>::type m_value;

	public:
		Iterator(typename std::underlying_type<T>::type value)
		    : m_value(value)
		{
		}

		const T operator*() const
		{
			return static_cast<T>(m_value);
		}

		void operator++()
		{
			m_value++;
		}

		bool operator!=(Iterator rhs) const
		{
			return m_value != rhs.m_value;
		}
	};
};

template <typename T>
typename enum_values<T>::Iterator begin(enum_values<T>)
{
	return typename enum_values<T>::Iterator(static_cast<typename std::underlying_type<T>::type>(T::FIRST));
}

template <typename T>
typename enum_values<T>::Iterator end(enum_values<T>)
{
	return typename enum_values<T>::Iterator(static_cast<typename std::underlying_type<T>::type>(T::LAST) + 1);
}

}

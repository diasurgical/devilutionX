#pragma once

#include <asio/detail/throw_exception.hpp>

namespace asio::detail {

void fatal_exception(const char *message);

template <typename Exception>
void throw_exception(
	const Exception &e
	ASIO_SOURCE_LOCATION_PARAM)
{
  fatal_exception(e.what());
}

} // namespace asio::detail

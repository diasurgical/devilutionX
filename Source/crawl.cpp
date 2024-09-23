#include "crawl.hpp"

#include <type_traits>

#include <function_ref.hpp>

#include "engine/displacement.hpp"

namespace devilution {

bool DoCrawl(unsigned radius, tl::function_ref<bool(Displacement)> function)
{
	return DoCrawl(radius, radius, function);
}

bool DoCrawl(unsigned minRadius, unsigned maxRadius, tl::function_ref<bool(Displacement)> function)
{
	for (int r = static_cast<int>(minRadius); r <= static_cast<int>(maxRadius); ++r) {
		if (!function(Displacement { 0, r })) return false;
		if (r == 0) continue;
		if (!function(Displacement { 0, -r })) return false;
		for (int x = 1; x < r; ++x) {
			if (!function(Displacement { -x, r })) return false;
			if (!function(Displacement { x, r })) return false;
			if (!function(Displacement { -x, -r })) return false;
			if (!function(Displacement { x, -r })) return false;
		}
		if (r > 1) {
			const int d = r - 1;
			if (!function(Displacement { -d, d })) return false;
			if (!function(Displacement { d, d })) return false;
			if (!function(Displacement { -d, -d })) return false;
			if (!function(Displacement { d, -d })) return false;
		}
		if (!function(Displacement { -r, 0 })) return false;
		if (!function(Displacement { r, 0 })) return false;
		for (int y = 1; y < r; ++y) {
			if (!function(Displacement { -r, y })) return false;
			if (!function(Displacement { r, y })) return false;
			if (!function(Displacement { -r, -y })) return false;
			if (!function(Displacement { r, -y })) return false;
		}
	}
	return true;
}

} // namespace devilution

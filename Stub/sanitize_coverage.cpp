#include "asserts.h"
#include <malloc.h>
#include <stdint.h>
#include <string.h>

#include "sanitize_coverage.h"

constexpr int pc_shift = 2;
constexpr uint32_t coverage_size = 1024 * 1024; // must be power of 2
constexpr uint32_t coverage_mask = coverage_size - 1;

extern char __executable_start;
extern char __etext;

static char coverage_data[coverage_size];

static bool found_new = false;

void coverage_init()
{
	uint32_t code_size = &__etext - &__executable_start;
	uint32_t required_coverage_size = (code_size >> pc_shift) + 1;
	if (coverage_size < required_coverage_size) {
		fprintf(stderr, "Warning: Not enough space for coverage data (%d < %d)\n", coverage_size,
		        required_coverage_size);
	}

	memset(coverage_data, 0, sizeof(coverage_data));
}

void coverage_start_round()
{
	found_new = false;
}

bool coverage_found_new()
{
	return found_new;
}

extern "C" void __sanitizer_cov_trace_pc(void)
{
	auto ip = (uint32_t)__builtin_return_address(0);

	// This function is optimized since it is called for every basic block (compiling to 6 x86 instructions on the hot
	// path currently). Just set a byte in the coverage array "somewhere", anywhere is fine as long as there is no
	// chance of overlap. This way we do not need to eg. subtract __executable_start or do bounds checks.
	uint32_t byte_pos = (ip >> pc_shift) & coverage_mask;

	if (!coverage_data[byte_pos]) {
		coverage_data[byte_pos] = 1;
		found_new = true;
	}
}
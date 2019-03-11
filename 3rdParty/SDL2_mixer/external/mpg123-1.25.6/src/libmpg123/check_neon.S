/*
	check_neon: check NEON availability

	copyright 1995-2014 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Momma
*/

#include "mangle.h"

#ifndef __aarch64__
#ifndef _M_ARM
	.code 32
#endif
#ifndef __APPLE__
	.fpu neon
#endif
#endif

	.text
	GLOBAL_SYMBOL ASM_NAME(check_neon)
#ifdef __ELF__
	.type ASM_NAME(check_neon), %function
#endif
	ALIGN4
ASM_NAME(check_neon):
#ifdef __aarch64__
	orr		v0.16b, v0.16b, v0.16b
	ret
#else
	vorr	d0, d0, d0
	bx		lr
#endif

NONEXEC_STACK

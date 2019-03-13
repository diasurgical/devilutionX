/*
	getcpuflags_x86_64: get cpuflags for x86-64

	copyright 1995-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

#include "mangle.h"

	.text
	ALIGN4
	.globl ASM_NAME(getcpuflags)
ASM_NAME(getcpuflags):
	push	%rbp
	mov		%rsp, %rbp
	push	%rbx
	
#ifdef IS_MSABI
	push	%rdi
	mov		%rcx, %rdi
#endif

	movl	$0, 12(%rdi)
	movl	$0, 16(%rdi)

	mov		$0x80000000, %eax
	cpuid
	cmp		$0x80000001, %eax
	jb		1f
	mov		$0x80000001, %eax
	cpuid
	movl	%edx, 12(%rdi)
1:
	mov		$0x00000001, %eax
	cpuid
	movl	%eax, (%rdi)
	movl	%ecx, 4(%rdi)
	movl	%edx, 8(%rdi)
	test	$0x04000000, %ecx
	jz		2f
	test	$0x08000000, %ecx
	jz		2f
	xor		%ecx, %ecx
	.byte	0x0f, 0x01, 0xd0 /* xgetbv instruction */
	movl	%eax, 16(%rdi)
	movl	(%rdi), %eax
2:
#ifdef IS_MSABI
	pop		%rdi
#endif
	pop		%rbx
	mov		%rbp, %rsp
	pop		%rbp
	ret
	
NONEXEC_STACK

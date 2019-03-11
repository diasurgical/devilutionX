/*
	synth_neon64: NEON optimized synth for AArch64

	copyright 1995-2014 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

#include "mangle.h"

#ifndef __APPLE__
	.section	.rodata
#else
	.data
#endif
	ALIGN16
maxmin_s16:
	.word   32767
	.word   -32768
	.text
	ALIGN4
	.globl ASM_NAME(synth_1to1_neon64_asm)
#ifdef __ELF__
	.type ASM_NAME(synth_1to1_neon64_asm), %function
#endif
ASM_NAME(synth_1to1_neon64_asm):
	add		x0, x0, #32
	sub		x0, x0, x3, lsl #1
	eor		v31.16b, v31.16b, v31.16b
	adrp	x5, AARCH64_PCREL_HI(maxmin_s16)
	add		x5, x5, AARCH64_PCREL_LO(maxmin_s16)
	ld2r	{v28.4s,v29.4s}, [x5]
	
	mov		w4, #4
	mov		x5, #64
1:
	ld1		{v0.8h,v1.8h}, [x0], x5
	ld1		{v2.8h,v3.8h}, [x0], x5
	ld1		{v4.8h,v5.8h}, [x0], x5
	ld1		{v6.8h,v7.8h}, [x0], x5
	ld1		{v16.8h,v17.8h,v18.8h,v19.8h}, [x1], #64
	ld1		{v20.8h,v21.8h,v22.8h,v23.8h}, [x1], #64
	
	smull	v24.4s, v0.4h, v16.4h
	smull	v25.4s, v2.4h, v18.4h
	smull	v26.4s, v4.4h, v20.4h
	smull	v27.4s, v6.4h, v22.4h
	smlal2	v24.4s, v0.8h, v16.8h
	smlal2	v25.4s, v2.8h, v18.8h
	smlal2	v26.4s, v4.8h, v20.8h
	smlal2	v27.4s, v6.8h, v22.8h
	smlal	v24.4s, v1.4h, v17.4h
	smlal	v25.4s, v3.4h, v19.4h
	smlal	v26.4s, v5.4h, v21.4h
	smlal	v27.4s, v7.4h, v23.4h
	smlal2	v24.4s, v1.8h, v17.8h
	smlal2	v25.4s, v3.8h, v19.8h
	smlal2	v26.4s, v5.8h, v21.8h
	smlal2	v27.4s, v7.8h, v23.8h
	
	addp	v0.4s, v24.4s, v25.4s
	addp	v1.4s, v26.4s, v27.4s
	addp	v0.4s, v0.4s, v1.4s
	ld2		{v4.4h,v5.4h}, [x2]
	sqrshrn	v4.4h, v0.4s, #13
	cmgt	v2.4s, v0.4s, v28.4s
	cmgt	v3.4s, v29.4s, v0.4s
	add		v2.4s, v2.4s, v3.4s
	add		v31.4s, v31.4s, v2.4s
	st2		{v4.4h,v5.4h}, [x2], #16
	
	subs	w4, w4, #1
	b.ne	1b
	
	mov		w4, #4
	mov		x6, #-32
2:
	ld1		{v0.8h,v1.8h}, [x0], x5
	ld1		{v2.8h,v3.8h}, [x0], x5
	ld1		{v4.8h,v5.8h}, [x0], x5
	ld1		{v6.8h,v7.8h}, [x0], x5
	ld1		{v16.8h,v17.8h}, [x1], x6
	ld1		{v18.8h,v19.8h}, [x1], x6
	ld1		{v20.8h,v21.8h}, [x1], x6
	ld1		{v22.8h,v23.8h}, [x1], x6
	
	smull	v24.4s, v0.4h, v16.4h
	smull	v25.4s, v2.4h, v18.4h
	smull	v26.4s, v4.4h, v20.4h
	smull	v27.4s, v6.4h, v22.4h
	smlal2	v24.4s, v0.8h, v16.8h
	smlal2	v25.4s, v2.8h, v18.8h
	smlal2	v26.4s, v4.8h, v20.8h
	smlal2	v27.4s, v6.8h, v22.8h
	smlal	v24.4s, v1.4h, v17.4h
	smlal	v25.4s, v3.4h, v19.4h
	smlal	v26.4s, v5.4h, v21.4h
	smlal	v27.4s, v7.4h, v23.4h
	smlal2	v24.4s, v1.8h, v17.8h
	smlal2	v25.4s, v3.8h, v19.8h
	smlal2	v26.4s, v5.8h, v21.8h
	smlal2	v27.4s, v7.8h, v23.8h
	
	addp	v0.4s, v24.4s, v25.4s
	addp	v1.4s, v26.4s, v27.4s
	addp	v0.4s, v0.4s, v1.4s
	ld2		{v4.4h,v5.4h}, [x2]
	sqrshrn	v4.4h, v0.4s, #13
	cmgt	v2.4s, v0.4s, v28.4s
	cmgt	v3.4s, v29.4s, v0.4s
	add		v2.4s, v2.4s, v3.4s
	add		v31.4s, v31.4s, v2.4s
	st2		{v4.4h,v5.4h}, [x2], #16
	
	subs	w4, w4, #1
	b.ne	2b
	
	AARCH64_DUP_2D(v0, v31, 1)
	add		v0.4s, v0.4s, v31.4s
	AARCH64_DUP_4S(v1, v0, 1)
	add		v0.4s, v0.4s, v1.4s
	umov	w0, v0.s[0]
	neg		w0, w0
	
	ret

NONEXEC_STACK

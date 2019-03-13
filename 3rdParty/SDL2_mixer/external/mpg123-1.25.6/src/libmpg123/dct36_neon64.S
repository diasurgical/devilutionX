/*
	dct36_neon64: NEON optimized dct36 for AArch64

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
dct36_aarch64_COS9:
	.word 0x3f5db3d7
	.word 0x3f5db3d7
	.word 0x3f000000
	.word 0x3f000000
	.word 0x3f7c1c5c
	.word 0x3f7c1c5c
	.word 0x3f708fb2
	.word 0x3f708fb2
	.word 0x3f248dbb
	.word 0x3f248dbb
	.word 0x3e31d0d4
	.word 0x3e31d0d4
	.word 0x3eaf1d44
	.word 0x3eaf1d44
	.word 0x3f441b7d
	.word 0x3f441b7d
	.word 0x3f007d2b
	.word 0x3f0483ee
	.word 0x3f0d3b7d
	.word 0x3f1c4257
	.word 0x40b79454
	.word 0x3ff746ea
	.word 0x3f976fd9
	.word 0x3f5f2944
	.word 0x3f800000
	.word 0x3f3504f3
	
	.text
	ALIGN4
	.globl ASM_NAME(dct36_neon64)
#ifdef __ELF__
	.type ASM_NAME(dct36_neon64), %function
#endif
ASM_NAME(dct36_neon64):
	adrp		x5, AARCH64_PCREL_HI(dct36_aarch64_COS9)
	add			x5, x5, AARCH64_PCREL_LO(dct36_aarch64_COS9)
	cmeq		v28.16b, v28.16b, v28.16b
	eor			v29.16b, v29.16b, v29.16b
	shl			v28.2d, v28.2d, #32
	ld1			{v0.4s,v1.4s,v2.4s,v3.4s}, [x0], #64
	ld1			{v4.2s}, [x0]
	
	ext			v16.16b, v29.16b, v0.16b, #12
	ext			v17.16b, v0.16b, v1.16b, #12
	ext			v18.16b, v1.16b, v2.16b, #12
	ext			v19.16b, v2.16b, v3.16b, #12
	ext			v20.16b, v3.16b, v4.16b, #12
	fadd		v0.4s, v0.4s, v16.4s
	fadd		v1.4s, v1.4s, v17.4s
	fadd		v2.4s, v2.4s, v18.4s
	fadd		v3.4s, v3.4s, v19.4s
	fadd		v4.2s, v4.2s, v20.2s
	
	ext			v16.16b, v0.16b, v1.16b, #8
	ext			v17.16b, v1.16b, v2.16b, #8
	ext			v18.16b, v2.16b, v3.16b, #8
	ext			v19.16b, v3.16b, v4.16b, #8
	and			v20.16b, v0.16b, v28.16b
	ext			v0.16b, v29.16b, v0.16b, #8
	and			v21.16b, v1.16b, v28.16b
	and			v22.16b, v2.16b, v28.16b
	and			v23.16b, v3.16b, v28.16b
	fadd		v1.4s, v20.4s, v16.4s
	fadd		v2.4s, v21.4s, v17.4s
	fadd		v3.4s, v22.4s, v18.4s
	fadd		v4.4s, v23.4s, v19.4s
	
/*
v0 in[-,-,0,1]
v1 in[2,3,4,5]
v2 in[6,7,8,9]
v3 in[10,11,12,13]
v4 in[14,15,16,17]
*/
	
	orr			v5.16b, v2.16b, v2.16b
	ins			v2.d[1], v3.d[1]
	ins			v3.d[1], v4.d[1]
	ins			v4.d[1], v5.d[1]
	
/*
v2 in[6,7,12,13]
v3 in[10,11,16,17]
v4 in[14,15,8,9]
*/
	
	ld1			{v16.4s,v17.4s,v18.4s,v19.4s}, [x5], #64
	orr			v20.16b, v0.16b, v0.16b
	fmla		v20.4s, v2.4s, v16.4s
	
/*
v17 COS9_[1,1,2,2]
v18 COS9_[5,5,8,8]
v19 COS9_[7,7,4,4]
v16 COS9_[3,3,6,6]
v20 [ta33,tb33,ta66,tb66]
*/
	
	orr			v21.16b, v20.16b, v20.16b
	orr			v23.16b, v20.16b, v20.16b
	zip2		v25.2d, v29.2d, v2.2d
	fsub		v22.4s, v1.4s, v3.4s
	fmul		v24.4s, v1.4s, v17.4s
	fmul		v26.4s, v1.4s, v18.4s
	fmul		v27.4s, v1.4s, v19.4s
	fmla		v21.4s, v3.4s, v18.4s
	fmla		v23.4s, v3.4s, v19.4s
	fmla		v20.4s, v4.4s, v18.4s
	fsub		v25.4s, v0.4s, v25.4s
	fsub		v22.4s, v22.4s, v4.4s
	fmla		v24.4s, v4.4s, v19.4s
	fmla		v26.4s, v4.4s, v17.4s
	fmla		v27.4s, v3.4s, v17.4s
	fmla		v25.4s, v22.4s, v16.4s
	fadd		v24.4s, v24.4s, v21.4s
	fsub		v26.4s, v26.4s, v23.4s
	fsub		v27.4s, v27.4s, v20.4s
	
	zip1		v16.4s, v24.4s, v25.4s
	zip2		v17.4s, v24.4s, v25.4s
	zip1		v18.4s, v26.4s, v27.4s
	zip2		v19.4s, v26.4s, v27.4s
	fneg		v19.4s, v19.4s
	zip1		v20.2d, v16.2d, v18.2d
	zip1		v21.2d, v17.2d, v19.2d
	zip2		v22.2d, v16.2d, v18.2d
	zip2		v23.2d, v17.2d, v19.2d
	
	ld1			{v5.4s,v6.4s}, [x5], #32
	ld1			{v7.2s}, [x5]
	fsub		v0.4s, v0.4s, v1.4s
	fsub		v4.4s, v4.4s, v2.4s
	fadd		v17.4s, v22.4s, v23.4s
	fsub		v19.4s, v23.4s, v22.4s
	fadd		v0.4s, v0.4s, v3.4s
	fadd		v16.4s, v20.4s, v21.4s
	fsub		v18.4s, v21.4s, v20.4s
	fadd		v0.4s, v0.4s, v4.4s
	fmul		v17.4s, v17.4s, v5.4s
	fmul		v19.4s, v19.4s, v6.4s
	AARCH64_DUP_2D(v0, v0, 1)
	fmul		v0.2s, v0.2s, v7.2s
	
/*
v16 tmp[0,1,2,3]
v17 tmp[17,16,15,14]
v18 tmp[8,7,6,5]
v19 tmp[9,10,11,12]
v0 tmp[4,13]
*/
	
	add			x0, x4, #640
	add			x5, x3, #20
	add			x6, x3, #92
	add			x7, x1, #20
	ld1			{v1.4s,v2.4s}, [x5]
	ld1			{v3.4s,v4.4s}, [x6]
	ld1			{v5.4s,v6.4s}, [x7]
	fadd		v20.4s, v16.4s, v17.4s
	fsub		v21.4s, v16.4s, v17.4s
	fmul		v4.4s, v20.4s, v4.4s
	fmla		v6.4s, v21.4s, v2.4s
	rev64		v20.4s, v20.4s
	rev64		v21.4s, v21.4s
	ext			v20.16b, v20.16b, v20.16b, #8
	ext			v21.16b, v21.16b, v21.16b, #8
	fmul		v3.4s, v20.4s, v3.4s
	fmla		v5.4s, v21.4s, v1.4s
	add			x5, x2, #20
	mov			x9, #128
	st1			{v3.4s,v4.4s}, [x5]
	st1			{v5.s}[0], [x0], x9
	st1			{v5.s}[1], [x0], x9
	st1			{v5.s}[2], [x0], x9
	st1			{v5.s}[3], [x0], x9
	st1			{v6.s}[0], [x0], x9
	st1			{v6.s}[1], [x0], x9
	st1			{v6.s}[2], [x0], x9
	st1			{v6.s}[3], [x0], x9
	
	add			x0, x4, #1792
	add			x5, x3, #56
	add			x6, x3, #128
	add			x7, x1, #56
	ld1			{v1.4s}, [x3]
	ld1			{v2.4s,v3.4s}, [x5]
	ld1			{v4.4s}, [x6]
	ld1			{v5.4s}, [x1]
	ld1			{v6.4s}, [x7]
	fadd		v20.4s, v18.4s, v19.4s
	fsub		v21.4s, v18.4s, v19.4s
	fmul		v3.4s, v20.4s, v3.4s
	fmla		v5.4s, v21.4s, v1.4s
	rev64		v20.4s, v20.4s
	rev64		v21.4s, v21.4s
	ext			v20.16b, v20.16b, v20.16b, #8
	ext			v21.16b, v21.16b, v21.16b, #8
	fmul		v4.4s, v20.4s, v4.4s
	fmla		v6.4s, v21.4s, v2.4s
	add			x5, x2, #56
	st1			{v3.4s}, [x2]
	st1			{v4.4s}, [x5]
	st1			{v5.s}[0], [x4], x9
	st1			{v5.s}[1], [x4], x9
	st1			{v5.s}[2], [x4], x9
	st1			{v5.s}[3], [x4], x9
	st1			{v6.s}[0], [x0], x9
	st1			{v6.s}[1], [x0], x9
	st1			{v6.s}[2], [x0], x9
	st1			{v6.s}[3], [x0], x9
	
	ins			v1.s[0], v0.s[1]
	ldr			s2, [x3, #16]
	ldr			s3, [x3, #52]
	ldr			s4, [x3, #88]
	ldr			s5, [x3, #124]
	ldr			s6, [x1, #16]
	ldr			s7, [x1, #52]
	fadd		s16, s0, s1
	fsub		s17, s0, s1
	fmul		s4, s16, s4
	fmul		s5, s16, s5
	fmadd		s6, s17, s2, s6
	fmadd		s7, s17, s3, s7
	str			s4, [x2, #16]
	str			s5, [x2, #52]
	str			s6, [x4]
	str			s7, [x4, #1152]
	
	ret
	
NONEXEC_STACK

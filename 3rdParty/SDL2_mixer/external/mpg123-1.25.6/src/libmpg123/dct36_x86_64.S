/*
	dct36_x86_64: SSE optimized dct36 for x86-64

	copyright 1995-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

#include "mangle.h"

#ifdef IS_MSABI
#define in %rcx
#define out1 %rdx
#define out2 %r8
#define w  %r9
#define ts %r10
#define COS9_ %rax
#define tfcos36_ %r11
#else
#define in %rdi
#define out1 %rsi
#define out2 %rdx
#define w  %rcx
#define ts %r8
#define COS9_ %rax
#define tfcos36_ %r9
#endif

/*
	void dct36_x86_64(real *inbuf,real *o1,real *o2,real *wintab,real *tsbuf);
*/
	
#ifndef __APPLE__
	.section	.rodata
#else
	.data
#endif
	ALIGN16
dct36_x86_64_COS9:
	.long 0x3f5db3d7
	.long 0x3f5db3d7
	.long 0x3f000000
	.long 0x3f000000
	.long 0x3f7c1c5c
	.long 0x3f7c1c5c
	.long 0x3f708fb2
	.long 0x3f708fb2
	.long 0x3f248dbb
	.long 0x3f248dbb
	.long 0x3e31d0d4
	.long 0x3e31d0d4
	.long 0x3eaf1d44
	.long 0x3eaf1d44
	.long 0x3f441b7d
	.long 0x3f441b7d
	ALIGN16
dct36_x86_64_tfcos36:
	.long 0x3f007d2b
	.long 0x3f0483ee
	.long 0x3f0d3b7d
	.long 0x3f1c4257
	.long 0x40b79454
	.long 0x3ff746ea
	.long 0x3f976fd9
	.long 0x3f5f2944
	.long 0x3f3504f3
	ALIGN16
dct36_x86_64_mask:
	.long 0,0xffffffff,0,0xffffffff
	ALIGN16
dct36_x86_64_sign:
	.long 0x80000000,0x80000000,0x80000000,0x80000000
	.text
	ALIGN16
	.globl ASM_NAME(dct36_x86_64)
ASM_NAME(dct36_x86_64):
#ifdef IS_MSABI
	push		%rbp
	mov			%rsp, %rbp
	sub			$160, %rsp
	movaps		%xmm6, (%rsp)
	movaps		%xmm7, 16(%rsp)
	movaps		%xmm8, 32(%rsp)
	movaps		%xmm9, 48(%rsp)
	movaps		%xmm10, 64(%rsp)
	movaps		%xmm11, 80(%rsp)
	movaps		%xmm12, 96(%rsp)
	movaps		%xmm13, 112(%rsp)
	movaps		%xmm14, 128(%rsp)
	movaps		%xmm15, 144(%rsp)
	movq		48(%rbp), ts
#endif
	lea			dct36_x86_64_COS9(%rip), COS9_
	lea			dct36_x86_64_tfcos36(%rip), tfcos36_
	
	xorps		%xmm5, %xmm5
	movups		(in), %xmm1
	movups		16(in), %xmm2
	movups		32(in), %xmm3
	movups		48(in), %xmm4
	movlps		64(in), %xmm5
	xorps		%xmm6, %xmm6
	movaps		%xmm1, %xmm7
	shufps		$0x93, %xmm7, %xmm7
	movaps		%xmm2, %xmm8
	shufps		$0x93, %xmm8, %xmm8
	movaps		%xmm3, %xmm9
	shufps		$0x93, %xmm9, %xmm9
	movaps		%xmm4, %xmm10
	shufps		$0x93, %xmm10, %xmm10
	movaps		%xmm5, %xmm11
	shufps		$0xe1, %xmm11, %xmm11
	movss		%xmm10, %xmm11
	addps		%xmm11, %xmm5
	movss		%xmm9, %xmm10
	addps		%xmm10, %xmm4
	movss		%xmm8, %xmm9
	addps		%xmm9, %xmm3
	movss		%xmm7, %xmm8
	addps		%xmm8, %xmm2
	movss		%xmm6, %xmm7
	addps		%xmm7, %xmm1
	
	movaps		dct36_x86_64_mask(%rip), %xmm0
	movaps		%xmm4, %xmm6
	shufps		$0x4e, %xmm5, %xmm4
	movaps		%xmm3, %xmm7
	shufps		$0x4e, %xmm6, %xmm3
	andps		%xmm0, %xmm6
	addps		%xmm6, %xmm4
	movaps		%xmm2, %xmm6
	shufps		$0x4e, %xmm7, %xmm2
	andps		%xmm0, %xmm7
	addps		%xmm7, %xmm3
	movaps		%xmm1, %xmm7
	shufps		$0x4e, %xmm6, %xmm1
	andps		%xmm0, %xmm6
	addps		%xmm6, %xmm2
	movaps		%xmm7, %xmm6
	andps		%xmm0, %xmm7
	xorps		%xmm0, %xmm0
	addps		%xmm7, %xmm1
	movlhps		%xmm6, %xmm0

/*
xmm0 in[-,-,0,1]
xmm1 in[2,3,4,5]
xmm2 in[6,7,8,9]
xmm3 in[10,11,12,13]
xmm4 in[14,15,16,17]
*/
	
	movaps		%xmm2, %xmm5
	shufps		$0xe4, %xmm3, %xmm5
	shufps		$0xe4, %xmm4, %xmm3
	shufps		$0xe4, %xmm2, %xmm4
	movaps		%xmm5, %xmm2
/*
xmm2 in[6,7,12,13]
xmm3 in[10,11,16,17]
xmm4 in[14,15,8,9]
*/

	movaps		(COS9_), %xmm15
	movaps		16(COS9_), %xmm6
	movaps		32(COS9_), %xmm7
	movaps		48(COS9_), %xmm8
	mulps		%xmm15, %xmm5
	addps		%xmm0, %xmm5
	
/*
xmm5 [ta33,tb33,ta66,tb66]
xmm6 COS9_[1,1,2,2]
xmm7 COS9_[5,5,8,8]
xmm8 COS9_[7,7,4,4]
xmm15 COS9_[3,3,6,6]
*/
	movaps		%xmm6, %xmm9
	movaps		%xmm7, %xmm12
	movaps		%xmm8, %xmm13
	mulps		%xmm1, %xmm9
	mulps		%xmm3, %xmm12
	mulps		%xmm4, %xmm13
	addps		%xmm5, %xmm9
	addps		%xmm13, %xmm12
	addps		%xmm9, %xmm12
	
	movaps		%xmm1, %xmm13
	subps		%xmm3, %xmm13
	movaps		%xmm0, %xmm10
	shufps		$0xe0, %xmm2, %xmm10
	movaps		%xmm0, %xmm14
	subps		%xmm10, %xmm14
	subps		%xmm4, %xmm13
	mulps		%xmm15, %xmm13
	addps		%xmm14, %xmm13
	
	movaps		%xmm7, %xmm9
	movaps		%xmm8, %xmm15
	movaps		%xmm6, %xmm14
	mulps		%xmm1, %xmm9
	mulps		%xmm3, %xmm15
	mulps		%xmm4, %xmm14
	subps		%xmm5, %xmm9
	subps		%xmm15, %xmm14
	addps		%xmm9, %xmm14
	
	mulps		%xmm1, %xmm8
	mulps		%xmm3, %xmm6
	mulps		%xmm4, %xmm7
	subps		%xmm5, %xmm8
	subps		%xmm7, %xmm6
	addps		%xmm6, %xmm8
	movaps		%xmm8, %xmm15
	
	movss		32(tfcos36_), %xmm5
	subps		%xmm1, %xmm0
	subps		%xmm2, %xmm4
	addps		%xmm3, %xmm0
	addps		%xmm4, %xmm0
	shufps		$0xaf, %xmm0, %xmm0
	mulss		%xmm5, %xmm0
	movaps		%xmm0, %xmm11

/*
xmm12 [1a-0,1b-0, 2a-0, 2b-0]
xmm13 [1a-1,1b-1, 2a-1, 2b-1]
xmm14 [1a-2,1b-2,-2a-2,-2b-2]
xmm15 [1a-3,1b-3,-2a-3,-2b-3]
*/
	movaps		%xmm12, %xmm5
	unpckhps	%xmm13, %xmm5
	unpcklps	%xmm13, %xmm12
	movaps		%xmm14, %xmm6
	unpckhps	%xmm15, %xmm6
	unpcklps	%xmm15, %xmm14
	xorps		dct36_x86_64_sign(%rip), %xmm6

/*
xmm12 [1a-0,1a-1,1b-0,1b-1]
xmm5  [2a-0,2a-1,2b-0,2b-1]
xmm14 [1a-2,1a-3,1b-2,1b-3]
xmm6 [2a-2,2a-3,2b-2,2b-3]
*/

	movaps		%xmm12, %xmm0
	movlhps		%xmm14, %xmm12
	movhlps		%xmm0, %xmm14
	movaps		%xmm5, %xmm0
	movlhps		%xmm6, %xmm0
	movhlps		%xmm5, %xmm6
	movaps		%xmm6, %xmm15

/*
xmm12 tmp1a
xmm0 tmp2a
xmm14 tmp1b
xmm15 tmp2b
*/

	movaps		(tfcos36_), %xmm6
	movaps		16(tfcos36_), %xmm7
	movaps		%xmm15, %xmm10
	addps		%xmm14, %xmm15
	subps		%xmm14, %xmm10
	movaps		%xmm0, %xmm14
	addps		%xmm12, %xmm0
	subps		%xmm12, %xmm14
	mulps		%xmm6, %xmm15
	mulps		%xmm10, %xmm7

/*
%xmm0  tmp[0,1,2,3]
%xmm15 tmp[17,16,15,14]
%xmm14 tmp[8,7,6,5]
%xmm7  tmp[9,10,11,12]
%xmm11 tmp[13,-,4,-]
*/

	movaps		%xmm15, %xmm1
	movups		108(w), %xmm2
	movups		92(w), %xmm3
	shufps		$0x1b, %xmm3, %xmm3
	movups		36(w), %xmm4
	movups		20(w), %xmm5
	shufps		$0x1b, %xmm5, %xmm5
	movaps		%xmm0, %xmm6
	addps		%xmm1, %xmm0
	subps		%xmm1, %xmm6
	mulps		%xmm0, %xmm2
	mulps		%xmm3, %xmm0
	mulps		%xmm6, %xmm4
	mulps		%xmm5, %xmm6
	movups		36(out1), %xmm1
	movups		20(out1), %xmm3
	shufps		$0x1b, %xmm6, %xmm6
	addps		%xmm4, %xmm1
	addps		%xmm6, %xmm3
	shufps		$0x1b, %xmm0, %xmm0
	movups		%xmm2, 36(out2)
	movups		%xmm0, 20(out2)
	movss		%xmm1, 32*36(ts)
	movss		%xmm3, 32*20(ts)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*44(ts)
	movss		%xmm4, 32*28(ts)
	shufps		$0xb1, %xmm1, %xmm1
	shufps		$0xb1, %xmm3, %xmm3
	movss		%xmm1, 32*40(ts)
	movss		%xmm3, 32*24(ts)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*48(ts)
	movss		%xmm4, 32*32(ts)
	
	movhlps		%xmm11, %xmm0
	movaps		%xmm11, %xmm1
	movss		124(w), %xmm2
	movss		88(w), %xmm3
	movss		52(w), %xmm4
	movss		16(w), %xmm5
	movss		%xmm0, %xmm6
	addss		%xmm1, %xmm0
	subss		%xmm1, %xmm6
	mulss		%xmm0, %xmm2
	mulss		%xmm3, %xmm0
	mulss		%xmm6, %xmm4
	mulss		%xmm5, %xmm6
	addss		52(out1), %xmm4
	addss		16(out1), %xmm6
	movss		%xmm2, 52(out2)
	movss		%xmm0, 16(out2)
	movss		%xmm4, 32*52(ts)
	movss		%xmm6, 32*16(ts)
	
	movaps		%xmm14, %xmm0
	movaps		%xmm7, %xmm1
	MOVUAPS		128(w), %xmm2
	movups		72(w), %xmm3
	shufps		$0x1b, %xmm2, %xmm2
	movlps		56(w), %xmm4
	movhps		64(w), %xmm4
	MOVUAPS		(w), %xmm5
	shufps		$0x1b, %xmm4, %xmm4
	movaps		%xmm0, %xmm6
	addps		%xmm1, %xmm0
	subps		%xmm1, %xmm6
	mulps		%xmm0, %xmm2
	mulps		%xmm3, %xmm0
	mulps		%xmm6, %xmm4
	mulps		%xmm5, %xmm6
	movlps		56(out1), %xmm1
	movhps		64(out1), %xmm1
	movups		(out1), %xmm3
	shufps		$0x1b, %xmm4, %xmm4
	addps		%xmm6, %xmm3
	addps		%xmm4, %xmm1
	shufps		$0x1b, %xmm2, %xmm2
	movups		%xmm0, (out2)
	movlps		%xmm2, 56(out2)
	movhps		%xmm2, 64(out2)
	movss		%xmm1, 32*56(ts)
	movss		%xmm3, (ts)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*64(ts)
	movss		%xmm4, 32*8(ts)
	shufps		$0xb1, %xmm1, %xmm1
	shufps		$0xb1, %xmm3, %xmm3
	movss		%xmm1, 32*60(ts)
	movss		%xmm3, 32*4(ts)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*68(ts)
	movss		%xmm4, 32*12(ts)
	
#ifdef IS_MSABI
	movaps		(%rsp), %xmm6
	movaps		16(%rsp), %xmm7
	movaps		32(%rsp), %xmm8
	movaps		48(%rsp), %xmm9
	movaps		64(%rsp), %xmm10
	movaps		80(%rsp), %xmm11
	movaps		96(%rsp), %xmm12
	movaps		112(%rsp), %xmm13
	movaps		128(%rsp), %xmm14
	movaps		144(%rsp), %xmm15
	mov			%rbp, %rsp
	pop			%rbp
#endif
	ret

NONEXEC_STACK

/*
	dct36_sse: SSE optimized dct36

	copyright 1995-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

#include "mangle.h"

#define in %edi
#define out1 %edi
#define out2 %edx
#define w  %ecx
#define ts %eax
#define tmp %esi

/*
	void dct36_sse(real *inbuf,real *o1,real *o2,real *wintab,real *tsbuf);
*/
	
#ifndef __APPLE__
	.section	.rodata
#else
	.data
#endif
	ALIGN16
dct36_sse_COS9:
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
dct36_sse_tfcos36:
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
dct36_sse_mask:
	.long 0,0xffffffff,0,0xffffffff
	ALIGN16
dct36_sse_sign:
	.long 0x80000000,0x80000000,0x80000000,0x80000000
	.text
	ALIGN16
	.globl ASM_NAME(dct36_sse)
ASM_NAME(dct36_sse):
	push		%ebp
	mov			%esp, %ebp
	and			$-16, %esp
	sub			$80, %esp
	push		%ebx
	push		%esi
	push		%edi
	lea			12(%esp), tmp
	movl		8(%ebp), in

	GET_GOT
	
	lea			LOCAL_VAR(dct36_sse_COS9), %eax
	lea			LOCAL_VAR(dct36_sse_tfcos36), %edx

	xorps		%xmm0, %xmm0
	xorps		%xmm5, %xmm5
	movlps		64(in), %xmm5
	movups		48(in), %xmm4
	movups		32(in), %xmm3
	movups		16(in), %xmm2
	movups		(in), %xmm1
	movaps		%xmm5, %xmm6
	shufps		$0xe1, %xmm6, %xmm6
	movaps		%xmm4, %xmm7
	shufps		$0x93, %xmm7, %xmm7
	movss		%xmm7, %xmm6
	addps		%xmm6, %xmm5
	movaps		%xmm3, %xmm6
	shufps		$0x93, %xmm6, %xmm6
	movss		%xmm6, %xmm7
	addps		%xmm7, %xmm4
	movaps		%xmm2, %xmm7
	shufps		$0x93, %xmm7, %xmm7
	movss		%xmm7, %xmm6
	addps		%xmm6, %xmm3
	movaps		%xmm1, %xmm6
	shufps		$0x93, %xmm6, %xmm6
	movss		%xmm6, %xmm7
	addps		%xmm7, %xmm2
	movss		%xmm0, %xmm6
	addps		%xmm6, %xmm1
	
	movaps		LOCAL_VAR(dct36_sse_mask), %xmm0
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

	mulps		(%eax), %xmm5
	addps		%xmm0, %xmm5
	
	movaps		%xmm0, (tmp)
	movaps		%xmm2, 16(tmp)

/*
0(tmp) in[-,-,0,1]
xmm5 [ta33,tb33,ta66,tb66]
*/

	movaps		%xmm1, %xmm6
	subps		%xmm3, %xmm6
	subps		%xmm4, %xmm6
	xorps		%xmm7, %xmm7
	shufps		$0xe0, %xmm2, %xmm7
	mulps		(%eax), %xmm6
	subps		%xmm7, %xmm0
	addps		%xmm0, %xmm6
	movaps		%xmm6, 48(tmp)
	
	movaps		16(%eax), %xmm2

	movaps		%xmm1, %xmm0
	movaps		%xmm3, %xmm6
	movaps		%xmm4, %xmm7
	mulps		%xmm2, %xmm0
	mulps		32(%eax), %xmm6
	mulps		48(%eax), %xmm7
	addps		%xmm5, %xmm0
	addps		%xmm7, %xmm6
	addps		%xmm6, %xmm0
	movaps		%xmm0, 32(tmp)
	
	movaps		%xmm1, %xmm0
	movaps		%xmm3, %xmm6
	movaps		%xmm4, %xmm7
	mulps		32(%eax), %xmm0
	mulps		48(%eax), %xmm6
	mulps		%xmm2, %xmm7
	subps		%xmm5, %xmm0
	subps		%xmm6, %xmm7
	addps		%xmm7, %xmm0
	movaps		%xmm0, 64(tmp)
	
	movaps		%xmm1, %xmm6
	movaps		%xmm4, %xmm7
	mulps		48(%eax), %xmm6
	mulps		%xmm3, %xmm2
	mulps		32(%eax), %xmm7
	subps		%xmm5, %xmm6
	subps		%xmm7, %xmm2
	addps		%xmm2, %xmm6
	
	movaps		(tmp), %xmm0
	movss		32(%edx), %xmm5
	subps		%xmm1, %xmm0
	subps		16(tmp), %xmm4
	addps		%xmm3, %xmm0
	addps		%xmm4, %xmm0
	shufps		$0xaf, %xmm0, %xmm0
	mulss		%xmm5, %xmm0
	movaps		%xmm0, (tmp)
	
	movaps		32(tmp), %xmm0
	movaps		48(tmp), %xmm1
	movaps		64(tmp), %xmm2

/*
xmm0 [1a-0,1b-0, 2a-0, 2b-0]
xmm1 [1a-1,1b-1, 2a-1, 2b-1]
xmm2 [1a-2,1b-2,-2a-2,-2b-2]
xmm6 [1a-3,1b-3,-2a-3,-2b-3]
*/

	movaps		%xmm0, %xmm3
	unpcklps	%xmm1, %xmm0
	unpckhps	%xmm1, %xmm3
	movaps		%xmm2, %xmm5
	unpcklps	%xmm6, %xmm2
	unpckhps	%xmm6, %xmm5
	xorps		LOCAL_VAR(dct36_sse_sign), %xmm5

/*
xmm0 [1a-0,1a-1,1b-0,1b-1]
xmm3 [2a-0,2a-1,2b-0,2b-1]
xmm2 [1a-2,1a-3,1b-2,1b-3]
xmm5 [2a-2,2a-3,2b-2,2b-3]
*/

	movaps		%xmm0, %xmm1
	movlhps		%xmm2, %xmm0
	movhlps		%xmm1, %xmm2
	movaps		%xmm3, %xmm4
	movlhps		%xmm5, %xmm3
	movhlps		%xmm4, %xmm5

/*
xmm0 tmp1a
xmm3 tmp2a
xmm2 tmp1b
xmm5 tmp2b
*/

	movaps		(%edx), %xmm6
	movaps		16(%edx), %xmm7
	movaps		%xmm5, %xmm1
	addps		%xmm2, %xmm5
	subps		%xmm2, %xmm1
	movaps		%xmm3, %xmm2
	addps		%xmm0, %xmm3
	subps		%xmm0, %xmm2
	mulps		%xmm6, %xmm5
	mulps		%xmm1, %xmm7
	
	movaps		%xmm2, 16(tmp)

/*
%xmm3 tmp[0,1,2,3]
%xmm5 tmp[17,16,15,14]
16(tmp) tmp[8,7,6,5]
%xmm7 tmp[9,10,11,12]
0(tmp) tmp[13,-,4,-]
*/

	movl		12(%ebp), out1
	movl		16(%ebp), out2
	movl		20(%ebp), w
	movl		24(%ebp), ts

	movaps		%xmm3, %xmm0
	movaps		%xmm5, %xmm1
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
	
	movss		8(tmp), %xmm0
	movss		(tmp), %xmm1
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
	
	movaps		16(tmp), %xmm0
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
	
	pop			%edi
	pop			%esi
	pop			%ebx
	mov			%ebp, %esp
	pop			%ebp
	
	ret

NONEXEC_STACK

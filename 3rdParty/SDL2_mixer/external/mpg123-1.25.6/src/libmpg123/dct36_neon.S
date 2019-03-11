/*
	dct36_neon: ARM NEON optimized dct36

	copyright 1995-2014 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/


#include "mangle.h"

#ifndef _M_ARM
	.code 32
#endif
#ifndef __APPLE__
	.fpu neon
#endif
	
	.text
	ALIGN16
dct36_neon_COS9:
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
	
	ALIGN4
	GLOBAL_SYMBOL ASM_NAME(dct36_neon)
#ifdef __ELF__
	.type ASM_NAME(dct36_neon), %function
#endif
ASM_NAME(dct36_neon):
	push		{r4-r5, lr}
	vpush		{q4-q7}
	ldr			r4, [sp, #76]
	adr			r5, dct36_neon_COS9
	
	vceq.i32	q14, q14, q14
	veor		q15, q15, q15
	vshl.i64	q14, q14, #32
	vld1.32		{q0, q1}, [r0]!
	vld1.32		{q2, q3}, [r0]!
	vld1.32		{d8}, [r0]
	
	vext.8		q5, q15, q0, #12
	vext.8		q6, q0, q1, #12
	vext.8		q7, q1, q2, #12
	vext.8		q8, q2, q3, #12
	vext.8		d18, d7, d8, #4
	vadd.f32	q0, q0, q5
	vadd.f32	q1, q1, q6
	vadd.f32	q2, q2, q7
	vadd.f32	q3, q3, q8
	vadd.f32	d8, d8, d18
	
	vext.8		q6, q0, q1, #8
	vext.8		q7, q1, q2, #8
	vext.8		q8, q2, q3, #8
	vext.8		q9, q3, q4, #8
	vand		q10, q0, q14
	vext.8		q0, q15, q0, #8
	vand		q11, q1, q14
	vand		q12, q2, q14
	vand		q13, q3, q14
	vadd.f32	q1, q10, q6
	vadd.f32	q2, q11, q7
	vadd.f32	q3, q12, q8
	vadd.f32	q4, q13, q9
	
/*
q0 in[-,-,0,1]
q1 in[2,3,4,5]
q2 in[6,7,8,9]
q3 in[10,11,12,13]
q4 in[14,15,16,17]
*/
	
	vswp		d5, d7
	vswp		d7, d9
	
/*
q2 in[6,7,12,13]
q3 in[10,11,16,17]
q4 in[14,15,8,9]
*/
	
	vld1.32		{q5, q6}, [r5, :128]!
	vld1.32		{q7, q8}, [r5, :128]!
	vmov		q9, q0
	vmla.f32	q9, q2, q5
	
/*
q6 COS9_[1,1,2,2]
q7 COS9_[5,5,8,8]
q8 COS9_[7,7,4,4]
q5 COS9_[3,3,6,6]
q9 [ta33,tb33,ta66,tb66]
*/
	
	vmov		q10, q9
	vmov		d26, d0
	vmov		d27, d5
	vmul.f32	q12, q1, q6
	vsub.f32	q11, q1, q3
	vmla.f32	q10, q3, q7
	vsub.f32	q13, q0, q13
	vmla.f32	q12, q4, q8
	vsub.f32	q11, q11, q4
	vmul.f32	q14, q1, q7
	vmul.f32	q15, q1, q8
	vadd.f32	q12, q12, q10
	vmov		q10, q9
	vmla.f32	q13, q11, q5
	vmla.f32	q10, q3, q8
	vmla.f32	q14, q4, q6
	vmla.f32	q9, q4, q7
	vmla.f32	q15, q3, q6
	vsub.f32	q14, q14, q10
	vsub.f32	q15, q15, q9
	
/*
q12 [1a-0,1b-0, 2a-0, 2b-0]
q13 [1a-1,1b-1, 2a-1, 2b-1]
q14 [1a-2,1b-2,-2a-2,-2b-2]
q15 [1a-3,1b-3,-2a-3,-2b-3]
*/
	
	vzip.32		q12, q13
	vzip.32		q14, q15
	vneg.f32	q15, q15

/*
q12 [1a-0,1a-1,1b-0,1b-1]
q13 [2a-0,2a-1,2b-0,2b-1]
q14 [1a-2,1a-3,1b-2,1b-3]
q15 [2a-2,2a-3,2b-2,2b-3]
*/
	
	vswp		d25, d28
	vswp		d27, d30
	
/*
q12 tmp1a
q13 tmp2a
q14 tmp1b
q15 tmp2b
*/
	vsub.f32	d1, d1, d3
	vsub.f32	d9, d9, d5
	vld1.32		{q5, q6}, [r5, :128]!
	vld1.32		{d0}, [r5, :64]
	vadd.f32	q10, q14, q15
	vsub.f32	q8, q15, q14
	vadd.f32	d1, d1, d7
	vadd.f32	q9, q12, q13
	vsub.f32	q7, q13, q12
	vadd.f32	d1, d1, d9
	vmul.f32	q10, q10, q5
	vmul.f32	q8, q8, q6
	vmul.f32	d0, d1, d0
	
/*
q9 tmp[0,1,2,3]
q10 tmp[17,16,15,14]
q7 tmp[8,7,6,5]
q8 tmp[9,10,11,12]
d0 tmp[4,13]
*/
	
	add			r0, r4, #640
	add			r5, r3, #20
	vld1.32		{q1,q2}, [r5]
	add			r5, r3, #92
	vld1.32		{q3,q4}, [r5]
	add			r5, r1, #20
	vld1.32		{q5,q6}, [r5] 
	vadd.f32	q11, q9, q10
	vsub.f32	q12, q9, q10
	vmul.f32	q10, q11, q4
	vmla.f32	q6, q12, q2
	vrev64.32	q11, q11
	vrev64.32	q12, q12
	vswp		d22, d23
	vswp		d24, d25
	vmul.f32	q9, q11, q3
	vmla.f32	q5, q12, q1
	add			r5, r2, #20
	vst1.32		{q9,q10}, [r5]
	mov			r5, #128
	vst1.32		{d10[0]}, [r0], r5
	vst1.32		{d10[1]}, [r0], r5
	vst1.32		{d11[0]}, [r0], r5
	vst1.32		{d11[1]}, [r0], r5
	vst1.32		{d12[0]}, [r0], r5
	vst1.32		{d12[1]}, [r0], r5
	vst1.32		{d13[0]}, [r0], r5
	vst1.32		{d13[1]}, [r0], r5
	
	add			r0, r4, #1792
	add			r5, r3, #56
	vld1.32		{q1}, [r3]
	vld1.32		{q2,q3}, [r5]
	add			r5, r3, #128
	vld1.32		{q4}, [r5]
	add			r5, r1, #56
	vld1.32		{q5}, [r1]
	vld1.32		{q6}, [r5]
	vadd.f32	q9, q7, q8
	vsub.f32	q10, q7, q8
	vmul.f32	q7, q9, q3
	vmla.f32	q5, q10, q1
	vrev64.32	q9, q9
	vrev64.32	q10, q10
	vswp		d18, d19
	vswp		d20, d21
	vmul.f32	q8, q9, q4
	vmla.f32	q6, q10, q2
	add			r5, r2, #56
	vst1.32		{q7}, [r2]
	vst1.32		{q8}, [r5]
	mov			r5, #128
	vst1.32		{d10[0]}, [r4], r5
	vst1.32		{d10[1]}, [r4], r5
	vst1.32		{d11[0]}, [r4], r5
	vst1.32		{d11[1]}, [r4], r5
	vst1.32		{d12[0]}, [r0], r5
	vst1.32		{d12[1]}, [r0], r5
	vst1.32		{d13[0]}, [r0], r5
	vst1.32		{d13[1]}, [r0], r5
	
	vtrn.32		d0, d1
	add			r5, r3, #16
	vld1.32		{d2}, [r5]
	add			r5, r3, #52
	vld1.32		{d3}, [r5]
	add			r5, r3, #88
	vld1.32		{d4}, [r5]
	add			r3, r3, #124
	vld1.32		{d5}, [r3]
	add			r5, r1, #16
	vld1.32		{d6}, [r5]
	add			r1, r1, #52
	vld1.32		{d7}, [r1]
	vadd.f32	d8, d0, d1
	vsub.f32	d9, d0, d1
	vmul.f32	d4, d8, d4
	vmul.f32	d5, d8, d5
	vmla.f32	d6, d9, d2
	vmla.f32	d7, d9, d3
	add			r2, r2, #16
	vst1.32		{d4[0]}, [r2]
	add			r2, r2, #36
	vst1.32		{d5[0]}, [r2]
	vst1.32		{d6[0]}, [r4]
	add			r4, r4, #1152
	vst1.32		{d7[0]}, [r4]
	
	vpop		{q4-q7}
	pop			{r4-r5, pc}

NONEXEC_STACK

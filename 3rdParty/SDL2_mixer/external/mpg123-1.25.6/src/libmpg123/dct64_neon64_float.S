/*
	dct64_neon64_float: NEON optimized dct64 for AArch64 (float output version)

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
costab_neon_aarch64:
	.word 1056974725
	.word 1057056395
	.word 1057223771
	.word 1057485416
	.word 1057855544
	.word 1058356026
	.word 1059019886
	.word 1059897405
	.word 1061067246
	.word 1062657950
	.word 1064892987
	.word 1066774581
	.word 1069414683
	.word 1073984175
	.word 1079645762
	.word 1092815430
	.word 1057005197
	.word 1057342072
	.word 1058087743
	.word 1059427869
	.word 1061799040
	.word 1065862217
	.word 1071413542
	.word 1084439708
	.word 1057128951
	.word 1058664893
	.word 1063675095
	.word 1076102863
	.word 1057655764
	.word 1067924853
	.word 1060439283
	.word 1060439283
	.text
	ALIGN4
	.globl ASM_NAME(dct64_real_neon64)
#ifdef __ELF__
	.type ASM_NAME(dct64_real_neon64), %function
#endif
ASM_NAME(dct64_real_neon64):
	add		x3, x2, #64
	adrp	x4, AARCH64_PCREL_HI(costab_neon_aarch64)
	add		x4, x4, AARCH64_PCREL_LO(costab_neon_aarch64)
	ld1		{v0.4s,v1.4s,v2.4s,v3.4s}, [x2]
	ld1		{v16.4s,v17.4s,v18.4s,v19.4s}, [x3]
	ld1		{v20.4s,v21.4s,v22.4s,v23.4s}, [x4], #64
	
	rev64	v19.4s, v19.4s
	rev64	v18.4s, v18.4s
	rev64	v17.4s, v17.4s
	rev64	v16.4s, v16.4s
	ext		v4.16b, v19.16b, v19.16b, #8
	ext		v5.16b, v18.16b, v18.16b, #8
	ext		v6.16b, v17.16b, v17.16b, #8
	ext		v7.16b, v16.16b, v16.16b, #8
	
	fsub	v16.4s, v3.4s, v7.4s
	fsub	v17.4s, v2.4s, v6.4s
	fsub	v18.4s, v1.4s, v5.4s
	fsub	v19.4s, v0.4s, v4.4s
	fadd	v0.4s, v0.4s, v4.4s		/* bs[0,1,2,3] */
	fadd	v1.4s, v1.4s, v5.4s		/* bs[4,5,6,7] */
	fadd	v2.4s, v2.4s, v6.4s		/* bs[8,9,10,11] */
	fadd	v3.4s, v3.4s, v7.4s		/* bs[12,13,14,15] */
	fmul	v16.4s, v16.4s, v23.4s	/* bs[19,18,17,16] */
	fmul	v17.4s, v17.4s, v22.4s	/* bs[23,22,21,20] */
	fmul	v18.4s, v18.4s, v21.4s	/* bs[27,26,25,24] */
	fmul	v19.4s, v19.4s, v20.4s	/* bs[31,30,29,28] */
	
	ld1		{v20.4s, v21.4s}, [x4], #32
	rev64	v22.4s, v3.4s
	rev64	v23.4s, v2.4s
	rev64	v24.4s, v16.4s
	rev64	v25.4s, v17.4s
	ext		v4.16b, v22.16b, v22.16b, #8	/* bs[15,14,13,12] */
	ext		v5.16b, v23.16b, v23.16b, #8	/* bs[11,10,9,8] */
	ext		v6.16b, v24.16b, v24.16b, #8	/* bs[16,17,18,19] */
	ext		v7.16b, v25.16b, v25.16b, #8	/* bs[20,21,22,23] */
	
	fsub	v26.4s, v1.4s, v5.4s
	fsub	v27.4s, v0.4s, v4.4s
	fsub	v28.4s, v18.4s, v7.4s
	fsub	v29.4s, v19.4s, v6.4s
	fadd	v4.4s, v0.4s, v4.4s		/* bs[32,33,34,35] */
	fadd	v5.4s, v1.4s, v5.4s		/* bs[36,37,38,39] */
	fadd	v6.4s, v6.4s, v19.4s	/* bs[48,49,50,51] */
	fadd	v7.4s, v7.4s, v18.4s	/* bs[52,53,54,55] */
	fmul	v26.4s, v26.4s, v21.4s	/* bs[43,42,41,40] */
	fmul	v27.4s, v27.4s, v20.4s	/* bs[47,46,45,44] */
	fmul	v28.4s, v28.4s, v21.4s	/* bs[59,58,57,56] */
	fmul	v29.4s, v29.4s, v20.4s	/* bs[63,62,61,60] */
	
	ld1		{v20.4s}, [x4], #16
	rev64	v16.4s, v5.4s
	rev64	v17.4s, v26.4s
	rev64	v18.4s, v7.4s
	rev64	v19.4s, v28.4s
	ext		v0.16b, v16.16b, v16.16b, #8	/* bs[39,38,37,36] */
	ext		v1.16b, v17.16b, v17.16b, #8	/* bs[40,41,42,43] */
	ext		v2.16b, v18.16b, v18.16b, #8	/* bs[55,54,53,52] */
	ext		v3.16b, v19.16b, v19.16b, #8	/* bs[56,57,58,59] */
	
	fsub	v16.4s, v4.4s, v0.4s
	fsub	v17.4s, v27.4s, v1.4s
	fsub	v18.4s, v6.4s, v2.4s
	fsub	v19.4s, v29.4s, v3.4s
	fadd	v0.4s, v4.4s, v0.4s		/* bs[0,1,2,3] */
	fadd	v1.4s, v1.4s, v27.4s	/* bs[8,9,10,11] */
	fadd	v2.4s, v6.4s, v2.4s		/* bs[16,17,18,19] */
	fadd	v3.4s, v3.4s, v29.4s	/* bs[24,25,26,27] */
	fmul	v16.4s, v16.4s, v20.4s	/* bs[7,6,5,4] */
	fmul	v17.4s, v17.4s, v20.4s	/* bs[15,14,13,12] */
	fmul	v18.4s, v18.4s, v20.4s	/* bs[23,22,21,20] */
	fmul	v19.4s, v19.4s, v20.4s	/* bs[31,30,29,28] */
	
	ld1		{v28.4s}, [x4]
	zip1	v4.2d, v0.2d, v16.2d	/* bs[0,1,7,6] */
	zip2	v5.2d, v0.2d, v16.2d	/* bs[2,3,5,4] */
	zip1	v6.2d, v1.2d, v17.2d	/* bs[8,9,15,14] */
	zip2	v7.2d, v1.2d, v17.2d	/* bs[10,11,13,12] */
	zip1	v20.2d, v2.2d, v18.2d	/* bs[16,17,23,22] */
	zip2	v21.2d, v2.2d, v18.2d	/* bs[18,19,21,20] */
	zip1	v22.2d, v3.2d, v19.2d	/* bs[24,25,31,30] */
	zip2	v23.2d, v3.2d, v19.2d	/* bs[26,27,29,28] */
	rev64	v5.4s, v5.4s			/* bs[3,2,4,5] */
	rev64	v7.4s, v7.4s			/* bs[11,10,12,13] */
	rev64	v21.4s, v21.4s			/* bs[19,18,20,21] */
	rev64	v23.4s, v23.4s			/* bs[27,26,28,29] */
	AARCH64_DUP_2D(v29, v28, 0)
	AARCH64_DUP_4S(v28, v28, 2)
	
	fsub	v16.4s, v4.4s, v5.4s	
	fsub	v17.4s, v6.4s, v7.4s
	fsub	v18.4s, v20.4s, v21.4s
	fsub	v19.4s, v22.4s, v23.4s
	fadd	v0.4s, v4.4s, v5.4s		/* bs[32,33,36,37] */
	fadd	v1.4s, v6.4s, v7.4s		/* bs[40,41,44,45] */
	fadd	v2.4s, v20.4s, v21.4s	/* bs[48,49,52,53] */
	fadd	v3.4s, v22.4s, v23.4s	/* bs[56,57,60,61] */
	fmul	v16.4s, v16.4s, v29.4s	/* bs[35,34,39,38] */
	fmul	v17.4s, v17.4s, v29.4s	/* bs[43,42,47,46] */
	fmul	v18.4s, v18.4s, v29.4s	/* bs[51,50,55,54] */
	fmul	v19.4s, v19.4s, v29.4s	/* bs[59,58,63,62] */
	
	uzp1	v4.4s, v0.4s, v16.4s	/* bs[32,36,35,39] */
	uzp2	v5.4s, v0.4s, v16.4s	/* bs[33,37,34,38] */
	uzp1	v6.4s, v1.4s, v17.4s	/* bs[40,44,43,47] */
	uzp2	v7.4s, v1.4s, v17.4s	/* bs[41,45,42,46] */
	uzp1	v20.4s, v2.4s, v18.4s	/* bs[48,52,51,55] */
	uzp2	v21.4s, v2.4s, v18.4s	/* bs[49,53,50,54] */
	uzp1	v22.4s, v3.4s, v19.4s	/* bs[56,60,59,63] */
	uzp2	v23.4s, v3.4s, v19.4s	/* bs[57,61,58,62] */
	
	fsub	v16.4s, v4.4s, v5.4s
	fsub	v17.4s, v6.4s, v7.4s
	fsub	v18.4s, v20.4s, v21.4s
	fsub	v19.4s, v22.4s, v23.4s
	fadd	v0.4s, v4.4s, v5.4s		/* bs[0,4,2,6] */
	fadd	v1.4s, v6.4s, v7.4s		/* bs[8,12,10,14] */
	fadd	v2.4s, v20.4s, v21.4s	/* bs[16,20,18,22] */
	fadd	v3.4s, v22.4s, v23.4s	/* bs[24,28,26,30] */
	fmul	v16.4s, v16.4s, v28.4s	/* bs[1,5,3,7] */
	fmul	v17.4s, v17.4s, v28.4s	/* bs[9,13,11,15] */
	fmul	v18.4s, v18.4s, v28.4s	/* bs[17,21,19,23] */
	fmul	v19.4s, v19.4s, v28.4s	/* bs[25,29,27,31] */
	
	zip2	v4.2d, v0.2d, v1.2d		/* bs[2,6,10,14] */
	zip2	v5.2d, v16.2d, v17.2d	/* bs[3,7,11,15] */
	zip2	v6.2d, v2.2d, v3.2d		/* bs[18,22,26,30] */
	zip2	v7.2d, v18.2d, v19.2d	/* bs[19,23,27,31] */
	fadd	v4.4s, v4.4s, v5.4s		/* bs[2,6,10,14] */
	fadd	v6.4s, v6.4s, v7.4s		/* bs[18,22,26,30] */
	ins		v0.d[1], v4.d[0]		/* bs[0,4,2,6] */
	ins		v1.d[1], v4.d[1]		/* bs[8,12,10,14] */
	ins		v2.d[1], v6.d[0]		/* bs[16,20,18,22] */
	ins		v3.d[1], v6.d[1]		/* bs[24,28,26,30] */
	
	eor		v31.16b, v31.16b, v31.16b
	zip1	v4.4s, v0.4s, v16.4s	/* bs[0,1,4,5] */
	zip2	v5.4s, v0.4s, v16.4s	/* bs[2,3,6,7] */
	zip1	v6.4s, v1.4s, v17.4s	/* bs[8,9,12,13] */
	zip2	v7.4s, v1.4s, v17.4s	/* bs[10,11,14,15] */
	zip1	v20.4s, v2.4s, v18.4s	/* bs[16,17,20,21] */
	zip2	v21.4s, v2.4s, v18.4s	/* bs[18,19,22,23] */
	zip1	v22.4s, v3.4s, v19.4s	/* bs[24,25,28,29] */
	zip2	v23.4s, v3.4s, v19.4s	/* bs[26,27,30,31] */
	zip1	v0.2d, v4.2d, v5.2d		/* bs[0,1,2,3] */
	zip2	v1.2d, v4.2d, v5.2d		/* bs[4,5,6,7] */
	zip1	v2.2d, v6.2d, v7.2d		/* bs[8,9,10,11] */
	zip2	v3.2d, v6.2d, v7.2d		/* bs[12,13,14,15] */
	rev64	v16.4s, v4.4s
	rev64	v17.4s,	v6.4s
	zip1	v24.2d, v7.2d, v17.2d
	zip2	v16.2d, v5.2d, v16.2d
	zip2	v17.2d, v7.2d, v17.2d
	zip1	v4.2d, v20.2d, v21.2d	/* bs[16,17,18,19] */
	zip2	v5.2d, v20.2d, v21.2d	/* bs[20,21,22,23] */
	zip1	v6.2d, v22.2d, v23.2d	/* bs[24,25,26,27] */
	zip2	v7.2d, v22.2d, v23.2d	/* bs[28,29,30,31] */
	rev64	v18.4s, v20.4s
	rev64	v19.4s, v22.4s
	zip1	v25.2d, v23.2d, v19.2d
	zip1	v26.2d, v21.2d, v18.2d
	zip2	v18.2d, v21.2d, v18.2d
	zip2	v19.2d, v23.2d, v19.2d
	ins		v16.s[3], v31.s[0]		/* bs[6,7,5,-] */
	ins		v17.s[3], v31.s[0]		/* bs[14,15,13,-] */
	ins		v18.s[3], v31.s[0]		/* bs[22,23,21,-] */
	ins		v19.s[3], v31.s[0]		/* bs[30,31,29,-] */
	ins		v24.s[3], v31.s[0]		/* bs[10,11,9,-] */
	ins		v25.s[3], v31.s[0]		/* bs[26,27,25,-] */
	ins		v26.s[3], v31.s[0]		/* bs[18,19,17,-] */
	
	fadd	v1.4s, v1.4s, v16.4s
	fadd	v3.4s, v3.4s, v17.4s
	fadd	v5.4s, v5.4s, v18.4s
	fadd	v7.4s, v7.4s, v19.4s
	
	fadd	v2.4s, v2.4s, v3.4s
	fadd	v3.4s, v3.4s, v24.4s
	fadd	v6.4s, v6.4s, v7.4s
	fadd	v7.4s, v7.4s, v25.4s
	
	fadd	v4.4s, v4.4s, v6.4s
	fadd	v6.4s, v6.4s, v5.4s
	fadd	v5.4s, v5.4s, v7.4s
	fadd	v7.4s, v7.4s, v26.4s
	
	mov		x3, #64
	st1		{v0.s}[1], [x0], x3
	st1		{v7.s}[2], [x0], x3
	st1		{v3.s}[2], [x0], x3
	st1		{v5.s}[2], [x0], x3
	st1		{v1.s}[2], [x0], x3
	st1		{v6.s}[2], [x0], x3
	st1		{v2.s}[2], [x0], x3
	st1		{v4.s}[2], [x0], x3
	st1		{v0.s}[2], [x0], x3
	st1		{v7.s}[0], [x0], x3
	st1		{v3.s}[0], [x0], x3
	st1		{v5.s}[0], [x0], x3
	st1		{v1.s}[0], [x0], x3
	st1		{v6.s}[0], [x0], x3
	st1		{v2.s}[0], [x0], x3
	st1		{v4.s}[0], [x0], x3
	st1		{v0.s}[0], [x0]
	st1		{v0.s}[1], [x1], x3
	st1		{v4.s}[1], [x1], x3
	st1		{v2.s}[1], [x1], x3
	st1		{v6.s}[1], [x1], x3
	st1		{v1.s}[1], [x1], x3
	st1		{v5.s}[1], [x1], x3
	st1		{v3.s}[1], [x1], x3
	st1		{v7.s}[1], [x1], x3
	st1		{v0.s}[3], [x1], x3
	st1		{v4.s}[3], [x1], x3
	st1		{v2.s}[3], [x1], x3
	st1		{v6.s}[3], [x1], x3
	st1		{v1.s}[3], [x1], x3
	st1		{v5.s}[3], [x1], x3
	st1		{v3.s}[3], [x1], x3
	st1		{v7.s}[3], [x1]
	
	ret

NONEXEC_STACK

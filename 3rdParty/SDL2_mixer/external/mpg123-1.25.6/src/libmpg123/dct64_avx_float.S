/*
	dct64_x86_64_float: SSE optimized dct64 for x86-64 (float output version)

	copyright 1995-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

#include "mangle.h"

#define samples %rdx
#define costab %rcx
#define out0 %rdi
#define out1 %rsi

/*
	void dct64_real_avx(real *out0, real *out1, real *samples);
*/

#ifndef __APPLE__
	.section	.rodata
#else
	.data
#endif
	ALIGN32
costab_avx:
	.long 1056974725
	.long 1057056395
	.long 1057223771
	.long 1057485416
	.long 1057855544
	.long 1058356026
	.long 1059019886
	.long 1059897405
	.long 1061067246
	.long 1062657950
	.long 1064892987
	.long 1066774581
	.long 1069414683
	.long 1073984175
	.long 1079645762
	.long 1092815430
	.long 1057005197
	.long 1057342072
	.long 1058087743
	.long 1059427869
	.long 1061799040
	.long 1065862217
	.long 1071413542
	.long 1084439708
	.long 1057128951
	.long 1058664893
	.long 1063675095
	.long 1076102863
	.long 1057655764
	.long 1067924853
	.long 1060439283
	.long 0
	.text
	ALIGN16
.globl ASM_NAME(dct64_real_avx)
ASM_NAME(dct64_real_avx):
#ifdef IS_MSABI
	push		%rbp
	mov			%rsp, %rbp
	sub			$112, %rsp
	movaps		%xmm6, (%rsp)
	movaps		%xmm7, 16(%rsp)
	movaps		%xmm8, 32(%rsp)
	movaps		%xmm9, 48(%rsp)
	movaps		%xmm10, 64(%rsp)
	movaps		%xmm11, 80(%rsp)
	movaps		%xmm12, 96(%rsp)
	push		%rdi
	push		%rsi
	mov			%rcx, %rdi
	mov			%rdx, %rsi
	mov			%r8, %rdx
#endif
	leaq		costab_avx(%rip), costab

	vmovups		(samples), %ymm0			# input[0,1,2,3,4,5,6,7]
	vmovups		32(samples), %ymm1			# input[8,9,10,11,12,13,14,15]
	vperm2f128	$0x23, 64(samples), %ymm2, %ymm2
	vperm2f128	$0x23, 96(samples), %ymm3, %ymm3
	vshufps		$0x1b, %ymm2, %ymm2, %ymm2	# input[23,22,21,20,19,18,17,16]
	vshufps		$0x1b, %ymm3, %ymm3, %ymm3	# input[31,30,29,28,27,26,25,24]
	vsubps		%ymm2, %ymm1, %ymm6
	vsubps		%ymm3, %ymm0, %ymm7
	vaddps		%ymm0, %ymm3, %ymm4			# bufs[0,1,2,3,4,5,6,7]
	vaddps		%ymm1, %ymm2, %ymm5			# bufs[8,9,10,11,12,13,14,15]
	vmulps		(costab), %ymm7, %ymm7		# bufs[31,30,29,28,27,26,25,24] cos64[0,1,2,3,4,5,6,7]
	vmulps		32(costab), %ymm6, %ymm6	# bufs[23,22,21,20,19,18,17,16] cos64[8,9,10,11,12,13,14,15]
	
	vmovaps		64(costab), %ymm8			# cos32[0,1,2,3,4,5,6,7]
	
	vshufps		$0x1b, %ymm5, %ymm5, %ymm5
	vshufps		$0x1b, %ymm6, %ymm6, %ymm6
	vperm2f128	$0x01, %ymm5, %ymm5, %ymm5	# bufs[15,14,13,12,11,10,9,8]
	vperm2f128	$0x01, %ymm6, %ymm6, %ymm6	# bufs[16,17,18,19,20,21,22,23]
	vsubps		%ymm5, %ymm4, %ymm1
	vsubps		%ymm6, %ymm7, %ymm3
	vaddps		%ymm5, %ymm4, %ymm0			# bufs[32,33,34,35,36,37,38,39]
	vaddps		%ymm6, %ymm7, %ymm2			# bufs[48,49,50,51,52,53,54,55]
	vmulps		%ymm1, %ymm8, %ymm1			# bufs[47,46,45,44,43,42,41,40]
	vmulps		%ymm3, %ymm8, %ymm3			# bufs[63,62,61,60,59,58,57,56]
	
	vmovaps		96(costab), %ymm8			# cos16[0,1,2,3]:cos8[0,1]:cos4[0]:-
	vperm2f128	$0x00, %ymm8, %ymm8, %ymm9	# cos16[0,1,2,3,0,1,2,3]
	
	vperm2f128	$0x20, %ymm1, %ymm0, %ymm4	# bufs[32,33,34,35,47,46,45,44]
	vperm2f128	$0x31, %ymm1, %ymm0, %ymm5
	vshufps		$0x1b, %ymm5, %ymm5, %ymm5	# bufs[39,38,37,36,40,41,42,43]
	vperm2f128	$0x20, %ymm3, %ymm2, %ymm6	# bufs[48,49,50,51,63,62,61,60]
	vperm2f128	$0x31, %ymm3, %ymm2, %ymm7
	vshufps		$0x1b, %ymm7, %ymm7, %ymm7	# bufs[55,54,53,52,56,57,58,59]
	vsubps		%ymm5, %ymm4, %ymm1
	vsubps		%ymm7, %ymm6, %ymm3
	vaddps		%ymm5, %ymm4, %ymm0			# bufs[0,1,2,3,8,9,10,11]
	vaddps		%ymm7, %ymm6, %ymm2			# bufs[16,17,18,19,24,25,26,27]
	vmulps		%ymm1, %ymm9, %ymm1			# bufs[7,6,5,4,15,14,13,12]
	vmulps		%ymm3, %ymm9, %ymm3			# bufs[23,22,21,20,31,30,29,28]
	
	vperm2f128	$0x11, %ymm8, %ymm8, %ymm8	# cos8[0,1]:cos4[0]:-:cos8[0,1]:cos4[0]:-
	vmovddup	%ymm8, %ymm9				# cos8[0,1,0,1,0,1,0,1]
	
	vunpcklps	%ymm1, %ymm0, %ymm4			# bufs[0,7,1,6,8,15,9,14]
	vunpckhps	%ymm1, %ymm0, %ymm5			# bufs[2,5,3,4,10,13,11,12]
	vunpcklps	%ymm3, %ymm2, %ymm6			# bufs[16,23,17,22,24,31,25,30]
	vunpckhps	%ymm3, %ymm2, %ymm7			# bufs[18,21,19,20,26,29,27,28]
	vshufps		$0xd8, %ymm4, %ymm4, %ymm4	# bufs[0,1,7,6,8,9,15,14]
	vshufps		$0x72, %ymm5, %ymm5, %ymm5	# bufs[3,2,4,5,11,10,12,13]
	vshufps		$0xd8, %ymm6, %ymm6, %ymm6	# bufs[16,17,23,22,24,25,31,30]
	vshufps		$0x72, %ymm7, %ymm7, %ymm7	# bufs[19,18,20,21,27,26,28,29]
	vsubps		%ymm5, %ymm4, %ymm1
	vsubps		%ymm7, %ymm6, %ymm3
	vaddps		%ymm5, %ymm4, %ymm0			# bufs[32,33,36,37,40,41,44,45]
	vaddps		%ymm7, %ymm6, %ymm2			# bufs[48,49,52,53,56,57,60,61]
	vmulps		%ymm1, %ymm9, %ymm1			# bufs[35,34,39,38,43,42,47,46]
	vmulps		%ymm3, %ymm9, %ymm3			# bufs[51,50,55,54,59,58,63,62]
	
	vpermilps	$0xaa, %ymm8, %ymm8			# cos4[0,0,0,0,0,0,0,0]
	
	vshufps		$0xd8, %ymm0, %ymm0, %ymm0	# bufs[32,36,33,37,40,44,41,45]
	vshufps		$0xd8, %ymm1, %ymm1, %ymm1	# bufs[35,39,34,38,43,47,42,46]
	vshufps		$0xd8, %ymm2, %ymm2, %ymm2	# bufs[48,52,49,53,56,60,57,61]
	vshufps		$0xd8, %ymm3, %ymm3, %ymm3	# bufs[51,55,50,54,59,63,58,62]
	vunpcklps	%ymm1, %ymm0, %ymm4			# bufs[32,35,36,39,40,43,44,47]
	vunpckhps	%ymm1, %ymm0, %ymm5			# bufs[33,34,37,38,41,42,45,46]
	vunpcklps	%ymm3, %ymm2, %ymm6			# bufs[48,51,52,55,56,59,60,63]
	vunpckhps	%ymm3, %ymm2, %ymm7			# bufs[49,50,53,54,57,58,61,62]
	vsubps		%ymm5, %ymm4, %ymm1
	vsubps		%ymm7, %ymm6, %ymm3
	vaddps		%ymm5, %ymm4, %ymm0			# bufs[0,2,4,6,8,10,12,14]
	vaddps		%ymm7, %ymm6, %ymm2			# bufs[16,18,20,22,24,26,28,30]
	vmulps		%ymm1, %ymm8, %ymm1			# bufs[1,3,5,7,9,11,13,15]
	vmulps		%ymm3, %ymm8, %ymm3			# bufs[17,19,21,23,25,27,29,31]
	
	vxorps		%ymm8, %ymm8, %ymm8
	vblendps	$0xaa, %ymm1, %ymm8, %ymm5
	vblendps	$0xaa, %ymm3, %ymm8, %ymm6
	vaddps		%ymm5, %ymm0, %ymm0
	vaddps		%ymm6, %ymm2, %ymm2
	vunpcklps	%ymm1, %ymm0, %ymm4			# bufs[0,1,2,3,8,9,10,11]
	vunpckhps	%ymm1, %ymm0, %ymm5			# bufs[4,5,6,7,12,13,14,15]
	vunpcklps	%ymm3, %ymm2, %ymm6			# bufs[16,17,18,19,24,25,26,27]
	vunpckhps	%ymm3, %ymm2, %ymm7			# bufs[20,21,22,23,28,29,30,31]
	
	vextractf128	$0x1, %ymm4, %xmm0		# bufs[8,9,10,11]
	vextractf128	$0x1, %ymm5, %xmm1		# bufs[12,13,14,15]
	vextractf128	$0x1, %ymm6, %xmm2		# bufs[24,25,26,27]
	vextractf128	$0x1, %ymm7, %xmm3		# bufs[28,29,30,31]
	
	vshufps		$0x1e, %xmm5, %xmm5, %xmm9	# bufs[6,7,5,4]
	vshufps		$0x1e, %xmm1, %xmm1, %xmm10	# bufs[14,15,13,12]
	vshufps		$0x1e, %xmm7, %xmm7, %xmm11	# bufs[22,23,21,20]
	vshufps		$0x1e, %xmm3, %xmm3, %xmm12	# bufs[30,31,29,28]
	vblendps	$0x7, %xmm9, %xmm8, %xmm9	# bufs[6,7,5,-]
	vblendps	$0x7, %xmm10, %xmm8, %xmm10 # bufs[14,15,13,-]
	vblendps	$0x7, %xmm11, %xmm8, %xmm11	# bufs[22,23,21,-]
	vblendps	$0x7, %xmm12, %xmm8, %xmm12	# bufs[30,31,29,-]
	vaddps		%xmm5, %xmm9, %xmm5
	vaddps		%xmm1, %xmm10, %xmm1
	vaddps		%xmm7, %xmm11, %xmm7
	vaddps		%xmm3, %xmm12, %xmm3
	
	prefetcht0	1024(out0)
	
	vshufps		$0x1e, %xmm0, %xmm0, %xmm9	# bufs[10,11,9,8]
	vshufps		$0x1e, %xmm2, %xmm2, %xmm10	# bufs[26,27,25,24]
	vaddps		%xmm1, %xmm0, %xmm0
	vaddps		%xmm3, %xmm2, %xmm2
	vblendps	$0x7, %xmm9, %xmm8, %xmm9	# bufs[10,11,9,-]
	vblendps	$0x7, %xmm10, %xmm8, %xmm10	# bufs[26,27,25,-]
	vaddps		%xmm1, %xmm9, %xmm1
	vaddps		%xmm3, %xmm10, %xmm3
	
	vzeroupper
	prefetcht0	1024(out1)
	
	addq		$1024, out0
	movq		$-128, %rax
	movss		%xmm4, (out0)
	movss		%xmm0, (out0,%rax,1)
	movss		%xmm5, (out0,%rax,2)
	movss		%xmm1, -128(out0,%rax,2)
	leaq		(out0,%rax,4), out0
	movhlps		%xmm4, %xmm9
	movhlps		%xmm0, %xmm10
	movhlps		%xmm5, %xmm11
	movhlps		%xmm1, %xmm12
	vmovss		%xmm9, (out0)
	vmovss		%xmm10, (out0,%rax,1)
	vmovss		%xmm11, (out0,%rax,2)
	vmovss		%xmm12, -128(out0,%rax,2)
	leaq		(out0,%rax,4), out0
	negq		%rax
	shufps		$0xb1, %xmm4, %xmm4
	shufps		$0xb1, %xmm0, %xmm0
	shufps		$0xb1, %xmm5, %xmm5
	shufps		$0xb1, %xmm1, %xmm1
	movss		%xmm4, (out0)
	movss		%xmm4, (out1)
	leaq		(out1,%rax,1), out1
	movss		%xmm0, (out1)
	movss		%xmm5, (out1,%rax,1)
	movss		%xmm1, (out1,%rax,2)
	leaq		(out1,%rax,4), out1
	movhlps		%xmm4, %xmm4
	movhlps		%xmm0, %xmm0
	movhlps		%xmm5, %xmm5
	movhlps		%xmm1, %xmm1
	movss		%xmm4, -128(out1)
	movss		%xmm0, (out1)
	movss		%xmm5, (out1,%rax,1)
	movss		%xmm1, (out1,%rax,2)
	
	leaq		-64(out0,%rax,8), out0
	negq		%rax
	vshufps		$0x1e, %xmm6, %xmm6, %xmm0
	vblendps	$0x7, %xmm0, %xmm8, %xmm0
	addps		%xmm2, %xmm6
	addps		%xmm7, %xmm2
	addps		%xmm3, %xmm7
	addps		%xmm0, %xmm3
	movss		%xmm6, (out0)
	movss		%xmm2, (out0,%rax,1)
	movss		%xmm7, (out0,%rax,2)
	movss		%xmm3, -128(out0,%rax,2)
	leaq		(out0,%rax,4), out0
	movhlps		%xmm6, %xmm0
	movhlps		%xmm2, %xmm1
	movhlps		%xmm7, %xmm4
	movhlps		%xmm3, %xmm5
	movss		%xmm0, (out0)
	movss		%xmm1, (out0,%rax,1)
	movss		%xmm4, (out0,%rax,2)
	movss		%xmm5, -128(out0,%rax,2)
	leaq		64(out1,%rax,4), out1
	negq		%rax
	shufps		$0xb1, %xmm6, %xmm6
	shufps		$0xb1, %xmm2, %xmm2
	shufps		$0xb1, %xmm7, %xmm7
	shufps		$0xb1, %xmm3, %xmm3
	movss		%xmm6, -128(out1)
	movss		%xmm2, (out1)
	movss		%xmm7, (out1,%rax,1)
	movss		%xmm3, (out1,%rax,2)
	leaq		(out1,%rax,4), out1
	movhlps		%xmm6, %xmm6
	movhlps		%xmm2, %xmm2
	movhlps		%xmm7, %xmm7
	movhlps		%xmm3, %xmm3
	movss		%xmm6, -128(out1)
	movss		%xmm2, (out1)
	movss		%xmm7, (out1,%rax,1)
	movss		%xmm3, (out1,%rax,2)

#ifdef IS_MSABI
	pop			%rsi
	pop			%rdi
	movaps		(%rsp), %xmm6
	movaps		16(%rsp), %xmm7
	movaps		32(%rsp), %xmm8
	movaps		48(%rsp), %xmm9
	movaps		64(%rsp), %xmm10
	movaps		80(%rsp), %xmm11
	movaps		96(%rsp), %xmm12
	mov			%rbp, %rsp
	pop			%rbp
#endif
	ret

NONEXEC_STACK

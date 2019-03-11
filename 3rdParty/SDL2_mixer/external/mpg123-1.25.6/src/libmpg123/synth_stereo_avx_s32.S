/*
	synth_stereo_avx_s32: AVX optimized synth for x86-64 (stereo specific, s32 output version)

	copyright 1995-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

#include "mangle.h"

#ifdef IS_MSABI
/* real *window; */
#define WINDOW %r10
/* real *b0l; */
#define B0L %rdx
/* real *b0r; */
#define B0R %r8
/* real *samples; */
#define SAMPLES %r9
#else
/* real *window; */
#define WINDOW %rdi
/* real *b0l; */
#define B0L %rsi
/* real *b0r; */
#define B0R %rdx
/* real *samples; */
#define SAMPLES %r9
#endif

/*
	int synth_1to1_s32_s_avx_asm(real *window, real *b0l, real *b0r, real *samples, int bo1);
	return value: number of clipped samples
*/

#ifndef __APPLE__
	.section	.rodata
#else
	.data
#endif
	ALIGN32
maxmin_avx:
	.long   1191182335
	.long   1191182335
	.long   1191182335
	.long   1191182335
	.long   1191182335
	.long   1191182335
	.long   1191182335
	.long   1191182335
	.long   -956301312
	.long   -956301312
	.long   -956301312
	.long   -956301312
	.long   -956301312
	.long   -956301312
	.long   -956301312
	.long   -956301312
scale_avx:
	.long   1199570944
	.text
	ALIGN16
	.globl ASM_NAME(synth_1to1_s32_s_avx_asm)
ASM_NAME(synth_1to1_s32_s_avx_asm):
#ifdef IS_MSABI /* should save xmm6-15 */
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
	movl		48(%rbp), %eax /* 5th argument; placed after 32-byte shadow space */
#endif

	vbroadcastss	scale_avx(%rip), %ymm14
	
#ifdef IS_MSABI
	shl			$2, %eax
	mov			%rcx, WINDOW
#else
	mov			%r8d, %eax
	shl			$2, %eax
	mov			%rcx, SAMPLES
#endif
	add			$64, WINDOW
	sub			%rax, WINDOW

	mov			$128, %rax
	mov			$4, %ecx
	vpxor		%xmm15, %xmm15, %xmm15
	
	ALIGN16
1:
	vmovups		(WINDOW), %ymm8
	vmovups		32(WINDOW), %ymm9
	vmovups		(WINDOW,%rax), %ymm10
	vmovups		32(WINDOW,%rax), %ymm11
	vmulps		(B0L), %ymm8, %ymm0
	vmulps		32(B0L), %ymm9, %ymm1
	vmulps		(B0R), %ymm8, %ymm2
	vmulps		32(B0R), %ymm9, %ymm3
	vmulps		64(B0L), %ymm10, %ymm4
	vmulps		96(B0L), %ymm11, %ymm5
	vmulps		64(B0R), %ymm10, %ymm6
	vmulps		96(B0R), %ymm11, %ymm7
	vaddps		%ymm1, %ymm0, %ymm8
	vaddps		%ymm3, %ymm2, %ymm0
	vaddps		%ymm5, %ymm4, %ymm9
	vaddps		%ymm7, %ymm6, %ymm1
	lea			(WINDOW,%rax,2), WINDOW
	add			%rax, B0L
	add			%rax, B0R
	
	vmovups		(WINDOW), %ymm10
	vmovups		32(WINDOW), %ymm11
	vmovups		(WINDOW,%rax), %ymm12
	vmovups		32(WINDOW,%rax), %ymm13
	vmulps		(B0L), %ymm10, %ymm2
	vmulps		32(B0L), %ymm11, %ymm3
	vmulps		(B0R), %ymm10, %ymm4
	vmulps		32(B0R), %ymm11, %ymm5
	vmulps		64(B0L), %ymm12, %ymm6
	vmulps		96(B0L), %ymm13, %ymm10
	vmulps		64(B0R), %ymm12, %ymm7
	vmulps		96(B0R), %ymm13, %ymm11
	vaddps		%ymm3, %ymm2, %ymm2
	vaddps		%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm10, %ymm4
	vaddps		%ymm7, %ymm11, %ymm5
	lea			(WINDOW,%rax,2), WINDOW
	add			%rax, B0L
	add			%rax, B0R
	
	vunpcklps	%ymm0, %ymm8, %ymm6
	vunpckhps	%ymm0, %ymm8, %ymm0
	vunpcklps	%ymm1, %ymm9, %ymm7
	vunpckhps	%ymm1, %ymm9, %ymm1
	vaddps		%ymm6, %ymm0, %ymm0
	vaddps		%ymm7, %ymm1, %ymm1
	vunpcklps	%ymm3, %ymm2, %ymm6
	vunpckhps	%ymm3, %ymm2, %ymm2
	vunpcklps	%ymm5, %ymm4, %ymm7
	vunpckhps	%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm2, %ymm2
	vaddps		%ymm7, %ymm3, %ymm3
	
	vunpcklpd	%ymm1, %ymm0, %ymm4
	vunpckhpd	%ymm1, %ymm0, %ymm0
	vunpcklpd	%ymm3, %ymm2, %ymm5
	vunpckhpd	%ymm3, %ymm2, %ymm1
	vsubps		%ymm0, %ymm4, %ymm0
	vsubps		%ymm1, %ymm5, %ymm1
	vperm2f128	$0x20, %ymm1, %ymm0, %ymm2
	vperm2f128	$0x31, %ymm1, %ymm0, %ymm3
	vaddps		%ymm3, %ymm2, %ymm0
	vcmpnleps	maxmin_avx(%rip), %ymm0, %ymm1
	vcmpltps	32+maxmin_avx(%rip), %ymm0, %ymm2
	vmulps		%ymm14, %ymm0, %ymm0
	vextractf128	$0x1, %ymm1, %xmm3
	vextractf128	$0x1, %ymm2, %xmm4
	vpackssdw	%xmm2, %xmm1, %xmm5
	vpackssdw	%xmm4, %xmm3, %xmm3
	vcvtps2dq	%ymm0, %ymm0
	vpaddw		%xmm3, %xmm5, %xmm5
	vpaddw		%xmm5, %xmm15, %xmm15
	vxorps		%ymm1, %ymm0, %ymm0
	
	vmovups		%ymm0, (SAMPLES)
	add			$32, SAMPLES
	dec			%ecx
	jnz			1b
	
	mov			$4, %ecx
	
	ALIGN16
1:
	vmovups		(WINDOW), %ymm8
	vmovups		32(WINDOW), %ymm9
	vmovups		(WINDOW,%rax), %ymm10
	vmovups		32(WINDOW,%rax), %ymm11
	vmulps		(B0L), %ymm8, %ymm0
	vmulps		32(B0L), %ymm9, %ymm1
	vmulps		(B0R), %ymm8, %ymm2
	vmulps		32(B0R), %ymm9, %ymm3
	vmulps		-64(B0L), %ymm10, %ymm4
	vmulps		-32(B0L), %ymm11, %ymm5
	vmulps		-64(B0R), %ymm10, %ymm6
	vmulps		-32(B0R), %ymm11, %ymm7
	vaddps		%ymm1, %ymm0, %ymm8
	vaddps		%ymm3, %ymm2, %ymm0
	vaddps		%ymm5, %ymm4, %ymm9
	vaddps		%ymm7, %ymm6, %ymm1
	lea			(WINDOW,%rax,2), WINDOW
	sub			%rax, B0L
	sub			%rax, B0R
	
	vmovups		(WINDOW), %ymm10
	vmovups		32(WINDOW), %ymm11
	vmovups		(WINDOW,%rax), %ymm12
	vmovups		32(WINDOW,%rax), %ymm13
	vmulps		(B0L), %ymm10, %ymm2
	vmulps		32(B0L), %ymm11, %ymm3
	vmulps		(B0R), %ymm10, %ymm4
	vmulps		32(B0R), %ymm11, %ymm5
	vmulps		-64(B0L), %ymm12, %ymm6
	vmulps		-32(B0L), %ymm13, %ymm10
	vmulps		-64(B0R), %ymm12, %ymm7
	vmulps		-32(B0R), %ymm13, %ymm11
	vaddps		%ymm3, %ymm2, %ymm2
	vaddps		%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm10, %ymm4
	vaddps		%ymm7, %ymm11, %ymm5
	lea			(WINDOW,%rax,2), WINDOW
	sub			%rax, B0L
	sub			%rax, B0R
	
	vunpcklps	%ymm0, %ymm8, %ymm6
	vunpckhps	%ymm0, %ymm8, %ymm0
	vunpcklps	%ymm1, %ymm9, %ymm7
	vunpckhps	%ymm1, %ymm9, %ymm1
	vaddps		%ymm6, %ymm0, %ymm0
	vaddps		%ymm7, %ymm1, %ymm1
	vunpcklps	%ymm3, %ymm2, %ymm6
	vunpckhps	%ymm3, %ymm2, %ymm2
	vunpcklps	%ymm5, %ymm4, %ymm7
	vunpckhps	%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm2, %ymm2
	vaddps		%ymm7, %ymm3, %ymm3
	
	vunpcklpd	%ymm1, %ymm0, %ymm4
	vunpckhpd	%ymm1, %ymm0, %ymm0
	vunpcklpd	%ymm3, %ymm2, %ymm5
	vunpckhpd	%ymm3, %ymm2, %ymm1
	vaddps		%ymm0, %ymm4, %ymm0
	vaddps		%ymm1, %ymm5, %ymm1
	vperm2f128	$0x20, %ymm1, %ymm0, %ymm2
	vperm2f128	$0x31, %ymm1, %ymm0, %ymm3
	vaddps		%ymm3, %ymm2, %ymm0
	vcmpnleps	maxmin_avx(%rip), %ymm0, %ymm1
	vcmpltps	32+maxmin_avx(%rip), %ymm0, %ymm2
	vmulps		%ymm14, %ymm0, %ymm0
	vextractf128	$0x1, %ymm1, %xmm3
	vextractf128	$0x1, %ymm2, %xmm4
	vpackssdw	%xmm2, %xmm1, %xmm5
	vpackssdw	%xmm4, %xmm3, %xmm3
	vcvtps2dq	%ymm0, %ymm0
	vpaddw		%xmm3, %xmm5, %xmm5
	vpaddw		%xmm5, %xmm15, %xmm15
	vxorps		%ymm1, %ymm0, %ymm0
	
	vmovups		%ymm0, (SAMPLES)
	add			$32, SAMPLES
	dec			%ecx
	jnz			1b
	
	vzeroupper
	
	pxor		%xmm1, %xmm1
	psubw		%xmm15, %xmm1
	pshufd		$0x4e, %xmm1, %xmm0
	paddw		%xmm1, %xmm0
	pshuflw		$0x4e, %xmm0, %xmm1
	paddw		%xmm1, %xmm0
	pshuflw		$0x11, %xmm0, %xmm1
	paddw		%xmm1, %xmm0
	movd		%xmm0, %eax
	and			$0x7f, %eax
	
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

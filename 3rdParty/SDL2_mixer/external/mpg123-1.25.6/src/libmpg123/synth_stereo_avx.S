/*
	synth_stereo_avx: AVX optimized synth for x86-64 (stereo specific version)

	copyright 1995-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

#include "mangle.h"

#ifdef IS_MSABI
/* short *window; */
#define WINDOW %r10
/* short *b0l; */
#define B0L %rdx
/* short *b0r; */
#define B0R %r8
/* short *samples; */
#define SAMPLES %r9
#else
/* short *window; */
#define WINDOW %rdi
/* short *b0l; */
#define B0L %rsi
/* short *b0r; */
#define B0R %rdx
/* short *samples; */
#define SAMPLES %r9
#endif

/*
	int synth_1to1_s_avx_asm(short *window, short *b0l, short *b0r, short *samples, int bo1);
	return value: number of clipped samples
*/

	ALIGN16
.globl ASM_NAME(synth_1to1_s_avx_asm)
ASM_NAME(synth_1to1_s_avx_asm):
#ifdef IS_MSABI /* should save xmm6-15 */
	push		%rbp
	mov			%rsp, %rbp
	sub			$144, %rsp
	movaps		%xmm6, (%rsp)
	movaps		%xmm7, 16(%rsp)
	movaps		%xmm8, 32(%rsp)
	movaps		%xmm9, 48(%rsp)
	movaps		%xmm10, 64(%rsp)
	movaps		%xmm11, 80(%rsp)
	movaps		%xmm12, 96(%rsp)
	movaps		%xmm13, 112(%rsp)
	movaps		%xmm14, 128(%rsp)
	movl		48(%rbp), %eax /* 5th argument; placed after 32-byte shadow space */
#endif
	
#ifdef IS_MSABI
	shl			$1, %eax
	mov			%rcx, WINDOW
#else
	mov			%r8d, %eax
	shl			$1, %eax
	movq		%rcx, SAMPLES
#endif
	add			$32, WINDOW
	sub			%rax, WINDOW
	
	mov			$64, %rax
	movl		$4, %ecx
	vpxor		%xmm14, %xmm14, %xmm14
	
	ALIGN16
1:
	movups		(WINDOW), %xmm8
	movups		16(WINDOW), %xmm9
	movups		(WINDOW,%rax), %xmm10
	movups		16(WINDOW,%rax), %xmm11
	vpmaddwd	(B0L), %xmm8, %xmm0
	vpmaddwd	16(B0L), %xmm9, %xmm1
	vpmaddwd	(B0R), %xmm8, %xmm2
	vpmaddwd	16(B0R), %xmm9, %xmm3
	vpmaddwd	32(B0L), %xmm10, %xmm4
	vpmaddwd	48(B0L), %xmm11, %xmm5
	vpmaddwd	32(B0R), %xmm10, %xmm6
	vpmaddwd	48(B0R), %xmm11, %xmm7
	vpaddd		%xmm1, %xmm0, %xmm8
	vpaddd		%xmm3, %xmm2, %xmm0
	vpaddd		%xmm5, %xmm4, %xmm9
	vpaddd		%xmm7, %xmm6, %xmm1
	lea			(WINDOW,%rax,2), WINDOW
	add			%rax, B0L
	add			%rax, B0R
	
	movups		(WINDOW), %xmm10
	movups		16(WINDOW), %xmm11
	movups		(WINDOW,%rax), %xmm12
	movups		16(WINDOW,%rax), %xmm13
	vpmaddwd	(B0L), %xmm10, %xmm2
	vpmaddwd	16(B0L), %xmm11, %xmm3
	vpmaddwd	(B0R), %xmm10, %xmm4
	vpmaddwd	16(B0R), %xmm11, %xmm5
	vpmaddwd	32(B0L), %xmm12, %xmm6
	vpmaddwd	48(B0L), %xmm13, %xmm10
	vpmaddwd	32(B0R), %xmm12, %xmm7
	vpmaddwd	48(B0R), %xmm13, %xmm11
	vpaddd		%xmm3, %xmm2, %xmm2
	vpaddd		%xmm5, %xmm4, %xmm3
	vpaddd		%xmm6, %xmm10, %xmm4
	vpaddd		%xmm7, %xmm11, %xmm5
	lea			(WINDOW,%rax,2), WINDOW
	add			%rax, B0L
	add			%rax, B0R
	
	vpunpckldq	%xmm0, %xmm8, %xmm6
	vpunpckhdq	%xmm0, %xmm8, %xmm0
	vpunpckldq	%xmm1, %xmm9, %xmm7
	vpunpckhdq	%xmm1, %xmm9, %xmm1
	vpaddd		%xmm6, %xmm0, %xmm0
	vpaddd		%xmm7, %xmm1, %xmm1
	vpunpckldq	%xmm3, %xmm2, %xmm6
	vpunpckhdq	%xmm3, %xmm2, %xmm2
	vpunpckldq	%xmm5, %xmm4, %xmm7
	vpunpckhdq	%xmm5, %xmm4, %xmm3
	vpaddd		%xmm6, %xmm2, %xmm2
	vpaddd		%xmm7, %xmm3, %xmm3
	
	vpunpcklqdq	%xmm1, %xmm0, %xmm4
	vpunpckhqdq	%xmm1, %xmm0, %xmm0
	vpunpcklqdq	%xmm3, %xmm2, %xmm5
	vpunpckhqdq	%xmm3, %xmm2, %xmm1
	vpaddd		%xmm0, %xmm4, %xmm0
	vpaddd		%xmm1, %xmm5, %xmm1
	vpsrad		$13, %xmm0, %xmm0
	vpsrad		$13, %xmm1, %xmm1
	vpackssdw	%xmm1, %xmm0, %xmm2
	vpcmpeqd	%xmm3, %xmm3, %xmm3
	vpslld		$16, %xmm0, %xmm0
	vpslld		$16, %xmm1, %xmm1
	vpsrld		$16, %xmm0, %xmm0
	vpsrld		$16, %xmm1, %xmm1
	vpackusdw	%xmm1, %xmm0, %xmm0
	vpcmpeqw	%xmm2, %xmm0, %xmm0
	vpxor		%xmm3, %xmm0, %xmm0
	vpaddw		%xmm0, %xmm14, %xmm14
	
	movups		%xmm2, (SAMPLES)
	add			$16, SAMPLES
	dec			%ecx
	jnz			1b
	
	movl		$4, %ecx
	
	ALIGN16
1:
	movups		(WINDOW), %xmm8
	movups		16(WINDOW), %xmm9
	movups		(WINDOW,%rax), %xmm10
	movups		16(WINDOW,%rax), %xmm11
	vpmaddwd	(B0L), %xmm8, %xmm0
	vpmaddwd	16(B0L), %xmm9, %xmm1
	vpmaddwd	(B0R), %xmm8, %xmm2
	vpmaddwd	16(B0R), %xmm9, %xmm3
	vpmaddwd	-32(B0L), %xmm10, %xmm4
	vpmaddwd	-16(B0L), %xmm11, %xmm5
	vpmaddwd	-32(B0R), %xmm10, %xmm6
	vpmaddwd	-16(B0R), %xmm11, %xmm7
	vpaddd		%xmm1, %xmm0, %xmm8
	vpaddd		%xmm3, %xmm2, %xmm0
	vpaddd		%xmm5, %xmm4, %xmm9
	vpaddd		%xmm7, %xmm6, %xmm1
	lea			(WINDOW,%rax,2), WINDOW
	sub			%rax, B0L
	sub			%rax, B0R
	
	movups		(WINDOW), %xmm10
	movups		16(WINDOW), %xmm11
	movups		(WINDOW,%rax), %xmm12
	movups		16(WINDOW,%rax), %xmm13
	vpmaddwd	(B0L), %xmm10, %xmm2
	vpmaddwd	16(B0L), %xmm11, %xmm3
	vpmaddwd	(B0R), %xmm10, %xmm4
	vpmaddwd	16(B0R), %xmm11, %xmm5
	vpmaddwd	-32(B0L), %xmm12, %xmm6
	vpmaddwd	-16(B0L), %xmm13, %xmm10
	vpmaddwd	-32(B0R), %xmm12, %xmm7
	vpmaddwd	-16(B0R), %xmm13, %xmm11
	vpaddd		%xmm3, %xmm2, %xmm2
	vpaddd		%xmm5, %xmm4, %xmm3
	vpaddd		%xmm6, %xmm10, %xmm4
	vpaddd		%xmm7, %xmm11, %xmm5
	lea			(WINDOW,%rax,2), WINDOW
	sub			%rax, B0L
	sub			%rax, B0R
	
	vpunpckldq	%xmm0, %xmm8, %xmm6
	vpunpckhdq	%xmm0, %xmm8, %xmm0
	vpunpckldq	%xmm1, %xmm9, %xmm7
	vpunpckhdq	%xmm1, %xmm9, %xmm1
	vpaddd		%xmm6, %xmm0, %xmm0
	vpaddd		%xmm7, %xmm1, %xmm1
	vpunpckldq	%xmm3, %xmm2, %xmm6
	vpunpckhdq	%xmm3, %xmm2, %xmm2
	vpunpckldq	%xmm5, %xmm4, %xmm7
	vpunpckhdq	%xmm5, %xmm4, %xmm3
	vpaddd		%xmm6, %xmm2, %xmm2
	vpaddd		%xmm7, %xmm3, %xmm3
	
	vpunpcklqdq	%xmm1, %xmm0, %xmm4
	vpunpckhqdq	%xmm1, %xmm0, %xmm0
	vpunpcklqdq	%xmm3, %xmm2, %xmm5
	vpunpckhqdq	%xmm3, %xmm2, %xmm1
	vpaddd		%xmm0, %xmm4, %xmm0
	vpaddd		%xmm1, %xmm5, %xmm1
	vpsrad		$13, %xmm0, %xmm0
	vpsrad		$13, %xmm1, %xmm1
	vpackssdw	%xmm1, %xmm0, %xmm2
	vpcmpeqd	%xmm3, %xmm3, %xmm3
	vpslld		$16, %xmm0, %xmm0
	vpslld		$16, %xmm1, %xmm1
	vpsrld		$16, %xmm0, %xmm0
	vpsrld		$16, %xmm1, %xmm1
	vpackusdw	%xmm1, %xmm0, %xmm0
	vpcmpeqw	%xmm2, %xmm0, %xmm0
	vpxor		%xmm3, %xmm0, %xmm0
	vpaddw		%xmm0, %xmm14, %xmm14
	
	movups		%xmm2, (SAMPLES)
	add			$16, SAMPLES
	dec			%ecx
	jnz			1b
	
	pxor		%xmm1, %xmm1
	psubw		%xmm14, %xmm1
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
	mov			%rbp, %rsp
	pop			%rbp
#endif
	ret

NONEXEC_STACK

/*
	tabinit_mmx: make_decode_tables_mmx

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by the mysterious higway (apparently)

	See synth_mmx.S about license history.
*/

#include "mangle.h"

.data
	ALIGN32
.globl ASM_NAME(costab_mmxsse)
ASM_NAME(costab_mmxsse):
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
	ALIGN32
intwinbase:
	.short      0,    -1,    -1,    -1,    -1,    -1,    -1,    -2
	.short     -2,    -2,    -2,    -3,    -3,    -4,    -4,    -5
	.short     -5,    -6,    -7,    -7,    -8,    -9,   -10,   -11
	.short    -13,   -14,   -16,   -17,   -19,   -21,   -24,   -26
	.short    -29,   -31,   -35,   -38,   -41,   -45,   -49,   -53
	.short    -58,   -63,   -68,   -73,   -79,   -85,   -91,   -97
	.short   -104,  -111,  -117,  -125,  -132,  -139,  -147,  -154
	.short   -161,  -169,  -176,  -183,  -190,  -196,  -202,  -208
	.short   -213,  -218,  -222,  -225,  -227,  -228,  -228,  -227
	.short   -224,  -221,  -215,  -208,  -200,  -189,  -177,  -163
	.short   -146,  -127,  -106,   -83,   -57,   -29,     2,    36
	.short     72,   111,   153,   197,   244,   294,   347,   401
	.short    459,   519,   581,   645,   711,   779,   848,   919
	.short    991,  1064,  1137,  1210,  1283,  1356,  1428,  1498
	.short   1567,  1634,  1698,  1759,  1817,  1870,  1919,  1962
	.short   2001,  2032,  2057,  2075,  2085,  2087,  2080,  2063
	.short   2037,  2000,  1952,  1893,  1822,  1739,  1644,  1535
	.short   1414,  1280,  1131,   970,   794,   605,   402,   185
	.short    -45,  -288,  -545,  -814, -1095, -1388, -1692, -2006
	.short  -2330, -2663, -3004, -3351, -3705, -4063, -4425, -4788
	.short  -5153, -5517, -5879, -6237, -6589, -6935, -7271, -7597
	.short  -7910, -8209, -8491, -8755, -8998, -9219, -9416, -9585
	.short  -9727, -9838, -9916, -9959, -9966, -9935, -9863, -9750
	.short  -9592, -9389, -9139, -8840, -8492, -8092, -7640, -7134
	.short  -6574, -5959, -5288, -4561, -3776, -2935, -2037, -1082
	.short    -70,   998,  2122,  3300,  4533,  5818,  7154,  8540
	.short   9975, 11455, 12980, 14548, 16155, 17799, 19478, 21189
	.short  22929, 24694, 26482, 28289, 30112, 31947,-26209,-24360
	.short -22511,-20664,-18824,-16994,-15179,-13383,-11610, -9863
	.short  -8147, -6466, -4822, -3222, -1667,  -162,  1289,  2684
	.short   4019,  5290,  6494,  7629,  8692,  9679, 10590, 11420
	.short  12169, 12835, 13415, 13908, 14313, 14630, 14856, 14992
	.short  15038

intwindiv:
	.long 0x47800000			# 65536.0
.text
	ALIGN32
/* void make_decode_tables_mmx_asm(long scaleval, float* decwin_mmx, float *decwins); */
.globl ASM_NAME(make_decode_tables_mmx_asm)
ASM_NAME(make_decode_tables_mmx_asm):
	pushl %ebp
	mov %esp,%ebp
	sub $12,%esp
	pushl %edi
	pushl %esi
	pushl %ebx

	GET_GOT
	lea LOCAL_VAR(intwinbase),%edi
	mov LOCAL_VAR(intwindiv),%ecx
	mov %edi,-4(%ebp)
	lea 444(%edi),%eax
	mov %eax,-8(%ebp)
	mov %ecx,-12(%ebp)

/* stack: -12(%ebp)=*intwindiv, -8(%ebp)=intwinbase+444, -4(%ebp)=intwinbase, ... ,
          4(%ebp)=back, 8(%ebp)=scaleval, 12(%ebp)=decwin_mmx, 16(%ebp)=decwins */
#define INTWINDIV -12(%ebp)
#define INTWINBASE_PLUS_444 -8(%ebp)
#define INTWINBASE -4(%ebp)
#define SCALEVAL 8(%ebp)
#define DECWIN_MMX 12(%ebp)
#define DECWINS 16(%ebp)
	
	xorl %ecx,%ecx
	xorl %ebx,%ebx
	movl $32,%esi
	
	negl SCALEVAL	/* scaleval */
	pushl $2	/* intwinbase step */
.L00:
	cmpl $528,%ecx
	jnc .L02
	movswl (%edi),%eax
	cmpl INTWINBASE_PLUS_444,%edi
	jc .L01
	addl $60000,%eax
.L01:
	pushl %eax
	fildl (%esp)
	fdivs INTWINDIV
	fimull SCALEVAL /* scaleval */
/* eax used to be popped the line before... I'll just use it here a bit */
	movl DECWIN_MMX,%eax /* decwin_mmx */
	fsts    (%eax,%ecx,4)
	fstps 64(%eax,%ecx,4)
	popl %eax
.L02:
	leal -1(%esi),%edx
	andl %ebx,%edx
	cmpl $31,%edx
	jnz .L03
	addl $-1023,%ecx
	testl %esi,%ebx
	jz  .L03
	negl SCALEVAL
.L03:
	addl %esi,%ecx
	addl (%esp),%edi
	incl %ebx
	cmpl INTWINBASE,%edi
	jz .L04
	cmpl $256,%ebx
	jnz .L00
	negl (%esp)
	jmp .L00
.L04:
	popl %eax

	xorl %ecx,%ecx
	xorl %ebx,%ebx
	pushl $2 /* paired with popl above */
.L05:
	cmpl $528,%ecx
	jnc .L11
	movswl (%edi),%eax
	cmpl INTWINBASE_PLUS_444,%edi
	jc .L06
	addl $60000,%eax
.L06:
	cltd
	imull SCALEVAL
	shrdl $17,%edx,%eax
	cmpl $32767,%eax
	movl $1055,%edx
	jle .L07
	movl $32767,%eax
	jmp .L08
.L07:
	cmpl $-32767,%eax
	jge .L08
	movl $-32767,%eax
.L08:
/* going to use ebx for decwins, watch the jumps */
	pushl %ebx 
	movl DECWINS,%ebx
	cmpl $512,%ecx
	jnc .L09
	subl %ecx,%edx
	movw %ax,(%ebx,%edx,2) /* decwins */
	movw %ax,-32(%ebx,%edx,2)
.L09:
	testl $1,%ecx
	jnz .L10
	negl %eax
.L10:
	movw %ax,(%ebx,%ecx,2)
	movw %ax,32(%ebx,%ecx,2)
	popl %ebx /* that has to match the pushl before */
.L11:
	leal -1(%esi),%edx
	andl %ebx,%edx
	cmpl $31,%edx
	jnz .L12
	addl $-1023,%ecx
	testl %esi,%ebx
	jz  .L12
	negl SCALEVAL
.L12:
	addl %esi,%ecx
	addl (%esp),%edi
	incl %ebx
	cmpl INTWINBASE,%edi
	jz .L13
	cmpl $256,%ebx
	jnz .L05
	negl (%esp)
	jmp .L05
.L13:
	popl %eax
	
	popl %ebx
	popl %esi
	popl %edi
	mov %ebp,%esp
	pop %ebp
	ret

NONEXEC_STACK

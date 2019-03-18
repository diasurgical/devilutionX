/*  Draws a half-transparent rectangle by blacking out odd pixels on odd lines,
 *  even pixels on even lines.
 *
 *  TRANS_RECT_X = x offset of upper-left corner
 *  TRANS_RECT_Y = y offset of upper-left corner
 *  TRANS_RECT_WIDTH = width of rectangle
 *  TRANS_RECT_HEIGHT = height of rectangle
 */

#if !defined(TRANS_RECT_X) || !defined(TRANS_RECT_Y) || !defined(TRANS_RECT_WIDTH) || !defined(TRANS_RECT_HEIGHT)
#error ASM_TRANS_RECT: Parameter not set
#endif

#if defined(_MSC_VER) && defined(_M_IX86)
__asm {
	mov edi, gpBuffer
	; origin is at 64,160
	add edi, (160 + TRANS_RECT_Y + TRANS_RECT_HEIGHT - 1) * ROW_PITCH + 64 + TRANS_RECT_X
	xor eax, eax
	mov edx, TRANS_RECT_HEIGHT >> 1
yloop:
	mov ecx, TRANS_RECT_WIDTH >> 1
x0loop:
	stosb
	inc edi
	loop x0loop
#if (TRANS_RECT_WIDTH & 1)
	stosb
#endif
	sub edi, ROW_PITCH + TRANS_RECT_WIDTH
	mov ecx, TRANS_RECT_WIDTH >> 1
x1loop:
	inc edi
	stosb
	loop x1loop
	sub edi, ROW_PITCH + (TRANS_RECT_WIDTH & ~1)
	dec edx
	jnz yloop
#if (TRANS_RECT_HEIGHT & 1)
	mov ecx, TRANS_RECT_WIDTH >> 1
x2loop:
	stosb
	inc edi
	loop x2loop
#if (TRANS_RECT_WIDTH & 1)
	stosb
#endif
#endif
}
#else // _MSC_VER && _M_IX86
{
	int row, col;
	BYTE *pix = &gpBuffer[SCREENXY(TRANS_RECT_X, TRANS_RECT_Y + TRANS_RECT_HEIGHT - 1)];
	for (row = TRANS_RECT_HEIGHT >> 1; row != 0; row--) {
		for (col = TRANS_RECT_WIDTH >> 1; col != 0; col--) {
			*pix++ = 0;
			pix++;
		}
#if (TRANS_RECT_WIDTH & 1)
		*pix++ = 0;
#endif
		pix -= ROW_PITCH + TRANS_RECT_WIDTH;
		for (col = TRANS_RECT_WIDTH >> 1; col != 0; col--) {
			pix++;
			*pix++ = 0;
		}
		pix -= ROW_PITCH + (TRANS_RECT_WIDTH & ~1);
	}
#if (TRANS_RECT_HEIGHT & 1)
	for (col = TRANS_RECT_WIDTH >> 1; col != 0; col--) {
		*pix++ = 0;
		pix++;
	}
#if (TRANS_RECT_WIDTH & 1)
	*pix++ = 0;
#endif
#endif
}
#endif

#undef TRANS_RECT_Y
#undef TRANS_RECT_X
#undef TRANS_RECT_WIDTH
#undef TRANS_RECT_HEIGHT

__declspec(naked) void drawTopArchesUpperScreen(BYTE *pBuff)
{
	__asm {
		push	ebx
		push	edx
		push	edi
		push	esi
		mov		edi, offset SpeedFrameTbl
		mov		gpCelFrame, edi
		mov		edi, ecx
		mov		eax, light_table_index
		test	al, al
		jz		loc_46316A
		cmp		al, lightmax
		jz		loc_4631CA
		mov		eax, level_cel_block
		and		eax, 8000h
		jnz		loc_4630FE
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		ebx, light_table_index
		shl		ebx, 8
		add		ebx, pLightTbl
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
		jz		loc_463255
		cmp		ax, 1
		jz		loc_4632C5
		cmp		ax, 2
		jz		loc_463425
		cmp		ax, 3
		jz		loc_463698
		cmp		ax, 4
		jz		loc_46390B
		jmp		loc_463AB3
	loc_4630FE:
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 4
		add		eax, light_table_index
		shl		eax, 2
		add		esi, eax
		mov		eax, [esi]
		mov		esi, pSpeedCels
		add		esi, eax
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
	loc_463133:
		cmp		ax, 8
		jz		loc_463C5B
		cmp		ax, 9
		jz		loc_463CC2
		cmp		ax, 0Ah
		jz		loc_463DA3
		cmp		ax, 0Bh
		jz		loc_463ED8
		cmp		ax, 0Ch
		jz		loc_464011
		jmp		loc_464112
	loc_46316A:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_46319C
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_46319C:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		add		eax, 8
		jmp		loc_463133
	loc_4631CA:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_4631FC
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_4631FC:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		jz		loc_46420F
		cmp		ax, 1
		jz		loc_464263
		cmp		ax, 2
		jz		loc_464321
		cmp		ax, 3
		jz		loc_46440E
		cmp		ax, 4
		jz		loc_4644FB
		jmp		loc_4645C7
	loc_463255:
		push	ebp
		mov		ebp, 10h
	loc_46325B:
		cmp		edi, gpBufEnd
		jb		loc_4632BF
		mov		ecx, 8
	loc_463268:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463268
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_4632BF
		mov		ecx, 8
	loc_463299:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463299
		sub		edi, 320h
		dec		ebp
		jnz		loc_46325B
	loc_4632BF:
		pop		ebp
		jmp		loc_464688
	loc_4632C5:
		push	ebp
		mov		eax, edi
		and		eax, 1
		mov		WorldBoolFlag, eax
		mov		ebp, 20h
	loc_4632D5:
		mov		edx, 20h
	loc_4632DA:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_4633F8
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_46341F
		mov		ecx, eax
		mov		eax, edi
		and		eax, 1
		cmp		eax, WorldBoolFlag
		jnz		loc_463377
		push	edx
		shr		ecx, 1
		jb		loc_463340
		shr		ecx, 1
		jnb		loc_46331C
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_46331C:
		test	cl, cl
		jz		loc_46333E
	loc_463320:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463320
	loc_46333E:
		jmp		loc_463374
	loc_463340:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_463353
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463353:
		test	cl, cl
		jz		loc_463374
	loc_463357:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463357
	loc_463374:
		pop		edx
		jmp		loc_4633EF
	loc_463377:
		push	edx
		shr		ecx, 1
		jb		loc_4633B0
		shr		ecx, 1
		jnb		loc_46338D
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_46338D:
		test	cl, cl
		jz		loc_4633AE
	loc_463391:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463391
	loc_4633AE:
		jmp		loc_4633EE
	loc_4633B0:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_4633CC
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4633CC:
		test	cl, cl
		jz		loc_4633EE
	loc_4633D0:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4633D0
	loc_4633EE:
		pop		edx
	loc_4633EF:
		test	edx, edx
		jz		loc_463404
		jmp		loc_4632DA
	loc_4633F8:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_4632DA
	loc_463404:
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		sub		edi, 320h
		dec		ebp
		jnz		loc_4632D5
	loc_46341F:
		pop		ebp
		jmp		loc_464688
	loc_463425:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
	loc_463432:
		cmp		edi, gpBufEnd
		jb		loc_463692
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4634D2
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_463499
		shr		ecx, 1
		jnb		loc_463475
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_463475:
		test	cl, cl
		jz		loc_463497
	loc_463479:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463479
	loc_463497:
		jmp		loc_4634CD
	loc_463499:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_4634AC
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_4634AC:
		test	cl, cl
		jz		loc_4634CD
	loc_4634B0:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_4634B0
	loc_4634CD:
		jmp		$+82h
	loc_4634D2:
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_463511
		shr		ecx, 1
		jnb		loc_4634EE
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_4634EE:
		test	cl, cl
		jz		loc_46350F
	loc_4634F2:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_4634F2
	loc_46350F:
		jmp		loc_46354F
	loc_463511:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_46352D
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_46352D:
		test	cl, cl
		jz		loc_46354F
	loc_463531:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463531
	loc_46354F:
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_463432
		mov		ebp, 2
	loc_463563:
		cmp		edi, gpBufEnd
		jb		loc_463692
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_463603
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_4635CA
		shr		ecx, 1
		jnb		loc_4635A6
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4635A6:
		test	cl, cl
		jz		loc_4635C8
	loc_4635AA:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4635AA
	loc_4635C8:
		jmp		loc_4635FE
	loc_4635CA:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_4635DD
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_4635DD:
		test	cl, cl
		jz		loc_4635FE
	loc_4635E1:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_4635E1
	loc_4635FE:
		jmp		$+82h
	loc_463603:
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_463642
		shr		ecx, 1
		jnb		loc_46361F
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_46361F:
		test	cl, cl
		jz		loc_463640
	loc_463623:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463623
	loc_463640:
		jmp		loc_463680
	loc_463642:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_46365E
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_46365E:
		test	cl, cl
		jz		loc_463680
	loc_463662:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463662
	loc_463680:
		sub		edi, 320h
		add		ebp, 2
		cmp		ebp, 20h
		jnz		loc_463563
	loc_463692:
		pop		ebp
		jmp		loc_464688
	loc_463698:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
	loc_4636A5:
		cmp		edi, gpBufEnd
		jb		loc_463905
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_463743
		shr		ecx, 1
		jb		loc_463703
		shr		ecx, 1
		jnb		loc_4636DF
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4636DF:
		test	cl, cl
		jz		loc_463701
	loc_4636E3:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4636E3
	loc_463701:
		jmp		loc_463737
	loc_463703:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_463716
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463716:
		test	cl, cl
		jz		loc_463737
	loc_46371A:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46371A
	loc_463737:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		jmp		$+82h
	loc_463743:
		shr		ecx, 1
		jb		loc_46377B
		shr		ecx, 1
		jnb		loc_463758
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463758:
		test	cl, cl
		jz		loc_463779
	loc_46375C:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46375C
	loc_463779:
		jmp		loc_4637B9
	loc_46377B:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_463797
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_463797:
		test	cl, cl
		jz		loc_4637B9
	loc_46379B:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_46379B
	loc_4637B9:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
	// loc_4637C0:
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_4636A5
		mov		ebp, 2
	loc_4637D6:
		cmp		edi, gpBufEnd
		jb		loc_463905
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_463874
		shr		ecx, 1
		jb		loc_463834
		shr		ecx, 1
		jnb		loc_463810
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_463810:
		test	cl, cl
		jz		loc_463832
	loc_463814:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463814
	loc_463832:
		jmp		loc_463868
	loc_463834:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_463847
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463847:
		test	cl, cl
		jz		loc_463868
	loc_46384B:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46384B
	loc_463868:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		jmp		$+82h
	loc_463874:
		shr		ecx, 1
		jb		loc_4638AC
		shr		ecx, 1
		jnb		loc_463889
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463889:
		test	cl, cl
		jz		loc_4638AA
	loc_46388D:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46388D
	loc_4638AA:
		jmp		loc_4638EA
	loc_4638AC:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_4638C8
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4638C8:
		test	cl, cl
		jz		loc_4638EA
	loc_4638CC:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4638CC
	loc_4638EA:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
	// loc_4638F1:
		sub		edi, 320h
		add		edi, ebp
		add		ebp, 2
		cmp		ebp, 20h
		jnz		loc_4637D6
	loc_463905:
		pop		ebp
		jmp		loc_464688
	loc_46390B:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
	loc_463918:
		cmp		edi, gpBufEnd
		jb		loc_463AAD
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4639B8
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_46397F
		shr		ecx, 1
		jnb		loc_46395B
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_46395B:
		test	cl, cl
		jz		loc_46397D
	loc_46395F:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_46395F
	loc_46397D:
		jmp		loc_4639B3
	loc_46397F:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_463992
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463992:
		test	cl, cl
		jz		loc_4639B3
	loc_463996:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463996
	loc_4639B3:
		jmp		$+82h
	loc_4639B8:
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_4639F7
		shr		ecx, 1
		jnb		loc_4639D4
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_4639D4:
		test	cl, cl
		jz		loc_4639F5
	loc_4639D8:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_4639D8
	loc_4639F5:
		jmp		loc_463A35
	loc_4639F7:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_463A13
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_463A13:
		test	cl, cl
		jz		loc_463A35
	loc_463A17:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463A17
	loc_463A35:
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_463918
		mov		ebp, 8
	loc_463A49:
		cmp		edi, gpBufEnd
		jb		loc_463AAD
		mov		ecx, 8
	loc_463A56:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463A56
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_463AAD
		mov		ecx, 8
	loc_463A87:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463A87
		sub		edi, 320h
		dec		ebp
		jnz		loc_463A49
	loc_463AAD:
		pop		ebp
		jmp		loc_464688
	loc_463AB3:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
	loc_463AC0:
		cmp		edi, gpBufEnd
		jb		loc_463C55
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_463B5E
		shr		ecx, 1
		jb		loc_463B1E
		shr		ecx, 1
		jnb		loc_463AFA
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_463AFA:
		test	cl, cl
		jz		loc_463B1C
	loc_463AFE:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463AFE
	loc_463B1C:
		jmp		loc_463B52
	loc_463B1E:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_463B31
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463B31:
		test	cl, cl
		jz		loc_463B52
	loc_463B35:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463B35
	loc_463B52:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		jmp		$+82h
	loc_463B5E:
		shr		ecx, 1
		jb		loc_463B96
		shr		ecx, 1
		jnb		loc_463B73
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_463B73:
		test	cl, cl
		jz		loc_463B94
	loc_463B77:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463B77
	loc_463B94:
		jmp		loc_463BD4
	loc_463B96:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_463BB2
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_463BB2:
		test	cl, cl
		jz		loc_463BD4
	loc_463BB6:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463BB6
	loc_463BD4:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
	// loc_463BDB:
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_463AC0
		mov		ebp, 8
	loc_463BF1:
		cmp		edi, gpBufEnd
		jb		loc_463C55
		mov		ecx, 8
	loc_463BFE:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_463BFE
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_463C55
		mov		ecx, 8
	loc_463C2F:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_463C2F
		sub		edi, 320h
		dec		ebp
		jnz		loc_463BF1
	loc_463C55:
		pop		ebp
		jmp		loc_464688
	loc_463C5B:
		mov		edx, 10h
	loc_463C60:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_463C71:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_463C71
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_463C9F:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_463C9F
		sub		edi, 320h
		dec		edx
		jnz		loc_463C60
		jmp		loc_464688
	loc_463CC2:
		push	ebp
		mov		eax, edi
		and		eax, 1
		mov		WorldBoolFlag, eax
		mov		ebp, 20h
	loc_463CD2:
		mov		edx, 20h
	loc_463CD7:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_463D76
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_463D9D
		mov		ecx, eax
		mov		eax, edi
		and		eax, 1
		cmp		eax, WorldBoolFlag
		jnz		loc_463D36
		shr		ecx, 1
		jnb		loc_463D0D
		inc		esi
		inc		edi
		test	ecx, ecx
		jz		loc_463D6D
		jmp		loc_463D46
	loc_463D0D:
		shr		ecx, 1
		jnb		loc_463D1D
		inc		esi
		inc		edi
		mov		al, [esi]
		inc		esi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_463D6D
	loc_463D1D:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_463D1D
		jmp		loc_463D6D
	loc_463D36:
		shr		ecx, 1
		jnb		loc_463D46
		mov		al, [esi]
		inc		esi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_463D6D
		jmp		loc_463D0D
	loc_463D46:
		shr		ecx, 1
		jnb		loc_463D58
		mov		al, [esi]
		add		esi, 2
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_463D6D
	loc_463D58:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_463D58
	loc_463D6D:
		test	edx, edx
		jz		loc_463D82
		jmp		loc_463CD7
	loc_463D76:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_463CD7
	loc_463D82:
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		sub		edi, 320h
		dec		ebp
		jnz		loc_463CD2
	loc_463D9D:
		pop		ebp
		jmp		loc_464688
	loc_463DA3:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_463DAF:
		cmp		edi, gpBufEnd
		jb		loc_464688
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_463E04
		shr		ecx, 2
		jnb		loc_463DEB
		mov		ax, [esi+2]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_463E2E
	loc_463DEB:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_463DEB
		jmp		loc_463E2E
	loc_463E04:
		shr		ecx, 2
		jnb		loc_463E19
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_463E2E
	loc_463E19:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_463E19
	loc_463E2E:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_463DAF
		mov		edx, 2
	loc_463E42:
		cmp		edi, gpBufEnd
		jb		loc_464688
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_463E97
		shr		ecx, 2
		jnb		loc_463E7E
		mov		ax, [esi+2]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_463EC1
	loc_463E7E:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_463E7E
		jmp		loc_463EC1
	loc_463E97:
		shr		ecx, 2
		jnb		loc_463EAC
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_463EC1
	loc_463EAC:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_463EAC
	loc_463EC1:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_463E42
		jmp		loc_464688
	loc_463ED8:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_463EE5:
		cmp		edi, gpBufEnd
		jb		loc_46400B
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		ebp, ecx
		mov		WorldBoolFlag, eax
		jz		loc_463F3A
		shr		ecx, 2
		jz		loc_463F26
	loc_463F0F:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_463F0F
	loc_463F26:
		and		ebp, 2
		jz		loc_463F64
		mov		ax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		jmp		loc_463F64
	loc_463F3A:
		shr		ecx, 2
		jz		loc_463F54
	loc_463F3F:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_463F3F
	loc_463F54:
		and		ebp, 2
		jz		loc_463F64
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
	loc_463F64:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_463EE5
		mov		edx, 2
	loc_463F7A:
		cmp		edi, gpBufEnd
		jb		loc_46400B
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_463FCD
		shr		ecx, 2
		jz		loc_463FB9
	loc_463FA2:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_463FA2
	loc_463FB9:
		and		ebp, 2
		jz		loc_463FF7
		mov		ax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		jmp		loc_463FF7
	loc_463FCD:
		shr		ecx, 2
		jz		loc_463FE7
	loc_463FD2:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_463FD2
	loc_463FE7:
		and		ebp, 2
		jz		loc_463FF7
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
	loc_463FF7:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_463F7A
	loc_46400B:
		pop		ebp
		jmp		loc_464688
	loc_464011:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_46401D:
		cmp		edi, gpBufEnd
		jb		loc_464688
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_464072
		shr		ecx, 2
		jnb		loc_464059
		mov		ax, [esi+2]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_46409C
	loc_464059:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_464059
		jmp		loc_46409C
	loc_464072:
		shr		ecx, 2
		jnb		loc_464087
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_46409C
	loc_464087:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_464087
	loc_46409C:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_46401D
		mov		edx, 8
	loc_4640B0:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_4640C1:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_4640C1
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_4640EF:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_4640EF
		sub		edi, 320h
		dec		edx
		jnz		loc_4640B0
		jmp		loc_464688
	loc_464112:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_46411F:
		cmp		edi, gpBufEnd
		jb		loc_464209
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		ebp, ecx
		mov		WorldBoolFlag, eax
		jz		loc_464174
		shr		ecx, 2
		jz		loc_464160
	loc_464149:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_464149
	loc_464160:
		and		ebp, 2
		jz		loc_46419E
		mov		ax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		jmp		loc_46419E
	loc_464174:
		shr		ecx, 2
		jz		loc_46418E
	loc_464179:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_464179
	loc_46418E:
		and		ebp, 2
		jz		loc_46419E
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
	loc_46419E:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_46411F
		mov		edx, 8
	loc_4641B4:
		cmp		edi, gpBufEnd
		jb		loc_464209
		mov		ecx, 8
	loc_4641C1:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_4641C1
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_464209
		mov		ecx, 8
	loc_4641EB:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_4641EB
		sub		edi, 320h
		dec		edx
		jnz		loc_4641B4
	loc_464209:
		pop		ebp
		jmp		loc_464688
	loc_46420F:
		mov		edx, 10h
		xor		eax, eax
	loc_464216:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_464227:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_464227
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_46424A:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_46424A
		sub		edi, 320h
		dec		edx
		jnz		loc_464216
		jmp		loc_464688
	loc_464263:
		push	ebp
		mov		eax, edi
		and		eax, 1
		mov		WorldBoolFlag, eax
		mov		ebp, 20h
	loc_464273:
		mov		edx, 20h
	loc_464278:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_4642F4
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_46431B
		mov		ecx, eax
		add		esi, ecx
		mov		eax, edi
		and		eax, 1
		cmp		eax, WorldBoolFlag
		jnz		loc_4642C7
		xor		eax, eax
		shr		ecx, 1
		jnb		loc_4642AD
		inc		edi
		test	ecx, ecx
		jz		loc_4642EE
		jmp		loc_4642D6
	loc_4642AD:
		shr		ecx, 1
		jnb		loc_4642B9
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_4642EE
	loc_4642B9:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_4642B9
		jmp		loc_4642EE
	loc_4642C7:
		xor		eax, eax
		shr		ecx, 1
		jnb		loc_4642D6
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_4642EE
		jmp		loc_4642AD
	loc_4642D6:
		shr		ecx, 1
		jnb		loc_4642E3
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_4642EE
	loc_4642E3:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4642E3
	loc_4642EE:
		test	edx, edx
		jz		loc_464300
		jmp		loc_464278
	loc_4642F4:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_464278
	loc_464300:
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		sub		edi, 320h
		dec		ebp
		jnz		loc_464273
	loc_46431B:
		pop		ebp
		jmp		loc_464688
	loc_464321:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_46432D:
		cmp		edi, gpBufEnd
		jb		loc_464688
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_46436F
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_464361
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_46438A
	loc_464361:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_464361
		jmp		loc_46438A
	loc_46436F:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_46437F
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_46438A
	loc_46437F:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_46437F
	loc_46438A:
		sub		edi, 320h
		test	edx, edx
		jz		loc_464399
		sub		edx, 2
		jmp		loc_46432D
	loc_464399:
		mov		edx, 2
	loc_46439E:
		cmp		edi, gpBufEnd
		jb		loc_464688
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4643E0
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4643D2
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_4643FB
	loc_4643D2:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_4643D2
		jmp		loc_4643FB
	loc_4643E0:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4643F0
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_4643FB
	loc_4643F0:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4643F0
	loc_4643FB:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_46439E
		jmp		loc_464688
	loc_46440E:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_46441A:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_46445A
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_46444C
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_464475
	loc_46444C:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_46444C
		jmp		loc_464475
	loc_46445A:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_46446A
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_464475
	loc_46446A:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_46446A
	loc_464475:
		sub		edi, 320h
		test	edx, edx
		jz		loc_464486
		add		edi, edx
		sub		edx, 2
		jmp		loc_46441A
	loc_464486:
		mov		edx, 2
	loc_46448B:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4644CB
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4644BD
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_4644E6
	loc_4644BD:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_4644BD
		jmp		loc_4644E6
	loc_4644CB:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4644DB
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_4644E6
	loc_4644DB:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4644DB
	loc_4644E6:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_46448B
		jmp		loc_464688
	loc_4644FB:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_464507:
		cmp		edi, gpBufEnd
		jb		loc_464688
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_464549
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_46453B
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_464564
	loc_46453B:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_46453B
		jmp		loc_464564
	loc_464549:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_464559
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_464564
	loc_464559:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_464559
	loc_464564:
		sub		edi, 320h
		test	edx, edx
		jz		loc_464573
		sub		edx, 2
		jmp		loc_464507
	loc_464573:
		mov		edx, 8
		xor		eax, eax
	loc_46457A:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_46458B:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_46458B
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_4645AE:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4645AE
		sub		edi, 320h
		dec		edx
		jnz		loc_46457A
		jmp		loc_464688
	loc_4645C7:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_4645D3:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_464613
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_464605
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_46462E
	loc_464605:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_464605
		jmp		loc_46462E
	loc_464613:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_464623
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_46462E
	loc_464623:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_464623
	loc_46462E:
		sub		edi, 320h
		test	edx, edx
		jz		loc_46463F
		add		edi, edx
		sub		edx, 2
		jmp		loc_4645D3
	loc_46463F:
		mov		edx, 8
		xor		eax, eax
	loc_464646:
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_464653:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_464653
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_464688
		mov		ecx, 8
	loc_464672:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_464672
		sub		edi, 320h
		dec		edx
		jnz		loc_464646
		jmp		loc_464688
	loc_464688:
		pop		esi
		pop		edi
		pop		edx
		pop		ebx
		retn
	}
}

__declspec(naked) void drawBottomArchesUpperScreen(BYTE *pBuff, DWORD *pMask)
{
	__asm {
		push	ebx
		push	edi
		push	esi
		mov		edi, offset SpeedFrameTbl
		mov		gpCelFrame, edi
		mov		edi, ecx
		mov		gpDrawMask, edx
		mov		eax, light_table_index
		test	al, al
		jz		loc_4647A2
		cmp		al, lightmax
		jz		loc_464802
		mov		eax, level_cel_block
		and		eax, 8000h
		jnz		loc_464736
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		ebx, light_table_index
		shl		ebx, 8
		add		ebx, pLightTbl
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
		jz		loc_46488D
		jz		loc_464FBC
		cmp		ax, 1
		jz		loc_4648D5
		cmp		ax, 2
		jz		loc_464964
		cmp		ax, 3
		jz		loc_464A30
		cmp		ax, 4
		jz		loc_464AFE
		jmp		loc_464BBC
	loc_464736:
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 4
		add		eax, light_table_index
		shl		eax, 2
		add		esi, eax
		mov		eax, [esi]
		mov		esi, pSpeedCels
		add		esi, eax
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
	loc_46476B:
		cmp		ax, 8
		jz		loc_464C7A
		cmp		ax, 9
		jz		loc_464CC1
		cmp		ax, 0Ah
		jz		loc_464D4B
		cmp		ax, 0Bh
		jz		loc_464DE3
		cmp		ax, 0Ch
		jz		loc_464E7D
		jmp		loc_464F19
	loc_4647A2:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_4647D4
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_4647D4:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		add		eax, 8
		jmp		loc_46476B
	loc_464802:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_464834
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_464834:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		jz		loc_464FBC
		cmp		ax, 1
		jz		loc_465002
		cmp		ax, 2
		jz		loc_465091
		cmp		ax, 3
		jz		loc_465117
		cmp		ax, 4
		jz		loc_46519D
		jmp		loc_465232
	loc_46488D:
		mov		edx, 20h
	loc_464892:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_4648AB:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_4648B5
		xlat
		mov		[edi], al
	loc_4648B5:
		inc		edi
		dec		ecx
		jnz		loc_4648AB
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_464892
		jmp		loc_4652C1
	loc_4648D5:
		mov		ecx, 20h
	loc_4648DA:
		push	ecx
		mov		eax, gpDrawMask
		mov		eax, [eax]
		mov		gdwCurrentMask, eax
		mov		edx, 20h
	loc_4648EC:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_464927
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_4652C0
		mov		ecx, eax
		push	edx
		mov		edx, gdwCurrentMask
	loc_46490C:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_464916
		xlat
		mov		[edi], al
	loc_464916:
		inc		edi
		dec		ecx
		jnz		loc_46490C
		mov		gdwCurrentMask, edx
		pop		edx
		test	edx, edx
		jz		loc_464944
		jmp		loc_4648EC
	loc_464927:
		neg		al
		add		edi, eax
		mov		ecx, eax
		and		ecx, 1Fh
		jz		loc_464940
		push	eax
		mov		eax, gdwCurrentMask
		shl		eax, cl
		mov		gdwCurrentMask, eax
		pop		eax
	loc_464940:
		sub		edx, eax
		jnz		loc_4648EC
	loc_464944:
		pop		ecx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		ecx
		jnz		loc_4648DA
		jmp		loc_4652C1
	loc_464964:
		mov		edx, 1Eh
	loc_464969:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46499E
		mov		ax, [esi+2]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4649BB
	loc_46499E:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46499E
	loc_4649BB:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_464969
		mov		edx, 2
	loc_4649CB:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_464A00
		mov		ax, [esi+2]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_464A1D
	loc_464A00:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464A00
	loc_464A1D:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_4649CB
		jmp		loc_4652C1
	loc_464A30:
		push	ebp
		mov		edx, 1Eh
	loc_464A36:
		cmp		edi, gpBufEnd
		jb		loc_464AF8
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_464A6D
	loc_464A50:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464A50
	loc_464A6D:
		and		ebp, 2
		jz		loc_464A88
		mov		ax, [esi]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
	loc_464A88:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_464A36
		mov		edx, 2
	loc_464A9A:
		cmp		edi, gpBufEnd
		jb		loc_464AF8
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_464ACD
	loc_464AB0:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464AB0
	loc_464ACD:
		and		ebp, 2
		jz		loc_464AE8
		mov		ax, [esi]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
	loc_464AE8:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_464A9A
	loc_464AF8:
		pop		ebp
		jmp		loc_4652C1
	loc_464AFE:
		mov		edx, 1Eh
	loc_464B03:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_464B38
		mov		ax, [esi+2]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_464B55
	loc_464B38:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464B38
	loc_464B55:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_464B03
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_464B72:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
		mov		eax, esi
		and		eax, 2
		add		esi, eax
	loc_464B92:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_464B9C
		xlat
		mov		[edi], al
	loc_464B9C:
		inc		edi
		dec		ecx
		jnz		loc_464B92
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_464B72
		jmp		loc_4652C1
	loc_464BBC:
		push	ebp
		mov		edx, 1Eh
	loc_464BC2:
		cmp		edi, gpBufEnd
		jb		loc_464C74
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_464BF9
	loc_464BDC:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464BDC
	loc_464BF9:
		and		ebp, 2
		jz		loc_464C14
		mov		ax, [esi]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
	loc_464C14:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_464BC2
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_464C33:
		cmp		edi, gpBufEnd
		jb		loc_464C74
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_464C48:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_464C52
		xlat
		mov		[edi], al
	loc_464C52:
		inc		edi
		dec		ecx
		jnz		loc_464C48
		mov		ebp, esi
		and		ebp, 2
		add		esi, ebp
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_464C33
	loc_464C74:
		pop		ebp
		jmp		loc_4652C1
	loc_464C7A:
		mov		edx, 20h
	loc_464C7F:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_464C98:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_464CA1
		mov		[edi], al
	loc_464CA1:
		inc		edi
		dec		ecx
		jnz		loc_464C98
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_464C7F
		jmp		loc_4652C1
	loc_464CC1:
		mov		ecx, 20h
	loc_464CC6:
		push	ecx
		mov		eax, gpDrawMask
		mov		eax, [eax]
		mov		gdwCurrentMask, eax
		mov		edx, 20h
	loc_464CD8:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_464D12
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_4652C0
		push	edx
		mov		edx, gdwCurrentMask
		mov		ecx, eax
	loc_464CF8:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_464D01
		mov		[edi], al
	loc_464D01:
		inc		edi
		dec		ecx
		jnz		loc_464CF8
		mov		gdwCurrentMask, edx
		pop		edx
		test	edx, edx
		jz		loc_464D2F
		jmp		loc_464CD8
	loc_464D12:
		neg		al
		add		edi, eax
		mov		ecx, eax
		and		ecx, 1Fh
		jz		loc_464D2B
		mov		ebx, gdwCurrentMask
		shl		ebx, cl
		mov		gdwCurrentMask, ebx
	loc_464D2B:
		sub		edx, eax
		jnz		loc_464CD8
	loc_464D2F:
		pop		ecx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		ecx
		jnz		loc_464CC6
		jmp		loc_4652C1
	loc_464D4B:
		mov		edx, 1Eh
	loc_464D50:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_464D7B
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_464D88
	loc_464D7B:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		dec		ecx
		lea		edi, [edi+4]
		jnz		loc_464D7B
	loc_464D88:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_464D50
		mov		edx, 2
	loc_464D98:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_464DC3
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		test	ecx, ecx
		lea		edi, [edi+2]
		jz		loc_464DD0
	loc_464DC3:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464DC3
	loc_464DD0:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_464D98
		jmp		loc_4652C1
	loc_464DE3:
		push	ebp
		mov		edx, 1Eh
	loc_464DE9:
		cmp		edi, gpBufEnd
		jb		loc_464E77
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_464E10
	loc_464E03:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464E03
	loc_464E10:
		and		ebp, 2
		jz		loc_464E21
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_464E21:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_464DE9
		mov		edx, 2
	loc_464E33:
		cmp		edi, gpBufEnd
		jb		loc_464E77
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_464E56
	loc_464E49:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464E49
	loc_464E56:
		and		ebp, 2
		jz		loc_464E67
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_464E67:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_464E33
	loc_464E77:
		pop		ebp
		jmp		loc_4652C1
	loc_464E7D:
		mov		edx, 1Eh
	loc_464E82:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_464EAD
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_464EBA
	loc_464EAD:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464EAD
	loc_464EBA:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_464E82
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_464ED7:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_464EF0:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_464EF9
		mov		[edi], al
	loc_464EF9:
		inc		edi
		dec		ecx
		jnz		loc_464EF0
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_464ED7
		jmp		loc_4652C1
	loc_464F19:
		push	ebp
		mov		edx, 1Eh
	loc_464F1F:
		cmp		edi, gpBufEnd
		jb		loc_464FB6
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_464F46
	loc_464F39:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_464F39
	loc_464F46:
		and		ebp, 2
		jz		loc_464F57
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_464F57:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_464F1F
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_464F76:
		cmp		edi, gpBufEnd
		jb		loc_464FB6
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_464F8B:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_464F94
		mov		[edi], al
	loc_464F94:
		inc		edi
		dec		ecx
		jnz		loc_464F8B
		mov		ebp, esi
		and		ebp, 2
		add		esi, ebp
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_464F76
	loc_464FB6:
		pop		ebp
		jmp		loc_4652C1
	loc_464FBC:
		mov		edx, 20h
	loc_464FC1:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		xor		eax, eax
		mov		ecx, 20h
	loc_464FDC:
		shl		edx, 1
		jnb		loc_464FE2
		mov		[edi], al
	loc_464FE2:
		inc		edi
		dec		ecx
		jnz		loc_464FDC
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_464FC1
		jmp		loc_4652C1
	loc_465002:
		mov		ecx, 20h
	loc_465007:
		push	ecx
		mov		eax, gpDrawMask
		mov		eax, [eax]
		mov		gdwCurrentMask, eax
		mov		edx, 20h
	loc_465019:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_465054
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_4652C0
		push	edx
		mov		edx, gdwCurrentMask
		mov		ecx, eax
		add		esi, ecx
		xor		eax, eax
	loc_46503D:
		shl		edx, 1
		jnb		loc_465043
		mov		[edi], al
	loc_465043:
		inc		edi
		dec		ecx
		jnz		loc_46503D
		mov		gdwCurrentMask, edx
		pop		edx
		test	edx, edx
		jz		loc_465071
		jmp		loc_465019
	loc_465054:
		neg		al
		add		edi, eax
		mov		ecx, eax
		and		ecx, 1Fh
		jz		loc_46506D
		mov		ebx, gdwCurrentMask
		shl		ebx, cl
		mov		gdwCurrentMask, ebx
	loc_46506D:
		sub		edx, eax
		jnz		loc_465019
	loc_465071:
		pop		ecx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		ecx
		jnz		loc_465007
		jmp		loc_4652C1
	loc_465091:
		mov		edx, 1Eh
		xor		eax, eax
	loc_465098:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4650BC
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4650C4
	loc_4650BC:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4650BC
	loc_4650C4:
		sub		edi, 320h
		test	edx, edx
		jz		loc_4650D3
		sub		edx, 2
		jmp		loc_465098
	loc_4650D3:
		mov		edx, 2
	loc_4650D8:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4650FC
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465104
	loc_4650FC:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4650FC
	loc_465104:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_4650D8
		jmp		loc_4652C1
	loc_465117:
		mov		edx, 1Eh
		xor		eax, eax
	loc_46511E:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465140
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465148
	loc_465140:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465140
	loc_465148:
		sub		edi, 320h
		test	edx, edx
		jz		loc_465159
		add		edi, edx
		sub		edx, 2
		jmp		loc_46511E
	loc_465159:
		mov		edx, 2
	loc_46515E:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465180
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465188
	loc_465180:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465180
	loc_465188:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_46515E
		jmp		loc_4652C1
	loc_46519D:
		mov		edx, 1Eh
		xor		eax, eax
	loc_4651A4:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4651C8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4651D0
	loc_4651C8:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4651C8
	loc_4651D0:
		sub		edi, 320h
		test	edx, edx
		jz		loc_4651DF
		sub		edx, 2
		jmp		loc_4651A4
	loc_4651DF:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_4651F1:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		xor		eax, eax
		mov		ecx, 20h
	loc_46520C:
		shl		edx, 1
		jnb		loc_465212
		mov		[edi], al
	loc_465212:
		inc		edi
		dec		ecx
		jnz		loc_46520C
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_4651F1
		jmp		loc_4652C1
	loc_465232:
		mov		edx, 1Eh
		xor		eax, eax
	loc_465239:
		cmp		edi, gpBufEnd
		jb		$+82h
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46525B
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465263
	loc_46525B:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46525B
	loc_465263:
		sub		edi, 320h
		test	edx, edx
		jz		loc_465274
		add		edi, edx
		sub		edx, 2
		jmp		loc_465239
	loc_465274:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_465286:
		cmp		edi, gpBufEnd
		jb		loc_4652C1
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		xor		eax, eax
		mov		ecx, 20h
	loc_46529D:
		shl		edx, 1
		jnb		loc_4652A3
		mov		[edi], al
	loc_4652A3:
		inc		edi
		dec		ecx
		jnz		loc_46529D
		pop		edx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_465286
		jmp		loc_4652C1
	loc_4652C0:
		pop		eax
	loc_4652C1:
		pop		esi
		pop		edi
		pop		ebx
		retn
	}
}

__declspec(naked) void drawUpperScreen(BYTE *pBuff)
{
	__asm {
		push	ebx
		push	edx
		push	edi
		push	esi
		mov		edx, cel_transparency_active
		test	edx, edx
		jz		loc_465372
		mov		dl, arch_draw_type
		cmp		dl, 0
		jnz		loc_4652EC
		call	drawTopArchesUpperScreen
		jmp		loc_465F33
	loc_4652EC:
		cmp		dl, 1
		jnz		loc_46532F
		mov		ebx, level_piece_id
		mov		al, block_lvid[ebx]
		cmp		al, 1
		jz		loc_465307
		cmp		al, 3
		jz		loc_465307
		jmp		loc_46532F
	loc_465307:
		mov		edx, offset LeftMask
		add		edx, 7Ch
		call	drawBottomArchesUpperScreen
		jmp		loc_465F33
		cmp		al, 4
		jnz		loc_46532F
		mov		edx, offset RightMask
		add		edx, 7Ch
		call	drawBottomArchesUpperScreen
		jmp		loc_465F33
	loc_46532F:
		cmp		dl, 2
		jnz		loc_465372
		mov		ebx, level_piece_id
		mov		al, block_lvid[ebx]
		cmp		al, 2
		jz		loc_46534A
		cmp		al, 3
		jz		loc_46534A
		jmp		loc_465372
	loc_46534A:
		mov		edx, offset RightMask
		add		edx, 7Ch
		call	drawBottomArchesUpperScreen
		jmp		loc_465F33
		cmp		al, 4
		jnz		loc_465372
		mov		edx, offset LeftMask
		add		edx, 7Ch
		call	drawBottomArchesUpperScreen
		jmp		loc_465F33
	loc_465372:
		mov		edi, offset SpeedFrameTbl
		mov		gpCelFrame, edi
		mov		edi, ecx
		mov		eax, light_table_index
		test	al, al
		jz		loc_465474
		cmp		al, lightmax
		jz		loc_4654D4
		mov		eax, level_cel_block
		and		eax, 8000h
		jnz		loc_46540A
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		ebx, light_table_index
		shl		ebx, 8
		add		ebx, pLightTbl
		mov		eax, level_cel_block
		shr		eax, 0Ch
		and		eax, 0Fh
		jz		loc_46555D
		cmp		ax, 1
		jz		loc_4655B5
		cmp		ax, 2
		jz		loc_465663
		cmp		ax, 3
		jz		loc_465765
		cmp		ax, 4
		jz		loc_465867
		jmp		loc_465939
	loc_46540A:
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 4
		add		eax, light_table_index
		shl		eax, 2
		add		esi, eax
		mov		eax, [esi]
		mov		esi, pSpeedCels
		add		esi, eax
		mov		eax, level_cel_block
		shr		eax, 0Ch
		and		eax, 0Fh
	loc_46543D:
		cmp		ax, 8
		jz		loc_465A0B
		cmp		ax, 9
		jz		loc_465A3C
		cmp		ax, 0Ah
		jz		loc_465AA8
		cmp		ax, 0Bh
		jz		loc_465B40
		cmp		ax, 0Ch
		jz		loc_465BDA
		jmp		loc_465C53
	loc_465474:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_4654A6
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_4654A6:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		add		eax, 8
		jmp		loc_46543D
	loc_4654D4:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_465506
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_465506:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		shr		eax, 0Ch
		and		eax, 7
		jz		loc_465CC8
		cmp		ax, 1
		jz		loc_465CF6
		cmp		ax, 2
		jz		loc_465D58
		cmp		ax, 3
		jz		loc_465DDE
		cmp		ax, 4
		jz		loc_465E64
		jmp		loc_465ECF
	loc_46555D:
		mov		edx, 20h
	loc_465562:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 20h
		push	edx
	loc_465574:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_465574
		pop		edx
		sub		edi, 320h
		dec		edx
		jnz		loc_465562
		jmp		loc_465F33
	loc_4655B5:
		push	ebp
		mov		ebp, 20h
	loc_4655BB:
		mov		edx, 20h
	loc_4655C0:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_465644
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_46565D
		mov		ecx, eax
		push	edx
		cmp		cl, 4
		jl		loc_465611
	loc_4655DF:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_4655DF
	loc_465611:
		cmp		cl, 2
		jl		loc_46562C
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_46562C:
		and		cl, 1
		jz		loc_46563A
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebx+edx]
		mov		[edi], dl
		inc		edi
	loc_46563A:
		pop		edx
		test	edx, edx
		jz		loc_465650
		jmp		loc_4655C0
	loc_465644:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_4655C0
	loc_465650:
		sub		edi, 320h
		dec		ebp
		jnz		loc_4655BB
	loc_46565D:
		pop		ebp
		jmp		loc_465F33
	loc_465663:
		push	ebp
		mov		ebp, 1Eh
	loc_465669:
		cmp		edi, gpBufEnd
		jb		loc_46575F
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		cmp		cl, 4
		jl		loc_4656BC
	loc_46568A:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_46568A
	loc_4656BC:
		cmp		cl, 2
		jl		loc_4656D7
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_4656D7:
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_465669
		mov		ebp, 2
	loc_4656E7:
		cmp		edi, gpBufEnd
		jb		loc_46575F
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		cmp		cl, 4
		jl		loc_465736
	loc_465704:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_465704
	loc_465736:
		cmp		cl, 2
		jl		loc_465751
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_465751:
		sub		edi, 320h
		add		ebp, 2
		cmp		ebp, 20h
		jnz		loc_4656E7
	loc_46575F:
		pop		ebp
		jmp		loc_465F33
	loc_465765:
		push	ebp
		mov		ebp, 1Eh
	loc_46576B:
		cmp		edi, gpBufEnd
		jb		loc_465861
		mov		ecx, 20h
		sub		ecx, ebp
		cmp		cl, 4
		jl		loc_4657B5
	loc_465783:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_465783
	loc_4657B5:
		cmp		cl, 2
		jl		loc_4657D0
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_4657D0:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_46576B
		mov		ebp, 2
	loc_4657E9:
		cmp		edi, gpBufEnd
		jb		loc_465861
		mov		ecx, 20h
		sub		ecx, ebp
		cmp		cl, 4
		jl		loc_46582F
	loc_4657FD:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_4657FD
	loc_46582F:
		cmp		cl, 2
		jl		loc_46584A
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_46584A:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		sub		edi, 320h
		add		edi, ebp
		add		ebp, 2
		cmp		ebp, 20h
		jnz		loc_4657E9
	loc_465861:
		pop		ebp
		jmp		loc_465F33
	loc_465867:
		push	ebp
		mov		ebp, 1Eh
	loc_46586D:
		cmp		edi, gpBufEnd
		jb		loc_465933
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		cmp		cl, 4
		jl		loc_4658C0
	loc_46588E:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_46588E
	loc_4658C0:
		cmp		cl, 2
		jl		loc_4658DB
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_4658DB:
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_46586D
		mov		ebp, 10h
	loc_4658EB:
		cmp		edi, gpBufEnd
		jb		loc_465933
		mov		ecx, 20h
	loc_4658F8:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_4658F8
		sub		edi, 320h
		dec		ebp
		jnz		loc_4658EB
	loc_465933:
		pop		ebp
		jmp		loc_465F33
	loc_465939:
		push	ebp
		mov		ebp, 1Eh
	loc_46593F:
		cmp		edi, gpBufEnd
		jb		loc_465A05
		mov		ecx, 20h
		sub		ecx, ebp
		cmp		cl, 4
		jl		loc_465989
	loc_465957:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_465957
	loc_465989:
		cmp		cl, 2
		jl		loc_4659A4
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_4659A4:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_46593F
		mov		ebp, 10h
	loc_4659BD:
		cmp		edi, gpBufEnd
		jb		loc_465A05
		mov		ecx, 20h
	loc_4659CA:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_4659CA
		sub		edi, 320h
		dec		ebp
		jnz		loc_4659BD
	loc_465A05:
		pop		ebp
		jmp		loc_465F33
	loc_465A0B:
		mov		edx, 20h
	loc_465A10:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 8
	loc_465A21:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465A21
		sub		edi, 320h
		dec		edx
		jnz		loc_465A10
		jmp		loc_465F33
	loc_465A3C:
		push	ebp
		mov		ebp, 20h
	loc_465A42:
		mov		edx, 20h
	loc_465A47:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_465A91
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_465AA2
		mov		ecx, eax
		shr		ecx, 1
		jnb		loc_465A6A
		mov		al, [esi]
		inc		esi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_465A8B
	loc_465A6A:
		shr		ecx, 1
		jnb		loc_465A7E
		mov		ax, [esi]
		add		esi, 2
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465A8B
	loc_465A7E:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465A7E
	loc_465A8B:
		test	edx, edx
		jz		loc_465A99
		jmp		loc_465A47
	loc_465A91:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_465A47
	loc_465A99:
		sub		edi, 320h
		dec		ebp
		jnz		loc_465A42
	loc_465AA2:
		pop		ebp
		jmp		loc_465F33
	loc_465AA8:
		mov		edx, 1Eh
	loc_465AAD:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465AD8
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465AE5
	loc_465AD8:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465AD8
	loc_465AE5:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_465AAD
		mov		edx, 2
	loc_465AF5:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465B20
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465B2D
	loc_465B20:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465B20
	loc_465B2D:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_465AF5
		jmp		loc_465F33
	loc_465B40:
		push	ebp
		mov		edx, 1Eh
	loc_465B46:
		cmp		edi, gpBufEnd
		jb		loc_465BD4
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_465B6D
	loc_465B60:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465B60
	loc_465B6D:
		and		ebp, 2
		jz		loc_465B7E
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_465B7E:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_465B46
		mov		edx, 2
	loc_465B90:
		cmp		edi, gpBufEnd
		jb		loc_465BD4
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_465BB3
	loc_465BA6:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465BA6
	loc_465BB3:
		and		ebp, 2
		jz		loc_465BC4
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_465BC4:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_465B90
	loc_465BD4:
		pop		ebp
		jmp		loc_465F33
	loc_465BDA:
		mov		edx, 1Eh
	loc_465BDF:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465C0A
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465C17
	loc_465C0A:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465C0A
	loc_465C17:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_465BDF
		mov		edx, 10h
	loc_465C27:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 8
	loc_465C38:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465C38
		sub		edi, 320h
		dec		edx
		jnz		loc_465C27
		jmp		loc_465F33
	loc_465C53:
		push	ebp
		mov		edx, 1Eh
	loc_465C59:
		cmp		edi, gpBufEnd
		jb		loc_465CC2
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_465C7C
	loc_465C6F:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465C6F
	loc_465C7C:
		and		ebp, 2
		jz		loc_465C8D
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_465C8D:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_465C59
		mov		edx, 10h
	loc_465C9F:
		cmp		edi, gpBufEnd
		jb		loc_465CC2
		mov		ecx, 8
	loc_465CAC:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465CAC
		sub		edi, 320h
		dec		edx
		jnz		loc_465C9F
	loc_465CC2:
		pop		ebp
		jmp		loc_465F33
	loc_465CC8:
		mov		edx, 20h
		xor		eax, eax
	loc_465CCF:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 8
	loc_465CE0:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465CE0
		sub		edi, 320h
		dec		edx
		jnz		loc_465CCF
		jmp		loc_465F33
	loc_465CF6:
		push	ebp
		mov		ebp, 20h
	loc_465CFC:
		mov		edx, 20h
	loc_465D01:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_465D41
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_465D52
		mov		ecx, eax
		add		esi, ecx
		xor		eax, eax
		shr		ecx, 1
		jnb		loc_465D25
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_465D3B
	loc_465D25:
		shr		ecx, 1
		jnb		loc_465D33
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465D3B
	loc_465D33:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465D33
	loc_465D3B:
		test	edx, edx
		jz		loc_465D49
		jmp		loc_465D01
	loc_465D41:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_465D01
	loc_465D49:
		sub		edi, 320h
		dec		ebp
		jnz		loc_465CFC
	loc_465D52:
		pop		ebp
		jmp		loc_465F33
	loc_465D58:
		mov		edx, 1Eh
		xor		eax, eax
	loc_465D5F:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465D83
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465D8B
	loc_465D83:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465D83
	loc_465D8B:
		sub		edi, 320h
		test	edx, edx
		jz		loc_465D9A
		sub		edx, 2
		jmp		loc_465D5F
	loc_465D9A:
		mov		edx, 2
	loc_465D9F:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465DC3
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465DCB
	loc_465DC3:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465DC3
	loc_465DCB:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_465D9F
		jmp		loc_465F33
	loc_465DDE:
		mov		edx, 1Eh
		xor		eax, eax
	loc_465DE5:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465E07
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465E0F
	loc_465E07:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465E07
	loc_465E0F:
		sub		edi, 320h
		test	edx, edx
		jz		loc_465E20
		add		edi, edx
		sub		edx, 2
		jmp		loc_465DE5
	loc_465E20:
		mov		edx, 2
	loc_465E25:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465E47
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465E4F
	loc_465E47:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465E47
	loc_465E4F:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_465E25
		jmp		loc_465F33
	loc_465E64:
		mov		edx, 1Eh
		xor		eax, eax
	loc_465E6B:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465E8F
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465E97
	loc_465E8F:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465E8F
	loc_465E97:
		sub		edi, 320h
		test	edx, edx
		jz		loc_465EA6
		sub		edx, 2
		jmp		loc_465E6B
	loc_465EA6:
		mov		edx, 10h
	loc_465EAB:
		cmp		edi, gpBufEnd
		jb		$+82h
		mov		ecx, 8
	loc_465EBC:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465EBC
		sub		edi, 320h
		dec		edx
		jnz		loc_465EAB
		jmp		loc_465F33
	loc_465ECF:
		mov		edx, 1Eh
		xor		eax, eax
	loc_465ED6:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_465EF4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_465EFC
	loc_465EF4:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465EF4
	loc_465EFC:
		sub		edi, 320h
		test	edx, edx
		jz		loc_465F0D
		add		edi, edx
		sub		edx, 2
		jmp		loc_465ED6
	loc_465F0D:
		mov		edx, 10h
	loc_465F12:
		cmp		edi, gpBufEnd
		jb		loc_465F33
		mov		ecx, 8
	loc_465F1F:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_465F1F
		sub		edi, 320h
		dec		edx
		jnz		loc_465F12
		jmp		loc_465F33
		pop		eax
	loc_465F33:
		pop		esi
		pop		edi
		pop		edx
		pop		ebx
		retn
	}
}

__declspec(naked) void drawTopArchesLowerScreen(BYTE *pBuff)
{
	__asm {
		push	ebx
		push	edx
		push	edi
		push	esi
		mov		edi, offset SpeedFrameTbl
		mov		gpCelFrame, edi
		mov		edi, ecx
		mov		eax, light_table_index
		test	al, al
		jz		loc_466042
		cmp		al, lightmax
		jz		loc_4660A2
		mov		eax, level_cel_block
		and		eax, 8000h
		jnz		loc_465FD6
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		ebx, light_table_index
		shl		ebx, 8
		add		ebx, pLightTbl
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
		jz		loc_46612D
		cmp		ax, 1
		jz		loc_4661AD
		cmp		ax, 2
		jz		loc_466310
		cmp		ax, 3
		jz		loc_466611
		cmp		ax, 4
		jz		loc_466912
		jmp		loc_466B11
	loc_465FD6:
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 4
		add		eax, light_table_index
		shl		eax, 2
		add		esi, eax
		mov		eax, [esi]
		mov		esi, pSpeedCels
		add		esi, eax
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
	loc_46600B:
		cmp		ax, 8
		jz		loc_466D10
		cmp		ax, 9
		jz		loc_466D7F
		cmp		ax, 0Ah
		jz		loc_466E62
		cmp		ax, 0Bh
		jz		loc_46701C
		cmp		ax, 0Ch
		jz		loc_4671E1
		jmp		loc_46732D
	loc_466042:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_466074
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_466074:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		add		eax, 8
		jmp		loc_46600B
	loc_4660A2:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_4660D4
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_4660D4:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		jz		loc_46747D
		cmp		ax, 1
		jz		loc_4674D9
		cmp		ax, 2
		jz		loc_467599
		cmp		ax, 3
		jz		loc_467692
		cmp		ax, 4
		jz		loc_46778F
		jmp		loc_46786B
	loc_46612D:
		push	ebp
		mov		ebp, 10h
	loc_466133:
		cmp		edi, gpBufEnd
		jb		loc_466143
		add		esi, 20h
		add		edi, 20h
		jmp		loc_466166
	loc_466143:
		mov		ecx, 8
	loc_466148:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466148
	loc_466166:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_46617C
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46619E
	loc_46617C:
		mov		ecx, 8
	loc_466181:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466181
	loc_46619E:
		sub		edi, 320h
		dec		ebp
		jnz		loc_466133
		pop		ebp
		jmp		loc_467944
	loc_4661AD:
		push	ebp
		mov		eax, edi
		and		eax, 1
		mov		WorldBoolFlag, eax
		mov		ecx, 20h
	loc_4661BD:
		push	ecx
		mov		ebp, 20h
	loc_4661C3:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_4662E2
		sub		ebp, eax
		cmp		edi, gpBufEnd
		jb		loc_4661E3
		add		esi, eax
		add		edi, eax
		jmp		loc_4662D9
	loc_4661E3:
		mov		ecx, eax
		mov		eax, edi
		and		eax, 1
		cmp		eax, WorldBoolFlag
		jnz		loc_466263
		shr		ecx, 1
		jb		loc_46622D
		shr		ecx, 1
		jnb		loc_466209
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_466209:
		test	cl, cl
		jz		loc_46622B
	loc_46620D:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_46620D
	loc_46622B:
		jmp		loc_466261
	loc_46622D:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_466240
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466240:
		test	cl, cl
		jz		loc_466261
	loc_466244:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466244
	loc_466261:
		jmp		loc_4662D9
	loc_466263:
		shr		ecx, 1
		jb		loc_46629B
		shr		ecx, 1
		jnb		loc_466278
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466278:
		test	cl, cl
		jz		loc_466299
	loc_46627C:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46627C
	loc_466299:
		jmp		loc_4662D9
	loc_46629B:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_4662B7
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4662B7:
		test	cl, cl
		jz		loc_4662D9
	loc_4662BB:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4662BB
	loc_4662D9:
		test	ebp, ebp
		jz		loc_4662EE
		jmp		loc_4661C3
	loc_4662E2:
		neg		al
		add		edi, eax
		sub		ebp, eax
		jnz		loc_4661C3
	loc_4662EE:
		pop		ecx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		sub		edi, 320h
		dec		ecx
		jnz		loc_4661BD
		pop		ebp
		jmp		loc_467944
	loc_466310:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_466370
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_46635F
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_466370
	loc_46635F:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_466490
	loc_466370:
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_466404
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_4663CB
		shr		ecx, 1
		jnb		loc_4663A7
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4663A7:
		test	cl, cl
		jz		loc_4663C9
	loc_4663AB:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4663AB
	loc_4663C9:
		jmp		loc_4663FF
	loc_4663CB:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_4663DE
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_4663DE:
		test	cl, cl
		jz		loc_4663FF
	loc_4663E2:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_4663E2
	loc_4663FF:
		jmp		$+82h
	loc_466404:
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_466443
		shr		ecx, 1
		jnb		loc_466420
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466420:
		test	cl, cl
		jz		loc_466441
	loc_466424:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466424
	loc_466441:
		jmp		loc_466481
	loc_466443:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_46645F
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_46645F:
		test	cl, cl
		jz		loc_466481
	loc_466463:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466463
	loc_466481:
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_466370
	loc_466490:
		mov		ebp, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4664E8
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_4664D7
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		ebp, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_4664E8
	loc_4664D7:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_46660B
	loc_4664E8:
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_46657C
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_466543
		shr		ecx, 1
		jnb		loc_46651F
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_46651F:
		test	cl, cl
		jz		loc_466541
	loc_466523:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466523
	loc_466541:
		jmp		loc_466577
	loc_466543:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_466556
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466556:
		test	cl, cl
		jz		loc_466577
	loc_46655A:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46655A
	loc_466577:
		jmp		$+82h
	loc_46657C:
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_4665BB
		shr		ecx, 1
		jnb		loc_466598
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466598:
		test	cl, cl
		jz		loc_4665B9
	loc_46659C:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46659C
	loc_4665B9:
		jmp		loc_4665F9
	loc_4665BB:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_4665D7
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4665D7:
		test	cl, cl
		jz		loc_4665F9
	loc_4665DB:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4665DB
	loc_4665F9:
		sub		edi, 320h
		add		ebp, 2
		cmp		ebp, 20h
		jnz		loc_4664E8
	loc_46660B:
		pop		ebp
		jmp		loc_467944
	loc_466611:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_466671
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_466660
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_466671
	loc_466660:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_466791
	loc_466671:
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_466703
		shr		ecx, 1
		jb		loc_4666C3
		shr		ecx, 1
		jnb		loc_46669F
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_46669F:
		test	cl, cl
		jz		loc_4666C1
	loc_4666A3:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4666A3
	loc_4666C1:
		jmp		loc_4666F7
	loc_4666C3:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_4666D6
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_4666D6:
		test	cl, cl
		jz		loc_4666F7
	loc_4666DA:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_4666DA
	loc_4666F7:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		jmp		$+82h
	loc_466703:
		shr		ecx, 1
		jb		loc_46673B
		shr		ecx, 1
		jnb		loc_466718
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466718:
		test	cl, cl
		jz		loc_466739
	loc_46671C:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_46671C
	loc_466739:
		jmp		loc_466779
	loc_46673B:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_466757
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_466757:
		test	cl, cl
		jz		loc_466779
	loc_46675B:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_46675B
	loc_466779:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
	// loc_466780:
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_466671
	loc_466791:
		mov		ebp, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4667E9
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_4667D8
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		ebp, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_4667E9
	loc_4667D8:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_46690C
	loc_4667E9:
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_46687B
		shr		ecx, 1
		jb		loc_46683B
		shr		ecx, 1
		jnb		loc_466817
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_466817:
		test	cl, cl
		jz		loc_466839
	loc_46681B:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_46681B
	loc_466839:
		jmp		loc_46686F
	loc_46683B:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_46684E
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_46684E:
		test	cl, cl
		jz		loc_46686F
	loc_466852:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466852
	loc_46686F:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		jmp		$+82h
	loc_46687B:
		shr		ecx, 1
		jb		loc_4668B3
		shr		ecx, 1
		jnb		loc_466890
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466890:
		test	cl, cl
		jz		loc_4668B1
	loc_466894:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466894
	loc_4668B1:
		jmp		loc_4668F1
	loc_4668B3:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_4668CF
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4668CF:
		test	cl, cl
		jz		loc_4668F1
	loc_4668D3:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4668D3
	loc_4668F1:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
	// loc_4668F8:
		sub		edi, 320h
		add		edi, ebp
		add		ebp, 2
		cmp		ebp, 20h
		jnz		loc_4667E9
	loc_46690C:
		pop		ebp
		jmp		loc_467944
	loc_466912:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_466972
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_466961
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_466972
	loc_466961:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_466A92
	loc_466972:
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_466A06
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_4669CD
		shr		ecx, 1
		jnb		loc_4669A9
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_4669A9:
		test	cl, cl
		jz		loc_4669CB
	loc_4669AD:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_4669AD
	loc_4669CB:
		jmp		loc_466A01
	loc_4669CD:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_4669E0
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_4669E0:
		test	cl, cl
		jz		loc_466A01
	loc_4669E4:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_4669E4
	loc_466A01:
		jmp		$+82h
	loc_466A06:
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		shr		ecx, 1
		jb		loc_466A45
		shr		ecx, 1
		jnb		loc_466A22
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466A22:
		test	cl, cl
		jz		loc_466A43
	loc_466A26:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466A26
	loc_466A43:
		jmp		loc_466A83
	loc_466A45:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_466A61
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_466A61:
		test	cl, cl
		jz		loc_466A83
	loc_466A65:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466A65
	loc_466A83:
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_466972
	loc_466A92:
		mov		ebp, 8
	loc_466A97:
		cmp		edi, gpBufEnd
		jb		loc_466AA7
		add		esi, 20h
		add		edi, 20h
		jmp		loc_466ACA
	loc_466AA7:
		mov		ecx, 8
	loc_466AAC:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466AAC
	loc_466ACA:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_466AE0
		add		esi, 20h
		add		edi, 20h
		jmp		loc_466B02
	loc_466AE0:
		mov		ecx, 8
	loc_466AE5:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466AE5
	loc_466B02:
		sub		edi, 320h
		dec		ebp
		jnz		loc_466A97
		pop		ebp
		jmp		loc_467944
	loc_466B11:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_466B71
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_466B60
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_466B71
	loc_466B60:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_466C91
	loc_466B71:
		mov		ecx, 20h
		sub		ecx, ebp
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_466C03
		shr		ecx, 1
		jb		loc_466BC3
		shr		ecx, 1
		jnb		loc_466B9F
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_466B9F:
		test	cl, cl
		jz		loc_466BC1
	loc_466BA3:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466BA3
	loc_466BC1:
		jmp		loc_466BF7
	loc_466BC3:
		inc		esi
		inc		edi
		shr		ecx, 1
		jnb		loc_466BD6
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466BD6:
		test	cl, cl
		jz		loc_466BF7
	loc_466BDA:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466BDA
	loc_466BF7:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		jmp		$+82h
	loc_466C03:
		shr		ecx, 1
		jb		loc_466C3B
		shr		ecx, 1
		jnb		loc_466C18
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi], dl
		add		edi, 2
	loc_466C18:
		test	cl, cl
		jz		loc_466C39
	loc_466C1C:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466C1C
	loc_466C39:
		jmp		loc_466C79
	loc_466C3B:
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		inc		esi
		mov		[edi], dl
		inc		edi
		shr		ecx, 1
		jnb		loc_466C57
		mov		dl, [esi+1]
		mov		dl, [ebx+edx]
		add		esi, 2
		mov		[edi+1], dl
		add		edi, 2
	loc_466C57:
		test	cl, cl
		jz		loc_466C79
	loc_466C5B:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466C5B
	loc_466C79:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
	// loc_466C80:
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_466B71
	loc_466C91:
		mov		ebp, 8
	loc_466C96:
		cmp		edi, gpBufEnd
		jb		loc_466CA6
		add		esi, 20h
		add		edi, 20h
		jmp		loc_466CC9
	loc_466CA6:
		mov		ecx, 8
	loc_466CAB:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, ah
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi+1], dl
		mov		dl, ah
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-1], dl
		jnz		loc_466CAB
	loc_466CC9:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_466CDF
		add		esi, 20h
		add		edi, 20h
		jmp		loc_466D01
	loc_466CDF:
		mov		ecx, 8
	loc_466CE4:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		shr		eax, 10h
		mov		dl, [ebx+edx]
		mov		[edi], dl
		mov		dl, al
		add		edi, 4
		mov		dl, [ebx+edx]
		dec		ecx
		mov		[edi-2], dl
		jnz		loc_466CE4
	loc_466D01:
		sub		edi, 320h
		dec		ebp
		jnz		loc_466C96
		pop		ebp
		jmp		loc_467944
	loc_466D10:
		mov		edx, 10h
	loc_466D15:
		cmp		edi, gpBufEnd
		jb		loc_466D25
		add		esi, 20h
		add		edi, 20h
		jmp		loc_466D41
	loc_466D25:
		mov		ecx, 8
	loc_466D2A:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_466D2A
	loc_466D41:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_466D57
		add		esi, 20h
		add		edi, 20h
		jmp		loc_466D71
	loc_466D57:
		mov		ecx, 8
	loc_466D5C:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_466D5C
	loc_466D71:
		sub		edi, 320h
		dec		edx
		jnz		loc_466D15
		jmp		loc_467944
	loc_466D7F:
		mov		eax, edi
		and		eax, 1
		mov		WorldBoolFlag, eax
		mov		ecx, 20h
	loc_466D8E:
		push	ecx
		mov		edx, 20h
	loc_466D94:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_466E35
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_466DB1
		add		esi, eax
		add		edi, eax
		jmp		loc_466E2C
	loc_466DB1:
		mov		ecx, eax
		mov		eax, edi
		and		eax, 1
		cmp		eax, WorldBoolFlag
		jnz		loc_466DF5
		shr		ecx, 1
		jnb		loc_466DCC
		inc		esi
		inc		edi
		test	ecx, ecx
		jz		loc_466E2C
		jmp		loc_466E05
	loc_466DCC:
		shr		ecx, 1
		jnb		loc_466DDC
		inc		esi
		inc		edi
		mov		al, [esi]
		inc		esi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_466E2C
	loc_466DDC:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_466DDC
		jmp		loc_466E2C
	loc_466DF5:
		shr		ecx, 1
		jnb		loc_466E05
		mov		al, [esi]
		inc		esi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_466E2C
		jmp		loc_466DCC
	loc_466E05:
		shr		ecx, 1
		jnb		loc_466E17
		mov		al, [esi]
		add		esi, 2
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_466E2C
	loc_466E17:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_466E17
	loc_466E2C:
		test	edx, edx
		jz		loc_466E41
		jmp		loc_466D94
	loc_466E35:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_466D94
	loc_466E41:
		pop		ecx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		sub		edi, 320h
		dec		ecx
		jnz		loc_466D8E
		jmp		loc_467944
	loc_466E62:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_466EC1
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_466EB0
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_466EC1
	loc_466EB0:
		sub		edi, 3000h
		add		esi, 120h
		jmp		$+83h
	loc_466EC1:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_466F0A
		shr		ecx, 2
		jnb		loc_466EF1
		mov		ax, [esi+2]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_466F34
	loc_466EF1:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_466EF1
		jmp		loc_466F34
	loc_466F0A:
		shr		ecx, 2
		jnb		loc_466F1F
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_466F34
	loc_466F1F:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_466F1F
	loc_466F34:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_466EC1
	// loc_466F3F:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_466F97
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_466F86
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_466F97
	loc_466F86:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_467944
	loc_466F97:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_466FE0
		shr		ecx, 2
		jnb		loc_466FC7
		mov		ax, [esi+2]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_467009
	loc_466FC7:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_466FC7
		jmp		loc_467009
	loc_466FE0:
		shr		ecx, 2
		jnb		loc_466FF4
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jz		loc_467009
	loc_466FF4:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_466FF4
	loc_467009:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_466F97
		jmp		loc_467944
	loc_46701C:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_46707C
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_46706B
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_46707C
	loc_46706B:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_4670FC
	loc_46707C:
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		ebp, ecx
		mov		WorldBoolFlag, eax
		jz		loc_4670C5
		shr		ecx, 2
		jz		loc_4670B1
	loc_46709A:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_46709A
	loc_4670B1:
		and		ebp, 2
		jz		loc_4670EF
		mov		ax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		jmp		loc_4670EF
	loc_4670C5:
		shr		ecx, 2
		jz		loc_4670DF
	loc_4670CA:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_4670CA
	loc_4670DF:
		and		ebp, 2
		jz		loc_4670EF
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
	loc_4670EF:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_46707C
	loc_4670FC:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467154
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_467143
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_467154
	loc_467143:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_4671DB
	loc_467154:
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		ebp, ecx
		mov		WorldBoolFlag, eax
		jz		loc_46719D
		shr		ecx, 2
		jz		loc_467189
	loc_467172:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_467172
	loc_467189:
		and		ebp, 2
		jz		loc_4671C7
		mov		ax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		jmp		loc_4671C7
	loc_46719D:
		shr		ecx, 2
		jz		loc_4671B7
	loc_4671A2:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_4671A2
	loc_4671B7:
		and		ebp, 2
		jz		loc_4671C7
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
	loc_4671C7:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_467154
	loc_4671DB:
		pop		ebp
		jmp		loc_467944
	loc_4671E1:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467240
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_46722F
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_467240
	loc_46722F:
		sub		edi, 3000h
		add		esi, 120h
		jmp		$+83h
	loc_467240:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_467289
		shr		ecx, 2
		jnb		loc_467270
		mov		ax, [esi+2]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_4672B3
	loc_467270:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_467270
		jmp		loc_4672B3
	loc_467289:
		shr		ecx, 2
		jnb		loc_46729E
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], al
		test	ecx, ecx
		lea		edi, [edi+2]
		jz		loc_4672B3
	loc_46729E:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		dec		ecx
		lea		edi, [edi+2]
		jnz		loc_46729E
	loc_4672B3:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_467240
	// loc_4672BE:
		mov		edx, 8
	loc_4672C3:
		cmp		edi, gpBufEnd
		jb		loc_4672D3
		add		esi, 20h
		add		edi, 20h
		jmp		loc_4672EF
	loc_4672D3:
		mov		ecx, 8
	loc_4672D8:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_4672D8
	loc_4672EF:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_467305
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46731F
	loc_467305:
		mov		ecx, 8
	loc_46730A:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_46730A
	loc_46731F:
		sub		edi, 320h
		dec		edx
		jnz		loc_4672C3
		jmp		loc_467944
	loc_46732D:
		push	ebp
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_46738D
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_46737C
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		mov		eax, WorldBoolFlag
		shr		ecx, 1
		add		eax, ecx
		mov		WorldBoolFlag, eax
		jmp		loc_46738D
	loc_46737C:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_46740D
	loc_46738D:
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		ebp, ecx
		mov		WorldBoolFlag, eax
		jz		loc_4673D6
		shr		ecx, 2
		jz		loc_4673C2
	loc_4673AB:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_4673AB
	loc_4673C2:
		and		ebp, 2
		jz		loc_467400
		mov		ax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		inc		edi
		jmp		loc_467400
	loc_4673D6:
		shr		ecx, 2
		jz		loc_4673F0
	loc_4673DB:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_4673DB
	loc_4673F0:
		and		ebp, 2
		jz		loc_467400
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
	loc_467400:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_46738D
	loc_46740D:
		mov		edx, 8
	loc_467412:
		cmp		edi, gpBufEnd
		jb		loc_467422
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46743E
	loc_467422:
		mov		ecx, 8
	loc_467427:
		mov		eax, [esi]
		add		esi, 4
		inc		edi
		ror		eax, 8
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		inc		edi
		dec		ecx
		jnz		loc_467427
	loc_46743E:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_467454
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46746E
	loc_467454:
		mov		ecx, 8
	loc_467459:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], al
		add		edi, 2
		ror		eax, 10h
		mov		[edi], al
		add		edi, 2
		dec		ecx
		jnz		loc_467459
	loc_46746E:
		sub		edi, 320h
		dec		edx
		jnz		loc_467412
		pop		ebp
		jmp		loc_467944
	loc_46747D:
		mov		edx, 10h
		xor		eax, eax
	loc_467484:
		cmp		edi, gpBufEnd
		jb		loc_467494
		add		esi, 20h
		add		edi, 20h
		jmp		loc_4674A5
	loc_467494:
		mov		ecx, 8
	loc_467499:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_467499
	loc_4674A5:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_4674BB
		add		esi, 20h
		add		edi, 20h
		jmp		loc_4674CB
	loc_4674BB:
		mov		ecx, 8
	loc_4674C0:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4674C0
	loc_4674CB:
		sub		edi, 320h
		dec		edx
		jnz		loc_467484
		jmp		loc_467944
	loc_4674D9:
		mov		eax, edi
		and		eax, 1
		mov		WorldBoolFlag, eax
		mov		ecx, 20h
	loc_4674E8:
		push	ecx
		mov		edx, 20h
	loc_4674EE:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_46756C
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_467507
		add		esi, eax
		add		edi, eax
		jmp		loc_467566
	loc_467507:
		mov		ecx, eax
		add		esi, ecx
		mov		eax, edi
		and		eax, 1
		cmp		eax, WorldBoolFlag
		jnz		loc_46753F
		xor		eax, eax
		shr		ecx, 1
		jnb		loc_467525
		inc		edi
		test	ecx, ecx
		jz		loc_467566
		jmp		loc_46754E
	loc_467525:
		shr		ecx, 1
		jnb		loc_467531
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_467566
	loc_467531:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_467531
		jmp		loc_467566
	loc_46753F:
		xor		eax, eax
		shr		ecx, 1
		jnb		loc_46754E
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_467566
		jmp		loc_467525
	loc_46754E:
		shr		ecx, 1
		jnb		loc_46755B
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_467566
	loc_46755B:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_46755B
	loc_467566:
		test	edx, edx
		jz		loc_467578
		jmp		loc_4674EE
	loc_46756C:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_4674EE
	loc_467578:
		pop		ecx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		sub		edi, 320h
		dec		ecx
		jnz		loc_4674E8
		jmp		loc_467944
	loc_467599:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_4675A5:
		cmp		edi, gpBufEnd
		jb		loc_4675B7
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_467608
	loc_4675B7:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4675ED
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4675DF
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_467608
	loc_4675DF:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_4675DF
		jmp		loc_467608
	loc_4675ED:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4675FD
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_467608
	loc_4675FD:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4675FD
	loc_467608:
		sub		edi, 320h
		test	edx, edx
		jz		loc_467617
		sub		edx, 2
		jmp		loc_4675A5
	loc_467617:
		mov		edx, 2
	loc_46761C:
		cmp		edi, gpBufEnd
		jb		loc_46762E
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_46767F
	loc_46762E:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_467664
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_467656
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_46767F
	loc_467656:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_467656
		jmp		loc_46767F
	loc_467664:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_467674
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_46767F
	loc_467674:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_467674
	loc_46767F:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_46761C
		jmp		loc_467944
	loc_467692:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_46769E:
		cmp		edi, gpBufEnd
		jb		loc_4676B2
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_467701
	loc_4676B2:
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4676E6
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4676D8
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_467701
	loc_4676D8:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_4676D8
		jmp		loc_467701
	loc_4676E6:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4676F6
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_467701
	loc_4676F6:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4676F6
	loc_467701:
		sub		edi, 320h
		test	edx, edx
		jz		loc_467712
		add		edi, edx
		sub		edx, 2
		jmp		loc_46769E
	loc_467712:
		mov		edx, 2
	loc_467717:
		cmp		edi, gpBufEnd
		jb		loc_46772B
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_46777A
	loc_46772B:
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_46775F
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_467751
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_46777A
	loc_467751:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_467751
		jmp		loc_46777A
	loc_46775F:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_46776F
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_46777A
	loc_46776F:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_46776F
	loc_46777A:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_467717
		jmp		loc_467944
	loc_46778F:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_46779B:
		cmp		edi, gpBufEnd
		jb		loc_4677AD
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_4677FE
	loc_4677AD:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4677E3
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4677D5
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_4677FE
	loc_4677D5:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_4677D5
		jmp		loc_4677FE
	loc_4677E3:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4677F3
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_4677FE
	loc_4677F3:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4677F3
	loc_4677FE:
		sub		edi, 320h
		test	edx, edx
		jz		loc_46780D
		sub		edx, 2
		jmp		loc_46779B
	loc_46780D:
		mov		edx, 8
	loc_467812:
		cmp		edi, gpBufEnd
		jb		loc_467822
		add		esi, 20h
		add		edi, 20h
		jmp		loc_467835
	loc_467822:
		mov		ecx, 8
		xor		eax, eax
	loc_467829:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_467829
	loc_467835:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_46784B
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46785D
	loc_46784B:
		mov		ecx, 8
		xor		eax, eax
	loc_467852:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_467852
	loc_46785D:
		sub		edi, 320h
		dec		edx
		jnz		loc_467812
		jmp		loc_467944
	loc_46786B:
		xor		eax, eax
		mov		WorldBoolFlag, eax
		mov		edx, 1Eh
	loc_467877:
		cmp		edi, gpBufEnd
		jb		loc_46788B
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_4678DA
	loc_46788B:
		mov		ecx, 20h
		sub		ecx, edx
		mov		eax, WorldBoolFlag
		inc		eax
		and		eax, 1
		mov		WorldBoolFlag, eax
		jz		loc_4678BF
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4678B1
		inc		edi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_4678DA
	loc_4678B1:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_4678B1
		jmp		loc_4678DA
	loc_4678BF:
		xor		eax, eax
		shr		ecx, 2
		jnb		loc_4678CF
		mov		[edi], al
		add		edi, 2
		test	ecx, ecx
		jz		loc_4678DA
	loc_4678CF:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_4678CF
	loc_4678DA:
		sub		edi, 320h
		test	edx, edx
		jz		loc_4678EB
		add		edi, edx
		sub		edx, 2
		jmp		loc_467877
	loc_4678EB:
		mov		edx, 8
	loc_4678F0:
		cmp		edi, gpBufEnd
		jb		loc_467900
		add		esi, 20h
		add		edi, 20h
		jmp		loc_467913
	loc_467900:
		mov		ecx, 8
		xor		eax, eax
	loc_467907:
		mov		[edi+1], al
		mov		[edi+3], al
		add		edi, 4
		dec		ecx
		jnz		loc_467907
	loc_467913:
		sub		edi, 320h
		cmp		edi, gpBufEnd
		jb		loc_467929
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46793B
	loc_467929:
		mov		ecx, 8
		xor		eax, eax
	loc_467930:
		mov		[edi], al
		mov		[edi+2], al
		add		edi, 4
		dec		ecx
		jnz		loc_467930
	loc_46793B:
		sub		edi, 320h
		dec		edx
		jnz		loc_4678F0
	loc_467944:
		pop		esi
		pop		edi
		pop		edx
		pop		ebx
		retn
	}
}

__declspec(naked) void drawBottomArchesLowerScreen(BYTE *pBuff, DWORD *pMask)
{
	__asm {
		push	ebx
		push	edi
		push	esi
		mov		edi, offset SpeedFrameTbl
		mov		gpCelFrame, edi
		mov		edi, ecx
		mov		gpDrawMask, edx
		mov		eax, light_table_index
		test	al, al
		jz		loc_467A58
		cmp		al, lightmax
		jz		loc_467AB8
		mov		eax, level_cel_block
		and		eax, 8000h
		jnz		loc_4679EC
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		ebx, light_table_index
		shl		ebx, 8
		add		ebx, pLightTbl
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
		jz		loc_467B43
		cmp		ax, 1
		jz		loc_467B8F
		cmp		ax, 2
		jz		loc_467C20
		cmp		ax, 3
		jz		loc_467D5B
		cmp		ax, 4
		jz		loc_467E99
		jmp		loc_467F8A
	loc_4679EC:
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 4
		add		eax, light_table_index
		shl		eax, 2
		add		esi, eax
		mov		eax, [esi]
		mov		esi, pSpeedCels
		add		esi, eax
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 0Fh
	loc_467A21:
		cmp		ax, 8
		jz		loc_468086
		cmp		ax, 9
		jz		loc_4680D1
		cmp		ax, 0Ah
		jz		loc_468161
		cmp		ax, 0Bh
		jz		loc_468268
		cmp		ax, 0Ch
		jz		loc_468372
		jmp		loc_468448
	loc_467A58:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_467A8A
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_467A8A:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		add		eax, 8
		jmp		loc_467A21
	loc_467AB8:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_467AEA
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_467AEA:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		jz		loc_468529
		cmp		ax, 1
		jz		loc_468573
		cmp		ax, 2
		jz		loc_468604
		cmp		ax, 3
		jz		loc_468696
		cmp		ax, 4
		jz		loc_46872C
		jmp		loc_4687CB
	loc_467B43:
		push	ebp
		mov		ebp, 20h
	loc_467B49:
		cmp		edi, gpBufEnd
		jb		loc_467B59
		add		esi, 20h
		add		edi, 20h
		jmp		loc_467B73
	loc_467B59:
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_467B65:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_467B6F
		xlat
		mov		[edi], al
	loc_467B6F:
		inc		edi
		dec		ecx
		jnz		loc_467B65
	loc_467B73:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		ebp
		jnz		loc_467B49
		pop		ebp
		jmp		loc_468867
	loc_467B8F:
		mov		ecx, 20h
	loc_467B94:
		push	ecx
		mov		eax, gpDrawMask
		mov		eax, [eax]
		mov		gdwCurrentMask, eax
		mov		edx, 20h
	loc_467BA6:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_467BE3
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_467BBF
		add		esi, eax
		add		edi, eax
		jmp		loc_467BDD
	loc_467BBF:
		mov		ecx, eax
		push	edx
		mov		edx, gdwCurrentMask
	loc_467BC8:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_467BD2
		xlat
		mov		[edi], al
	loc_467BD2:
		inc		edi
		dec		ecx
		jnz		loc_467BC8
		mov		gdwCurrentMask, edx
		pop		edx
	loc_467BDD:
		test	edx, edx
		jz		loc_467C00
		jmp		loc_467BA6
	loc_467BE3:
		neg		al
		add		edi, eax
		mov		ecx, eax
		and		ecx, 1Fh
		jz		loc_467BFC
		push	eax
		mov		eax, gdwCurrentMask
		shl		eax, cl
		mov		gdwCurrentMask, eax
		pop		eax
	loc_467BFC:
		sub		edx, eax
		jnz		loc_467BA6
	loc_467C00:
		pop		ecx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		ecx
		jnz		loc_467B94
		jmp		loc_468867
	loc_467C20:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467C67
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_467C59
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_467C67
	loc_467C59:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_467CB8
	loc_467C67:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_467C90
		mov		ax, [esi+2]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_467CAD
	loc_467C90:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_467C90
	loc_467CAD:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_467C67
	loc_467CB8:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467D02
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_467CF1
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		jmp		loc_467D02
	loc_467CF1:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_468867
	loc_467D02:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_467D2B
		mov		ax, [esi+2]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_467D48
	loc_467D2B:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_467D2B
	loc_467D48:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_467D02
		jmp		loc_468867
	loc_467D5B:
		push	ebp
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467DA3
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_467D95
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_467DA3
	loc_467D95:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_467DF6
	loc_467DA3:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_467DCE
	loc_467DB1:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_467DB1
	loc_467DCE:
		and		ebp, 2
		jz		loc_467DE9
		mov		ax, [esi]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
	loc_467DE9:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_467DA3
	loc_467DF6:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467E3D
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_467E2F
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		jmp		loc_467E3D
	loc_467E2F:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_467E93
	loc_467E3D:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_467E68
	loc_467E4B:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_467E4B
	loc_467E68:
		and		ebp, 2
		jz		loc_467E83
		mov		ax, [esi]
		add		esi, 2
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
	loc_467E83:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_467E3D
	loc_467E93:
		pop		ebp
		jmp		loc_468867
	loc_467E99:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467EE0
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_467ED2
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_467EE0
	loc_467ED2:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_467F31
	loc_467EE0:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_467F09
		mov		ax, [esi+2]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_467F26
	loc_467F09:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_467F09
	loc_467F26:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_467EE0
	loc_467F31:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_467F43:
		cmp		edi, gpBufEnd
		jb		loc_467F53
		add		esi, 20h
		add		edi, 20h
		jmp		loc_467F6F
	loc_467F53:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_467F60:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_467F6A
		xlat
		mov		[edi], al
	loc_467F6A:
		inc		edi
		dec		ecx
		jnz		loc_467F60
		pop		edx
	loc_467F6F:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_467F43
		jmp		loc_468867
	loc_467F8A:
		push	ebp
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_467FD2
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_467FC4
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_467FD2
	loc_467FC4:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_468025
	loc_467FD2:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_467FFD
	loc_467FE0:
		mov		eax, [esi]
		add		esi, 4
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_467FE0
	loc_467FFD:
		and		ebp, 2
		jz		loc_468018
		mov		ax, [esi]
		add		esi, 4
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		mov		[edi], ax
		add		edi, 2
	loc_468018:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_467FD2
	loc_468025:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_468037:
		cmp		edi, gpBufEnd
		jb		loc_468047
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46806A
	loc_468047:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_468054:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_46805E
		xlat
		mov		[edi], al
	loc_46805E:
		inc		edi
		dec		ecx
		jnz		loc_468054
		mov		ebp, esi
		and		ebp, 2
		add		esi, ebp
		pop		edx
	loc_46806A:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_468037
		pop		ebp
		jmp		loc_468867
	loc_468086:
		mov		edx, 20h
	loc_46808B:
		cmp		edi, gpBufEnd
		jb		loc_46809B
		add		esi, 20h
		add		edi, 20h
		jmp		loc_4680B6
	loc_46809B:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_4680A8:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_4680B1
		mov		[edi], al
	loc_4680B1:
		inc		edi
		dec		ecx
		jnz		loc_4680A8
		pop		edx
	loc_4680B6:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_46808B
		jmp		loc_468867
	loc_4680D1:
		mov		ecx, 20h
	loc_4680D6:
		push	ecx
		mov		eax, gpDrawMask
		mov		eax, [eax]
		mov		gdwCurrentMask, eax
		mov		edx, 20h
	loc_4680E8:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_468124
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_468101
		add		esi, eax
		add		edi, eax
		jmp		loc_46811E
	loc_468101:
		mov		ecx, eax
		push	edx
		mov		edx, gdwCurrentMask
	loc_46810A:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_468113
		mov		[edi], al
	loc_468113:
		inc		edi
		dec		ecx
		jnz		loc_46810A
		mov		gdwCurrentMask, edx
		pop		edx
	loc_46811E:
		test	edx, edx
		jz		loc_468141
		jmp		loc_4680E8
	loc_468124:
		neg		al
		add		edi, eax
		mov		ecx, eax
		and		ecx, 1Fh
		jz		loc_46813D
		mov		ebx, gdwCurrentMask
		shl		ebx, cl
		mov		gdwCurrentMask, ebx
	loc_46813D:
		sub		edx, eax
		jnz		loc_4680E8
	loc_468141:
		pop		ecx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		ecx
		jnz		loc_4680D6
		jmp		loc_468867
	loc_468161:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4681A8
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_46819A
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_4681A8
	loc_46819A:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_4681DF
	loc_4681A8:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4681C7
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4681D4
	loc_4681C7:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4681C7
	loc_4681D4:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_4681A8
	loc_4681DF:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468229
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_468218
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		jmp		loc_468229
	loc_468218:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_468867
	loc_468229:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_468248
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_468255
	loc_468248:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_468248
	loc_468255:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_468229
		jmp		loc_468867
	loc_468268:
		push	ebp
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4682B0
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_4682A2
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_4682B0
	loc_4682A2:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_4682E9
	loc_4682B0:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_4682CB
	loc_4682BE:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4682BE
	loc_4682CB:
		and		ebp, 2
		jz		loc_4682DC
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_4682DC:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_4682B0
	loc_4682E9:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468330
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_468322
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		jmp		loc_468330
	loc_468322:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_46836C
	loc_468330:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_46834B
	loc_46833E:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46833E
	loc_46834B:
		and		ebp, 2
		jz		loc_46835C
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_46835C:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_468330
	loc_46836C:
		pop		ebp
		jmp		loc_468867
	loc_468372:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4683B9
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_4683AB
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_4683B9
	loc_4683AB:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_4683F0
	loc_4683B9:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4683D8
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4683E5
	loc_4683D8:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4683D8
	loc_4683E5:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_4683B9
	loc_4683F0:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_468402:
		cmp		edi, gpBufEnd
		jb		loc_468412
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46842D
	loc_468412:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_46841F:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_468428
		mov		[edi], al
	loc_468428:
		inc		edi
		dec		ecx
		jnz		loc_46841F
		pop		edx
	loc_46842D:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_468402
		jmp		loc_468867
	loc_468448:
		push	ebp
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468490
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_468482
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_468490
	loc_468482:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_4684C9
	loc_468490:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebp, ecx
		shr		ecx, 2
		jz		loc_4684AB
	loc_46849E:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46849E
	loc_4684AB:
		and		ebp, 2
		jz		loc_4684BC
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_4684BC:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_468490
	loc_4684C9:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_4684DB:
		cmp		edi, gpBufEnd
		jb		loc_4684EB
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46850D
	loc_4684EB:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		mov		ecx, 20h
	loc_4684F8:
		mov		al, [esi]
		inc		esi
		shl		edx, 1
		jnb		loc_468501
		mov		[edi], al
	loc_468501:
		inc		edi
		dec		ecx
		jnz		loc_4684F8
		mov		ebp, esi
		and		ebp, 2
		add		esi, ebp
		pop		edx
	loc_46850D:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_4684DB
		pop		ebp
		jmp		loc_468867
	loc_468529:
		mov		edx, 20h
	loc_46852E:
		cmp		edi, gpBufEnd
		jb		loc_46853E
		add		esi, 20h
		add		edi, 20h
		jmp		loc_468558
	loc_46853E:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		xor		eax, eax
		mov		ecx, 20h
	loc_46854D:
		shl		edx, 1
		jnb		loc_468553
		mov		[edi], al
	loc_468553:
		inc		edi
		dec		ecx
		jnz		loc_46854D
		pop		edx
	loc_468558:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_46852E
		jmp		loc_468867
	loc_468573:
		mov		ecx, 20h
	loc_468578:
		push	ecx
		mov		eax, gpDrawMask
		mov		eax, [eax]
		mov		gdwCurrentMask, eax
		mov		edx, 20h
	loc_46858A:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_4685C7
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_4685A3
		add		esi, eax
		add		edi, eax
		jmp		loc_4685C1
	loc_4685A3:
		mov		ecx, eax
		add		esi, ecx
		push	edx
		mov		edx, gdwCurrentMask
		xor		eax, eax
	loc_4685B0:
		shl		edx, 1
		jnb		loc_4685B6
		mov		[edi], al
	loc_4685B6:
		inc		edi
		dec		ecx
		jnz		loc_4685B0
		mov		gdwCurrentMask, edx
		pop		edx
	loc_4685C1:
		test	edx, edx
		jz		loc_4685E4
		jmp		loc_46858A
	loc_4685C7:
		neg		al
		add		edi, eax
		mov		ecx, eax
		and		ecx, 1Fh
		jz		loc_4685E0
		mov		ebx, gdwCurrentMask
		shl		ebx, cl
		mov		gdwCurrentMask, ebx
	loc_4685E0:
		sub		edx, eax
		jnz		loc_46858A
	loc_4685E4:
		pop		ecx
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		ecx
		jnz		loc_468578
		jmp		loc_468867
	loc_468604:
		mov		edx, 1Eh
		xor		eax, eax
	loc_46860B:
		cmp		edi, gpBufEnd
		jb		loc_46861D
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_46863D
	loc_46861D:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_468635
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_46863D
	loc_468635:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_468635
	loc_46863D:
		sub		edi, 320h
		test	edx, edx
		jz		loc_46864C
		sub		edx, 2
		jmp		loc_46860B
	loc_46864C:
		mov		edx, 2
	loc_468651:
		cmp		edi, gpBufEnd
		jb		loc_468663
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_468683
	loc_468663:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46867B
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_468683
	loc_46867B:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46867B
	loc_468683:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_468651
		jmp		loc_468867
	loc_468696:
		mov		edx, 1Eh
		xor		eax, eax
	loc_46869D:
		cmp		edi, gpBufEnd
		jb		loc_4686B1
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_4686CF
	loc_4686B1:
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4686C7
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4686CF
	loc_4686C7:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4686C7
	loc_4686CF:
		sub		edi, 320h
		test	edx, edx
		jz		loc_4686E0
		add		edi, edx
		sub		edx, 2
		jmp		loc_46869D
	loc_4686E0:
		mov		edx, 2
	loc_4686E5:
		cmp		edi, gpBufEnd
		jb		loc_4686F9
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_468717
	loc_4686F9:
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46870F
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_468717
	loc_46870F:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46870F
	loc_468717:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_4686E5
		jmp		loc_468867
	loc_46872C:
		mov		edx, 1Eh
		xor		eax, eax
	loc_468733:
		cmp		edi, gpBufEnd
		jb		loc_468745
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_468765
	loc_468745:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46875D
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_468765
	loc_46875D:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46875D
	loc_468765:
		sub		edi, 320h
		test	edx, edx
		jz		loc_468774
		sub		edx, 2
		jmp		loc_468733
	loc_468774:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_468786:
		cmp		edi, gpBufEnd
		jb		loc_468796
		add		esi, 20h
		add		edi, 20h
		jmp		loc_4687B0
	loc_468796:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		xor		eax, eax
		mov		ecx, 20h
	loc_4687A5:
		shl		edx, 1
		jnb		loc_4687AB
		mov		[edi], al
	loc_4687AB:
		inc		edi
		dec		ecx
		jnz		loc_4687A5
		pop		edx
	loc_4687B0:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_468786
		jmp		loc_468867
	loc_4687CB:
		mov		edx, 1Eh
		xor		eax, eax
	loc_4687D2:
		cmp		edi, gpBufEnd
		jb		loc_4687E6
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_468804
	loc_4687E6:
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4687FC
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_468804
	loc_4687FC:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4687FC
	loc_468804:
		sub		edi, 320h
		test	edx, edx
		jz		loc_468815
		add		edi, edx
		sub		edx, 2
		jmp		loc_4687D2
	loc_468815:
		mov		eax, gpDrawMask
		sub		eax, 40h
		mov		gpDrawMask, eax
		mov		edx, 10h
	loc_468827:
		cmp		edi, gpBufEnd
		jb		loc_468837
		add		esi, 20h
		add		edi, 20h
		jmp		loc_468851
	loc_468837:
		push	edx
		mov		eax, gpDrawMask
		mov		edx, [eax]
		xor		eax, eax
		mov		ecx, 20h
	loc_468846:
		shl		edx, 1
		jnb		loc_46884C
		mov		[edi], al
	loc_46884C:
		inc		edi
		dec		ecx
		jnz		loc_468846
		pop		edx
	loc_468851:
		sub		edi, 320h
		mov		eax, gpDrawMask
		sub		eax, 4
		mov		gpDrawMask, eax
		dec		edx
		jnz		loc_468827
	loc_468867:
		pop		esi
		pop		edi
		pop		ebx
		retn
	}
}

__declspec(naked) void drawLowerScreen(BYTE *pBuff)
{
	__asm {
		push	ebx
		push	edx
		push	edi
		push	esi
		mov		edx, cel_transparency_active
		test	edx, edx
		jz		loc_468918
		mov		dl, arch_draw_type
		cmp		dl, 0
		jnz		loc_468892
		call	drawTopArchesLowerScreen
		jmp		loc_4696B9
	loc_468892:
		cmp		dl, 1
		jnz		loc_4688D5
		mov		ebx, level_piece_id
		mov		al, block_lvid[ebx]
		cmp		al, 1
		jz		loc_4688AD
		cmp		al, 3
		jz		loc_4688AD
		jmp		loc_4688D5
	loc_4688AD:
		mov		edx, offset LeftMask
		add		edx, 7Ch
		call	drawBottomArchesLowerScreen
		jmp		loc_4696B9
		cmp		al, 4
		jnz		loc_4688D5
		mov		edx, offset RightMask
		add		edx, 7Ch
		call	drawBottomArchesLowerScreen
		jmp		loc_4696B9
	loc_4688D5:
		cmp		dl, 2
		jnz		loc_468918
		mov		ebx, level_piece_id
		mov		al, block_lvid[ebx]
		cmp		al, 2
		jz		loc_4688F0
		cmp		al, 3
		jz		loc_4688F0
		jmp		loc_468918
	loc_4688F0:
		mov		edx, offset RightMask
		add		edx, 7Ch
		call	drawBottomArchesLowerScreen
		jmp		loc_4696B9
		cmp		al, 4
		jnz		loc_468918
		mov		edx, offset LeftMask
		add		edx, 7Ch
		call	drawBottomArchesLowerScreen
		jmp		loc_4696B9
	loc_468918:
		mov		edi, offset SpeedFrameTbl
		mov		gpCelFrame, edi
		mov		edi, ecx
		mov		eax, light_table_index
		test	al, al
		jz		loc_468A1A
		cmp		al, lightmax
		jz		loc_468A78
		mov		eax, level_cel_block
		and		eax, 8000h
		jnz		loc_4689B0
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		ebx, light_table_index
		shl		ebx, 8
		add		ebx, pLightTbl
		mov		eax, level_cel_block
		shr		eax, 0Ch
		and		eax, 0Fh
		jz		loc_468B03
		cmp		ax, 1
		jz		loc_468B5B
		cmp		ax, 2
		jz		loc_468C06
		cmp		ax, 3
		jz		loc_468CEC
		cmp		ax, 4
		jz		loc_468DD2
		jmp		loc_468EE2
	loc_4689B0:
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 4
		add		eax, light_table_index
		shl		eax, 2
		add		esi, eax
		mov		eax, [esi]
		mov		esi, pSpeedCels
		add		esi, eax
		mov		eax, level_cel_block
		shr		eax, 0Ch
		and		eax, 0Fh
	loc_4689E3:
		cmp		ax, 8
		jz		loc_468FF2
		cmp		ax, 9
		jz		loc_469027
		cmp		ax, 0Ah
		jz		loc_469099
		cmp		ax, 0Bh
		jz		loc_46919D
		cmp		ax, 0Ch
		jz		loc_4692A5
		jmp		loc_469358
	loc_468A1A:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_468A4C
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_468A4C:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		shr		eax, 0Ch
		and		eax, 7
		add		eax, 8
		jmp		loc_4689E3
	loc_468A78:
		mov		eax, level_cel_block
		and		eax, 8000h
		jz		loc_468AAA
		mov		esi, gpCelFrame
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 6
		add		esi, eax
		mov		eax, level_cel_block
		and		eax, 0F000h
		add		eax, [esi]
		mov		level_cel_block, eax
	loc_468AAA:
		mov		ebx, pDungeonCels
		mov		esi, ebx
		mov		eax, level_cel_block
		and		eax, 0FFFh
		shl		eax, 2
		add		ebx, eax
		add		esi, [ebx]
		mov		eax, level_cel_block
		mov		al, ah
		shr		eax, 4
		and		eax, 7
		jz		loc_46940D
		cmp		ax, 1
		jz		loc_46943F
		cmp		ax, 2
		jz		loc_4694A7
		cmp		ax, 3
		jz		loc_469539
		cmp		ax, 4
		jz		loc_4695CF
		jmp		loc_469644
	loc_468B03:
		mov		edx, 20h
		push	ebp
	loc_468B09:
		push	edx
		cmp		edi, gpBufEnd
		jb		loc_468B1A
		add		esi, 20h
		add		edi, 20h
		jmp		loc_468B4B
	loc_468B1A:
		xor		edx, edx
		mov		ebp, 8
	loc_468B21:
		mov		eax, [esi]
		add		esi, 4
		ror		eax, 10h
		mov		dl, al
		mov		cl, [ebx+edx]
		mov		dl, ah
		mov		ch, [ebx+edx]
		ror		eax, 10h
		shl		ecx, 10h
		mov		dl, al
		mov		cl, [ebx+edx]
		mov		dl, ah
		mov		ch, [ebx+edx]
		mov		[edi], ecx
		add		edi, 4
		dec		ebp
		jnz		loc_468B21
	loc_468B4B:
		sub		edi, 320h
		pop		edx
		dec		edx
		jnz		loc_468B09
		pop		ebp
		jmp		loc_4696B9
	loc_468B5B:
		push	ebp
		mov		ecx, 20h
	loc_468B61:
		push	ecx
		mov		ebp, 20h
	loc_468B67:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		jns		loc_468B78
		neg		al
		add		edi, eax
		sub		ebp, eax
		jmp		loc_468BEA
	loc_468B78:
		sub		ebp, eax
		cmp		edi, gpBufEnd
		jb		loc_468B88
		add		esi, eax
		add		edi, eax
		jmp		loc_468BEA
	loc_468B88:
		mov		ecx, eax
		cmp		cl, 4
		jl		loc_468BC1
	loc_468B8F:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_468B8F
	loc_468BC1:
		cmp		cl, 2
		jl		loc_468BDC
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_468BDC:
		and		cl, 1
		jz		loc_468BEA
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebx+edx]
		mov		[edi], dl
		inc		edi
	loc_468BEA:
		test	ebp, ebp
		jnz		loc_468B67
		pop		ecx
		sub		edi, 320h
		dec		ecx
		jnz		loc_468B61
		pop		ebp
		jmp		loc_4696B9
	loc_468C06:
		push	ebp
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468C4E
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_468C40
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		jmp		loc_468C4E
	loc_468C40:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_468C75
	loc_468C4E:
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
	loc_468C5E:
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebx+edx]
		mov		[edi], dl
		inc		edi
		dec		ecx
		jnz		loc_468C5E
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_468C4E
	loc_468C75:
		mov		ebp, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468CBC
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_468CAE
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		ebp, ecx
		jmp		loc_468CBC
	loc_468CAE:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_468CE6
	loc_468CBC:
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
	loc_468CCC:
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebx+edx]
		mov		[edi], dl
		inc		edi
		dec		ecx
		jnz		loc_468CCC
		add		ebp, 2
		sub		edi, 320h
		cmp		ebp, 20h
		jnz		loc_468CBC
	loc_468CE6:
		pop		ebp
		jmp		loc_4696B9
	loc_468CEC:
		push	ebp
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468D34
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_468D26
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		jmp		loc_468D34
	loc_468D26:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_468D5B
	loc_468D34:
		mov		ecx, 20h
		sub		ecx, ebp
	loc_468D3B:
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebx+edx]
		mov		[edi], dl
		inc		edi
		dec		ecx
		jnz		loc_468D3B
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_468D34
	loc_468D5B:
		mov		ebp, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468DA2
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_468D94
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		ebp, ecx
		jmp		loc_468DA2
	loc_468D94:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_468DCC
	loc_468DA2:
		mov		ecx, 20h
		sub		ecx, ebp
	loc_468DA9:
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebx+edx]
		mov		[edi], dl
		inc		edi
		dec		ecx
		jnz		loc_468DA9
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		sub		edi, 320h
		add		edi, ebp
		add		ebp, 2
		cmp		ebp, 20h
		jnz		loc_468DA2
	loc_468DCC:
		pop		ebp
		jmp		loc_4696B9
	loc_468DD2:
		push	ebp
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468E1A
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_468E0C
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		jmp		loc_468E1A
	loc_468E0C:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_468E87
	loc_468E1A:
		add		edi, ebp
		mov		ecx, 20h
		sub		ecx, ebp
		mov		edx, ecx
		and		edx, 2
		add		esi, edx
		cmp		cl, 4
		jl		loc_468E61
	loc_468E2F:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_468E2F
	loc_468E61:
		cmp		cl, 2
		jl		loc_468E7C
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_468E7C:
		sub		edi, 320h
		sub		ebp, 2
		jge		loc_468E1A
	loc_468E87:
		mov		ebp, 10h
	loc_468E8C:
		cmp		edi, gpBufEnd
		jb		loc_468E9C
		add		esi, 20h
		add		edi, 20h
		jmp		loc_468ED3
	loc_468E9C:
		mov		ecx, 20h
	loc_468EA1:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_468EA1
	loc_468ED3:
		sub		edi, 320h
		dec		ebp
		jnz		loc_468E8C
		pop		ebp
		jmp		loc_4696B9
	loc_468EE2:
		push	ebp
		mov		ebp, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_468F2A
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_468F1C
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		ebp, ecx
		jmp		loc_468F2A
	loc_468F1C:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_468F97
	loc_468F2A:
		mov		ecx, 20h
		sub		ecx, ebp
		cmp		cl, 4
		jl		loc_468F68
	loc_468F36:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_468F36
	loc_468F68:
		cmp		cl, 2
		jl		loc_468F83
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	loc_468F83:
		mov		edx, esi
		and		edx, 2
		add		esi, edx
		sub		edi, 320h
		add		edi, ebp
		sub		ebp, 2
		jge		loc_468F2A
	loc_468F97:
		mov		ebp, 10h
	loc_468F9C:
		cmp		edi, gpBufEnd
		jb		loc_468FAC
		add		esi, 20h
		add		edi, 20h
		jmp		loc_468FE3
	loc_468FAC:
		mov		ecx, 20h
	loc_468FB1:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		sub		cl, 4
		cmp		cl, 4
		jge		loc_468FB1
	loc_468FE3:
		sub		edi, 320h
		dec		ebp
		jnz		loc_468F9C
		pop		ebp
		jmp		loc_4696B9
	loc_468FF2:
		mov		edx, 20h
	loc_468FF7:
		cmp		edi, gpBufEnd
		jb		loc_469007
		add		esi, 20h
		add		edi, 20h
		jmp		loc_469019
	loc_469007:
		mov		ecx, 8
	loc_46900C:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46900C
	loc_469019:
		sub		edi, 320h
		dec		edx
		jnz		loc_468FF7
		jmp		loc_4696B9
	loc_469027:
		mov		ecx, 20h
	loc_46902C:
		push	ecx
		mov		edx, 20h
	loc_469032:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_469082
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_46904B
		add		esi, eax
		add		edi, eax
		jmp		loc_46907C
	loc_46904B:
		mov		ecx, eax
		shr		ecx, 1
		jnb		loc_46905B
		mov		al, [esi]
		inc		esi
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_46907C
	loc_46905B:
		shr		ecx, 1
		jnb		loc_46906F
		mov		ax, [esi]
		add		esi, 2
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_46907C
	loc_46906F:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46906F
	loc_46907C:
		test	edx, edx
		jz		loc_46908A
		jmp		loc_469032
	loc_469082:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_469032
	loc_46908A:
		pop		ecx
		sub		edi, 320h
		dec		ecx
		jnz		loc_46902C
		jmp		loc_4696B9
	loc_469099:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4690E0
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_4690D2
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_4690E0
	loc_4690D2:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_469117
	loc_4690E0:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4690FF
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_46910C
	loc_4690FF:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4690FF
	loc_46910C:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_4690E0
	loc_469117:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_46915E
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_469150
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		jmp		loc_46915E
	loc_469150:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_469198
	loc_46915E:
		mov		ecx, 20h
		add		edi, edx
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46917D
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_46918A
	loc_46917D:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46917D
	loc_46918A:
		add		edx, 2
		sub		edi, 320h
		cmp		edx, 20h
		jl		loc_46915E
	loc_469198:
		jmp		loc_4696B9
	loc_46919D:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4691E4
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_4691D6
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_4691E4
	loc_4691D6:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_46921D
	loc_4691E4:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebx, ecx
		shr		ecx, 2
		jz		loc_4691FF
	loc_4691F2:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4691F2
	loc_4691FF:
		and		ebx, 2
		jz		loc_469210
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_469210:
		add		edi, edx
		sub		edi, 320h
		sub		edx, 2
		jge		loc_4691E4
	loc_46921D:
		mov		edx, 2
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_469264
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Ah
		jg		loc_469256
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_2[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		add		edx, ecx
		jmp		loc_469264
	loc_469256:
		sub		edi, 2D00h
		add		esi, 100h
		jmp		loc_4692A0
	loc_469264:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebx, ecx
		shr		ecx, 2
		jz		loc_46927F
	loc_469272:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_469272
	loc_46927F:
		and		ebx, 2
		jz		loc_469290
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_469290:
		add		edi, edx
		add		edx, 2
		sub		edi, 320h
		cmp		edx, 20h
		jl		loc_469264
	loc_4692A0:
		jmp		loc_4696B9
	loc_4692A5:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_4692EC
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_4692DE
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_4692EC
	loc_4692DE:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_469323
	loc_4692EC:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46930B
		mov		ax, [esi+2]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_469318
	loc_46930B:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46930B
	loc_469318:
		sub		edi, 320h
		sub		edx, 2
		jge		loc_4692EC
	loc_469323:
		mov		edx, 10h
	loc_469328:
		cmp		edi, gpBufEnd
		jb		loc_469338
		add		esi, 20h
		add		edi, 20h
		jmp		loc_46934A
	loc_469338:
		mov		ecx, 8
	loc_46933D:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46933D
	loc_46934A:
		sub		edi, 320h
		dec		edx
		jnz		loc_469328
		jmp		loc_4696B9
	loc_469358:
		mov		edx, 1Eh
		mov		eax, edi
		sub		eax, gpBufEnd
		jb		loc_46939F
		add		eax, 3FFh
		shr		eax, 8
		cmp		eax, 2Dh
		jg		loc_469391
		mov		ecx, WorldTbl3x16[eax*4]
		mov		eax, ecx
		add		esi, WorldTbl17_1[ecx]
		shl		eax, 6
		lea		eax, [eax+eax*2]
		shr		ecx, 1
		sub		edi, eax
		sub		edx, ecx
		jmp		loc_46939F
	loc_469391:
		sub		edi, 3000h
		add		esi, 120h
		jmp		loc_4693D8
	loc_46939F:
		mov		ecx, 20h
		sub		ecx, edx
		mov		ebx, ecx
		shr		ecx, 2
		jz		loc_4693BA
	loc_4693AD:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4693AD
	loc_4693BA:
		and		ebx, 2
		jz		loc_4693CB
		mov		ax, [esi]
		add		esi, 4
		mov		[edi], ax
		add		edi, 2
	loc_4693CB:
		sub		edi, 320h
		add		edi, edx
		sub		edx, 2
		jge		loc_46939F
	loc_4693D8:
		mov		edx, 10h
	loc_4693DD:
		cmp		edi, gpBufEnd
		jb		loc_4693ED
		add		esi, 20h
		add		edi, 20h
		jmp		loc_4693FF
	loc_4693ED:
		mov		ecx, 8
	loc_4693F2:
		mov		eax, [esi]
		add		esi, 4
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4693F2
	loc_4693FF:
		sub		edi, 320h
		dec		edx
		jnz		loc_4693DD
		jmp		loc_4696B9
	loc_46940D:
		mov		edx, 20h
		xor		eax, eax
	loc_469414:
		cmp		edi, gpBufEnd
		jb		loc_469424
		add		esi, 20h
		add		edi, 20h
		jmp		loc_469431
	loc_469424:
		mov		ecx, 8
	loc_469429:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_469429
	loc_469431:
		sub		edi, 320h
		dec		edx
		jnz		loc_469414
		jmp		loc_4696B9
	loc_46943F:
		mov		ecx, 20h
	loc_469444:
		push	ecx
		mov		edx, 20h
	loc_46944A:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		loc_469490
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		loc_469463
		add		esi, eax
		add		edi, eax
		jmp		loc_46948A
	loc_469463:
		mov		ecx, eax
		add		esi, ecx
		xor		eax, eax
		shr		ecx, 1
		jnb		loc_469474
		mov		[edi], al
		inc		edi
		test	ecx, ecx
		jz		loc_46948A
	loc_469474:
		shr		ecx, 1
		jnb		loc_469482
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_46948A
	loc_469482:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_469482
	loc_46948A:
		test	edx, edx
		jz		loc_469498
		jmp		loc_46944A
	loc_469490:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		loc_46944A
	loc_469498:
		pop		ecx
		sub		edi, 320h
		dec		ecx
		jnz		loc_469444
		jmp		loc_4696B9
	loc_4694A7:
		mov		edx, 1Eh
		xor		eax, eax
	loc_4694AE:
		cmp		edi, gpBufEnd
		jb		loc_4694C0
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_4694E0
	loc_4694C0:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4694D8
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4694E0
	loc_4694D8:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4694D8
	loc_4694E0:
		sub		edi, 320h
		test	edx, edx
		jz		loc_4694EF
		sub		edx, 2
		jmp		loc_4694AE
	loc_4694EF:
		mov		edx, 2
	loc_4694F4:
		cmp		edi, gpBufEnd
		jb		loc_469506
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_469526
	loc_469506:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46951E
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_469526
	loc_46951E:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46951E
	loc_469526:
		sub		edi, 320h
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_4694F4
		jmp		loc_4696B9
	loc_469539:
		mov		edx, 1Eh
		xor		eax, eax
	loc_469540:
		cmp		edi, gpBufEnd
		jb		loc_469554
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_469572
	loc_469554:
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_46956A
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_469572
	loc_46956A:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_46956A
	loc_469572:
		sub		edi, 320h
		test	edx, edx
		jz		loc_469583
		add		edi, edx
		sub		edx, 2
		jmp		loc_469540
	loc_469583:
		mov		edx, 2
	loc_469588:
		cmp		edi, gpBufEnd
		jb		loc_46959C
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_4695BA
	loc_46959C:
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_4695B2
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_4695BA
	loc_4695B2:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4695B2
	loc_4695BA:
		sub		edi, 320h
		add		edi, edx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_469588
		jmp		loc_4696B9
	loc_4695CF:
		mov		edx, 1Eh
		xor		eax, eax
	loc_4695D6:
		cmp		edi, gpBufEnd
		jb		loc_4695E8
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		jmp		loc_469608
	loc_4695E8:
		add		edi, edx
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_469600
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_469608
	loc_469600:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_469600
	loc_469608:
		sub		edi, 320h
		test	edx, edx
		jz		loc_469617
		sub		edx, 2
		jmp		loc_4695D6
	loc_469617:
		mov		edx, 10h
	loc_46961C:
		cmp		edi, gpBufEnd
		jb		loc_46962C
		add		esi, 20h
		add		edi, 20h
		jmp		loc_469639
	loc_46962C:
		mov		ecx, 8
	loc_469631:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_469631
	loc_469639:
		sub		edi, 320h
		dec		edx
		jnz		loc_46961C
		jmp		loc_4696B9
	loc_469644:
		mov		edx, 1Eh
		xor		eax, eax
	loc_46964B:
		cmp		edi, gpBufEnd
		jb		loc_46965F
		add		esi, 20h
		sub		esi, edx
		add		edi, 20h
		sub		edi, edx
		jmp		loc_46967D
	loc_46965F:
		mov		ecx, 20h
		sub		ecx, edx
		shr		ecx, 2
		jnb		loc_469675
		mov		[edi], ax
		add		edi, 2
		test	ecx, ecx
		jz		loc_46967D
	loc_469675:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_469675
	loc_46967D:
		sub		edi, 320h
		test	edx, edx
		jz		loc_46968E
		add		edi, edx
		sub		edx, 2
		jmp		loc_46964B
	loc_46968E:
		mov		edx, 10h
	loc_469693:
		cmp		edi, gpBufEnd
		jb		loc_4696A3
		add		esi, 20h
		add		edi, 20h
		jmp		loc_4696B0
	loc_4696A3:
		mov		ecx, 8
	loc_4696A8:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4696A8
	loc_4696B0:
		sub		edi, 320h
		dec		edx
		jnz		loc_469693
	loc_4696B9:
		pop		esi
		pop		edi
		pop		edx
		pop		ebx
		retn
	}
}

__declspec(naked) void world_draw_black_tile(BYTE *pBuff)
{
	__asm {
		push	ebx
		push	edx
		push	edi
		push	esi
		mov		edi, ecx
		mov		edx, 1Eh
		mov		ebx, 1
		xor		eax, eax
	loc_4696D0:
		add		edi, edx
		mov		ecx, ebx
	loc_4696D4:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4696D4
		add		edi, edx
		sub		edi, 340h
		test	edx, edx
		jz		loc_4696EE
		sub		edx, 2
		inc		ebx
		jmp		loc_4696D0
	loc_4696EE:
		mov		edx, 2
		mov		ebx, 0Fh
	loc_4696F8:
		add		edi, edx
		mov		ecx, ebx
	loc_4696FC:
		mov		[edi], eax
		add		edi, 4
		dec		ecx
		jnz		loc_4696FC
		add		edi, edx
		sub		edi, 340h
		dec		ebx
		add		edx, 2
		cmp		edx, 20h
		jnz		loc_4696F8
		pop		esi
		pop		edi
		pop		edx
		pop		ebx
		retn
	}
}

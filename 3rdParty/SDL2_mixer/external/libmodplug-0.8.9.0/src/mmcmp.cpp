/*
 * This source code is public domain.
 *
 * Handles unpacking of Powerpack PP20
 * Authors: Olivier Lapicque <olivierl@jps.net>
*/

#include "stdafx.h"
#include "sndfile.h"

#ifdef MMCMP_SUPPORT
BOOL PP20_Unpack(LPCBYTE *ppMemFile, LPDWORD pdwMemLength);

#pragma pack(1)
typedef struct MMCMPFILEHEADER
{
	char id[8];	// "ziRCONia"
	WORD hdrsize;
} MMCMPFILEHEADER, *LPMMCMPFILEHEADER;

typedef struct MMCMPHEADER
{
	WORD version;
	WORD nblocks;
	DWORD filesize;
	DWORD blktable;
	BYTE glb_comp;
	BYTE fmt_comp;
} MMCMPHEADER, *LPMMCMPHEADER;

typedef struct MMCMPBLOCK
{
	DWORD unpk_size;
	DWORD pk_size;
	DWORD xor_chk;
	WORD sub_blk;
	WORD flags;
	WORD tt_entries;
	USHORT num_bits;
} MMCMPBLOCK, *LPMMCMPBLOCK;

typedef struct MMCMPSUBBLOCK
{
	DWORD unpk_pos;
	DWORD unpk_size;
} MMCMPSUBBLOCK, *LPMMCMPSUBBLOCK;
#pragma pack()

// make sure of structure sizes
typedef int chk_MMCMPFILEHEADER[(sizeof(struct MMCMPFILEHEADER) == 10) * 2 - 1];
typedef int chk_MMCMPHEADER[(sizeof(struct MMCMPHEADER) == 14) * 2 - 1];
typedef int chk_MMCMPBLOCK[(sizeof(struct MMCMPBLOCK) == 20) * 2 - 1];
typedef int chk_MMCMPSUBBLOCK[(sizeof(struct MMCMPSUBBLOCK) == 8) * 2 - 1];

#define MMCMP_COMP		0x0001
#define MMCMP_DELTA		0x0002
#define MMCMP_16BIT		0x0004
#define MMCMP_STEREO	0x0100
#define MMCMP_ABS16		0x0200
#define MMCMP_ENDIAN	0x0400

typedef struct MMCMPBITBUFFER
{
	UINT bitcount;
	DWORD bitbuffer;
	LPCBYTE pSrc;
	LPCBYTE pEnd;

	DWORD GetBits(UINT nBits);
} MMCMPBITBUFFER;


DWORD MMCMPBITBUFFER::GetBits(UINT nBits)
//---------------------------------------
{
	DWORD d;
	if (!nBits) return 0;
	while (bitcount < 24)
	{
		bitbuffer |= ((pSrc < pEnd) ? *pSrc++ : 0) << bitcount;
		bitcount += 8;
	}
	d = bitbuffer & ((1 << nBits) - 1);
	bitbuffer >>= nBits;
	bitcount -= nBits;
	return d;
}

//#define MMCMP_LOG

#ifdef MMCMP_LOG
extern void Log(LPCSTR s, ...);
#endif

static const DWORD MMCMP8BitCommands[8] =
{
	0x01, 0x03,	0x07, 0x0F,	0x1E, 0x3C,	0x78, 0xF8
};

static const UINT MMCMP8BitFetch[8] =
{
	3, 3, 3, 3, 2, 1, 0, 0
};

static const DWORD MMCMP16BitCommands[16] =
{
	0x01, 0x03,	0x07, 0x0F,	0x1E, 0x3C,	0x78, 0xF0,
	0x1F0, 0x3F0, 0x7F0, 0xFF0, 0x1FF0, 0x3FF0, 0x7FF0, 0xFFF0
};

static const UINT MMCMP16BitFetch[16] =
{
	4, 4, 4, 4, 3, 2, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};


static void swap_mfh(LPMMCMPFILEHEADER fh)
{
	fh->hdrsize = bswapLE16(fh->hdrsize);
}

static void swap_mmh(LPMMCMPHEADER mh)
{
	mh->version = bswapLE16(mh->version);
	mh->nblocks = bswapLE16(mh->nblocks);
	mh->filesize = bswapLE32(mh->filesize);
	mh->blktable = bswapLE32(mh->blktable);
}

static void swap_block (LPMMCMPBLOCK blk)
{
	blk->unpk_size = bswapLE32(blk->unpk_size);
	blk->pk_size = bswapLE32(blk->pk_size);
	blk->xor_chk = bswapLE32(blk->xor_chk);
	blk->sub_blk = bswapLE16(blk->sub_blk);
	blk->flags = bswapLE16(blk->flags);
	blk->tt_entries = bswapLE16(blk->tt_entries);
	blk->num_bits = bswapLE16(blk->num_bits);
}

static void swap_subblock (LPMMCMPSUBBLOCK sblk)
{
	sblk->unpk_pos = bswapLE32(sblk->unpk_pos);
	sblk->unpk_size = bswapLE32(sblk->unpk_size);
}


BOOL MMCMP_Unpack(LPCBYTE *ppMemFile, LPDWORD pdwMemLength)
//---------------------------------------------------------
{
	DWORD dwMemLength;
	LPCBYTE lpMemFile;
	LPBYTE pBuffer;
	LPMMCMPFILEHEADER pmfh;
	LPMMCMPHEADER pmmh;
	const DWORD *pblk_table;
	DWORD dwFileSize;
	BYTE tmp0[32], tmp1[32];

	if (PP20_Unpack(ppMemFile, pdwMemLength))
	{
		return TRUE;
	}

	dwMemLength = *pdwMemLength;
	lpMemFile = *ppMemFile;
	if ((dwMemLength < 256) || (!lpMemFile)) return FALSE;
	memcpy(tmp0, lpMemFile, 24);
	pmfh = (LPMMCMPFILEHEADER)(tmp0);
	pmmh = (LPMMCMPHEADER)(tmp0+10);
	swap_mfh(pmfh);
	swap_mmh(pmmh);

	if ((memcmp(pmfh->id,"ziRCONia",8) != 0) || (pmfh->hdrsize < 14)
	 || (!pmmh->nblocks) || (pmmh->filesize < 16) || (pmmh->filesize > 0x8000000)
	 || (pmmh->blktable >= dwMemLength) || (pmmh->blktable + 4*pmmh->nblocks > dwMemLength)) return FALSE;
	dwFileSize = pmmh->filesize;
	if ((pBuffer = (LPBYTE)GlobalAllocPtr(GHND, (dwFileSize + 31) & ~15)) == NULL) return FALSE;
	pblk_table = (const DWORD *)(lpMemFile+pmmh->blktable);
	for (UINT nBlock=0; nBlock<pmmh->nblocks; nBlock++)
	{
		DWORD dwMemPos = bswapLE32(pblk_table[nBlock]);
		DWORD dwSubPos;
		LPMMCMPBLOCK pblk;
		LPMMCMPSUBBLOCK psubblk;

		if (dwMemPos >= dwMemLength - 20) break;
		memcpy(tmp1, lpMemFile+dwMemPos, 28);
		pblk = (LPMMCMPBLOCK)(tmp1);
		psubblk = (LPMMCMPSUBBLOCK)(tmp1+20);
		swap_block(pblk);
		swap_subblock(psubblk);

		if (dwMemPos + 20 + pblk->sub_blk*8 >= dwMemLength) break;
		dwSubPos = dwMemPos + 20;
		dwMemPos += 20 + pblk->sub_blk*8;
#ifdef MMCMP_LOG
		Log("block %d: flags=%04X sub_blocks=%d", nBlock, (UINT)pblk->flags, (UINT)pblk->sub_blk);
		Log(" pksize=%d unpksize=%d", pblk->pk_size, pblk->unpk_size);
		Log(" tt_entries=%d num_bits=%d\n", pblk->tt_entries, pblk->num_bits);
#endif
		// Data is not packed
		if (!(pblk->flags & MMCMP_COMP))
		{
			for (UINT i=0; i<pblk->sub_blk; i++)
			{
				if ((psubblk->unpk_pos >= dwFileSize) ||
					(psubblk->unpk_size >= dwFileSize) ||
					(psubblk->unpk_size > dwFileSize - psubblk->unpk_pos) ||
					psubblk->unpk_size > dwMemLength - dwMemPos) break;
#ifdef MMCMP_LOG
				Log("  Unpacked sub-block %d: offset %d, size=%d\n", i, psubblk->unpk_pos, psubblk->unpk_size);
#endif
				memcpy(pBuffer+psubblk->unpk_pos, lpMemFile+dwMemPos, psubblk->unpk_size);
				dwMemPos += psubblk->unpk_size;
				memcpy(tmp1+20,lpMemFile+dwSubPos+i*8,8);
				swap_subblock(psubblk);
			}
		} else
		// Data is 16-bit packed
		if (pblk->flags & MMCMP_16BIT && pblk->num_bits < 16)
		{
			MMCMPBITBUFFER bb;
			LPWORD pDest = (LPWORD)(pBuffer + psubblk->unpk_pos);
			DWORD dwSize = psubblk->unpk_size >> 1;
			DWORD dwPos = 0;
			UINT numbits = pblk->num_bits;
			UINT subblk = 0, oldval = 0;

			if (dwSize * 2 > dwFileSize-psubblk->unpk_pos ||
				psubblk->unpk_pos > dwMemLength-dwMemPos)
				break;

#ifdef MMCMP_LOG
			Log("  16-bit block: pos=%d size=%d ", psubblk->unpk_pos, psubblk->unpk_size);
			if (pblk->flags & MMCMP_DELTA) Log("DELTA ");
			if (pblk->flags & MMCMP_ABS16) Log("ABS16 ");
			Log("\n");
#endif
			bb.bitcount = 0;
			bb.bitbuffer = 0;
			bb.pSrc = lpMemFile+dwMemPos+pblk->tt_entries;
			bb.pEnd = lpMemFile+dwMemPos+pblk->pk_size;
			if (bb.pEnd > lpMemFile+dwMemLength)
				bb.pEnd = lpMemFile+dwMemLength;
			while (subblk < pblk->sub_blk)
			{
				UINT newval = 0x10000;
				DWORD d = bb.GetBits(numbits+1);

				if ((psubblk->unpk_pos >= dwFileSize) ||
					(psubblk->unpk_size >= dwFileSize) ||
					(psubblk->unpk_size > dwFileSize - psubblk->unpk_pos))
					dwPos = dwSize;

				if (d >= MMCMP16BitCommands[numbits])
				{
					UINT nFetch = MMCMP16BitFetch[numbits];
					UINT newbits = bb.GetBits(nFetch) + ((d - MMCMP16BitCommands[numbits]) << nFetch);
					if (newbits != numbits)
					{
						numbits = newbits & 0x0F;
					} else
					{
						if ((d = bb.GetBits(4)) == 0x0F)
						{
							if (bb.GetBits(1)) break;
							newval = 0xFFFF;
						} else
						{
							newval = 0xFFF0 + d;
						}
					}
				} else
				{
					newval = d;
				}
				if (newval < 0x10000 && dwPos < dwSize)
				{
					newval = (newval & 1) ? (UINT)(-(LONG)((newval+1) >> 1)) : (UINT)(newval >> 1);
					if (pblk->flags & MMCMP_DELTA)
					{
						newval += oldval;
						oldval = newval;
					} else
					if (!(pblk->flags & MMCMP_ABS16))
					{
						newval ^= 0x8000;
					}
					WORD swapped = (WORD)newval;
					pDest[dwPos++] = bswapLE16(swapped);
				}
				if (dwPos >= dwSize)
				{
					subblk++;
					memcpy(tmp1+20,lpMemFile+dwSubPos+subblk*8,8);
					swap_subblock(psubblk);
					dwPos = 0;
					dwSize = psubblk->unpk_size >> 1;
					if ( psubblk->unpk_pos >= dwFileSize ||
					 	dwSize * 2 > dwFileSize ) {
						break;
					}
					pDest = (LPWORD)(pBuffer + psubblk->unpk_pos);
				}
			}
		} else if (pblk->num_bits < 8)
		// Data is 8-bit packed
		{
			MMCMPBITBUFFER bb;
			LPBYTE pDest = pBuffer + psubblk->unpk_pos;
			DWORD dwSize = psubblk->unpk_size;
			DWORD dwPos = 0;
			UINT numbits = pblk->num_bits;
			UINT subblk = 0, oldval = 0;
			LPCBYTE ptable = lpMemFile+dwMemPos;

			if (dwSize > dwFileSize-psubblk->unpk_pos ||
				psubblk->unpk_pos > dwMemLength-dwMemPos)
				break;

			bb.bitcount = 0;
			bb.bitbuffer = 0;
			bb.pSrc = lpMemFile+dwMemPos+pblk->tt_entries;
			bb.pEnd = lpMemFile+dwMemPos+pblk->pk_size;
			if (bb.pEnd > lpMemFile+dwMemLength)
				bb.pEnd = lpMemFile+dwMemLength;	
			while (subblk < pblk->sub_blk)
			{
				UINT newval = 0x100;
				DWORD d = bb.GetBits(numbits+1);

				if ((psubblk->unpk_pos >= dwFileSize) ||
					(psubblk->unpk_size >= dwFileSize) ||
					(psubblk->unpk_size > dwFileSize - (psubblk->unpk_pos)))
					dwPos = dwSize;

				if (d >= MMCMP8BitCommands[numbits])
				{
					UINT nFetch = MMCMP8BitFetch[numbits];
					UINT newbits = bb.GetBits(nFetch) + ((d - MMCMP8BitCommands[numbits]) << nFetch);
					if (newbits != numbits)
					{
						numbits = newbits & 0x07;
					} else
					{
						if ((d = bb.GetBits(3)) == 7)
						{
							if (bb.GetBits(1)) break;
							newval = 0xFF;
						} else
						{
							newval = 0xF8 + d;
						}
					}
				} else
				{
					newval = d;
				}
				if (newval < 0x100 && dwPos < dwSize && dwMemPos < dwMemLength - newval)
				{
					int n = ptable[newval];
					if (pblk->flags & MMCMP_DELTA)
					{
						n += oldval;
						oldval = n;
					}
					pDest[dwPos++] = (BYTE)n;
				}
				if (dwPos >= dwSize)
				{
					subblk++;
					memcpy(tmp1+20,lpMemFile+dwSubPos+subblk*8,8);
					swap_subblock(psubblk);
					dwPos = 0;
					dwSize = psubblk->unpk_size;
					if ( psubblk->unpk_pos >= dwFileSize ||
					 	dwSize > dwFileSize )
						break;
					pDest = pBuffer + psubblk->unpk_pos;
				}
			}
		} else
		{
			GlobalFreePtr(pBuffer);
			return FALSE;
		}
	}
	*ppMemFile = pBuffer;
	*pdwMemLength = dwFileSize;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
// PowerPack PP20 Unpacker
//

typedef struct _PPBITBUFFER
{
	UINT bitcount;
	ULONG bitbuffer;
	LPCBYTE pStart;
	LPCBYTE pSrc;

	ULONG GetBits(UINT n);
} PPBITBUFFER;


ULONG PPBITBUFFER::GetBits(UINT n)
{
	ULONG result = 0;

	for (UINT i=0; i<n; i++)
	{
		if (!bitcount)
		{
			bitcount = 8;
			if (pSrc != pStart) pSrc--;
			bitbuffer = *pSrc;
		}
		result = (result<<1) | (bitbuffer&1);
		bitbuffer >>= 1;
		bitcount--;
	}
	return result;
}


static VOID PP20_DoUnpack(const BYTE *pSrc, UINT nSrcLen, BYTE *pDst, UINT nDstLen)
{
	PPBITBUFFER BitBuffer;
	ULONG nBytesLeft;

	BitBuffer.pStart = pSrc;
	BitBuffer.pSrc = pSrc + nSrcLen - 4;
	BitBuffer.bitbuffer = 0;
	BitBuffer.bitcount = 0;
	BitBuffer.GetBits(pSrc[nSrcLen-1]);
	nBytesLeft = nDstLen;
	while (nBytesLeft > 0)
	{
		if (!BitBuffer.GetBits(1))
		{
			UINT n = 1;
			while (n < nBytesLeft)
			{
				UINT code = BitBuffer.GetBits(2);
				n += code;
				if (code != 3) break;
			}
			for (UINT i=0; i<n; i++)
			{
				pDst[nBytesLeft - 1] = (BYTE)BitBuffer.GetBits(8);
				if (!--nBytesLeft) break;
			}
			if (!nBytesLeft) break;
		}
		{
			UINT n = BitBuffer.GetBits(2)+1;
			UINT nbits = pSrc[n-1];
			UINT nofs;
			if (n==4)
			{
				nofs = BitBuffer.GetBits( (BitBuffer.GetBits(1)) ? nbits : 7 );
				while (n < nBytesLeft)
				{
					UINT code = BitBuffer.GetBits(3);
					n += code;
					if (code != 7) break;
				}
			} else
			{
				nofs = BitBuffer.GetBits(nbits);
			}
			for (UINT i=0; i<=n; i++)
			{
				pDst[nBytesLeft-1] = (nBytesLeft+nofs < nDstLen) ? pDst[nBytesLeft+nofs] : 0;
				if (!--nBytesLeft) break;
			}
		}
	}
}


BOOL PP20_Unpack(LPCBYTE *ppMemFile, LPDWORD pdwMemLength)
{
	DWORD dwMemLength = *pdwMemLength;
	LPCBYTE lpMemFile = *ppMemFile;
	DWORD dwDstLen;
	LPBYTE pBuffer;

	if ((!lpMemFile) || (dwMemLength < 256) || (memcmp(lpMemFile,"PP20",4) != 0)) return FALSE;
	dwDstLen = (lpMemFile[dwMemLength-4]<<16) | (lpMemFile[dwMemLength-3]<<8) | (lpMemFile[dwMemLength-2]);
	//Log("PP20 detected: Packed length=%d, Unpacked length=%d\n", dwMemLength, dwDstLen);
	if ((dwDstLen < 512) || (dwDstLen > 0x400000) || (dwDstLen > 16*dwMemLength)) return FALSE;
	if ((pBuffer = (LPBYTE)GlobalAllocPtr(GHND, (dwDstLen + 31) & ~15)) == NULL) return FALSE;
	PP20_DoUnpack(lpMemFile+4, dwMemLength-4, pBuffer, dwDstLen);
	*ppMemFile = pBuffer;
	*pdwMemLength = dwDstLen;
	return TRUE;
}
#endif /* MMCMP_SUPPORT */

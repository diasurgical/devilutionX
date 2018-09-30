//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int WorldBoolFlag = 0;
unsigned int gdwCurrentMask = 0;
// char world_4B3264 = 0;
unsigned char *gpCelFrame = NULL;
unsigned int *gpDrawMask = NULL;
// char world_4B326D[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

unsigned int RightMask[32] =
{
	0xEAAAAAAA, 0xF5555555, 0xFEAAAAAA, 0xFF555555, 0xFFEAAAAA, 0xFFF55555, 0xFFFEAAAA, 0xFFFF5555,
	0xFFFFEAAA, 0xFFFFF555, 0xFFFFFEAA, 0xFFFFFF55, 0xFFFFFFEA, 0xFFFFFFF5, 0xFFFFFFFE, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

unsigned int LeftMask[32] =
{
	0xAAAAAAAB, 0x5555555F, 0xAAAAAABF, 0x555555FF, 0xAAAAABFF, 0x55555FFF, 0xAAAABFFF, 0x5555FFFF,
	0xAAABFFFF, 0x555FFFFF, 0xAABFFFFF, 0x55FFFFFF, 0xABFFFFFF, 0x5FFFFFFF, 0xBFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

unsigned int WallMask[32] =
{
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555
};

int WorldTbl3x16[48] =
{
	0, 0, 0,
	4, 4, 4,
	8, 8, 8,
	12, 12, 12,
	16, 16, 16,
	20, 20, 20,
	24, 24, 24,
	28, 28, 28,
	32, 32, 32,
	36, 36, 36,
	40, 40, 40,
	44, 44, 44,
	48, 48, 48,
	52, 52, 52,
	56, 56, 56,
	60, 60, 60
};

// slope/angle tables, left and right
int WorldTbl17_1[17] = { 0, 4, 8, 16, 24, 36, 48, 64, 80, 100, 120, 144, 168, 196, 224, 256, 288 };
int WorldTbl17_2[17] = { 0, 32, 60, 88, 112, 136, 156, 176, 192, 208, 220, 232, 240, 248, 252, 256, 288 };

/*
 32x32 arch types
 add 8 if light index is 0

	|-| 0x8 (0)
	|-|

	/\  0x9 (1)
	\/

	 /| 0xA (2)
	 \|

	|\  0xB (3)
	|/

	|-| 0xC (4)
	 \|

	|-| 0xD (5)
	|/
*/

void __fastcall drawTopArchesUpperScreen(unsigned char *pbDst)
{
	unsigned char *tmp_pbDst; // edi MAPDST
	unsigned char *l_tbl; // ebx
	unsigned char *pdung_cels; // esi MAPDST
	short cel_type_16; // ax MAPDST
	signed int xx_32; // ebp MAPDST
	signed int yy_32; // edx MAPDST
	unsigned int dung_and80; // eax MAPDST
	unsigned int chk_sh_and; // ecx MAPDST
	unsigned int n_draw_shift; // ecx MAPDST
	unsigned int x_minus; // ecx MAPDST
	unsigned int y_minus; // ecx MAPDST
	signed int i; // edx MAPDST
	signed int j; // ecx MAPDST

	gpCelFrame = (unsigned char *)speed_cel_frame_num_from_light_index_frame_num;
	tmp_pbDst = pbDst;
	if ( !(_BYTE)light_table_index )
	{
		if ( level_cel_block & 0x8000 )
			level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
							+ (unsigned short)(level_cel_block & 0xF000);
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
		goto LABEL_11;
	}
	if ( (_BYTE)light_table_index != lightmax )
	{
		if ( !(level_cel_block & 0x8000) )
		{
			pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			l_tbl = (unsigned char *)&pLightTbl[256 * light_table_index];
			cel_type_16 = (unsigned char)(level_cel_block >> 12);
			switch ( cel_type_16 )
			{
				case 0: // upper (top transparent), with lighting
/*
	|--|
	|  |
	|--|
*/
					i = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						j = 8;
						do
						{
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							tmp_pbDst[3] = l_tbl[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
						tmp_pbDst -= 800;
						if ( tmp_pbDst < gpBufEnd )
							break;
						j = 8;
						do
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[2] = l_tbl[pdung_cels[2]];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					break;
				case 1: // upper (top transparent), with lighting
/*
	 /\
	(  )
	 \/
*/
					WorldBoolFlag = (unsigned char)pbDst & 1;
					xx_32 = 32;
					do
					{
						yy_32 = 32;
						do
						{
							while ( 1 )
							{
								dung_and80 = (unsigned char)*pdung_cels++;
								if ( (dung_and80 & 0x80u) == 0 )
									break;
								_LOBYTE(dung_and80) = -(char)dung_and80;
								tmp_pbDst += dung_and80;
								yy_32 -= dung_and80;
								if ( !yy_32 )
									goto LABEL_67;
							}
							if ( tmp_pbDst < gpBufEnd )
								return;
							if ( ((unsigned char)tmp_pbDst & 1) == WorldBoolFlag )
							{
								chk_sh_and = dung_and80 >> 1;
								if ( dung_and80 & 1 )
								{
									++pdung_cels;
									++tmp_pbDst;
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
							}
							else
							{
								chk_sh_and = dung_and80 >> 1;
								if ( dung_and80 & 1 )
								{
									*tmp_pbDst++ = l_tbl[*pdung_cels++];
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
							}
							yy_32 -= dung_and80;
						}
						while ( yy_32 );
LABEL_67:
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						tmp_pbDst -= 800;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 2: // upper (top transparent), with lighting
/*
	 /-|
	(  |
	 \-|
*/
					WorldBoolFlag = 0;
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						tmp_pbDst += xx_32;
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							pdung_cels += x_minus & 2;
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								++pdung_cels;
								++tmp_pbDst;
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
						}
						else
						{
							pdung_cels += x_minus & 2;
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								*tmp_pbDst++ = l_tbl[*pdung_cels++];
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 2;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								tmp_pbDst += yy_32;
								y_minus = 32 - yy_32;
								WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
								if ( WorldBoolFlag )
								{
									pdung_cels += y_minus & 2;
									chk_sh_and = y_minus >> 1;
									if ( y_minus & 1 )
									{
										++pdung_cels;
										++tmp_pbDst;
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[0] = l_tbl[pdung_cels[0]];
												tmp_pbDst[2] = l_tbl[pdung_cels[2]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
									else
									{
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[1] = l_tbl[pdung_cels[1]];
												tmp_pbDst[3] = l_tbl[pdung_cels[3]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
								}
								else
								{
									pdung_cels += y_minus & 2;
									chk_sh_and = y_minus >> 1;
									if ( y_minus & 1 )
									{
										*tmp_pbDst++ = l_tbl[*pdung_cels++];
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[1] = l_tbl[pdung_cels[1]];
												tmp_pbDst[3] = l_tbl[pdung_cels[3]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
									else
									{
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[0] = l_tbl[pdung_cels[0]];
												tmp_pbDst[2] = l_tbl[pdung_cels[2]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
								}
								tmp_pbDst -= 800;
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
					}
					break;
				case 3: // upper (top transparent), with lighting
/*
	|-\
	|  )
	|-/
*/
					WorldBoolFlag = 0;
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								++pdung_cels;
								++tmp_pbDst;
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							pdung_cels += (unsigned char)pdung_cels & 2;
						}
						else
						{
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								*tmp_pbDst++ = l_tbl[*pdung_cels++];
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							pdung_cels += (unsigned char)pdung_cels & 2;
						}
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 2;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								y_minus = 32 - yy_32;
								WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
								if ( WorldBoolFlag )
								{
									chk_sh_and = y_minus >> 1;
									if ( y_minus & 1 )
									{
										++pdung_cels;
										++tmp_pbDst;
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[0] = l_tbl[pdung_cels[0]];
												tmp_pbDst[2] = l_tbl[pdung_cels[2]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
									else
									{
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[1] = l_tbl[pdung_cels[1]];
												tmp_pbDst[3] = l_tbl[pdung_cels[3]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
									pdung_cels += (unsigned char)pdung_cels & 2;
								}
								else
								{
									chk_sh_and = y_minus >> 1;
									if ( y_minus & 1 )
									{
										*tmp_pbDst++ = l_tbl[*pdung_cels++];
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[1] = l_tbl[pdung_cels[1]];
												tmp_pbDst[3] = l_tbl[pdung_cels[3]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
									else
									{
										n_draw_shift = chk_sh_and >> 1;
										if ( chk_sh_and & 1 )
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											pdung_cels += 2;
											tmp_pbDst += 2;
										}
										if ( (_BYTE)n_draw_shift )
										{
											do
											{
												tmp_pbDst[0] = l_tbl[pdung_cels[0]];
												tmp_pbDst[2] = l_tbl[pdung_cels[2]];
												pdung_cels += 4;
												tmp_pbDst += 4;
												--n_draw_shift;
											}
											while ( n_draw_shift );
										}
									}
									pdung_cels += (unsigned char)pdung_cels & 2;
								}
								tmp_pbDst = &tmp_pbDst[yy_32 - 800];
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
					}
					break;
				case 4: // upper (top transparent), with lighting
/*
	  /|
	 / |
	|__|
*/
					WorldBoolFlag = 0;
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						tmp_pbDst += xx_32;
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							pdung_cels += x_minus & 2;
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								++pdung_cels;
								++tmp_pbDst;
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
						}
						else
						{
							pdung_cels += x_minus & 2;
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								*tmp_pbDst++ = l_tbl[*pdung_cels++];
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							i = 8;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								j = 8;
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
								tmp_pbDst -= 800;
								if ( tmp_pbDst < gpBufEnd )
									break;
								j = 8;
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
								tmp_pbDst -= 800;
								--i;
							}
							while ( i );
							return;
						}
					}
					break;
				default: // upper (top transparent), with lighting
/*
	|\
	| \
	|__|
*/
					WorldBoolFlag = 0;
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								++pdung_cels;
								++tmp_pbDst;
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							pdung_cels += (unsigned char)pdung_cels & 2;
						}
						else
						{
							chk_sh_and = x_minus >> 1;
							if ( x_minus & 1 )
							{
								*tmp_pbDst++ = l_tbl[*pdung_cels++];
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								if ( (_BYTE)n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							pdung_cels += (unsigned char)pdung_cels & 2;
						}
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							i = 8;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								j = 8;
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
								tmp_pbDst -= 800;
								if ( tmp_pbDst < gpBufEnd )
									break;
								j = 8;
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
								tmp_pbDst -= 800;
								--i;
							}
							while ( i );
							return;
						}
					}
					break;
			}
			return;
		}
		pdung_cels = (unsigned char *)pSpeedCels
				   + *(_DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (unsigned char)(level_cel_block >> 12);
LABEL_11:

		switch ( cel_type_16 )
		{
			case 8: // upper (top transparent), without lighting
/*
	|--|
	|  |
	|--|
*/
				i = 16;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
						break;
					j = 8;
					do
					{
						tmp_pbDst[1] = pdung_cels[1];
						tmp_pbDst[3] = pdung_cels[3];
						pdung_cels += 4;
						tmp_pbDst += 4;
						--j;
					}
					while ( j );
					tmp_pbDst -= 800;
					if ( tmp_pbDst < gpBufEnd )
						break;
					j = 8;
					do
					{
						tmp_pbDst[0] = pdung_cels[0];
						tmp_pbDst[2] = pdung_cels[2];
						pdung_cels += 4;
						tmp_pbDst += 4;
						--j;
					}
					while ( j );
					tmp_pbDst -= 800;
					--i;
				}
				while ( i );
				break;
			case 9: // upper (top transparent), without lighting
/*
	 /\
	(  )
	 \/
*/
				WorldBoolFlag = (unsigned char)pbDst & 1;
				yy_32 = 32;
LABEL_251:
				xx_32 = 32;
				while ( 1 )
				{
					while ( 1 )
					{
						dung_and80 = (unsigned char)*pdung_cels++;
						if ( (dung_and80 & 0x80u) == 0 )
							break;
						_LOBYTE(dung_and80) = -(char)dung_and80;
						tmp_pbDst += dung_and80;
						xx_32 -= dung_and80;
						if ( !xx_32 )
						{
LABEL_271:
							WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
							tmp_pbDst -= 800;
							if ( !--yy_32 )
								return;
							goto LABEL_251;
						}
					}
					xx_32 -= dung_and80;
					if ( tmp_pbDst < gpBufEnd )
						return;
					if ( ((unsigned char)tmp_pbDst & 1) == WorldBoolFlag )
					{
						chk_sh_and = dung_and80 >> 1;
						if ( !(dung_and80 & 1) )
							goto LABEL_258;
						++pdung_cels;
						++tmp_pbDst;
						if ( chk_sh_and )
						{
LABEL_265:
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = pdung_cels[0];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = pdung_cels[0];
									tmp_pbDst[2] = pdung_cels[2];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
							goto LABEL_268;
						}
					}
					else
					{
						chk_sh_and = dung_and80 >> 1;
						if ( !(dung_and80 & 1) )
							goto LABEL_265;
						*tmp_pbDst++ = *pdung_cels++;
						if ( chk_sh_and )
						{
LABEL_258:
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = pdung_cels[1];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = pdung_cels[1];
									tmp_pbDst[3] = pdung_cels[3];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
							goto LABEL_268;
						}
					}
LABEL_268:
					if ( !xx_32 )
						goto LABEL_271;
				}
				break;
			case 10: // upper (top transparent), without lighting
/*
	 /-|
	(  |
	 \-|
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					tmp_pbDst += xx_32;
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						n_draw_shift = x_minus >> 2;
						if ( x_minus & 2 )
						{
							tmp_pbDst[1] = pdung_cels[3];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								tmp_pbDst[1] = pdung_cels[1];
								tmp_pbDst[3] = pdung_cels[3];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
					}
					else
					{
						n_draw_shift = x_minus >> 2;
						if ( x_minus & 2 )
						{
							tmp_pbDst[0] = pdung_cels[2];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								tmp_pbDst[0] = pdung_cels[0];
								tmp_pbDst[2] = pdung_cels[2];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						yy_32 = 2;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							tmp_pbDst += yy_32;
							y_minus = 32 - yy_32;
							WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
							if ( WorldBoolFlag )
							{
								n_draw_shift = y_minus >> 2;
								if ( y_minus & 2 )
								{
									tmp_pbDst[1] = pdung_cels[3];
									pdung_cels += 4;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = pdung_cels[1];
										tmp_pbDst[3] = pdung_cels[3];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							else
							{
								n_draw_shift = y_minus >> 2;
								if ( y_minus & 2 )
								{
									tmp_pbDst[0] = pdung_cels[2];
									pdung_cels += 4;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = pdung_cels[0];
										tmp_pbDst[2] = pdung_cels[2];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
							}
							tmp_pbDst -= 800;
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
				}
				break;
			case 11: // upper (top transparent), without lighting
/*
	|-\
	|  )
	|-/
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[1] = pdung_cels[1];
							tmp_pbDst[3] = pdung_cels[3];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( x_minus & 2 )
						{
							tmp_pbDst[1] = pdung_cels[1];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
					}
					else
					{
						for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[0] = pdung_cels[0];
							tmp_pbDst[2] = pdung_cels[2];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( x_minus & 2 )
						{
							tmp_pbDst[0] = pdung_cels[0];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						yy_32 = 2;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							y_minus = 32 - yy_32;
							WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
							if ( WorldBoolFlag )
							{
								for ( n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift )
								{
									tmp_pbDst[1] = pdung_cels[1];
									tmp_pbDst[3] = pdung_cels[3];
									pdung_cels += 4;
									tmp_pbDst += 4;
								}
								if ( x_minus & 2 ) /// BUGFIX: change to `y_minus & 2`
								{
									tmp_pbDst[1] = pdung_cels[1];
									pdung_cels += 4;
									tmp_pbDst += 2;
								}
							}
							else
							{
								for ( n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift )
								{
									tmp_pbDst[0] = pdung_cels[0];
									tmp_pbDst[2] = pdung_cels[2];
									pdung_cels += 4;
									tmp_pbDst += 4;
								}
								if ( x_minus & 2 ) /// BUGFIX: change to `y_minus & 2`
								{
									tmp_pbDst[0] = pdung_cels[0];
									pdung_cels += 4;
									tmp_pbDst += 2;
								}
							}
							tmp_pbDst = &tmp_pbDst[yy_32 - 800];
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
				}
				break;
			case 12: // upper (top transparent), without lighting
/*
	  /|
	 / |
	|__|
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					tmp_pbDst += xx_32;
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						n_draw_shift = x_minus >> 2;
						if ( x_minus & 2 )
						{
							tmp_pbDst[1] = pdung_cels[3];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift)
						{
							do
							{
								tmp_pbDst[1] = pdung_cels[1];
								tmp_pbDst[3] = pdung_cels[3];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
					}
					else
					{
						n_draw_shift = x_minus >> 2;
						if ( x_minus & 2 )
						{
							tmp_pbDst[0] = pdung_cels[2];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								tmp_pbDst[0] = pdung_cels[0];
								tmp_pbDst[2] = pdung_cels[2];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						i = 8;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							j = 8;
							do
							{
								tmp_pbDst[1] = pdung_cels[1];
								tmp_pbDst[3] = pdung_cels[3];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
							tmp_pbDst -= 800;
							if ( tmp_pbDst < gpBufEnd )
								break;
							j = 8;
							do
							{
								tmp_pbDst[0] = pdung_cels[0];
								tmp_pbDst[2] = pdung_cels[2];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
							tmp_pbDst -= 800;
							--i;
						}
						while ( i );
						return;
					}
				}
				break;
			default: // upper (top transparent), without lighting
/*
	|\
	| \
	|__|
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[1] = pdung_cels[1];
							tmp_pbDst[3] = pdung_cels[3];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[1] = pdung_cels[1];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
					}
					else
					{
						for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[0] = pdung_cels[0];
							tmp_pbDst[2] = pdung_cels[2];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[0] = pdung_cels[0];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						i = 8;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							j = 8;
							do
							{
								tmp_pbDst[1] = pdung_cels[1];
								tmp_pbDst[3] = pdung_cels[3];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
							tmp_pbDst -= 800;
							if ( tmp_pbDst < gpBufEnd )
								break;
							j = 8;
							do
							{
								tmp_pbDst[0] = pdung_cels[0];
								tmp_pbDst[2] = pdung_cels[2];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
							tmp_pbDst -= 800;
							--i;
						}
						while ( i );
						return;
					}
				}
				break;
		}
		return;
	}
	if ( level_cel_block & 0x8000 )
		level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
						+ (unsigned short)(level_cel_block & 0xF000);
	pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
	cel_type_16 = (level_cel_block >> 12) & 7;
	switch ( cel_type_16 )
	{
		case 0: // upper (top transparent), black
/*
	|--|
	|  |
	|--|
*/
			i = 16;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
					break;
				j = 8;
				do
				{
					tmp_pbDst[1] = 0;
					tmp_pbDst[3] = 0;
					tmp_pbDst += 4;
					--j;
				}
				while ( j );
				tmp_pbDst -= 800;
				if ( tmp_pbDst < gpBufEnd )
					break;
				j = 8;
				do
				{
					tmp_pbDst[0] = 0;
					tmp_pbDst[2] = 0;
					tmp_pbDst += 4;
					--j;
				}
				while ( j );
				tmp_pbDst -= 800;
				--i;
			}
			while ( i );
			break;
		case 1: // upper (top transparent), black
/*
	 /\
	(  )
	 \/
*/
			WorldBoolFlag = (unsigned char)pbDst & 1;
			xx_32 = 32;
			while ( 1 )
			{
				yy_32 = 32;
				do
				{
					while ( 1 )
					{
						dung_and80 = (unsigned char)*pdung_cels++;
						if ( (dung_and80 & 0x80u) != 0 )
							break;
						yy_32 -= dung_and80;
						if ( tmp_pbDst < gpBufEnd )
							return;
						pdung_cels += dung_and80;
						if ( ((unsigned char)tmp_pbDst & 1) == WorldBoolFlag )
						{
							chk_sh_and = dung_and80 >> 1;
							if ( !(dung_and80 & 1) )
								goto LABEL_378;
							++tmp_pbDst;
							if ( chk_sh_and )
							{
LABEL_385:
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = 0;
										tmp_pbDst[2] = 0;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
								goto LABEL_388;
							}
						}
						else
						{
							chk_sh_and = dung_and80 >> 1;
							if ( !(dung_and80 & 1) )
								goto LABEL_385;
							*tmp_pbDst++ = 0;
							if ( chk_sh_and )
							{
LABEL_378:
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = 0;
										tmp_pbDst[3] = 0;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
								goto LABEL_388;
							}
						}
LABEL_388:
						if ( !yy_32 )
							goto LABEL_391;
					}
					_LOBYTE(dung_and80) = -(char)dung_and80;
					tmp_pbDst += dung_and80;
					yy_32 -= dung_and80;
				}
				while ( yy_32 );
LABEL_391:
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				tmp_pbDst -= 800;
				if ( !--xx_32 )
					return;
			}
		case 2: // upper (top transparent), black
/*
	 /-|
	(  |
	 \-|
*/
			WorldBoolFlag = 0;
			for ( xx_32 = 30; ; xx_32 -= 2 )
			{
				if ( tmp_pbDst < gpBufEnd )
					return;
				tmp_pbDst += xx_32;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[1] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[0] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift)
					{
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
					break;
			}
			yy_32 = 2;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
					break;
				tmp_pbDst += yy_32;
				y_minus = 32 - yy_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = y_minus >> 2;
					if ( y_minus & 2 )
					{
						tmp_pbDst[1] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = y_minus >> 2;
					if ( y_minus & 2 )
					{
						tmp_pbDst[0] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift)
					{
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst -= 800;
				yy_32 += 2;
			}
			while ( yy_32 != 32 );
			break;
		case 3: // upper (top transparent), black
/*
	|-\
	|  )
	|-/
*/
			WorldBoolFlag = 0;
			for ( xx_32 = 30; ; xx_32 -= 2 )
			{
				if ( tmp_pbDst < gpBufEnd )
					return;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[1] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[0] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
					break;
				tmp_pbDst += xx_32;
			}
			yy_32 = 2;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
					break;
				y_minus = 32 - yy_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = y_minus >> 2;
					if ( y_minus & 2 )
					{
						tmp_pbDst[1] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = y_minus >> 2;
					if ( y_minus & 2 )
					{
						tmp_pbDst[0] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst = &tmp_pbDst[yy_32 - 800];
				yy_32 += 2;
			}
			while ( yy_32 != 32 );
			break;
		case 4: // upper (top transparent), black
/*
	  /|
	 / |
	|__|
*/
			WorldBoolFlag = 0;
			for ( xx_32 = 30; ; xx_32 -= 2 )
			{
				if ( tmp_pbDst < gpBufEnd )
					return;
				tmp_pbDst += xx_32;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[1] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[0] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
					break;
			}
			i = 8;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
					break;
				j = 8;
				do
				{
					tmp_pbDst[1] = 0;
					tmp_pbDst[3] = 0;
					tmp_pbDst += 4;
					--j;
				}
				while ( j );
				tmp_pbDst -= 800;
				if ( tmp_pbDst < gpBufEnd )
					break;
				j = 8;
				do
				{
					tmp_pbDst[0] = 0;
					tmp_pbDst[2] = 0;
					tmp_pbDst += 4;
					--j;
				}
				while ( j );
				tmp_pbDst -= 800;
				--i;
			}
			while ( i );
			break;
		default: // upper (top transparent), black
/*
	|\
	| \
	|__|
*/
			WorldBoolFlag = 0;
			for ( xx_32 = 30; ; xx_32 -= 2 )
			{
				if ( tmp_pbDst < gpBufEnd )
					return;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[1] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[0] = 0;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
					break;
				tmp_pbDst += xx_32;
			}
			i = 8;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
					break;
				j = 8;
				do
				{
					tmp_pbDst[1] = 0;
					tmp_pbDst[3] = 0;
					tmp_pbDst += 4;
					--j;
				}
				while ( j );
				tmp_pbDst -= 800;
				if ( tmp_pbDst < gpBufEnd )
					break;
				j = 8;
				do
				{
					tmp_pbDst[0] = 0;
					tmp_pbDst[2] = 0;
					tmp_pbDst += 4;
					--j;
				}
				while ( j );
				tmp_pbDst -= 800;
				--i;
			}
			while ( i );
			break;
	}
}

void __fastcall drawBottomArchesUpperScreen(unsigned char *pbDst, unsigned int *pMask)
{
	unsigned char *tmp_pbDst; // edi MAPDST
	unsigned char *pdung_cels; // esi MAPDST
	short cel_type_16; // ax MAPDST
	int xx_32; // edx MAPDST
	unsigned int left_shift; // edx MAPDST
	int yy_32; // edx MAPDST
	int dung_and80; // eax MAPDST
	int and80_i; // ecx MAPDST
	unsigned int n_draw_shift; // ecx MAPDST
	signed int i; // ecx MAPDST
	unsigned char *_EBX;

	gpCelFrame = (unsigned char *)speed_cel_frame_num_from_light_index_frame_num;
	tmp_pbDst = pbDst;
	gpDrawMask = pMask;
	if ( !(_BYTE)light_table_index )
	{
		if ( level_cel_block & 0x8000 )
			level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
							+ (unsigned short)(level_cel_block & 0xF000);
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
LABEL_12:
		switch ( cel_type_16 )
		{
			case 8: // upper (bottom transparent), without lighting
/*
	|--|
	|  |
	|--|
*/
				xx_32 = 32;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
						break;
					left_shift = *gpDrawMask;
					i = 32;
					do
					{
						if ( left_shift & 0x80000000 )
							tmp_pbDst[0] = pdung_cels[0];
						left_shift *= 2;
						++pdung_cels;
						++tmp_pbDst;
						--i;
					}
					while ( i );
					tmp_pbDst -= 800;
					--gpDrawMask;
					--xx_32;
				}
				while ( xx_32 );
				break;
			case 9: // upper (bottom transparent), without lighting
/*
	 /\
	(  )
	 \/
*/
				xx_32 = 32;
				do
				{
					gdwCurrentMask = *gpDrawMask;
					yy_32 = 32;
					do
					{
						while ( 1 )
						{
							dung_and80 = (unsigned char)*pdung_cels++;
							if ( (dung_and80 & 0x80u) == 0 )
								break;
							_LOBYTE(dung_and80) = -(char)dung_and80;
							tmp_pbDst += dung_and80;
							if ( dung_and80 & 0x1F )
								gdwCurrentMask <<= dung_and80 & 0x1F;
							yy_32 -= dung_and80;
							if ( !yy_32 )
								goto LABEL_129;
						}
						yy_32 -= dung_and80;
						if ( tmp_pbDst < gpBufEnd )
							return;
						left_shift = gdwCurrentMask;
						and80_i = dung_and80;
						do
						{
							if ( left_shift & 0x80000000 )
								tmp_pbDst[0] = pdung_cels[0];
							left_shift *= 2;
							++pdung_cels;
							++tmp_pbDst;
							--and80_i;
						}
						while ( and80_i );
						gdwCurrentMask = left_shift;
					}
					while ( yy_32 );
LABEL_129:
					tmp_pbDst -= 800;
					--gpDrawMask;
					--xx_32;
				}
				while ( xx_32 );
				break;
			case 10: // upper (bottom transparent), without lighting
/*
	 /-|
	(  |
	 \-|
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					tmp_pbDst += xx_32;
					n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
					if ( (32 - xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						yy_32 = 2;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							tmp_pbDst += yy_32;
							n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
							if ( (32 - yy_32) & 2 )
							{
								*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
								pdung_cels += 4;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
							tmp_pbDst -= 800;
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
				}
				break;
			case 11: // upper (bottom transparent), without lighting
/*
	|-\
	|  )
	|-/
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						yy_32 = 2;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							for ( n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift )
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
							}
							if ( (32 - (_BYTE)yy_32) & 2 )
							{
								*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 2;
							}
							tmp_pbDst = &tmp_pbDst[yy_32 - 800];
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
				}
				break;
			case 12: // upper (bottom transparent), without lighting
/*
	  /|
	 / |
	|__|
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					tmp_pbDst += xx_32;
					n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
					if ( (32 - xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						gpDrawMask -= 16;
						yy_32 = 16;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = pdung_cels[0];
								left_shift *= 2;
								++pdung_cels;
								++tmp_pbDst;
								--i;
							}
							while ( i );
							tmp_pbDst -= 800;
							--gpDrawMask;
							--yy_32;
						}
						while ( yy_32 );
						return;
					}
				}
				break;
			default: // upper (bottom transparent), without lighting
/*
	|\
	| \
	|__|
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						gpDrawMask -= 16;
						yy_32 = 16;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = pdung_cels[0];
								left_shift *= 2;
								++pdung_cels;
								++tmp_pbDst;
								--i;
							}
							while ( i );
							pdung_cels += (unsigned char)pdung_cels & 2;
							tmp_pbDst -= 800;
							--gpDrawMask;
							--yy_32;
						}
						while ( yy_32 );
						return;
					}
				}
				break;
		}
		return;
	}
	if ( (_BYTE)light_table_index != lightmax )
	{
		if ( !(level_cel_block & 0x8000) )
		{
			pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			_EBX = (unsigned char *)&pLightTbl[256 * light_table_index];
			cel_type_16 = (unsigned char)(level_cel_block >> 12);
			switch ( cel_type_16 )
			{
				case 0: // upper (bottom transparent), with lighting
/*
	|--|
	|  |
	|--|
*/
					xx_32 = 32;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						left_shift = *gpDrawMask;
						i = 32;
						do
						{
							if ( left_shift & 0x80000000 )
								tmp_pbDst[0] = _EBX[pdung_cels[0]];
							left_shift *= 2;
							++pdung_cels;
							++tmp_pbDst;
							--i;
						}
						while ( i );
						tmp_pbDst -= 800;
						--gpDrawMask;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 1: // upper (bottom transparent), with lighting
/*
	 /\
	(  )
	 \/
*/
					xx_32 = 32;
					do
					{
						gdwCurrentMask = *gpDrawMask;
						yy_32 = 32;
						do
						{
							while ( 1 )
							{
								dung_and80 = (unsigned char)*pdung_cels++;
								if ( (dung_and80 & 0x80u) == 0 )
									break;
								_LOBYTE(dung_and80) = -(char)dung_and80;
								tmp_pbDst += dung_and80;
								if ( dung_and80 & 0x1F )
									gdwCurrentMask <<= dung_and80 & 0x1F;
								yy_32 -= dung_and80;
								if ( !yy_32 )
									goto LABEL_50;
							}
							yy_32 -= dung_and80;
							if ( tmp_pbDst < gpBufEnd )
								return;
							and80_i = dung_and80;
							left_shift = gdwCurrentMask;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = _EBX[pdung_cels[0]];
								left_shift *= 2;
								++pdung_cels;
								++tmp_pbDst;
								--and80_i;
							}
							while ( and80_i );
							gdwCurrentMask = left_shift;
						}
						while ( yy_32 );
LABEL_50:
						tmp_pbDst -= 800;
						--gpDrawMask;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 2: // upper (bottom transparent), with lighting
/*
	 /-|
	(  |
	 \-|
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						tmp_pbDst += xx_32;
						n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
						if ( (32 - xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[2]];
							tmp_pbDst[1] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								tmp_pbDst[0] = _EBX[pdung_cels[0]];
								tmp_pbDst[1] = _EBX[pdung_cels[1]];
								tmp_pbDst[2] = _EBX[pdung_cels[2]];
								tmp_pbDst[3] = _EBX[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 2;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								tmp_pbDst += yy_32;
								n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
								if ( (32 - yy_32) & 2 )
								{
									tmp_pbDst[0] = _EBX[pdung_cels[2]];
									tmp_pbDst[1] = _EBX[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = _EBX[pdung_cels[0]];
										tmp_pbDst[1] = _EBX[pdung_cels[1]];
										tmp_pbDst[2] = _EBX[pdung_cels[2]];
										tmp_pbDst[3] = _EBX[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
								tmp_pbDst -= 800;
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
					}
					break;
				case 3: // upper (bottom transparent), with lighting
/*
	|-\
	|  )
	|-/
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							tmp_pbDst[2] = _EBX[pdung_cels[2]];
							tmp_pbDst[3] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 2;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								for ( n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift )
								{
									tmp_pbDst[0] = _EBX[pdung_cels[0]];
									tmp_pbDst[1] = _EBX[pdung_cels[1]];
									tmp_pbDst[2] = _EBX[pdung_cels[2]];
									tmp_pbDst[3] = _EBX[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
								}
								if ( (32 - (_BYTE)yy_32) & 2 )
								{
									tmp_pbDst[0] = _EBX[pdung_cels[0]];
									tmp_pbDst[1] = _EBX[pdung_cels[1]];
									pdung_cels += 4;
									tmp_pbDst += 2;
								}
								tmp_pbDst = &tmp_pbDst[yy_32 - 800];
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
					}
					break;
				case 4: // upper (bottom transparent), with lighting
/*
	  /|
	 / |
	|__|
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						tmp_pbDst += xx_32;
						n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
						if ( (32 - xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[2]];
							tmp_pbDst[1] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								tmp_pbDst[0] = _EBX[pdung_cels[0]];
								tmp_pbDst[1] = _EBX[pdung_cels[1]];
								tmp_pbDst[2] = _EBX[pdung_cels[2]];
								tmp_pbDst[3] = _EBX[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							gpDrawMask -= 16;
							yy_32 = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								left_shift = *gpDrawMask;
								i = 32;
								pdung_cels += (unsigned char)pdung_cels & 2;
								do
								{
									if ( left_shift & 0x80000000 )
										tmp_pbDst[0] = _EBX[pdung_cels[0]];
									left_shift *= 2;
									++pdung_cels;
									++tmp_pbDst;
									--i;
								}
								while ( i );
								tmp_pbDst -= 800;
								--gpDrawMask;
								--yy_32;
							}
							while ( yy_32 );
							return;
						}
					}
					break;
				default: // upper (bottom transparent), with lighting
/*
	|\
	| \
	|__|
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							tmp_pbDst[2] = _EBX[pdung_cels[2]];
							tmp_pbDst[3] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							gpDrawMask -= 16;
							yy_32 = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								left_shift = *gpDrawMask;
								i = 32;
								do
								{
									if ( left_shift & 0x80000000 )
										tmp_pbDst[0] = _EBX[pdung_cels[0]];
									left_shift *= 2;
									++pdung_cels;
									++tmp_pbDst;
									--i;
								}
								while ( i );
								pdung_cels += (unsigned char)pdung_cels & 2;
								tmp_pbDst -= 800;
								--gpDrawMask;
								--yy_32;
							}
							while ( yy_32 );
							return;
						}
					}
					break;
			}
			return;
		}
		pdung_cels = (unsigned char *)pSpeedCels
				   + *(_DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (unsigned char)(level_cel_block >> 12);
		goto LABEL_12;
	}
	if ( level_cel_block & 0x8000 )
		level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
						+ (unsigned short)(level_cel_block & 0xF000);
	pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
	cel_type_16 = (level_cel_block >> 12) & 7;
	switch ( cel_type_16 )
	{
		case 0: // upper (bottom transparent), black
/*
	|--|
	|  |
	|--|
*/
			xx_32 = 32;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
					break;
				left_shift = *gpDrawMask;
				i = 32;
				do
				{
					if ( left_shift & 0x80000000 )
						tmp_pbDst[0] = 0;
					left_shift *= 2;
					++tmp_pbDst;
					--i;
				}
				while ( i );
				tmp_pbDst -= 800;
				--gpDrawMask;
				--xx_32;
			}
			while ( xx_32 );
			break;
		case 1: // upper (bottom transparent), black
/*
	 /\
	(  )
	 \/
*/
			xx_32 = 32;
			do
			{
				gdwCurrentMask = *gpDrawMask;
				yy_32 = 32;
				do
				{
					while ( 1 )
					{
						dung_and80 = (unsigned char)*pdung_cels++;
						if ( (dung_and80 & 0x80u) == 0 )
							break;
						_LOBYTE(dung_and80) = -(char)dung_and80;
						tmp_pbDst += dung_and80;
						if ( dung_and80 & 0x1F )
							gdwCurrentMask <<= dung_and80 & 0x1F;
						yy_32 -= dung_and80;
						if ( !yy_32 )
							goto LABEL_208;
					}
					yy_32 -= dung_and80;
					if ( tmp_pbDst < gpBufEnd )
						return;
					left_shift = gdwCurrentMask;
					and80_i = dung_and80;
					pdung_cels += dung_and80;
					do
					{
						if ( left_shift & 0x80000000 )
							tmp_pbDst[0] = 0;
						left_shift *= 2;
						++tmp_pbDst;
						--and80_i;
					}
					while ( and80_i );
					gdwCurrentMask = left_shift;
				}
				while ( yy_32 );
LABEL_208:
				tmp_pbDst -= 800;
				--gpDrawMask;
				--xx_32;
			}
			while ( xx_32 );
			break;
		case 2: // upper (bottom transparent), black
/*
	 /-|
	(  |
	 \-|
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					yy_32 = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						tmp_pbDst += yy_32;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ( (32 - yy_32) & 2 )
						{
							*(_WORD *)tmp_pbDst = 0;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								*(_DWORD *)tmp_pbDst = 0;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst -= 800;
						yy_32 += 2;
					}
					while ( yy_32 != 32 );
					return;
				}
				xx_32 -= 2;
			}
			break;
		case 3: // upper (bottom transparent), black
/*
	|-\
	|  )
	|-/
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					yy_32 = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ( (32 - yy_32) & 2 )
						{
							*(_WORD *)tmp_pbDst = 0;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								*(_DWORD *)tmp_pbDst = 0;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst = &tmp_pbDst[yy_32 - 800];
						yy_32 += 2;
					}
					while ( yy_32 != 32 );
					return;
				}
				tmp_pbDst += xx_32;
				xx_32 -= 2;
			}
			break;
		case 4: // upper (bottom transparent), black
/*
	  /|
	 / |
	|__|
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					gpDrawMask -= 16;
					yy_32 = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						left_shift = *gpDrawMask;
						i = 32;
						do
						{
							if ( left_shift & 0x80000000 )
								tmp_pbDst[0] = 0;
							left_shift *= 2;
							++tmp_pbDst;
							--i;
						}
						while ( i );
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					return;
				}
				xx_32 -= 2;
			}
			break;
		default: // upper (bottom transparent), black
/*
	|\
	| \
	|__|
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					gpDrawMask -= 16;
					yy_32 = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						left_shift = *gpDrawMask;
						i = 32;
						do
						{
							if ( left_shift & 0x80000000 )
								tmp_pbDst[0] = 0;
							left_shift *= 2;
							++tmp_pbDst;
							--i;
						}
						while ( i );
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					return;
				}
				tmp_pbDst += xx_32;
				xx_32 -= 2;
			}
			break;
	}
}

void __fastcall drawUpperScreen(unsigned char *pbDst)
{
	unsigned char *tmp_pbDst; // edi MAPDST
	unsigned char *l_tbl; // ebx
	unsigned char *pdung_cels; // esi MAPDST
	short cel_type_16; // ax MAPDST
	char base_4; // cl MAPDST
	char x_minus; // cl MAPDST
	char y_minus; // cl
	signed int xx_32; // ebp MAPDST
	signed int yy_32; // edx MAPDST
	unsigned int dung_and80; // eax MAPDST
	unsigned int chk_sh_and; // ecx MAPDST
	unsigned int n_draw_shift; // ecx MAPDST
	signed int i; // edx MAPDST
	signed int j; // ecx MAPDST

	if ( cel_transparency_active )
	{
		if ( !arch_draw_type )
		{
			drawTopArchesUpperScreen(pbDst);
			return;
		}
		if ( arch_draw_type == 1 )
		{
			if ( block_lvid[level_piece_id] == 1 || block_lvid[level_piece_id] == 3 )
			{
				drawBottomArchesUpperScreen(pbDst, &LeftMask[31]);
				return;
			}
		}
		if ( arch_draw_type == 2 )
		{
			if ( block_lvid[level_piece_id] == 2 || block_lvid[level_piece_id] == 3 )
			{
				drawBottomArchesUpperScreen(pbDst, &RightMask[31]);
				return;
			}
		}
	}
	gpCelFrame = (unsigned char *)speed_cel_frame_num_from_light_index_frame_num;
	tmp_pbDst = pbDst;
	if ( !(_BYTE)light_table_index )
	{
		if ( level_cel_block & 0x8000 )
			level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
							+ (unsigned short)(level_cel_block & 0xF000);
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
LABEL_22:
		switch ( cel_type_16 )
		{
			case 8: // upper (solid), without lighting
/*
	|--|
	|  |
	|--|
*/
				i = 32;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
						break;
					j = 8;
					do
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
						--j;
					}
					while ( j );
					tmp_pbDst -= 800;
					--i;
				}
				while ( i );
				break;
			case 9: // upper (solid), without lighting
/*
	 /\
	(  )
	 \/
*/
				xx_32 = 32;
				do
				{
					yy_32 = 32;
					do
					{
						while ( 1 )
						{
							dung_and80 = *pdung_cels++;
							if ( (dung_and80 & 0x80u) == 0 )
								break;
							_LOBYTE(dung_and80) = -(char)dung_and80;
							tmp_pbDst += dung_and80;
							yy_32 -= dung_and80;
							if ( !yy_32 )
								goto LABEL_133;
						}
						yy_32 -= dung_and80;
						if ( tmp_pbDst < gpBufEnd )
							return;
						chk_sh_and = dung_and80 >> 1;
						if ( dung_and80 & 1 )
						{
							*tmp_pbDst++ = *pdung_cels++;
							if ( !chk_sh_and )
								continue;
						}
						n_draw_shift = chk_sh_and >> 1;
						if ( chk_sh_and & 1 )
						{
							*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
							pdung_cels += 2;
							tmp_pbDst += 2;
							if ( !n_draw_shift )
								continue;
						}
						do
						{
							*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
					while ( yy_32 );
LABEL_133:
					tmp_pbDst -= 800;
					--xx_32;
				}
				while ( xx_32 );
				break;
			case 10: // upper (solid), without lighting
/*
	 /-|
	(  |
	 \-|
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					tmp_pbDst += xx_32;
					n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
					if ( (32 - xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						yy_32 = 2;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							tmp_pbDst += yy_32;
							n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
							if ( (32 - yy_32) & 2 )
							{
								*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
								pdung_cels += 4;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
							tmp_pbDst -= 800;
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
				}
				break;
			case 11: // upper (solid), without lighting
/*
	|-\
	|  )
	|-/
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						yy_32 = 2;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							for ( n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift )
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
							}
							if ( (32 - (_BYTE)yy_32) & 2 )
							{
								*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 2;
							}
							tmp_pbDst = &tmp_pbDst[yy_32 - 800];
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
				}
				break;
			case 12: // upper (solid), without lighting
/*
	  /|
	 / |
	|__|
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					tmp_pbDst += xx_32;
					n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
					if ( (32 - xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						i = 16;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							j = 8;
							do
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
							tmp_pbDst -= 800;
							--i;
						}
						while ( i );
						return;
					}
				}
				break;
			default: // upper (solid), without lighting
/*
	|\
	| \
	|__|
*/
				xx_32 = 30;
				while ( tmp_pbDst >= gpBufEnd )
				{
					for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
					if ( xx_32 < 0 )
					{
						i = 16;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
								break;
							j = 8;
							do
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
							tmp_pbDst -= 800;
							--i;
						}
						while ( i );
						return;
					}
				}
				break;
		}
		return;
	}
	if ( (_BYTE)light_table_index != lightmax )
	{
		if ( !(level_cel_block & 0x8000) )
		{
			pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			l_tbl = (unsigned char *)&pLightTbl[256 * light_table_index];
			cel_type_16 = (unsigned short)level_cel_block >> 12;
			switch ( cel_type_16 )
			{
				case 0: // upper (solid), with lighting
/*
	|--|
	|  |
	|--|
*/
					xx_32 = 32;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						base_4 = 32;
						do
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							tmp_pbDst[2] = l_tbl[pdung_cels[2]];
							tmp_pbDst[3] = l_tbl[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
							base_4 -= 4;
						}
						while ( base_4 >= 4 );
						tmp_pbDst -= 800;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 1: // upper (solid), with lighting
/*
	 /\
	(  )
	 \/
*/
					xx_32 = 32;
					do
					{
						yy_32 = 32;
						do
						{
							while ( 1 )
							{
								dung_and80 = *pdung_cels++;
								if ( (dung_and80 & 0x80u) == 0 )
									break;
								_LOBYTE(dung_and80) = -(char)dung_and80;
								tmp_pbDst += dung_and80;
								yy_32 -= dung_and80;
								if ( !yy_32 )
									goto LABEL_58;
							}
							yy_32 -= dung_and80;
							if ( tmp_pbDst < gpBufEnd )
								return;
							for ( base_4 = dung_and80; base_4 >= 4; base_4 -= 4 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								tmp_pbDst[2] = l_tbl[pdung_cels[2]];
								tmp_pbDst[3] = l_tbl[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
							}
							if ( base_4 >= 2 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( base_4 & 1 )
							{
								*tmp_pbDst++ = l_tbl[*pdung_cels++];
							}
						}
						while ( yy_32 );
LABEL_58:
						tmp_pbDst -= 800;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 2: // upper (solid), with lighting
/*
	 /-|
	(  |
	 \-|
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						tmp_pbDst += xx_32;
						x_minus = 32 - xx_32;
						pdung_cels += (32 - (_BYTE)xx_32) & 2;
						if ( (char)(32 - xx_32) >= 4 )
						{
							do
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								tmp_pbDst[2] = l_tbl[pdung_cels[2]];
								tmp_pbDst[3] = l_tbl[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								x_minus -= 4;
							}
							while ( x_minus >= 4 );
						}
						if ( x_minus >= 2 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							pdung_cels += 2;
							tmp_pbDst += 2;
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 2;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								tmp_pbDst += yy_32;
								y_minus = 32 - yy_32;
								pdung_cels += (32 - (_BYTE)yy_32) & 2;
								if ( (char)(32 - yy_32) >= 4 )
								{
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										y_minus -= 4;
									}
									while ( y_minus >= 4 );
								}
								if ( y_minus >= 2 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								tmp_pbDst -= 800;
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
					}
					break;
				case 3: // upper (solid), with lighting
/*
	|-\
	|  )
	|-/
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						for ( base_4 = 32 - xx_32; base_4 >= 4; base_4 -= 4 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							tmp_pbDst[2] = l_tbl[pdung_cels[2]];
							tmp_pbDst[3] = l_tbl[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( base_4 >= 2 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							pdung_cels += 2;
							tmp_pbDst += 2;
						}
						pdung_cels += (unsigned char)pdung_cels & 2;
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 2;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								for ( base_4 = 32 - yy_32; base_4 >= 4; base_4 -= 4 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
								}
								if ( base_4 >= 2 )
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									pdung_cels += 2;
									tmp_pbDst += 2;
								}
								pdung_cels += (unsigned char)pdung_cels & 2;
								tmp_pbDst = &tmp_pbDst[yy_32 - 800];
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
					}
					break;
				case 4: // upper (solid), with lighting
/*
	  /|
	 / |
	|__|
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						tmp_pbDst += xx_32;
						x_minus = 32 - xx_32;
						pdung_cels += (32 - (_BYTE)xx_32) & 2;
						if ( (char)(32 - xx_32) >= 4 )
						{
							do
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								tmp_pbDst[2] = l_tbl[pdung_cels[2]];
								tmp_pbDst[3] = l_tbl[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								x_minus -= 4;
							}
							while ( x_minus >= 4 );
						}
						if ( x_minus >= 2 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							pdung_cels += 2;
							tmp_pbDst += 2;
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								base_4 = 32;
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									base_4 -= 4;
								}
								while ( base_4 >= 4 );
								tmp_pbDst -= 800;
								--yy_32;
							}
							while ( yy_32 );
							return;
						}
					}
					break;
				default: // upper (solid), with lighting
/*
	|\
	| \
	|__|
*/
					xx_32 = 30;
					while ( tmp_pbDst >= gpBufEnd )
					{
						for ( base_4 = 32 - xx_32; base_4 >= 4; base_4 -= 4 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							tmp_pbDst[2] = l_tbl[pdung_cels[2]];
							tmp_pbDst[3] = l_tbl[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( base_4 >= 2 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							pdung_cels += 2;
							tmp_pbDst += 2;
						}
						pdung_cels += (unsigned char)pdung_cels & 2;
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
						if ( xx_32 < 0 )
						{
							yy_32 = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
									break;
								base_4 = 32;
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									base_4 -= 4;
								}
								while ( base_4 >= 4 );
								tmp_pbDst -= 800;
								--yy_32;
							}
							while ( yy_32 );
							return;
						}
					}
					break;
			}
			return;
		}
		pdung_cels = (unsigned char *)pSpeedCels
				   + *(_DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (unsigned short)level_cel_block >> 12;
		goto LABEL_22;
	}
	if ( level_cel_block & 0x8000 )
		level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
						+ (unsigned short)(level_cel_block & 0xF000);
	pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
	cel_type_16 = ((unsigned int)level_cel_block >> 12) & 7;
	switch ( cel_type_16 )
	{
		case 0: // upper (solid), black
/*
	|--|
	|  |
	|--|
*/
			i = 32;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
					break;
				j = 8;
				do
				{
					*(_DWORD *)tmp_pbDst = 0;
					tmp_pbDst += 4;
					--j;
				}
				while ( j );
				tmp_pbDst -= 800;
				--i;
			}
			while ( i );
			break;
		case 1: // upper (solid), black
/*
	 /\
	(  )
	 \/
*/
			xx_32 = 32;
			do
			{
				yy_32 = 32;
				do
				{
					while ( 1 )
					{
						dung_and80 = *pdung_cels++;
						if ( (dung_and80 & 0x80u) == 0 )
							break;
						_LOBYTE(dung_and80) = -(char)dung_and80;
						tmp_pbDst += dung_and80;
						yy_32 -= dung_and80;
						if ( !yy_32 )
							goto LABEL_205;
					}
					yy_32 -= dung_and80;
					if ( tmp_pbDst < gpBufEnd )
						return;
					pdung_cels += dung_and80;
					chk_sh_and = dung_and80 >> 1;
					if ( dung_and80 & 1 )
					{
						*tmp_pbDst++ = 0;
						if ( !chk_sh_and )
							continue;
					}
					n_draw_shift = dung_and80 >> 2;
					if ( chk_sh_and & 1 )
					{
						*(_WORD *)tmp_pbDst = 0;
						tmp_pbDst += 2;
						if ( !n_draw_shift )
							continue;
					}
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				while ( yy_32 );
LABEL_205:
				tmp_pbDst -= 800;
				--xx_32;
			}
			while ( xx_32 );
			break;
		case 2: // upper (solid), black
/*
	 /-|
	(  |
	 \-|
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					yy_32 = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						tmp_pbDst += yy_32;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ( (32 - yy_32) & 2 )
						{
							*(_WORD *)tmp_pbDst = 0;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								*(_DWORD *)tmp_pbDst = 0;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst -= 800;
						yy_32 += 2;
					}
					while ( yy_32 != 32 );
					return;
				}
				xx_32 -= 2;
			}
			break;
		case 3: // upper (solid), black
/*
	|-\
	|  )
	|-/
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					yy_32 = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ( (32 - yy_32) & 2 )
						{
							*(_WORD *)tmp_pbDst = 0;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								*(_DWORD *)tmp_pbDst = 0;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst = &tmp_pbDst[yy_32 - 800];
						yy_32 += 2;
					}
					while ( yy_32 != 32 );
					return;
				}
				tmp_pbDst += xx_32;
				xx_32 -= 2;
			}
			break;
		case 4: // upper (solid), black
/*
	  /|
	 / |
	|__|
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					i = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						j = 8;
						do
						{
							*(_DWORD *)tmp_pbDst = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					return;
				}
				xx_32 -= 2;
			}
			break;
		default: // upper (solid), black
/*
	|\
	| \
	|__|
*/
			xx_32 = 30;
			while ( tmp_pbDst >= gpBufEnd )
			{
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = 0;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = 0;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				if ( !xx_32 )
				{
					i = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
							break;
						j = 8;
						do
						{
							*(_DWORD *)tmp_pbDst = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					return;
				}
				tmp_pbDst += xx_32;
				xx_32 -= 2;
			}
			break;
	}
}

void __fastcall drawTopArchesLowerScreen(unsigned char *pbDst)
{
	unsigned char *tmp_pbDst; // edi MAPDST
	unsigned char *l_tbl; // ebx
	unsigned char *pdung_cels; // esi MAPDST
	short cel_type_16; // ax MAPDST
	signed int tile_42_45; // eax MAPDST
	unsigned int world_tbl; // ecx MAPDST
	int world_192; // eax MAPDST
	unsigned int dung_and80; // eax MAPDST
	unsigned int chk_sh_and; // ecx MAPDST
	int xx_32; // edx MAPDST
	unsigned int x_minus; // ecx MAPDST
	unsigned int n_draw_shift; // ecx MAPDST
	int yy_32; // edx MAPDST
	unsigned int y_minus; // ecx MAPDST
	signed int i; // edx MAPDST
	signed int j; // ecx MAPDST

	gpCelFrame = (unsigned char *)speed_cel_frame_num_from_light_index_frame_num;
	tmp_pbDst = pbDst;
	if ( !(_BYTE)light_table_index )
	{
		if ( level_cel_block & 0x8000 )
			level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
							+ (unsigned short)(level_cel_block & 0xF000);
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
		goto LABEL_11;
	}
	if ( (_BYTE)light_table_index == lightmax )
	{
		if ( level_cel_block & 0x8000 )
			level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
							+ (unsigned short)(level_cel_block & 0xF000);
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = (level_cel_block >> 12) & 7;
		switch ( cel_type_16 )
		{
			case 0: // lower (top transparent), black
/*
	|--|
	|  |
	|--|
*/
				i = 16;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					--i;
				}
				while ( i );
				break;
			case 1: // lower (top transparent), black
/*
	 /\
	(  )
	 \/
*/
				WorldBoolFlag = (unsigned char)pbDst & 1;
				xx_32 = 32;
LABEL_412:
				yy_32 = 32;
				while ( 1 )
				{
					while ( 1 )
					{
						dung_and80 = (unsigned char)*pdung_cels++;
						if ( (dung_and80 & 0x80u) == 0 )
							break;
						_LOBYTE(dung_and80) = -(char)dung_and80;
						tmp_pbDst += dung_and80;
						yy_32 -= dung_and80;
						if ( !yy_32 )
						{
LABEL_433:
							WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
							tmp_pbDst -= 800;
							if ( !--xx_32 )
								return;
							goto LABEL_412;
						}
					}
					yy_32 -= dung_and80;
					if ( tmp_pbDst < gpBufEnd )
					{
						pdung_cels += dung_and80;
						if ( ((unsigned char)tmp_pbDst & 1) == WorldBoolFlag )
						{
							chk_sh_and = dung_and80 >> 1;
							if ( !(dung_and80 & 1) )
								goto LABEL_420;
							++tmp_pbDst;
							if ( chk_sh_and )
							{
LABEL_427:
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = 0;
										tmp_pbDst[2] = 0;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
								goto LABEL_430;
							}
						}
						else
						{
							chk_sh_and = dung_and80 >> 1;
							if ( !(dung_and80 & 1) )
								goto LABEL_427;
							*tmp_pbDst++ = 0;
							if ( chk_sh_and )
							{
LABEL_420:
								n_draw_shift = chk_sh_and >> 1;
								if ( chk_sh_and & 1 )
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[1] = 0;
										tmp_pbDst[3] = 0;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
								goto LABEL_430;
							}
						}
					}
					else
					{
						pdung_cels += dung_and80;
						tmp_pbDst += dung_and80;
					}
LABEL_430:
					if ( !yy_32 )
						goto LABEL_433;
				}
				break;
			case 2: // lower (top transparent), black
/*
	 /-|
	(  |
	 \-|
*/
				WorldBoolFlag = 0;
				for ( xx_32 = 30; ; xx_32 -= 2 )
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						tmp_pbDst += xx_32;
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[1] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst[3] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[0] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst[2] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels = &pdung_cels[-xx_32 + 32];
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					if ( !xx_32 )
						break;
				}
				yy_32 = 2;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						tmp_pbDst += yy_32;
						y_minus = 32 - yy_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							n_draw_shift = y_minus >> 2;
							if ( y_minus & 2 )
							{
								tmp_pbDst[1] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst[3] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = y_minus >> 2;
							if ( y_minus & 2 )
							{
								tmp_pbDst[0] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst[2] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels = &pdung_cels[-yy_32 + 32];
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					yy_32 += 2;
				}
				while ( yy_32 != 32 );
				break;
			case 3: // lower (top transparent), black
/*
	|-\
	|  )
	|-/
*/
				WorldBoolFlag = 0;
				for ( xx_32 = 30; ; xx_32 -= 2 )
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[1] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst[3] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[0] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst[2] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels = &pdung_cels[-xx_32 + 32];
						tmp_pbDst = &tmp_pbDst[-xx_32 + 32];
					}
					tmp_pbDst -= 800;
					if ( !xx_32 )
						break;
					tmp_pbDst += xx_32;
				}
				yy_32 = 2;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						y_minus = 32 - yy_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							n_draw_shift = y_minus >> 2;
							if ( y_minus & 2 )
							{
								tmp_pbDst[1] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst[3] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = y_minus >> 2;
							if ( y_minus & 2 )
							{
								tmp_pbDst[0] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst[2] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels = &pdung_cels[-yy_32 + 32];
						tmp_pbDst = &tmp_pbDst[-yy_32 + 32];
					}
					tmp_pbDst = &tmp_pbDst[yy_32 - 800];
					yy_32 += 2;
				}
				while ( yy_32 != 32 );
				break;
			case 4: // lower (top transparent), black
/*
	  /|
	 / |
	|__|
*/
				WorldBoolFlag = 0;
				for ( xx_32 = 30; ; xx_32 -= 2 )
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						tmp_pbDst += xx_32;
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[1] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst[3] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[0] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst[2] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels = &pdung_cels[-xx_32 + 32];
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					if ( !xx_32 )
						break;
				}
				i = 8;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					--i;
				}
				while ( i );
				break;
			default: // lower (top transparent), black
/*
	|\
	| \
	|__|
*/
				WorldBoolFlag = 0;
				for ( xx_32 = 30; ; xx_32 -= 2 )
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						x_minus = 32 - xx_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[1] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = 0;
									tmp_pbDst[3] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = x_minus >> 2;
							if ( x_minus & 2 )
							{
								tmp_pbDst[0] = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = 0;
									tmp_pbDst[2] = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels = &pdung_cels[-xx_32 + 32];
						tmp_pbDst = &tmp_pbDst[-xx_32 + 32];
					}
					tmp_pbDst -= 800;
					if ( !xx_32 )
						break;
					tmp_pbDst += xx_32;
				}
				i = 8;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[1] = 0;
							tmp_pbDst[3] = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[0] = 0;
							tmp_pbDst[2] = 0;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					--i;
				}
				while ( i );
				break;
		}
		return;
	}
	if ( !(level_cel_block & 0x8000) )
	{
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		l_tbl = (unsigned char *)&pLightTbl[256 * light_table_index];
		cel_type_16 = (unsigned char)(level_cel_block >> 12);
		switch ( cel_type_16 )
		{
			case 0: // lower (top transparent), with lighting
/*
	|--|
	|  |
	|--|
*/
				i = 16;
				do
				{
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							tmp_pbDst[3] = l_tbl[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					if ( tmp_pbDst < gpBufEnd )
					{
						j = 8;
						do
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[2] = l_tbl[pdung_cels[2]];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--j;
						}
						while ( j );
					}
					else
					{
						pdung_cels += 32;
						tmp_pbDst += 32;
					}
					tmp_pbDst -= 800;
					--i;
				}
				while ( i );
				break;
			case 1: // lower (top transparent), with lighting
/*
	 /\
	(  )
	 \/
*/
				WorldBoolFlag = (unsigned char)pbDst & 1;
				xx_32 = 32;
				do
				{
					yy_32 = 32;
					do
					{
						while ( 1 )
						{
							dung_and80 = (unsigned char)*pdung_cels++;
							if ( (dung_and80 & 0x80u) == 0 )
								break;
							_LOBYTE(dung_and80) = -(char)dung_and80;
							tmp_pbDst += dung_and80;
							yy_32 -= dung_and80;
							if ( !yy_32 )
								goto LABEL_69;
						}
						yy_32 -= dung_and80;
						if ( tmp_pbDst < gpBufEnd )
						{
							if ( ((unsigned char)tmp_pbDst & 1) == WorldBoolFlag )
							{
								chk_sh_and = dung_and80 >> 1;
								if ( dung_and80 & 1 )
								{
									++pdung_cels;
									++tmp_pbDst;
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
							}
							else
							{
								chk_sh_and = dung_and80 >> 1;
								if ( dung_and80 & 1 )
								{
									*tmp_pbDst++ = l_tbl[*pdung_cels++];
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
							}
						}
						else
						{
							pdung_cels += dung_and80;
							tmp_pbDst += dung_and80;
						}
					}
					while ( yy_32 );
LABEL_69:
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					tmp_pbDst -= 800;
					--xx_32;
				}
				while ( xx_32 );
				break;
			case 2: // lower (top transparent), with lighting
/*
	 /-|
	(  |
	 \-|
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				if ( pbDst >= gpBufEnd )
				{
					tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
					if ( tile_42_45 > 45 )
					{
						tmp_pbDst = pbDst - 12288;
						pdung_cels += 288;
LABEL_98:
						yy_32 = 2;
						if ( tmp_pbDst >= gpBufEnd )
						{
							tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
							if ( tile_42_45 > 42 )
								return;
							world_tbl = WorldTbl3x16[tile_42_45];
							pdung_cels += WorldTbl17_2[world_tbl >> 2];
							world_192 = 192 * world_tbl;
							world_tbl >>= 1;
							tmp_pbDst -= world_192;
							yy_32 = world_tbl + 2;
							WorldBoolFlag += world_tbl >> 1;
						}
						do
						{
							tmp_pbDst += yy_32;
							y_minus = 32 - yy_32;
							WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
							if ( WorldBoolFlag )
							{
								pdung_cels += y_minus & 2;
								chk_sh_and = y_minus >> 1;
								if ( y_minus & 1 )
								{
									++pdung_cels;
									++tmp_pbDst;
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
							}
							else
							{
								pdung_cels += y_minus & 2;
								chk_sh_and = y_minus >> 1;
								if ( y_minus & 1 )
								{
									*tmp_pbDst++ = l_tbl[*pdung_cels++];
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
							}
							tmp_pbDst -= 800;
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					pdung_cels += WorldTbl17_1[world_tbl >> 2];
					world_192 = 192 * world_tbl;
					world_tbl >>= 1;
					tmp_pbDst -= world_192;
					xx_32 = 30 - world_tbl;
					WorldBoolFlag += world_tbl >> 1;
				}
				do
				{
					tmp_pbDst += xx_32;
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						pdung_cels += x_minus & 2;
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							++pdung_cels;
							++tmp_pbDst;
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels += x_minus & 2;
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							*tmp_pbDst++ = l_tbl[*pdung_cels++];
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
				}
				while ( xx_32 >= 0 );
				goto LABEL_98;
			case 3: // lower (top transparent), with lighting
/*
	|-\
	|  )
	|-/
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				if ( pbDst >= gpBufEnd )
				{
					tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
					if ( tile_42_45 > 45 )
					{
						tmp_pbDst = pbDst - 12288;
						pdung_cels += 288;
LABEL_154:
						yy_32 = 2;
						if ( tmp_pbDst >= gpBufEnd )
						{
							tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
							if ( tile_42_45 > 42 )
								return;
							world_tbl = WorldTbl3x16[tile_42_45];
							pdung_cels += WorldTbl17_2[world_tbl >> 2];
							world_192 = 192 * world_tbl;
							world_tbl >>= 1;
							tmp_pbDst -= world_192;
							yy_32 = world_tbl + 2;
							WorldBoolFlag += world_tbl >> 1;
						}
						do
						{
							y_minus = 32 - yy_32;
							WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
							if ( WorldBoolFlag )
							{
								chk_sh_and = y_minus >> 1;
								if ( y_minus & 1 )
								{
									++pdung_cels;
									++tmp_pbDst;
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								pdung_cels += (unsigned char)pdung_cels & 2;
							}
							else
							{
								chk_sh_and = y_minus >> 1;
								if ( y_minus & 1 )
								{
									*tmp_pbDst++ = l_tbl[*pdung_cels++];
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = l_tbl[pdung_cels[1]];
											tmp_pbDst[3] = l_tbl[pdung_cels[3]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								else
								{
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( (_BYTE)n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = l_tbl[pdung_cels[0]];
											tmp_pbDst[2] = l_tbl[pdung_cels[2]];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
								pdung_cels += (unsigned char)pdung_cels & 2;
							}
							tmp_pbDst = &tmp_pbDst[yy_32 - 800];
							yy_32 += 2;
						}
						while ( yy_32 != 32 );
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					pdung_cels += WorldTbl17_1[world_tbl >> 2];
					world_192 = 192 * world_tbl;
					world_tbl >>= 1;
					tmp_pbDst -= world_192;
					xx_32 = 30 - world_tbl;
					WorldBoolFlag += world_tbl >> 1;
				}
				do
				{
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							++pdung_cels;
							++tmp_pbDst;
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						pdung_cels += (unsigned char)pdung_cels & 2;
					}
					else
					{
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							*tmp_pbDst++ = l_tbl[*pdung_cels++];
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						pdung_cels += (unsigned char)pdung_cels & 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
				}
				while ( xx_32 >= 0 );
				goto LABEL_154;
			case 4: // lower (top transparent), with lighting
/*
	  /|
	 / |
	|__|
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				if ( pbDst >= gpBufEnd )
				{
					tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
					if ( tile_42_45 > 45 )
					{
						tmp_pbDst = pbDst - 12288;
						pdung_cels += 288;
LABEL_210:
						i = 8;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
							{
								j = 8;
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
							}
							else
							{
								pdung_cels += 32;
								tmp_pbDst += 32;
							}
							tmp_pbDst -= 800;
							if ( tmp_pbDst < gpBufEnd )
							{
								j = 8;
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
							}
							else
							{
								pdung_cels += 32;
								tmp_pbDst += 32;
							}
							tmp_pbDst -= 800;
							--i;
						}
						while ( i );
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					pdung_cels += WorldTbl17_1[world_tbl >> 2];
					world_192 = 192 * world_tbl;
					world_tbl >>= 1;
					tmp_pbDst -= world_192;
					xx_32 = 30 - world_tbl;
					WorldBoolFlag += world_tbl >> 1;
				}
				do
				{
					tmp_pbDst += xx_32;
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						pdung_cels += x_minus & 2;
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							++pdung_cels;
							++tmp_pbDst;
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels += x_minus & 2;
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							*tmp_pbDst++ = l_tbl[*pdung_cels++];
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					tmp_pbDst -= 800;
					xx_32 -= 2;
				}
				while ( xx_32 >= 0 );
				goto LABEL_210;
			default: // lower (top transparent), with lighting
/*
	|\
	| \
	|__|
*/
				WorldBoolFlag = 0;
				xx_32 = 30;
				if ( pbDst >= gpBufEnd )
				{
					tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
					if ( tile_42_45 > 45 )
					{
						tmp_pbDst = pbDst - 12288;
						pdung_cels += 288;
LABEL_249:
						i = 8;
						do
						{
							if ( tmp_pbDst < gpBufEnd )
							{
								j = 8;
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
							}
							else
							{
								pdung_cels += 32;
								tmp_pbDst += 32;
							}
							tmp_pbDst -= 800;
							if ( tmp_pbDst < gpBufEnd )
							{
								j = 8;
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--j;
								}
								while ( j );
							}
							else
							{
								pdung_cels += 32;
								tmp_pbDst += 32;
							}
							tmp_pbDst -= 800;
							--i;
						}
						while ( i );
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					pdung_cels += WorldTbl17_1[world_tbl >> 2];
					world_192 = 192 * world_tbl;
					world_tbl >>= 1;
					tmp_pbDst -= world_192;
					xx_32 = 30 - world_tbl;
					WorldBoolFlag += world_tbl >> 1;
				}
				do
				{
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							++pdung_cels;
							++tmp_pbDst;
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						pdung_cels += (unsigned char)pdung_cels & 2;
					}
					else
					{
						chk_sh_and = x_minus >> 1;
						if ( x_minus & 1 )
						{
							*tmp_pbDst++ = l_tbl[*pdung_cels++];
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = l_tbl[pdung_cels[1]];
									tmp_pbDst[3] = l_tbl[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( (_BYTE)n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = l_tbl[pdung_cels[0]];
									tmp_pbDst[2] = l_tbl[pdung_cels[2]];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						pdung_cels += (unsigned char)pdung_cels & 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
				}
				while ( xx_32 >= 0 );
				goto LABEL_249;
		}
		return;
	}
	pdung_cels = (unsigned char *)pSpeedCels + *(_DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
	cel_type_16 = (unsigned char)(level_cel_block >> 12);
LABEL_11:
	switch ( cel_type_16 )
	{
		case 8: // lower (top transparent), without lighting
/*
	|--|
	|  |
	|--|
*/
			i = 16;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
				{
					j = 8;
					do
					{
						tmp_pbDst[1] = pdung_cels[1];
						tmp_pbDst[3] = pdung_cels[3];
						pdung_cels += 4;
						tmp_pbDst += 4;
						--j;
					}
					while ( j );
				}
				else
				{
					pdung_cels += 32;
					tmp_pbDst += 32;
				}
				tmp_pbDst -= 800;
				if ( tmp_pbDst < gpBufEnd )
				{
					j = 8;
					do
					{
						tmp_pbDst[0] = pdung_cels[0];
						tmp_pbDst[2] = pdung_cels[2];
						pdung_cels += 4;
						tmp_pbDst += 4;
						--j;
					}
					while ( j );
				}
				else
				{
					pdung_cels += 32;
					tmp_pbDst += 32;
				}
				tmp_pbDst -= 800;
				--i;
			}
			while ( i );
			break;
		case 9: // lower (top transparent), without lighting
/*
	 /\
	(  )
	 \/
*/
			WorldBoolFlag = (unsigned char)pbDst & 1;
			xx_32 = 32;
			while ( 1 )
			{
				yy_32 = 32;
				do
				{
					while ( 1 )
					{
						dung_and80 = (unsigned char)*pdung_cels++;
						if ( (dung_and80 & 0x80u) != 0 )
							break;
						yy_32 -= dung_and80;
						if ( tmp_pbDst < gpBufEnd )
						{
							if ( ((unsigned char)tmp_pbDst & 1) == WorldBoolFlag )
							{
								chk_sh_and = dung_and80 >> 1;
								if ( !(dung_and80 & 1) )
									goto LABEL_280;
								++pdung_cels;
								++tmp_pbDst;
								if ( chk_sh_and )
								{
LABEL_287:
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[0] = pdung_cels[0];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( n_draw_shift )
									{
										do
										{
											tmp_pbDst[0] = pdung_cels[0];
											tmp_pbDst[2] = pdung_cels[2];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
									goto LABEL_290;
								}
							}
							else
							{
								chk_sh_and = dung_and80 >> 1;
								if ( !(dung_and80 & 1) )
									goto LABEL_287;
								*tmp_pbDst++ = *pdung_cels++;
								if ( chk_sh_and )
								{
LABEL_280:
									n_draw_shift = chk_sh_and >> 1;
									if ( chk_sh_and & 1 )
									{
										tmp_pbDst[1] = pdung_cels[1];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( n_draw_shift )
									{
										do
										{
											tmp_pbDst[1] = pdung_cels[1];
											tmp_pbDst[3] = pdung_cels[3];
											pdung_cels += 4;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
									goto LABEL_290;
								}
							}
						}
						else
						{
							pdung_cels += dung_and80;
							tmp_pbDst += dung_and80;
						}
LABEL_290:
						if ( !yy_32 )
							goto LABEL_293;
					}
					_LOBYTE(dung_and80) = -(char)dung_and80;
					tmp_pbDst += dung_and80;
					yy_32 -= dung_and80;
				}
				while ( yy_32 );
LABEL_293:
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				tmp_pbDst -= 800;
				if ( !--xx_32 )
					return;
			}
		case 10: // lower (top transparent), without lighting
/*
	 /-|
	(  |
	 \-|
*/
			WorldBoolFlag = 0;
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_308:
					yy_32 = 2;
					if ( tmp_pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 42 )
							return;
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_2[world_tbl >> 2];
						world_192 = 192 * world_tbl;
						world_tbl >>= 1;
						tmp_pbDst -= world_192;
						yy_32 = world_tbl + 2;
						WorldBoolFlag += world_tbl >> 1;
					}
					do
					{
						tmp_pbDst += yy_32;
						y_minus = 32 - yy_32;
						WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
						if ( WorldBoolFlag )
						{
							n_draw_shift = y_minus >> 2;
							if ( y_minus & 2 )
							{
								tmp_pbDst[1] = pdung_cels[3];
								pdung_cels += 4;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[1] = pdung_cels[1];
									tmp_pbDst[3] = pdung_cels[3];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							n_draw_shift = y_minus >> 2;
							if ( y_minus & 2 )
							{
								tmp_pbDst[0] = pdung_cels[2];
								pdung_cels += 4;
								tmp_pbDst += 2;
								--n_draw_shift; /// BUGFIX: delete this line
							}
							if ( n_draw_shift )
							{
								do
								{
									tmp_pbDst[0] = pdung_cels[0];
									tmp_pbDst[2] = pdung_cels[2];
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						tmp_pbDst -= 800;
						yy_32 += 2;
					}
					while ( yy_32 != 32 );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				world_192 = 192 * world_tbl;
				world_tbl >>= 1;
				tmp_pbDst -= world_192;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
			}
			do
			{
				tmp_pbDst += xx_32;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[1] = pdung_cels[3];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = pdung_cels[1];
							tmp_pbDst[3] = pdung_cels[3];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[0] = pdung_cels[2];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[0] = pdung_cels[0];
							tmp_pbDst[2] = pdung_cels[2];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst -= 800;
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_308;
		case 11: // lower (top transparent), without lighting
/*
	|-\
	|  )
	|-/
*/
			WorldBoolFlag = 0;
			xx_32 = 30;
			if ( pbDst < gpBufEnd )
				goto LABEL_326;
			tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
			if ( tile_42_45 <= 45 )
			{
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				world_192 = 192 * world_tbl;
				world_tbl >>= 1;
				tmp_pbDst -= world_192;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
				do
				{
LABEL_326:
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
					if ( WorldBoolFlag )
					{
						for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[1] = pdung_cels[1];
							tmp_pbDst[3] = pdung_cels[3];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[1] = pdung_cels[1];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
					}
					else
					{
						for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[0] = pdung_cels[0];
							tmp_pbDst[2] = pdung_cels[2];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[0] = pdung_cels[0];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
				}
				while ( xx_32 >= 0 );
				goto LABEL_336;
			}
			tmp_pbDst = pbDst - 12288;
			pdung_cels += 288;
LABEL_336:
			yy_32 = 2;
			if ( tmp_pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 42 )
					return;
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_2[world_tbl >> 2];
				world_192 = 192 * world_tbl;
				world_tbl >>= 1;
				tmp_pbDst -= world_192;
				yy_32 = world_tbl + 2;
				WorldBoolFlag += world_tbl >> 1;
			}
			do
			{
				y_minus = 32 - yy_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					for ( n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift )
					{
						tmp_pbDst[1] = pdung_cels[1];
						tmp_pbDst[3] = pdung_cels[3];
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)yy_32) & 2 )
					{
						tmp_pbDst[1] = pdung_cels[1];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
				}
				else
				{
					for ( n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift )
					{
						tmp_pbDst[0] = pdung_cels[0];
						tmp_pbDst[2] = pdung_cels[2];
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)yy_32) & 2 )
					{
						tmp_pbDst[0] = pdung_cels[0];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
				}
				tmp_pbDst = &tmp_pbDst[yy_32 - 800];
				yy_32 += 2;
			}
			while ( yy_32 != 32 );
			break;
		case 12: // lower (top transparent), without lighting
/*
	  /|
	 / |
	|__|
*/
			WorldBoolFlag = 0;
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_364:
					i = 8;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								tmp_pbDst[1] = pdung_cels[1];
								tmp_pbDst[3] = pdung_cels[3];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								tmp_pbDst[0] = pdung_cels[0];
								tmp_pbDst[2] = pdung_cels[2];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				world_192 = 192 * world_tbl;
				world_tbl >>= 1;
				tmp_pbDst -= world_192;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
			}
			do
			{
				tmp_pbDst += xx_32;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[1] = pdung_cels[3];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[1] = pdung_cels[1];
							tmp_pbDst[3] = pdung_cels[3];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				else
				{
					n_draw_shift = x_minus >> 2;
					if ( x_minus & 2 )
					{
						tmp_pbDst[0] = pdung_cels[2];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					if ( n_draw_shift )
					{
						do
						{
							tmp_pbDst[0] = pdung_cels[0];
							tmp_pbDst[2] = pdung_cels[2];
							pdung_cels += 4;
							tmp_pbDst += 4;
							--n_draw_shift;
						}
						while ( n_draw_shift );
					}
				}
				tmp_pbDst -= 800;
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_364;
		default: // lower (top transparent), without lighting
/*
	|\
	| \
	|__|
*/
			WorldBoolFlag = 0;
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_389:
					i = 8;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								tmp_pbDst[1] = pdung_cels[1];
								tmp_pbDst[3] = pdung_cels[3];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								tmp_pbDst[0] = pdung_cels[0];
								tmp_pbDst[2] = pdung_cels[2];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				world_192 = 192 * world_tbl;
				world_tbl >>= 1;
				tmp_pbDst -= world_192;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
			}
			do
			{
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((_BYTE)WorldBoolFlag + 1) & 1;
				if ( WorldBoolFlag )
				{
					for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
					{
						tmp_pbDst[1] = pdung_cels[1];
						tmp_pbDst[3] = pdung_cels[3];
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						tmp_pbDst[1] = pdung_cels[1];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
				}
				else
				{
					for ( n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift )
					{
						tmp_pbDst[0] = pdung_cels[0];
						tmp_pbDst[2] = pdung_cels[2];
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						tmp_pbDst[0] = pdung_cels[0];
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
				}
				tmp_pbDst = &tmp_pbDst[xx_32 - 800];
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_389;
	}
}

void __fastcall drawBottomArchesLowerScreen(unsigned char *pbDst, unsigned int *pMask)
{
	unsigned char *tmp_pbDst; // edi MAPDST
	short cel_type_16; // ax MAPDST
	unsigned char *pdung_cels; // esi MAPDST
	int and80_i; // ecx MAPDST
	signed int tile_42_45; // eax MAPDST
	unsigned int world_tbl; // ecx MAPDST
	int xx_32; // ecx MAPDST
	int yy_32; // edx MAPDST
	int dung_and80; // eax MAPDST
	unsigned int left_shift; // edx MAPDST
	signed int i; // edx MAPDST
	unsigned int n_draw_shift; // ecx MAPDST
	unsigned char *_EBX;

	gpCelFrame = (unsigned char *)speed_cel_frame_num_from_light_index_frame_num;
	tmp_pbDst = pbDst;
	gpDrawMask = pMask;
	if ( (_BYTE)light_table_index )
	{
		if ( (_BYTE)light_table_index == lightmax )
		{
			if ( level_cel_block & 0x8000 )
				level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
								+ (unsigned short)(level_cel_block & 0xF000);
			pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			cel_type_16 = (level_cel_block >> 12) & 7;
			switch ( cel_type_16 )
			{
				case 0: // lower (bottom transparent), black
/*
	|--|
	|  |
	|--|
*/
					yy_32 = 32;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = 0;
								left_shift *= 2;
								++tmp_pbDst;
								--i;
							}
							while ( i );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					break;
				case 1: // lower (bottom transparent), black
/*
	 /\
	(  )
	 \/
*/
					xx_32 = 32;
					do
					{
						gdwCurrentMask = *gpDrawMask;
						yy_32 = 32;
						do
						{
							while ( 1 )
							{
								dung_and80 = (unsigned char)*pdung_cels++;
								if ( (dung_and80 & 0x80u) != 0 )
									break;
								yy_32 -= dung_and80;
								if ( tmp_pbDst < gpBufEnd )
								{
									and80_i = dung_and80;
									pdung_cels += dung_and80;
									left_shift = gdwCurrentMask;
									do
									{
										if ( left_shift & 0x80000000 )
											tmp_pbDst[0] = 0;
										left_shift *= 2;
										++tmp_pbDst;
										--and80_i;
									}
									while ( and80_i );
									gdwCurrentMask = left_shift;
								}
								else
								{
									pdung_cels += dung_and80;
									tmp_pbDst += dung_and80;
								}
								if ( !yy_32 )
									goto LABEL_252;
							}
							_LOBYTE(dung_and80) = -(char)dung_and80;
							tmp_pbDst += dung_and80;
							if ( dung_and80 & 0x1F )
								gdwCurrentMask <<= dung_and80 & 0x1F;
							yy_32 -= dung_and80;
						}
						while ( yy_32 );
LABEL_252:
						tmp_pbDst -= 800;
						--gpDrawMask;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 2: // lower (bottom transparent), black
/*
	 /-|
	(  |
	 \-|
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							tmp_pbDst += i;
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
					}
					i = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							tmp_pbDst += i;
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						i += 2;
					}
					while ( i != 32 );
					break;
				case 3: // lower (bottom transparent), black
/*
	|-\
	|  )
	|-/
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst = &tmp_pbDst[32 - i];
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
						tmp_pbDst += i;
					}
					i = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst = &tmp_pbDst[32 - i];
						}
						tmp_pbDst = &tmp_pbDst[i - 800];
						i += 2;
					}
					while ( i != 32 );
					break;
				case 4: // lower (bottom transparent), black
/*
	  /|
	 / |
	|__|
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							tmp_pbDst += i;
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
					}
					gpDrawMask -= 16;
					yy_32 = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = 0;
								left_shift *= 2;
								++tmp_pbDst;
								--i;
							}
							while ( i );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					break;
				default: // lower (bottom transparent), black
/*
	|\
	| \
	|__|
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst = &tmp_pbDst[32 - i];
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
						tmp_pbDst += i;
					}
					gpDrawMask -= 16;
					yy_32 = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = 0;
								left_shift *= 2;
								++tmp_pbDst;
								--i;
							}
							while ( i );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					break;
			}
			return;
		}
		if ( !(level_cel_block & 0x8000) )
		{
			pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			_EBX = (unsigned char *)&pLightTbl[256 * light_table_index];
			cel_type_16 = (unsigned char)(level_cel_block >> 12);
			switch ( cel_type_16 )
			{
				case 0: // lower (bottom transparent), with lighting
/*
	|--|
	|  |
	|--|
*/
					yy_32 = 32;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = _EBX[pdung_cels[0]];
								left_shift *= 2;
								++pdung_cels;
								++tmp_pbDst;
								--i;
							}
							while ( i );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					break;
				case 1: // lower (bottom transparent), with lighting
/*
	 /\
	(  )
	 \/
*/
					xx_32 = 32;
					do
					{
						gdwCurrentMask = *gpDrawMask;
						yy_32 = 32;
						do
						{
							while ( 1 )
							{
								dung_and80 = (unsigned char)*pdung_cels++;
								if ( (dung_and80 & 0x80u) != 0 )
									break;
								yy_32 -= dung_and80;
								if ( tmp_pbDst < gpBufEnd )
								{
									and80_i = dung_and80;
									left_shift = gdwCurrentMask;
									do
									{
										if ( left_shift & 0x80000000 )
											tmp_pbDst[0] = _EBX[pdung_cels[0]];
										left_shift *= 2;
										++pdung_cels;
										++tmp_pbDst;
										--and80_i;
									}
									while ( and80_i );
									gdwCurrentMask = left_shift;
								}
								else
								{
									pdung_cels += dung_and80;
									tmp_pbDst += dung_and80;
								}
								if ( !yy_32 )
									goto LABEL_52;
							}
							_LOBYTE(dung_and80) = -(char)dung_and80;
							tmp_pbDst += dung_and80;
							if ( dung_and80 & 0x1F )
								gdwCurrentMask <<= dung_and80 & 0x1F;
							yy_32 -= dung_and80;
						}
						while ( yy_32 );
LABEL_52:
						tmp_pbDst -= 800;
						--gpDrawMask;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 2: // lower (bottom transparent), with lighting
/*
	 /-|
	(  |
	 \-|
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_62:
							yy_32 = 2;
							if ( tmp_pbDst >= gpBufEnd )
							{
								tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
								if ( tile_42_45 > 42 )
									return;
								world_tbl = WorldTbl3x16[tile_42_45];
								pdung_cels += WorldTbl17_2[world_tbl >> 2];
								tmp_pbDst -= 192 * world_tbl;
								yy_32 = (world_tbl >> 1) + 2;
							}
							do
							{
								tmp_pbDst += yy_32;
								n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
								if ( (32 - yy_32) & 2 )
								{
									tmp_pbDst[0] = _EBX[pdung_cels[2]];
									tmp_pbDst[1] = _EBX[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 2;
								}
								if ( n_draw_shift )
								{
									do
									{
										tmp_pbDst[0] = _EBX[pdung_cels[0]];
										tmp_pbDst[1] = _EBX[pdung_cels[1]];
										tmp_pbDst[2] = _EBX[pdung_cels[2]];
										tmp_pbDst[3] = _EBX[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										--n_draw_shift;
									}
									while ( n_draw_shift );
								}
								tmp_pbDst -= 800;
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						tmp_pbDst += xx_32;
						n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
						if ( (32 - xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[2]];
							tmp_pbDst[1] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								tmp_pbDst[0] = _EBX[pdung_cels[0]];
								tmp_pbDst[1] = _EBX[pdung_cels[1]];
								tmp_pbDst[2] = _EBX[pdung_cels[2]];
								tmp_pbDst[3] = _EBX[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_62;
				case 3: // lower (bottom transparent), with lighting
/*
	|-\
	|  )
	|-/
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_80:
							yy_32 = 2;
							if ( tmp_pbDst >= gpBufEnd )
							{
								tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
								if ( tile_42_45 > 42 )
									return;
								world_tbl = WorldTbl3x16[tile_42_45];
								pdung_cels += WorldTbl17_2[world_tbl >> 2];
								tmp_pbDst -= 192 * world_tbl;
								yy_32 = (world_tbl >> 1) + 2;
							}
							do
							{
								for ( n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift )
								{
									tmp_pbDst[0] = _EBX[pdung_cels[0]];
									tmp_pbDst[1] = _EBX[pdung_cels[1]];
									tmp_pbDst[2] = _EBX[pdung_cels[2]];
									tmp_pbDst[3] = _EBX[pdung_cels[3]];
									pdung_cels += 4;
									tmp_pbDst += 4;
								}
								if ( (32 - (_BYTE)yy_32) & 2 )
								{
									tmp_pbDst[0] = _EBX[pdung_cels[0]];
									tmp_pbDst[1] = _EBX[pdung_cels[1]];
									pdung_cels += 2; /// BUGFIX: change to `pdung_cels += 4`
									tmp_pbDst += 2;
								}
								tmp_pbDst = &tmp_pbDst[yy_32 - 800];
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							tmp_pbDst[2] = _EBX[pdung_cels[2]];
							tmp_pbDst[3] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_80;
				case 4: // lower (bottom transparent), with lighting
/*
	  /|
	 / |
	|__|
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_98:
							gpDrawMask -= 16;
							yy_32 = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
								{
									left_shift = *gpDrawMask;
									i = 32;
									do
									{
										if ( left_shift & 0x80000000 )
											tmp_pbDst[0] = _EBX[pdung_cels[0]];
										left_shift *= 2;
										++pdung_cels;
										++tmp_pbDst;
										--i;
									}
									while ( i );
								}
								else
								{
									pdung_cels += 32;
									tmp_pbDst += 32;
								}
								tmp_pbDst -= 800;
								--gpDrawMask;
								--yy_32;
							}
							while ( yy_32 );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						tmp_pbDst += xx_32;
						n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
						if ( (32 - xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[2]];
							tmp_pbDst[1] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								tmp_pbDst[0] = _EBX[pdung_cels[0]];
								tmp_pbDst[1] = _EBX[pdung_cels[1]];
								tmp_pbDst[2] = _EBX[pdung_cels[2]];
								tmp_pbDst[3] = _EBX[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_98;
				default: // lower (bottom transparent), with lighting
/*
	|\
	| \
	|__|
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_117:
							gpDrawMask -= 16;
							yy_32 = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
								{
									left_shift = *gpDrawMask;
									i = 32;
									do
									{
										if ( left_shift & 0x80000000 )
											tmp_pbDst[0] = _EBX[pdung_cels[0]];
										left_shift *= 2;
										++pdung_cels;
										++tmp_pbDst;
										--i;
									}
									while ( i );
									pdung_cels += (unsigned char)pdung_cels & 2;
								}
								else
								{
									pdung_cels += 32;
									tmp_pbDst += 32;
								}
								tmp_pbDst -= 800;
								--gpDrawMask;
								--yy_32;
							}
							while ( yy_32 );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							tmp_pbDst[2] = _EBX[pdung_cels[2]];
							tmp_pbDst[3] = _EBX[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( (32 - (_BYTE)xx_32) & 2 )
						{
							tmp_pbDst[0] = _EBX[pdung_cels[0]];
							tmp_pbDst[1] = _EBX[pdung_cels[1]];
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_117;
			}
			return;
		}
		pdung_cels = (unsigned char *)pSpeedCels
				   + *(_DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (unsigned char)(level_cel_block >> 12);
	}
	else
	{
		if ( level_cel_block & 0x8000 )
			level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
							+ (unsigned short)(level_cel_block & 0xF000);
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
	}
	switch ( cel_type_16 )
	{
		case 8: // lower (bottom transparent), without lighting
/*
	|--|
	|  |
	|--|
*/
			yy_32 = 32;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
				{
					left_shift = *gpDrawMask;
					i = 32;
					do
					{
						if ( left_shift & 0x80000000 )
							tmp_pbDst[0] = pdung_cels[0];
						left_shift *= 2;
						++pdung_cels;
						++tmp_pbDst;
						--i;
					}
					while ( i );
				}
				else
				{
					pdung_cels += 32;
					tmp_pbDst += 32;
				}
				tmp_pbDst -= 800;
				--gpDrawMask;
				--yy_32;
			}
			while ( yy_32 );
			break;
		case 9: // lower (bottom transparent), without lighting
/*
	 /\
	(  )
	 \/
*/
			xx_32 = 32;
			do
			{
				gdwCurrentMask = *gpDrawMask;
				yy_32 = 32;
				do
				{
					while ( 1 )
					{
						dung_and80 = (unsigned char)*pdung_cels++;
						if ( (dung_and80 & 0x80u) != 0 )
							break;
						yy_32 -= dung_and80;
						if ( tmp_pbDst < gpBufEnd )
						{
							and80_i = dung_and80;
							left_shift = gdwCurrentMask;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = pdung_cels[0];
								left_shift *= 2;
								++pdung_cels;
								++tmp_pbDst;
								--and80_i;
							}
							while ( and80_i );
							gdwCurrentMask = left_shift;
						}
						else
						{
							pdung_cels += dung_and80;
							tmp_pbDst += dung_and80;
						}
						if ( !yy_32 )
							goto LABEL_152;
					}
					_LOBYTE(dung_and80) = -(char)dung_and80;
					tmp_pbDst += dung_and80;
					if ( dung_and80 & 0x1F )
						gdwCurrentMask <<= dung_and80 & 0x1F;
					yy_32 -= dung_and80;
				}
				while ( yy_32 );
LABEL_152:
				tmp_pbDst -= 800;
				--gpDrawMask;
				--xx_32;
			}
			while ( xx_32 );
			break;
		case 10: // lower (bottom transparent), without lighting
/*
	 /-|
	(  |
	 \-|
*/
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_162:
					yy_32 = 2;
					if ( tmp_pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 42 )
							return;
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_2[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						yy_32 = (world_tbl >> 1) + 2;
					}
					do
					{
						tmp_pbDst += yy_32;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ( (32 - yy_32) & 2 )
						{
							*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						tmp_pbDst -= 800;
						yy_32 += 2;
					}
					while ( yy_32 != 32 );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
			}
			do
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_162;
		case 11: // lower (bottom transparent), without lighting
/*
	|-\
	|  )
	|-/
*/
			xx_32 = 30;
			if ( pbDst < gpBufEnd )
				goto LABEL_175;
			tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
			if ( tile_42_45 <= 45 )
			{
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
				do
				{
LABEL_175:
					for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
				}
				while ( xx_32 >= 0 );
				goto LABEL_180;
			}
			tmp_pbDst = pbDst - 12288;
			pdung_cels += 288;
LABEL_180:
			yy_32 = 2;
			if ( tmp_pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 42 )
					return;
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_2[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				yy_32 = (world_tbl >> 1) + 2;
			}
			do
			{
				for ( n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift )
				{
					*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 4;
				}
				if ( (32 - (_BYTE)yy_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				tmp_pbDst = &tmp_pbDst[yy_32 - 800];
				yy_32 += 2;
			}
			while ( yy_32 != 32 );
			break;
		case 12: // lower (bottom transparent), without lighting
/*
	  /|
	 / |
	|__|
*/
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_198:
					gpDrawMask -= 16;
					yy_32 = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = pdung_cels[0];
								left_shift *= 2;
								++pdung_cels;
								++tmp_pbDst;
								--i;
							}
							while ( i );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
			}
			do
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_198;
		default: // lower (bottom transparent), without lighting
/*
	|\
	| \
	|__|
*/
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_217:
					gpDrawMask -= 16;
					yy_32 = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							left_shift = *gpDrawMask;
							i = 32;
							do
							{
								if ( left_shift & 0x80000000 )
									tmp_pbDst[0] = pdung_cels[0];
								left_shift *= 2;
								++pdung_cels;
								++tmp_pbDst;
								--i;
							}
							while ( i );
							pdung_cels += (unsigned char)pdung_cels & 2;
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--gpDrawMask;
						--yy_32;
					}
					while ( yy_32 );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
			}
			do
			{
				for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
				{
					*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 4;
				}
				if ( (32 - (_BYTE)xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				tmp_pbDst = &tmp_pbDst[xx_32 - 800];
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_217;
	}
}

void __fastcall drawLowerScreen(unsigned char *pbDst)
{
	unsigned char *tmp_pbDst; // edi MAPDST
	unsigned char *pdung_cels; // esi MAPDST
	unsigned char *l_tbl; // ebx
	short cel_type_16; // ax MAPDST
	int xx_32; // edx MAPDST
	int yy_32; // ebp MAPDST
	char block_4; // cl MAPDST
	int x_minus; // ecx MAPDST
	int y_minus; // ecx MAPDST
	unsigned int chk_sh_and; // ecx MAPDST
	signed int tile_42_45; // eax MAPDST
	unsigned int world_tbl; // ecx MAPDST
	unsigned int n_draw_shift; // ecx MAPDST
	unsigned int dung_and80; // eax MAPDST
	signed int i; // edx MAPDST
	signed int j; // ecx MAPDST

	if ( cel_transparency_active )
	{
		if ( !arch_draw_type )
		{
			drawTopArchesLowerScreen(pbDst);
			return;
		}
		if ( arch_draw_type == 1 )
		{
			if ( block_lvid[level_piece_id] == 1 || block_lvid[level_piece_id] == 3 )
			{
				drawBottomArchesLowerScreen(pbDst, &LeftMask[31]);
				return;
			}
		}
		if ( arch_draw_type == 2 )
		{
			if ( block_lvid[level_piece_id] == 2 || block_lvid[level_piece_id] == 3 )
			{
				drawBottomArchesLowerScreen(pbDst, &RightMask[31]);
				return;
			}
		}
	}
	gpCelFrame = (unsigned char *)speed_cel_frame_num_from_light_index_frame_num;
	tmp_pbDst = pbDst;
	if ( (_BYTE)light_table_index )
	{
		if ( (_BYTE)light_table_index == lightmax )
		{
			if ( level_cel_block & 0x8000 )
				level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
								+ (unsigned short)(level_cel_block & 0xF000);
			pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			cel_type_16 = (level_cel_block >> 12) & 7;
			switch ( cel_type_16 )
			{
				case 0: // lower (solid), black
/*
	|--|
	|  |
	|--|
*/
					i = 32;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								*(_DWORD *)tmp_pbDst = 0;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					break;
				case 1: // lower (solid), black
/*
	 /\
	(  )
	 \/
*/
					xx_32 = 32;
					do
					{
						yy_32 = 32;
						do
						{
							while ( 1 )
							{
								dung_and80 = (unsigned char)*pdung_cels++;
								if ( (dung_and80 & 0x80u) == 0 )
									break;
								_LOBYTE(dung_and80) = -(char)dung_and80;
								tmp_pbDst += dung_and80;
								yy_32 -= dung_and80;
								if ( !yy_32 )
									goto LABEL_232;
							}
							yy_32 -= dung_and80;
							if ( tmp_pbDst < gpBufEnd )
							{
								pdung_cels += dung_and80;
								chk_sh_and = dung_and80 >> 1;
								if ( dung_and80 & 1 )
								{
									tmp_pbDst[0] = 0;
									++tmp_pbDst;
								}
								if ( chk_sh_and )
								{
									n_draw_shift = dung_and80 >> 2;
									if ( chk_sh_and & 1 )
									{
										*(_WORD *)tmp_pbDst = 0;
										tmp_pbDst += 2;
									}
									if ( n_draw_shift )
									{
										do
										{
											*(_DWORD *)tmp_pbDst = 0;
											tmp_pbDst += 4;
											--n_draw_shift;
										}
										while ( n_draw_shift );
									}
								}
							}
							else
							{
								pdung_cels += dung_and80;
								tmp_pbDst += dung_and80;
							}
						}
						while ( yy_32 );
LABEL_232:
						tmp_pbDst -= 800;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 2: // lower (solid), black
/*
	 /-|
	(  |
	 \-|
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							tmp_pbDst += i;
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
					}
					i = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							tmp_pbDst += i;
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						i += 2;
					}
					while ( i != 32 );
					break;
				case 3: // lower (solid), black
/*
	|-\
	|  )
	|-/
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst = &tmp_pbDst[32 - i];
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
						tmp_pbDst += i;
					}
					i = 2;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst = &tmp_pbDst[32 - i];
						}
						tmp_pbDst = &tmp_pbDst[i - 800];
						i += 2;
					}
					while ( i != 32 );
					break;
				case 4: // lower (solid), black
/*
	  /|
	 / |
	|__|
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							tmp_pbDst += i;
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
					}
					i = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								*(_DWORD *)tmp_pbDst = 0;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					break;
				default: // lower (solid), black
/*
	|\
	| \
	|__|
*/
					for ( i = 30; ; i -= 2 )
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							n_draw_shift = (unsigned int)(32 - i) >> 2;
							if ( (32 - i) & 2 )
							{
								*(_WORD *)tmp_pbDst = 0;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = 0;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
						else
						{
							pdung_cels = &pdung_cels[32 - i];
							tmp_pbDst = &tmp_pbDst[32 - i];
						}
						tmp_pbDst -= 800;
						if ( !i )
							break;
						tmp_pbDst += i;
					}
					i = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								*(_DWORD *)tmp_pbDst = 0;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					break;
			}
			return;
		}
		if ( !(level_cel_block & 0x8000) )
		{
			pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			l_tbl = (unsigned char *)&pLightTbl[256 * light_table_index];
			cel_type_16 = (unsigned short)level_cel_block >> 12;
			switch ( cel_type_16 )
			{
				case 0: // lower (solid), with lighting
/*
	|--|
	|  |
	|--|
*/
					xx_32 = 32;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								tmp_pbDst[2] = l_tbl[pdung_cels[2]];
								tmp_pbDst[3] = l_tbl[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 1: // lower (solid), with lighting
/*
	 /\
	(  )
	 \/
*/
					xx_32 = 32;
					do
					{
						yy_32 = 32;
						do
						{
							dung_and80 = (unsigned char)*pdung_cels++;
							if ( (dung_and80 & 0x80u) == 0 )
							{
								yy_32 -= dung_and80;
								if ( tmp_pbDst < gpBufEnd )
								{
									for ( block_4 = dung_and80; block_4 >= 4; block_4 -= 4 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
									}
									if ( block_4 >= 2 )
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										pdung_cels += 2;
										tmp_pbDst += 2;
									}
									if ( block_4 & 1 )
									{
										*tmp_pbDst++ = l_tbl[*pdung_cels++];
									}
								}
								else
								{
									pdung_cels += dung_and80;
									tmp_pbDst += dung_and80;
								}
							}
							else
							{
								_LOBYTE(dung_and80) = -(char)dung_and80;
								tmp_pbDst += dung_and80;
								yy_32 -= dung_and80;
							}
						}
						while ( yy_32 );
						tmp_pbDst -= 800;
						--xx_32;
					}
					while ( xx_32 );
					break;
				case 2: // lower (solid), with lighting
/*
	 /-|
	(  |
	 \-|
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_68:
							yy_32 = 2;
							if ( tmp_pbDst >= gpBufEnd )
							{
								tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
								if ( tile_42_45 > 42 )
									return;
								world_tbl = WorldTbl3x16[tile_42_45];
								pdung_cels += WorldTbl17_2[world_tbl >> 2];
								tmp_pbDst -= 192 * world_tbl;
								yy_32 = (world_tbl >> 1) + 2;
							}
							do
							{
								tmp_pbDst += yy_32;
								y_minus = 32 - yy_32;
								pdung_cels += (32 - (_BYTE)yy_32) & 2;
								do
								{
									*tmp_pbDst++ = l_tbl[*pdung_cels++];
									--y_minus;
								}
								while ( y_minus );
								yy_32 += 2;
								tmp_pbDst -= 800;
							}
							while ( yy_32 != 32 );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						tmp_pbDst += xx_32;
						x_minus = 32 - xx_32;
						pdung_cels += (32 - (_BYTE)xx_32) & 2;
						do
						{
							*tmp_pbDst++ = l_tbl[*pdung_cels++];
							--x_minus;
						}
						while ( x_minus );
						tmp_pbDst -= 800;
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_68;
				case 3: // lower (solid), with lighting
/*
	|-\
	|  )
	|-/
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_83:
							yy_32 = 2;
							if ( tmp_pbDst >= gpBufEnd )
							{
								tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
								if ( tile_42_45 > 42 )
									return;
								world_tbl = WorldTbl3x16[tile_42_45];
								pdung_cels += WorldTbl17_2[world_tbl >> 2];
								tmp_pbDst -= 192 * world_tbl;
								yy_32 = (world_tbl >> 1) + 2;
							}
							do
							{
								y_minus = 32 - yy_32;
								do
								{
									*tmp_pbDst++ = l_tbl[*pdung_cels++];
									--y_minus;
								}
								while ( y_minus );
								pdung_cels += (unsigned char)pdung_cels & 2;
								tmp_pbDst = &tmp_pbDst[yy_32 - 800];
								yy_32 += 2;
							}
							while ( yy_32 != 32 );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						x_minus = 32 - xx_32;
						do
						{
							*tmp_pbDst++ = l_tbl[*pdung_cels++];
							--x_minus;
						}
						while ( x_minus );
						pdung_cels += (unsigned char)pdung_cels & 2;
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_83;
				case 4: // lower (solid), with lighting
/*
	  /|
	 / |
	|__|
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_100:
							i = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
								{
									block_4 = 32;
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										block_4 -= 4;
									}
									while ( block_4 >= 4 );
								}
								else
								{
									pdung_cels += 32;
									tmp_pbDst += 32;
								}
								tmp_pbDst -= 800;
								--i;
							}
							while ( i );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						tmp_pbDst += xx_32;
						block_4 = 32 - xx_32;
						pdung_cels += (32 - (_BYTE)xx_32) & 2;
						if ( (char)(32 - xx_32) >= 4 )
						{
							do
							{
								tmp_pbDst[0] = l_tbl[pdung_cels[0]];
								tmp_pbDst[1] = l_tbl[pdung_cels[1]];
								tmp_pbDst[2] = l_tbl[pdung_cels[2]];
								tmp_pbDst[3] = l_tbl[pdung_cels[3]];
								pdung_cels += 4;
								tmp_pbDst += 4;
								block_4 -= 4;
							}
							while ( block_4 >= 4 );
						}
						if ( block_4 >= 2 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							pdung_cels += 2;
							tmp_pbDst += 2;
						}
						tmp_pbDst -= 800;
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_100;
				default: // lower (solid), with lighting
/*
	|\
	| \
	|__|
*/
					xx_32 = 30;
					if ( pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 45 )
						{
							tmp_pbDst = pbDst - 12288;
							pdung_cels += 288;
LABEL_116:
							j = 16;
							do
							{
								if ( tmp_pbDst < gpBufEnd )
								{
									block_4 = 32;
									do
									{
										tmp_pbDst[0] = l_tbl[pdung_cels[0]];
										tmp_pbDst[1] = l_tbl[pdung_cels[1]];
										tmp_pbDst[2] = l_tbl[pdung_cels[2]];
										tmp_pbDst[3] = l_tbl[pdung_cels[3]];
										pdung_cels += 4;
										tmp_pbDst += 4;
										block_4 -= 4;
									}
									while ( block_4 >= 4 );
								}
								else
								{
									pdung_cels += 32;
									tmp_pbDst += 32;
								}
								tmp_pbDst -= 800;
								--j;
							}
							while ( j );
							return;
						}
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_1[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						xx_32 = 30 - (world_tbl >> 1);
					}
					do
					{
						for ( block_4 = 32 - xx_32; block_4 >= 4; block_4 -= 4 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							tmp_pbDst[2] = l_tbl[pdung_cels[2]];
							tmp_pbDst[3] = l_tbl[pdung_cels[3]];
							pdung_cels += 4;
							tmp_pbDst += 4;
						}
						if ( block_4 >= 2 )
						{
							tmp_pbDst[0] = l_tbl[pdung_cels[0]];
							tmp_pbDst[1] = l_tbl[pdung_cels[1]];
							pdung_cels += 2;
							tmp_pbDst += 2;
						}
						pdung_cels += (unsigned char)pdung_cels & 2;
						tmp_pbDst = &tmp_pbDst[xx_32 - 800];
						xx_32 -= 2;
					}
					while ( xx_32 >= 0 );
					goto LABEL_116;
			}
			return;
		}
		pdung_cels = (unsigned char *)pSpeedCels
				   + *(_DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (unsigned short)level_cel_block >> 12;
	}
	else
	{
		if ( level_cel_block & 0x8000 )
			level_cel_block = *(_DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
							+ (unsigned short)(level_cel_block & 0xF000);
		pdung_cels = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = (((unsigned int)level_cel_block >> 12) & 7) + 8;
	}
	switch ( cel_type_16 )
	{
		case 8: // lower (solid), without lighting
/*
	|--|
	|  |
	|--|
*/
			i = 32;
			do
			{
				if ( tmp_pbDst < gpBufEnd )
				{
					j = 8;
					do
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
						--j;
					}
					while ( j );
				}
				else
				{
					pdung_cels += 32;
					tmp_pbDst += 32;
				}
				tmp_pbDst -= 800;
				--i;
			}
			while ( i );
			break;
		case 9: // lower (solid), without lighting
/*
	 /\
	(  )
	 \/
*/
			xx_32 = 32;
			do
			{
				yy_32 = 32;
				do
				{
					while ( 1 )
					{
						dung_and80 = (unsigned char)*pdung_cels++;
						if ( (dung_and80 & 0x80u) == 0 )
							break;
						_LOBYTE(dung_and80) = -(char)dung_and80;
						tmp_pbDst += dung_and80;
						yy_32 -= dung_and80;
						if ( !yy_32 )
							goto LABEL_143;
					}
					yy_32 -= dung_and80;
					if ( tmp_pbDst < gpBufEnd )
					{
						chk_sh_and = dung_and80 >> 1;
						if ( dung_and80 & 1 )
						{
							tmp_pbDst[0] = pdung_cels[0];
							++pdung_cels;
							++tmp_pbDst;
						}
						if ( chk_sh_and )
						{
							n_draw_shift = chk_sh_and >> 1;
							if ( chk_sh_and & 1 )
							{
								*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
								pdung_cels += 2;
								tmp_pbDst += 2;
							}
							if ( n_draw_shift )
							{
								do
								{
									*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
									pdung_cels += 4;
									tmp_pbDst += 4;
									--n_draw_shift;
								}
								while ( n_draw_shift );
							}
						}
					}
					else
					{
						pdung_cels += dung_and80;
						tmp_pbDst += dung_and80;
					}
				}
				while ( yy_32 );
LABEL_143:
				tmp_pbDst -= 800;
				--xx_32;
			}
			while ( xx_32 );
			break;
		case 10: // lower (solid), without lighting
/*
	 /-|
	(  |
	 \-|
*/
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_153:
					yy_32 = 2;
					if ( tmp_pbDst >= gpBufEnd )
					{
						tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
						if ( tile_42_45 > 42 )
							return;
						world_tbl = WorldTbl3x16[tile_42_45];
						pdung_cels += WorldTbl17_2[world_tbl >> 2];
						tmp_pbDst -= 192 * world_tbl;
						yy_32 = (world_tbl >> 1) + 2;
					}
					do
					{
						tmp_pbDst += yy_32;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ( (32 - yy_32) & 2 )
						{
							*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
							pdung_cels += 4;
							tmp_pbDst += 2;
						}
						if ( n_draw_shift )
						{
							do
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
								--n_draw_shift;
							}
							while ( n_draw_shift );
						}
						yy_32 += 2;
						tmp_pbDst -= 800;
					}
					while ( yy_32 < 32 );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
			}
			do
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_153;
		case 11: // lower (solid), without lighting
/*
	|-\
	|  )
	|-/
*/
			xx_32 = 30;
			if ( pbDst < gpBufEnd )
				goto LABEL_166;
			tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
			if ( tile_42_45 <= 45 )
			{
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
				do
				{
LABEL_166:
					for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
					}
					if ( (32 - (_BYTE)xx_32) & 2 )
					{
						*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 2;
					}
					tmp_pbDst = &tmp_pbDst[xx_32 - 800];
					xx_32 -= 2;
				}
				while ( xx_32 >= 0 );
				goto LABEL_171;
			}
			tmp_pbDst = pbDst - 12288;
			pdung_cels += 288;
LABEL_171:
			yy_32 = 2;
			if ( tmp_pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(tmp_pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 42 )
					return;
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_2[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				yy_32 = (world_tbl >> 1) + 2;
			}
			do
			{
				for ( n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift )
				{
					*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 4;
				}
				if ( (32 - (_BYTE)yy_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				tmp_pbDst += yy_32;
				yy_32 += 2;
				tmp_pbDst -= 800;
			}
			while ( yy_32 < 32 );
			break;
		case 12: // lower (solid), without lighting
/*
	  /|
	 / |
	|__|
*/
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_189:
					i = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
			}
			do
			{
				tmp_pbDst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ( (32 - xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *((_WORD *)pdung_cels + 1);
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				if ( n_draw_shift )
				{
					do
					{
						*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
						pdung_cels += 4;
						tmp_pbDst += 4;
						--n_draw_shift;
					}
					while ( n_draw_shift );
				}
				tmp_pbDst -= 800;
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_189;
		default: // lower (solid), without lighting
/*
	|\
	| \
	|__|
*/
			xx_32 = 30;
			if ( pbDst >= gpBufEnd )
			{
				tile_42_45 = (unsigned int)(pbDst - gpBufEnd + 1023) >> 8;
				if ( tile_42_45 > 45 )
				{
					tmp_pbDst = pbDst - 12288;
					pdung_cels += 288;
LABEL_205:
					i = 16;
					do
					{
						if ( tmp_pbDst < gpBufEnd )
						{
							j = 8;
							do
							{
								*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
								pdung_cels += 4;
								tmp_pbDst += 4;
								--j;
							}
							while ( j );
						}
						else
						{
							pdung_cels += 32;
							tmp_pbDst += 32;
						}
						tmp_pbDst -= 800;
						--i;
					}
					while ( i );
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				pdung_cels += WorldTbl17_1[world_tbl >> 2];
				tmp_pbDst -= 192 * world_tbl;
				xx_32 = 30 - (world_tbl >> 1);
			}
			do
			{
				for ( n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift )
				{
					*(_DWORD *)tmp_pbDst = *(_DWORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 4;
				}
				if ( (32 - (_BYTE)xx_32) & 2 )
				{
					*(_WORD *)tmp_pbDst = *(_WORD *)pdung_cels;
					pdung_cels += 4;
					tmp_pbDst += 2;
				}
				tmp_pbDst = &tmp_pbDst[xx_32 - 800];
				xx_32 -= 2;
			}
			while ( xx_32 >= 0 );
			goto LABEL_205;
	}
}

void __fastcall world_draw_black_tile(unsigned char *pbDst)
{
	unsigned char *tmp_pbDst; // edi MAPDST
	signed int xx_32; // edx
	signed int i; // ebx MAPDST
	signed int j; // ecx MAPDST
	signed int yy_32; // edx

	tmp_pbDst = pbDst;
	xx_32 = 30;
	for ( i = 1; ; ++i )
	{
		tmp_pbDst += xx_32;
		j = i;
		do
		{
			*(_DWORD *)tmp_pbDst = 0;
			tmp_pbDst += 4;
			--j;
		}
		while ( j );
		tmp_pbDst = &tmp_pbDst[xx_32 - 832];
		if ( !xx_32 )
			break;
		xx_32 -= 2;
	}
	yy_32 = 2;
	i = 15;
	do
	{
		tmp_pbDst += yy_32;
		j = i;
		do
		{
			*(_DWORD *)tmp_pbDst = 0;
			tmp_pbDst += 4;
			--j;
		}
		while ( j );
		tmp_pbDst = &tmp_pbDst[yy_32 - 832];
		--i;
		yy_32 += 2;
	}
	while ( yy_32 != 32 );
}

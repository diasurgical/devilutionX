// some global definitions, found in debug release

#define DMAXX					40
#define DMAXY					40

#define LIGHTSIZE				6912 // 27 * 256
#define ROW_PITCH				768

// must be unsigned to generate unsigned comparisons with pnum
#define MAX_PLRS				4

#define MAX_CHARACTERS			10
#define MAX_LVLMTYPES			16
// #define MAX_PATH				260
#define MAX_SEND_STR_LEN		80
#define MAX_SPELLS				37

#define MAXDEAD					31
#define MAXDUNX					112
#define MAXDUNY					112
#define MAXITEMS				127
#define MAXBELTITEMS			8
#define MAXMISSILES				125
#define MAXMONSTERS				200
#define MAXMULTIQUESTS			4
#define MAXOBJECTS				127
#define MAXPORTAL				4
#define MAXQUESTS				16
#define MAXTHEMES				50
#define MAXTILES				2048
#define MAXTRIGGERS				5
#define MDMAXX					40
#define MDMAXY					40
#define MAXCHARLEVEL			51

// number of inventory grid cells
#define NUM_INV_GRID_ELEM		40
#define INV_SLOT_SIZE_PX		28

// Item indestructible durability
#define DUR_INDESTRUCTIBLE		255

#define VOLUME_MIN				-1600
#define VOLUME_MAX				0

// todo: enums
#define NUM_SFX					858
#define NUMLEVELS				17

// from diablo 2 beta
#define MAXEXP					2000000000

#define PLR_NAME_LEN			32

#define MAXPATHNODES			300

// 256 kilobytes + 3 bytes (demo leftover) for file magic (262147)
// final game uses 4-byte magic instead of 3
#define FILEBUFF				((256*1024)+3)

#define PMSG_COUNT				8

// Diablo Retail Version Game ID
#define GAME_ID					((int)'DRTL')

// Diablo uses a 256 color palette
// Entry 0-127 (0x00-0x7F) are level specific
// Entry 128-255 (0x80-0xFF) are global

// standard palette for all levels
// 8 or 16 shades per color
// example (dark blue): PAL16_BLUE+14, PAL8_BLUE+7
// example (light red): PAL16_RED+2, PAL8_RED
// example (orange): PAL16_ORANGE+8, PAL8_ORANGE+4
#define PAL8_BLUE		128
#define PAL8_RED		136
#define PAL8_YELLOW		144
#define PAL8_ORANGE		152
#define PAL16_BEIGE		160
#define PAL16_BLUE		176
#define PAL16_YELLOW	192
#define PAL16_ORANGE	208
#define PAL16_RED		224
#define PAL16_GRAY		240

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

/////////////////////////////////////////////////////////////////////////
/* temporary stuff from the decompiler */
/* remove all the garbage below in the future */
/////////////////////////////////////////////////////////////////////////
#ifndef IDA_GARBAGE
#define IDA_GARBAGE

// Partially defined types. They are used when the decompiler does not know
// anything about the type except its size.
#define _BYTE  unsigned char
#define _WORD  unsigned short
#define _DWORD unsigned int

// Some convenience macros to make partial accesses nicer
#define LAST_IND(x,part_type)    (sizeof(x)/sizeof(part_type) - 1)
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#  define LOW_IND(x,part_type)   LAST_IND(x,part_type)
#  define HIGH_IND(x,part_type)  0
#else
#  define HIGH_IND(x,part_type)  LAST_IND(x,part_type)
#  define LOW_IND(x,part_type)   0
#endif

// first unsigned macros:
#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define WORDn(x, n)   (*((WORD*)&(x)+n))

#define _LOBYTE(x)  BYTEn(x,LOW_IND(x,BYTE))
#define _LOWORD(x)  WORDn(x,LOW_IND(x,WORD))
#define _HIBYTE(x)  BYTEn(x,HIGH_IND(x,BYTE))
#define _HIWORD(x)  WORDn(x,HIGH_IND(x,WORD))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)

// now signed macros (the same but with sign extension)
#define SBYTEn(x, n)   (*((char*)&(x)+n))

#define SLOBYTE(x)  SBYTEn(x,LOW_IND(x,char))

// Helper functions to represent some assembly instructions.

__inline void *qmemcpy(void *dst, const void *src, size_t cnt)
{
	char *out      = (char *)dst;
	const char *in = (const char *)src;
	while (cnt > 0) {
		*out++ = *in++;
		--cnt;
	}
	return dst;
}

// rotate right
__inline WORD __ROR2__(WORD value, DWORD count)
{
	count %= 16;

	return value >> count | value << (16 - count);
}

#endif /* IDA_GARBAGE */

// Typedef for the function pointer
typedef void (*_PVFV)(void);

#if defined(_MSC_VER) && !defined(__APPLE__)
// Define our segment names
#define SEGMENT_C_INIT ".CRT$XCU"

// Build our various function tables and insert them into the correct segments.
#pragma data_seg(SEGMENT_C_INIT)
#pragma data_seg() // Switch back to the default segment
// Call function pointer arrays and place them in the segments created above
#define SEG_ALLOCATE(SEGMENT) __declspec(allocate(SEGMENT))
#else
#define SEG_ALLOCATE(SEGMENT)
#endif

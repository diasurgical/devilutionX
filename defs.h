// some global definitions, found in debug release

#define DMAXX					40
#define DMAXY					40

#define LIGHTSIZE				6912 // 27 * 256

// must be unsigned to generate unsigned comparisons with pnum
#define MAX_PLRS				4

#define MAX_CHARACTERS			10
#define MAX_LVLMTYPES			16
// #define MAX_PATH				260
#define MAX_SEND_STR_LEN		80

#define MAXDEAD					31
#define MAXDUNX					112
#define MAXDUNY					112
#define MAXITEMS				127
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


// number of inventory grid cells
#define NUM_INV_GRID_ELEM			40
#define INV_SLOT_SIZE_PX		28

// todo: enums
#define NUM_SFX					858
#define NUMLEVELS				17

// from diablo 2 beta
#define MAXEXP					2000000000

#define PLR_NAME_LEN			32

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





/////////////////////////////////////////////////////////////////////////
/* temporary stuff from the decompiler */
/* remove all the garbage below in the future */
/////////////////////////////////////////////////////////////////////////
#ifndef IDA_GARBAGE
#define IDA_GARBAGE

inline void memset32(void *s, unsigned int c, size_t n)
{
	unsigned int *p = (unsigned int *)s;
	for (int i = 0; i < n; i++) {
		p[i] = c;
	}
}

typedef          __int64 ll;
typedef unsigned __int64 ull;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;
typedef ull             uint64;

// Partially defined types. They are used when the decompiler does not know
// anything about the type except its size.
#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64

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
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define WORDn(x, n)   (*((_WORD*)&(x)+n))
#define DWORDn(x, n)  (*((_DWORD*)&(x)+n))

#define _LOBYTE(x)  BYTEn(x,LOW_IND(x,_BYTE))
#define _LOWORD(x)  WORDn(x,LOW_IND(x,_WORD))
#define LODWORD(x) DWORDn(x,LOW_IND(x,_DWORD))
#define _HIBYTE(x)  BYTEn(x,HIGH_IND(x,_BYTE))
#define _HIWORD(x)  WORDn(x,HIGH_IND(x,_WORD))
#define HIDWORD(x) DWORDn(x,HIGH_IND(x,_DWORD))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)


// now signed macros (the same but with sign extension)
#define SBYTEn(x, n)   (*((int8*)&(x)+n))
#define SWORDn(x, n)   (*((int16*)&(x)+n))

#define SLOBYTE(x)  SBYTEn(x,LOW_IND(x,int8))
#define SHIWORD(x)  SWORDn(x,HIGH_IND(x,int16))



// Helper functions to represent some assembly instructions.

#ifdef __cplusplus

#ifdef FAST_MEMCPY
#define qmemcpy memcpy
#else
inline void *qmemcpy(void *dst, const void *src, size_t cnt)
{
  char *out = (char *)dst;
  const char *in = (const char *)src;
  while ( cnt > 0 )
  {
    *out++ = *in++;
    --cnt;
  }
  return dst;
}
#endif

// Generate a reference to pair of operands
template<class T>  int16 __PAIR__( int8  high, T low) { return ((( int16)high) << sizeof(high)*8) | uint8(low); }
template<class T>  int32 __PAIR__( int16 high, T low) { return ((( int32)high) << sizeof(high)*8) | uint16(low); }
template<class T>  int64 __PAIR__( int32 high, T low) { return ((( int64)high) << sizeof(high)*8) | uint32(low); }
template<class T> uint16 __PAIR__(uint8  high, T low) { return (((uint16)high) << sizeof(high)*8) | uint8(low); }
template<class T> uint32 __PAIR__(uint16 high, T low) { return (((uint32)high) << sizeof(high)*8) | uint16(low); }
template<class T> uint64 __PAIR__(uint32 high, T low) { return (((uint64)high) << sizeof(high)*8) | uint32(low); }

// rotate left
template<class T> T __ROL__(T value, int count)
{
  const uint nbits = sizeof(T) * 8;

  if ( count > 0 )
  {
    count %= nbits;
    T high = value >> (nbits - count);
    if ( T(-1) < 0 ) // signed value
      high &= ~((T(-1) << count));
    value <<= count;
    value |= high;
  }
  else
  {
    count = -count % nbits;
    T low = value << (nbits - count);
    value >>= count;
    value |= low;
  }
  return value;
}

inline uint16 __ROR2__(uint16 value, int count) { return __ROL__((uint16)value, -count); }
inline uint32 __ROR4__(uint32 value, int count) { return __ROL__((uint32)value, -count); }

// sign flag
template<class T> int8 __SETS__(T x)
{
  if ( sizeof(T) == 1 )
    return int8(x) < 0;
  if ( sizeof(T) == 2 )
    return int16(x) < 0;
  if ( sizeof(T) == 4 )
    return int32(x) < 0;
  return int64(x) < 0;
}

// overflow flag of subtraction (x-y)
template<class T, class U> int8 __OFSUB__(T x, U y)
{
  if ( sizeof(T) < sizeof(U) )
  {
    U x2 = x;
    int8 sx = __SETS__(x2);
    return (sx ^ __SETS__(y)) & (sx ^ __SETS__(x2-y));
  }
  else
  {
    T y2 = y;
    int8 sx = __SETS__(x);
    return (sx ^ __SETS__(y2)) & (sx ^ __SETS__(x-y2));
  }
}

#endif

#endif /* IDA_GARBAGE */

#ifndef INFINITY
#include <limits>
#define INFINITY std::numeric_limits<float>::infinity()
#endif

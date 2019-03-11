#define strcasecmp _strcmpi
#define strncasecmp _strnicmp

#define HAVE_STRERROR 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_LIMITS_H 1

#define HAVE_STRDUP
#define HAVE_STDLIB_H
#define HAVE_STDINT_H
#define HAVE_INTTYPES_H
#define HAVE_STRING_H

#ifdef _M_ARM
#define ASMALIGN_ARMASM 1
#else
#define ASMALIGN_BALIGN 1
#endif


/* We want some frame index, eh? */
#define FRAME_INDEX 1
#define INDEX_SIZE 1000

/* also gapless playback! */
#define GAPLESS 1
/* #ifdef GAPLESS
#undef GAPLESS
#endif */

/* #define DEBUG
#define EXTRA_DEBUG */

#define REAL_IS_FLOAT

#define inline __inline

/* we are on win32 */
#define HAVE_WINDOWS_H

/* use the unicode support within libmpg123 */
#define WANT_WIN32_UNICODE

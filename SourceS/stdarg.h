#if defined(__MACH__)
#ifndef __STDARG_H
#define __STDARG_H

#ifndef _VA_LIST_T
typedef __builtin_va_list va_list;
#define _VA_LIST_T
#endif
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

/* GCC always defines __va_copy, but does not define va_copy unless in c99 mode
 * or -ansi is not specified, since it was not part of C90.
 */
#define __va_copy(d,s) __builtin_va_copy(d,s)

#if __STDC_VERSION__ >= 199900L || __cplusplus >= 201103L || !defined(__STRICT_ANSI__)
#define va_copy(dest, src)  __builtin_va_copy(dest, src)
#endif

/* Hack required to make standard headers work, at least on Ubuntu */
#define __GNUC_VA_LIST 1
typedef __builtin_va_list __gnuc_va_list;

#endif /* __STDARG_H */
#endif
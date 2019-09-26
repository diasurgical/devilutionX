#ifndef __STDARG_H
#define __STDARG_H

typedef __builtin_va_list va_list;
#define _VA_LIST_T
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)

#endif /* __STDARG_H */

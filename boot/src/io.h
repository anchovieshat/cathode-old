#ifndef _IO_H_
#define _IO_H_

#include "efi.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

i32 puts(const i8 *s);
i32 putc(i8 c);
i32 printf(const i8 *fmt, ...);
i32 vprintf(const i8 *fmt, __builtin_va_list args);
i8 *itoa(i64 value, i8 *result, i32 base);

#endif // _IO_H_

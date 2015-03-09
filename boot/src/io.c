#include "io.h"

static i64 strlen(const i8 *s) {
	i64 c = 0;

	while (*s++ != 0)
		c++;

	return c;
}

i32 puts(const i8 *s) {
	int st;

	for (; *s != 0; ++s)
		if (!(st = putc(*s)))
			return st;
	return putc('\n');
}

i32 putc(i8 c) {
	u16 buf[2];

    if (c == '\n')
		putc('\r');

	buf[0] = c;
	buf[1] = 0;

	return (ST->con_out->output_string(ST->con_out, buf) == EFI_SUCCESS);
}

i32 printf(const i8 *fmt, ...) {
	__builtin_va_list args;
	i32 ret;

	__builtin_va_start(args, fmt);
	ret = vprintf(fmt, args);
	__builtin_va_end(args);
	return ret;
}

i32 vprintf(const i8 *fmt, __builtin_va_list args) {
	i32 ret;
	i8 ch;
	i32 lflag, zflag, sharpflag, uflag;
	i64 num;
	i8 tmp[64];
	i32 base;
	const i8 *p;
	i8 pad;
	i32 n;
	i32 width, dwidth;

	ret = 0;

	for (;;) {
		while ((ch = *fmt++) != '%') {
			if (ch == '\0')
				return ret;
			putc(ch); ret++;
		}
		pad = ' ';
		uflag = lflag = zflag = sharpflag = 0;
		width = dwidth = 0;
sw:		switch (ch = *fmt++) {
			case 'l':
				lflag = 1;
				goto sw;
			case 'z':
				zflag = 1;
				goto sw;
			case '#':
				sharpflag = 1;
				goto sw;
			case 'u':
				uflag = 1;
				goto sw;
			case '%':
				putc(ch);
				break;
			case '0':
				pad = '0';
				goto sw;
			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				for (n = 0;; ++fmt) {
					n = n * 10 + ch - '0';
					ch = *fmt;
					if (ch < '0' || ch > '9')
						break;
				}
				width = n;
				goto sw;
			case 's':
				p = __builtin_va_arg(args, i8 *);
				if (p == NULL)
					p = "(null)";
				while (*p != '\0') {
					putc(*p++);
					ret++;
				}
				break;
			case 'd':
				base = 10;
				goto snum;
			case 'x':
				base = 16;
				goto snum;
			snum:
				if (zflag || lflag)
					num = __builtin_va_arg(args, i64);
				else
					num = (i64)__builtin_va_arg(args, i32);
				itoa(num, tmp, base);
				n = strlen(tmp);
				if (pad == '0')
					dwidth = width;
				width -= max(dwidth, n);
				dwidth -= n;
				while (width-- > 0)
					putc(' ');
				while (dwidth-- > 0)
					putc('0');
				p = tmp;
				while (*p != '\0') {
					putc(*p++);
					ret++;
				}
				break;
			default:
				return -1;
		}
	}

}

i8 *itoa(i64 value, i8 *result, i32 base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    i8* ptr = result, *ptr1 = result, tmp_char;
    i64 tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

/*
 * kernel/src/stubout.c
 */

#include <util.h>
#include "asm.h"
#include "stubout.h"

#define OUTPUT_TO_SERIAL

void dputc(char c)
{
#define DINK 0x75

#ifdef OUTPUT_TO_SERIAL
	static ushort sport = 0;
#endif
	static uchar * debug_cur = (uchar *)0xB8000; /* TODO fix magic */

	/* vga output */
	if (c == 10) {
		debug_cur = (uchar *)align((ulong)debug_cur - 0xB8000, 80*2) + 0xB8000;
	} else {
		/* TODO scroll or wrap */
		debug_cur[0] = c;
		debug_cur[1] = DINK;
		debug_cur += 2;
	}

#ifdef OUTPUT_TO_SERIAL
	if (!sport)
		sport = *(ushort *)0x400;

	while (~inb(sport + 5) & 0x20);
	outb(sport, c);
#endif

#undef DINK
}

void dputs(const char * str)
{
	for (; *str != 0; str++) {
		dputc(*str);
	}
}

void dputu(ulong val, int radix)
{
	char ch;
	ulong div, tmp;

	if (val == 0) {
		dputc('0');
		return;
	}

	div = 1;
	tmp = val / radix;

	while (tmp > 0) {
		tmp /= radix;
		div *= radix;
	}

	while (div > 0) {
		ch = val / div;
		val %= div;
		div /= radix;

		if (ch < 10) ch += '0';
		else ch += 'A' - 10;

		dputc(ch);
	}
}

void dputl(long val, int radix)
{
	if (val < 0) {
		dputc('-');
		val = -val;
	}

	dputu(val, radix);
}

void dprintf(const char * fmt, ...)
{
	va_list ls;
	va_start(ls, fmt);
	vdprintf(fmt, ls);
	va_end(ls);
}

void vdprintf(const char * fmt, va_list ls)
{
	void * p;
	char * s;

	for (; *fmt != 0; fmt++) {
		if (*fmt == '%') {
			switch (*++fmt) {
			case 'c':
				dputc(va_arg(ls, int));
				break;
			case 's':
				s = va_arg(ls, char *);
				if (s != NULL) dputs(s);
				else dputs("(null string)");
				break;
			case 'd':
			case 'i':
				dputl(va_arg(ls, int), 10);
				break;
			case 'D':
			case 'I':
			case 'l':
				dputl(va_arg(ls, long), 10);
				break;
			case 'u':
				dputu(va_arg(ls, uint), 10);
				break;
			case 'o':
				dputu(va_arg(ls, uint), 010);
				break;
			case 'x':
				dputu(va_arg(ls, uint), 0x10);
				break;
			case 'X':
				dputu(va_arg(ls, ulong), 0x10);
				break;
			case 'p':
				p = va_arg(ls, void *);
				if (p != NULL) dputu((ulong)p, 0x10);
				else dputs("(null)");
				break;
			default:
				dputc(*fmt);
			}
		} else {
			dputc(*fmt);
		}
	}
}

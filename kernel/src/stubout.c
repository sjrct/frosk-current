//
// kernel/src/stubout.c
//

#include <types.h>
#include "stubout.h"

void dputc(char c)
{
#define DINK 0x75

	static ushort sport = 0;
	static uchar * debug_cur = 0xB8000;//TODO fix magic

	// vga output
	if (c == 10) {
		do {
			dputc(' ');
		while ((ulong)debug_cur % 160);
	} else {
		// TODO scroll or wrap
		debug_cur[0] = c;
		debug_cur[1] = DINK;
		debug_cur += 2;
	}

	// serial output
	if (!sport)
		sport = *(ushort *)0x400;

	while (~inb(sport + 5) & 0x20);
	outb(sport, c);

#undef DINK
}  

void dputs(char * str)
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
	for (; *fmt != 0; fmt++) {
		if (*fmt == '%') {
			switch (*++fmt) {
			case 'c':
				dputc(va_arg(ls, int));
				break;
			case 's':
				dputs(va_arg(ls, char *));
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
				dputu((ulong)va_arg(ls, void *), 0x10);
				break;
			default:
				dputc(*fmt);
			}
		} else {
			dputc(*fmt);
		}
	}
}

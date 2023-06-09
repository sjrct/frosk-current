/*
 * kernel/src/stubout.c
 */

#include <util.h>
#include <string.h>
#include "asm.h"
#include "stubout.h"

#define OUTPUT_TO_SERIAL
//#define OUTPUT_TO_CGA

#ifdef OUTPUT_TO_CGA
void dputc_cga(char c)
{
#define DINK 0x75

#define CGA_START       0xB8000
#define CGA_COLUMNS     80
#define CGA_LINES       25
#define CGA_LINE_SIZE   (CGA_COLUMNS * 2)
#define CGA_SCREEN_SIZE (CGA_LINE_SIZE * CGA_LINES)
#define CGA_END         (CGA_START + CGA_SCREEN_SIZE)

    static uchar * debug_cur = (uchar *)CGA_START;

    if (c == 10) {
        debug_cur = (uchar *)align((ulong)debug_cur - CGA_START, CGA_LINE_SIZE) + CGA_START;
    } else {
        if (debug_cur >= (uchar *)CGA_END) {
            /* scroll, should be debug_cur == CGA_END */
            debug_cur = (uchar *)CGA_END - CGA_LINE_SIZE;

            for (uchar *ln = (uchar *)CGA_START; ln < debug_cur; ln++) {
                ln[0] = ln[CGA_LINE_SIZE];
            }

            memset(debug_cur, 0, CGA_LINE_SIZE);
        }
        debug_cur[0] = c;
        debug_cur[1] = DINK;
        debug_cur += 2;
    }
}
#endif

#ifdef OUTPUT_TO_SERIAL
void dputc_serial(char c)
{
    static ushort sport = 0;
    if (!sport) sport = *(ushort *)0x400;
    while (~inb(sport + 5) & 0x20);
    outb(sport, c);
}
#endif

void dputc(char c)
{
#ifdef OUTPUT_TO_CGA
    dputc_cga(c);
#endif
#ifdef OUTPUT_TO_SERIAL
    dputc_serial(c);
#endif
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

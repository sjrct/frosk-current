//
// lib/cstd/src/ctype.c
//

#include <ctype.h>

int isupper(int c)
{
	return c >= 'A' && c <= 'Z';
}

int islower(int c)
{
	return c >= 'a' && c <= 'z';
}

int isalpha(int c)
{
	return isupper(c) || islower(c);
}

int isdigit(int c)
{
	return c >= '0' && c <= '9';
}

int isxdigit(int c)
{
	return isdigit(c)
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

int iscntrl(int c)
{
	return c == 0 || c == 0x1f || c == 0x7f;
}

int isprint(int c)
{
	return c >= 0x20 && c <= 0x7e;
}

int isspace(int c)
{
	return c == ' ' || c == '\n' || c == '\t'
		|| c == '\f' || c == '\r' || c == '\v';
}

int isgraph(int c)
{
	return isprint(c) && !isspace(c);
}

int ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}

int tolower(int c)
{
	if (isupper(c)) c = c + 0x20;
	return c;
}

int toupper(int c)
{
	if (islower(c)) c = c - 0x20;
	return c;
}


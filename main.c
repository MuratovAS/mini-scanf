/*
 *  mini-scanf.c - Minimal scanf() implementation for embedded projects. 
 * Copyright (c) 2023 Aleksej Muratov
 */

#include <stdio.h> // TEST only
#include <stdarg.h>
#include <stdbool.h>

#ifndef NULL
#define NULL 0
#endif

//conf
// %[..]
#define LENSCANS 10

// test
#define TEXT "9 5 2 asd-   en d$ 3"

#define scanF "%d %*d %u %c %s %[^$] %d"
#define scanA &d, &u, &c, s, ss, &test

#define printF "D=%d U=%d C=%c S=%s SS=%s  test=%d  R=%d\n\r"
#define printA d, u, c, s, ss, test, ret

int	c_isdigit(int c);

// implementation of non-standard functions
long atoie(char const* c, char** p)
{
	if (c == NULL)
		return 0;

	// Account for a leading positive or negative sign.
	int sign = 1;
	while (!c_isdigit(*c))
	{
		if (*c == '\0')
			return 0;
		if (*c == '+' || *c == '-')
			if (*c == '-')
				sign = -1;
		c++;
	}

	// Read in each successive characters until a non-digit character is reached.
	long value = 0;
	for (; c_isdigit(*c); ++c)
	{
		value *= 10;
		value += (int)(*c - '0');
	}
	*p = (char*)c;
	return value * sign;
}

// implementation of basic dependencies
int c_isspace(const int c)
{
	switch (c)
	{ /* in the "C" locale: */
		case ' ': /* space */
		case '\f': /* form feed */
		case '\n': /* new-line */
		case '\r': /* carriage return */
		case '\t': /* horizontal tab */
		case '\v': /* vertical tab */
			return 1;
		default:
			return 0;
	}
}

int	c_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	else
		return (0);
}

int c_sscanf(const char* buff, char* format, ...)
{
	char* out_loc;

	int count = 0;

	int PointBuf = 0;
	int PointFt = 0;

	va_list ap;
	va_start(ap, format);
	while (format && format[PointFt]) // Read format
	{
		if (format[PointFt] == '%')
		{
			PointFt++;

			// for %*
			bool save = true;
			if (format[PointFt] == '*')
			{
				save = false;
				PointFt++;
			}

			// for %[]
			char stop[LENSCANS];
			unsigned stopN = 0;
			if (format[PointFt] == '[')
			{
				while (format[PointFt] != ']')
				{
					if (format[PointFt] != '[')
					{
						stop[stopN] = format[PointFt];
						stopN++;
					}
					PointFt++;
				}
			}

			switch (format[PointFt])
			{
				case 'c': // ignore isspace (std)
					while (c_isspace(buff[PointBuf]))
						PointBuf++;
					if (save)
						*(char*)va_arg(ap, char*) = buff[PointBuf];
					PointBuf++;
					count++;
					break;
				case 'u': // clone %d (?)
				case 'd':
					int d = atoie(&buff[PointBuf], &out_loc);
					if (save)
						*(int*)va_arg(ap, int*) = d;
					PointBuf += out_loc - &buff[PointBuf];
					count++;
					break;
				/*case 'o':
						int o = strtol(&buff[PointBuf], &out_loc, 8);
						if(save)
								*(int*)va_arg(ap, int*) = o;
						PointBuf += out_loc - &buff[PointBuf];
						count++;
						break;*/
				case ']':
				case 's':
					char* t = va_arg(ap, char*);

					while (c_isspace(buff[PointBuf])) // ignor isspace (std)
						PointBuf++;

					while (1)
					{
						bool con = false;
						if (stopN != 0)
						{
							bool invert = (stop[0] == '^');
							con = !invert;
							for (unsigned i = (invert ? 1 : 0); i < stopN; i++)
								if (stop[i] == buff[PointBuf])
								{
									con = invert;
									break;
								}

							if (con == true)
								break;
						}
						if (!c_isspace(buff[PointBuf]) || (!con && stopN != 0))
						{
							if (save)
								*t = buff[PointBuf];
							PointBuf++;
							t++;
						}
						else
							break;
					}
					count++;
					break;
					/*case 'x':
							int x = strtol(&buff[PointBuf], &out_loc, 16);
							if(save)
									*(int*)va_arg(ap, int*) = x;
							PointBuf += out_loc - &buff[PointBuf];
							count++;
							break;*/
			}
		}
		// else  // drop char in buff (no std)
		// PointBuf++;
		PointFt++;
	}
	va_end(ap);
	return count;
}

// test
int main(int argc, char* argv[])
{
	char buff[100] = TEXT;

	char s[100];
	char ss[100];
	char c;
	int d;
	unsigned u;
	int test;

	int ret;

	printf("TEST(sscanf):\n\r%s\n\r", buff);
	ret = c_sscanf(buff, scanF, scanA);
	printf(printF, printA);
	ret = sscanf(buff, scanF, scanA);
	printf(printF, printA);
}
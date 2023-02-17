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
#define C_SCANF
// %[..]
#define LENSCANS 10

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

int c_getchar()
{
    return getchar();
}

int c_sscanf(const char* buff, char* format, ...)
{
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

			// for %1234567890
			unsigned len = 0;
			bool lenEn = false;
			while (c_isdigit(format[PointFt]))
			{
				lenEn = true;
				len *= 10;
				len += (format[PointFt] - '0');
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
				case 'c':
					while (c_isspace(buff[PointBuf])) // ignore isspace (std)
						PointBuf++; //
					if (save)
						*(char*)va_arg(ap, char*) = buff[PointBuf];
					PointBuf++;
					count++;
					break;
				case 'u':
				case 'd':
					int sign = 1;
					while (!c_isdigit(buff[PointBuf]))
					{
						if (buff[PointBuf] == '+' || buff[PointBuf] == '-')
							if (buff[PointBuf] == '-')
								//if(format[PointFt] != 'u') // ignore sign (no std)
									sign = -1;
						PointBuf++;
					}
					long value = 0;
					while(c_isdigit(buff[PointBuf]) && (lenEn != true || len > 0))
					{
						value *= 10;
						value += (int)(buff[PointBuf] - '0');
						PointBuf++;
						len--;
					}

					if (save)
						*(int*)va_arg(ap, int*) = value * sign;
					count++;
					break;
				case ']':
				case 's':
					char* t = va_arg(ap, char*);

					while (c_isspace(buff[PointBuf])) // ignor isspace (std)
						PointBuf++; //

					while (true)
					{
						//
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

						if (!c_isspace(buff[PointBuf]) || (!con && stopN != 0) && (lenEn != true || len > 0))
						{
							if (save)
								*t = buff[PointBuf];
							PointBuf++;
							t++;
							len--;
						}
						else
							break;
					}
					count++;
					break;
			}
		}
		// else  // drop char in buff (no std)
		// PointBuf++; //
		PointFt++;
	}
	va_end(ap);
	return count;
}

// test
#define TEXT "-9 5 20 asd-   en d$ 3"

#define SCANF "%d %*d %2u %c %s %[^$] %d"
#define SCANA &d, &u, &c, s, ss, &test

#define PRINTF "D=%d U=%d C=%c S=%s SS=%s  test=%d  R=%d\n\r"
#define PRINTA d, u, c, s, ss, test, ret

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

	printf("TEST:\n\rref in: %s\n\r", buff);
	printf("imp:    ");
	ret = c_sscanf(buff, SCANF, SCANA);
	printf(PRINTF, PRINTA);
	printf("ref:    ");
	ret = sscanf(buff, SCANF, SCANA);
	printf(PRINTF, PRINTA);
}
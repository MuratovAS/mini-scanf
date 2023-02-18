/*
 * mini-scanf.c - Minimal scanf() implementation for embedded projects. 
 * Copyright (c) 2023 Aleksej Muratov
 */
#include <stdarg.h>
#include <stdbool.h>

#ifndef NULL
#define NULL 0
#endif

// conf
// sscanf / scanf
#define C_SSCANF
// %[..]
#define LENSCANS 10

// header
char c_getch();
bool c_getbackch(char b);

// implementation of basic dependencies
// std
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

// std
int	c_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	else
		return (0);
}

// parodies the standard
#ifdef C_SSCANF
	#define NEXTCHAR (PointBuf++)
	#define CURCHAR (buff[PointBuf])
	int c_sscanf(const char* buff, char* format, ...)
#else
	#define NEXTCHAR (charBuf=c_getch())
	#define CURCHAR (charBuf)
	int c_scanf(char* format, ...)
#endif
{
	int count = 0;

	#ifdef C_SSCANF
		int PointBuf = 0;
	#else
		char charBuf = c_getch();
	#endif

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
			// %?
			switch (format[PointFt])
			{
				case 'c':
					while (c_isspace(CURCHAR)) // ignore isspace (std)
						NEXTCHAR; //
					if (save)
						*(char*)va_arg(ap, char*) = CURCHAR;
					NEXTCHAR;
					//if (save) // ignore %* (std)
						count++;
					break;
				case 'u':
				case 'd':
					int sign = 1;
					while (!c_isdigit(CURCHAR))
					{
						if (CURCHAR == '+' || CURCHAR == '-')
							if (CURCHAR == '-')
								//if(format[PointFt] != 'u') // ignore sign (no std)
									sign = -1;
						NEXTCHAR;
					}
					long value = 0;
					while(c_isdigit(CURCHAR) && (lenEn != true || len > 0))
					{
						value *= 10;
						value += (int)(CURCHAR - '0');
						NEXTCHAR;
						len--;
					}

					if (save)
						*(int*)va_arg(ap, int*) = value * sign;
					//if (save) // ignore %* (std)
						count++;
					break;
				case ']':
				case 's':
					char* t = save ? va_arg(ap, char*) : NULL;

					while (c_isspace(CURCHAR)) // ignor isspace (std)
						NEXTCHAR; //

					while (true)
					{
						bool con = false;
						if (stopN != 0)
						{
							bool invert = (stop[0] == '^');
							con = !invert;
							for (unsigned i = (invert ? 1 : 0); i < stopN; i++)
								if (stop[i] == CURCHAR)
								{
									con = invert;
									break;
								}

							if (con == true)
								break;
						}

						if (!c_isspace(CURCHAR) || (!con && stopN != 0) && (lenEn != true || len > 0))
						{
							if (save)
								*t = CURCHAR;
							NEXTCHAR;
							t++;
							len--;
						}
						else
							break;
					}
					// add \0
					{
						if (save)
							*t = '\0';
						t++;
					}
					//if (save) // ignore %* (std)
						count++;
					break;
			}
			#ifndef C_SSCANF
			if(format[PointFt] != 'c' 
			&& format[PointFt] != 'u'
			&& format[PointFt] != 'd')
				c_getbackch(CURCHAR);
			#endif
		}
		//else  // drop char in buff (no std)
		//	NEXTCHAR; //
		PointFt++;
	}
	va_end(ap);
	return count;
}

int getch(); // header

// custom
char backch = 0;
char c_getch()
{
	if(backch == 0)
		return (char)getch();
	else
	{	
		char tmp = backch;
		backch = 0;
		return tmp;
	}
}

// new
bool c_getbackch(char b)
{
	char tmp = backch;
	backch = b;
	if (tmp != 0)
		return 1;
	else 
		return 0;
}

// test only
#include <stdio.h> // TEST only
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
int getch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~ICANON;
	//if(echo != 0)
	//newt.c_lflag &=  ECHO;
	//else
	newt.c_lflag &= ~ECHO;

	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

#define TEXT "-9 5 20 asd-   en d$"

#define SCANF "%d %*d %2u %c %s %[^$]"
#define SCANA &d, &u, &c, s, ss

#define PRINTF "D=%d U=%d C=%c S=%s SS=%s  R=%d\n\r"
#define PRINTA d, u, c, s, ss, ret

int main(int argc, char* argv[])
{
	char buff[100] = TEXT;

	char s[100];
	char ss[100];
	char c;
	int d;
	unsigned u;

	int ret;
	char buf;


	#ifdef C_SSCANF
		printf("in: %s\n\r", buff);
	#endif

	printf("imp:  ");
	#ifdef C_SSCANF
		ret = c_sscanf(buff, SCANF, SCANA);
	#else
		ret = c_scanf(SCANF, SCANA);
		buf=c_getch();
	#endif
	printf(PRINTF, PRINTA);
	#ifndef C_SSCANF
	printf(" next=d%d(%c)\n\r", buf,buf);
	#endif

	printf("ref:  ");
	#ifdef C_SSCANF
		ret = sscanf(buff, SCANF, SCANA);
	#else
		ret = scanf(SCANF, SCANA);
		buf=c_getch();
	#endif
	printf(PRINTF, PRINTA);	
	#ifndef C_SSCANF
	printf(" next=d%d(%c)\n\r", buf,buf);
	#endif
}
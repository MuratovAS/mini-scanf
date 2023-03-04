/*
 * mini-scanf - Minimal scanf() implementation for embedded projects. 
 * Copyright (c) 2023 Aleksej Muratov
 */

#include "c_scan.h"
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
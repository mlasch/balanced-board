#include <stm32f3xx.h>
#include <stdio.h>

struct __FILE { int handle; };

FILE __stdout;
FILE __stin;

int fputc(int c, FILE *stream) {
	return(ITM_SendChar(c));
}

int ferror(FILE *f)
{
	return 0;
}

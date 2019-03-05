#include <stdlib.h>
#include <stdio.h>
	
#include "..\..\modules\spi.h"

int main(void)
{
	SPI* test = spi.new(9600, 500);
	
	// stall input
	getchar();

	return 0;
}
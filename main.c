#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "spi.h"

int main(void)
{
	SPI* test = spi.new(2, 500);

	for(;;)
	{
		spi.send(test, MOT1, 0b00110010, false);
		for (int i = 0; i < 1000; i++);
	}

	return 0;
}

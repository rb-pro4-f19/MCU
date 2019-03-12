#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main()
{

	uint8_t* test2 = "dfsdfsd";

	int16_t num_of_chars = -1;
	while (test2[++num_of_chars] != '\0');

	printf("Size is %u", num_of_chars);

	getchar();

}
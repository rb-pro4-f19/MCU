#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "checksum_prototype.h"
#include "..\..\modules\chksum2.h"

int main(void)
{	
	// 4-bit checksum test
	// input:   1011 1000 1110
	// size:    3 nibbles (12 bits)
	// output:  1000

	uint16_t data_4bit		= 0b101110001110;
	uint8_t  chks_4bit		= chksum2.gen_4bit(data_4bit, 3);
	bool	 vali_4bit		= chksum2.val_8bit(data_4bit, 2, chks_4bit);
	
	printf("[4-bit checksum]\n\n");

	printf("input:\n");
	printf_binary(data_4bit);

	printf("\noutput:\n");
	printf_binary(chks_4bit);

	printf("\nvalidation:\n");
	printf("The cheksum is: %s\n", (vali_4bit) ? "valid" : "invalid");

	printf("\n");

	// 8-bit checksum test
	// input:   10101011 10001110
	// size:    2 bytes (16 bits)
	// output:  11010110

	uint16_t data_8bit[2]	= { 0b10101011, 0b10001110 };
	uint8_t	 chks_8bit		= chksum2.gen_8bit(data_8bit, 2);
	bool	 vali_8bit		= chksum2.val_8bit(data_8bit, 2, chks_8bit);

	printf("[8-bit checksum]\n\n");

	printf("input:\n");
	for (int i = 0; i < 2; i++)
	{
		printf_binary(data_8bit[i]);
	}

	printf("\noutput:\n");
	printf_binary(chks_8bit);

	printf("\nvalidation:\n");
	printf("The cheksum is: %s\n", (vali_8bit) ? "valid" : "invalid");

	printf("\n");

	// stall
	getchar();
}
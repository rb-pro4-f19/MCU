#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "checksum_prototype.h"
#include "..\..\modules\chksum.h"

int main(void)
{	
	// 4-bit checksum test
	// input:   1011'1000'1110
	// size:    3 nibbles (12 bits)
	// output:  1000

	uint16_t data_4bit		= 0b1011'1000'1110;
	uint8_t  chks_4bit		= chksum.gen_4bit(data_4bit, 3);
	bool	 vali_4bit		= chksum.val_4bit(data_4bit, 2, 0b1000);
	
	printf("[4-bit checksum]\n\n");

	printf("input:\n");
	printf_binary(data_4bit);

	printf("\noutput:\n");
	printf_binary(chks_4bit);

	printf("\nvalidation:\n");
	printf("The cheksum is: %s\n", (vali_4bit) ? "valid" : "invalid");

	printf("\n");

	// 8-bit checksum test
	// input:   1010'1011 ; 1000'1110
	// size:    2 bytes (16 bits)
	// output:  0010'0101

	uint8_t  data_8bit[2]	= { 0b1010'1011, 0b1000'1110 };
	uint8_t	 chks_8bit		= chksum.gen_8bit(data_8bit, 2);
	bool	 vali_8bit		= chksum.val_8bit(data_8bit, 2, 0b0010'0101);

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

	// 8-bit checksum test
	// input:   1010'1011 ; 1000'1110
	// size:    2 bytes (16 bits)
	// output:  0010'0101

	uint8_t  data_8bit2[3] = { 0b1110'0010, 'a', 'f' };
	uint8_t	 chks_8bit2 = chksum.gen_8bit(data_8bit, 3);

	printf("[8-bit checksum]\n\n");

	printf("input:\n");
	for (int i = 0; i < 3; i++)
	{
		printf_binary(data_8bit2[i]);
	}

	printf("\noutput:\n");
	printf_binary(chks_8bit2);

	printf("\nvalidation:\n");
	printf("The cheksum is: %s\n", (vali_8bit) ? "valid" : "invalid");

	printf("\n");

	// stall
	getchar();
}
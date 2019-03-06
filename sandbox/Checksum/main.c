#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// prototypes
void	printf_binary(uint16_t v);
uint8_t	bsd_chksum_4bit(uint16_t frame, int num_of_nibbles);
uint8_t extract_nibble(uint16_t data, int pos);
uint8_t ror_nibble(uint8_t data);
uint8_t rol_nibble(uint8_t data);

uint8_t bsd_chksum_4bit(uint16_t data, int num_of_nibbles)
{
	// https://en.wikipedia.org/wiki/BSD_checksum

	// variables
	uint8_t checksum = 0;

	// algorithm starting from Most Signifigant Nibble
	for (int i = num_of_nibbles; i >= 0; --i)
	{
		checksum = ror_nibble(checksum);
		checksum = checksum + extract_nibble(data, i);
		checksum = checksum & 0xF;
	}

	return checksum;
}

void printf_binary(uint16_t v)
{
	unsigned int mask = 1 << ((sizeof(uint16_t) << 3) - 1);

	printf("0x%04x : ", v);

	while (mask) {
		printf("%d", (v&mask ? 1 : 0));
		mask >>= 1;
	}

	printf("\n");
}

uint8_t extract_nibble(uint16_t data, int pos)
{	
	// extract nibble at position n
	// test = 0101 1111 1010 0000 -> extract_nibble(test, 2) = 1111

	return ((data & (0xF << pos * 4)) >> (pos * 4));
}

uint8_t rol_nibble(uint8_t data)
{
	return ((data << 1) | (data >> 3) & 0xF);
}

uint8_t ror_nibble(uint8_t data)
{
	return ((data >> 1) | (data << 3) & 0xF);
}

int main(void)
{	
	// 12 bit checksum test
	uint16_t test12bit_val = 0b101110001110;
	uint16_t test12bit_sum = bsd_chksum_4bit(test12bit_val, 3);

	printf("12-bit checksum\n");
	printf_binary(test12bit_val);
	printf_binary(test12bit_sum);

	printf("\n");

	// 16 bit checksum test
	uint16_t test16bit_val = 0b1011100011100000;
	uint16_t test16bit_sum = bsd_chksum_4bit(test16bit_val, 4);

	printf("16-bit checksum\n");
	printf_binary(test16bit_val);
	printf_binary(test16bit_sum);

	// stall
	getchar();
}
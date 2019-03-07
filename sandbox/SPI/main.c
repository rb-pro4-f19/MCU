#include <stdlib.h>
#include <stdio.h>
	
#include "..\..\modules\spi.h"


int main(void)
{
	//SPI* test = spi.new(2, 4);
	SPI_FRAME frm = { 0b0001, 0b00000011, 0b0000 };
	SPI_FRAME* frame = &frm;

	printf("output: 0x%x", (frame->addr << 12) | (frame->data << 4) | (frame->chksum << 0));
	// stall input
	getchar();

	return 0;
}
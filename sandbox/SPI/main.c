#include <stdlib.h>
#include <stdio.h>
	
#include "..\..\modules\spi.h"

#define FRAME_DATA(x)	(((x.addr << 8) | x.data) & 0x0FFF)

int main(void)
{
	//SPI* test = spi.new(2, 4);
	SPI_FRAME frm = { 0b0110, 0b10000011, 0b0000 };
	SPI_FRAME* frame = &frm;

	uint16_t a = FRAME_DATA(frm);

	printf("output: 0x%x", (frame->addr << 12) | (frame->data << 4) | (frame->chksum << 0));
	// stall input
	getchar();

	return 0;
}
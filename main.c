#include "modules/spi.h"
int main(void)
{
	SPI* test = spi.new(10,10);
	SPI* test1 = spi.new(10,10);
	SPI_FRAME frame_test = {3,17,2};
	frame_test.addr;

}

#include <stdint.h>
#include <stdio.h>

#include "../../modules/tp.h"

void printstuff(TIMEPOINT* tp)
{
	printf("s: %03llu | ms: %03llu | us: %03llu | ns: %03llu \n", tp->time_array[s], tp->time_array[ms], tp->time_array[us], tp->time_array[ns]);
}

int main(void)
{
	// initialize systick to 200ns per systick
	tp.init_systick(200, ns);

	// call this only from the SYSTICK ISR
	tp.systick();

	// init test TIMEPOINT instances
	TIMEPOINT* tp_test1 = tp.new();
	TIMEPOINT* tp_test2 = tp.new();
	TIMEPOINT* tp_test3 = tp.new();
	
	// set tp_test1 to current (global) time (value of SYSTICK)
	tp.set(tp_test1, tp.now());

	// set tp_test2 to 0ns:0us:10ms:2s
	tp.set(tp_test2, (uint64_t[]){ 0, 0, 10, 2 });

	// copy value of tp_test2 into tp_test3
	tp.copy(tp_test3, tp_test2);

	// get absolute difference between tp_test1 and tp_test3 in ms
	int32_t diff1 = tp.delta(tp_test1, tp_test3, ms);

	// get absolute difference between current time and tp_test2 in ms
	int32_t diff2 = tp.delta_now(tp_test2, ms);
	
	while (1)
	{
		tp.systick();
		for (int i = 0; i < 100000000; i++);

		tp.set(tp_test1, tp.now());
		printstuff(tp_test1);
	}

	getchar();
	return 0;
}
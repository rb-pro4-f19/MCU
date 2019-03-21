#include <stdint.h>
#include <stdio.h>

#include <time.h>

#include "../../modules/tp.h"

void printstuff(TIMEPOINT* tp)
{
	printf("h: %02u | m: %02u | s: %02u | ms: %03u | us: %03u \n", tp->time_array[h], tp->time_array[m], tp->time_array[s], tp->time_array[ms], tp->time_array[us]);
}

void print_time(TIMEPOINT* tp)
{
	static char time_str[6] = "00:00";
	
	sprintf_s(time_str, 6, "%02u%c%02u",
		tp->time_array[h],
		tp->time_array[s] % 2 ? ':' : ' ',
		tp->time_array[m]
	);

	printf("%s\n", time_str);
}

int main(void)
{
	// initialize systick to 200ns per systick
	tp.init_systick(1, ms);

	// call this only from the SYSTICK ISR
	tp.systick();

	// init test TIMEPOINT instances
	TIMEPOINT* tp_test1 = tp.new();
	TIMEPOINT* tp_test2 = tp.new();
	TIMEPOINT* tp_test3 = tp.new();

	// manually read value of a TIMEPOINT
	// DONT'T WRITE TO IT!
	uint32_t tp_test1_minutes = tp_test1->time_array[m];
	
	// set tp_test1 to current (global) time (value of SYSTICK)
	tp.set(tp_test1, tp.now());

	// set tp_test2 to 0us:10ms:2s:0m:0h
	tp.set(tp_test2, (uint16_t[]) { 0, 10, 2, 0, 0 });

	// copy value of tp_test2 into tp_test3
	tp.copy(tp_test3, tp_test2);

	// get absolute difference between tp_test1 and tp_test3 in ms
	int64_t diff1 = tp.delta(tp_test1, tp_test3, ms);

	// get absolute difference between current time and tp_test2 in ms
	int64_t diff2 = tp.delta_now(tp_test2, ms);

	while (1)
	{
		tp.systick();

		tp.set(tp_test1, tp.now());
		printstuff(tp_test1);
		//print_time(tp_test1);
	}

	getchar();
	return 0;
}
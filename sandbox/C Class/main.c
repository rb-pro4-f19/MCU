#include <stdlib.h>
#include <stdio.h>

#include "src/test.h"

void main()
{
	TEST	t_obj1 = t.new(10, 3);
	TEST*	t_obj2 = t.new_ptr(2, 3);

	printf("Difference: %d", t.diff_a(&t_obj1, t_obj2));

	return;
}
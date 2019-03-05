#include "test.h"

static TEST new(int a, int b)
{
	return(TEST) { .a = a, .b = b };
}

static TEST* new_ptr(int a, int b)
{
	TEST* tst = malloc(sizeof(TEST));

	tst->a = a;
	tst->b = b;

	return tst;
}

static int diff_a(TEST * t1, TEST * t2)
{
	return(t1->a - t2->a);
}

const struct TEST_CLASS t =
{
	.new		= &new,
	.new_ptr	= &new_ptr,
	.diff_a		= &diff_a
};

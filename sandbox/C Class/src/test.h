#include <stdlib.h>

// https://www.pvv.ntnu.no/~hakonhal/main.cgi/c/classes/

typedef struct TEST TEST;

// Object declaration
struct TEST
{
	int a, b;
};

// Static Methods (constructor etc.) in variable t
extern const struct TEST_CLASS
{
	TEST	(*new)(int a, int b);				// funptr to constructor
	TEST*	(*new_ptr)(int a, int b);			// funptr to constructor2
	int		(*diff_a)(TEST * t1, TEST * t2);	// funptr to method
} t;
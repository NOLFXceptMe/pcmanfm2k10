#include <stdio.h>
#include "parser.h"

int main()
{
	int ret_val = parse("test.desktop");
	if(ret_val >0) printf("Successfully parsed test.desktop");
	printf("Test 1 completed");

	return 0;
}

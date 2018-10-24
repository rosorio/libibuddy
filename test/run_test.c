#include "ibuddy.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	ibuddy_ref ibuddy;

	ibuddy = ibuddy_open(0);
	if(ibuddy == NULL){
		printf("can't open, exit\n");
		exit(1);
	}
	ibuddy_reset_state(ibuddy);

	ibuddy_full_test(ibuddy);

	ibuddy_reset_state(ibuddy);

	ibuddy_close(ibuddy);
	exit(0);
	
}

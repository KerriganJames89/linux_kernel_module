#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "wrappers.h"

int main(int argc, char **argv) {
	long ret;

	if (argc != 2) {
		printf("wrong number of args\n");
		return -1;
	}
	
	if (strcmp(argv[1], "--start") == 0) {
		ret = start_elevator();
		printf("Start elevator returned %ld\n", ret);
	}
	else if (strcmp(argv[1], "--stop") == 0) {
		ret = stop_elevator();
		printf("Stop elevator returned %ld\n", ret);
	}
	else
		return -1;
	
	return 0;
}

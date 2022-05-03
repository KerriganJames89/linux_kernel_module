#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include "wrappers.h"

int rnd(int min, int max)
{
	return rand() % (max - min + 1) + min; //slight bias towards first k
}

int main(int argc, char **argv)
{
	int type;
	int start;
	int dest;
	int i;
	int num;
	srand(time(0));

	if (argc != 2)
	{
		printf("wrong number of args. producer.x num_of_requests\n");
		return -1;
	}
	sscanf(argv[1], "%d", &num);
	for (i = 0; i < num; i += 1)
	{
		type = rnd(0, 2);
		start = rnd(1, 10);
		do
		{
			dest = rnd(1, 10);
		} while (dest == start);
		long ret = issue_request(start, dest, type);
		printf("Issue (%d, %d, %d) returned %ld\n", start, dest, type, ret);
	}
	return 0;
}
all:
	gcc -o part1.x part1.c
	gcc -o empty.x empty.c
	strace -o part1.trace ./part1.x
	strace -o empty.trace ./empty.x

clean:
	rm *.x *.trace


obj-m += my_timer.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=/usr/src/my_timer modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=/usr/src/my_timer clean

# FSU COP4610 Project 2
- This project has three parts 
    1) System-call Tracing 
    2) Timer Project
    3) Elevator Project

# Team Members
- Sam Anderson 
- Luis Corps
- James Kerrigan

# File listing
- Our project multiple contains files and directories

    1) Part 1
        - empty.c 
        - makefile 	
        - part1.c
    2) Part 2
        - my_timer.c
        - Makefile   
    3) Part 3
        - elevator_syscall
            - elevator_syscall.c
            - Makefile
        - kernel_module 
            - elevator.c   
            - Makefile

# Makefile description
- **Part 1**
    - How to run how program:
        1) type `make` which will create the following:  `empty.x`, `part1.x`, `empty.trace`, `part1.trace` 

- **Part 2** 
    - How to run how program:
        1) type `sudo make` which will create the following:  `Module.symvers`, `my_timer.ko`, `my_timer.mod.o`, `modules.order`, `my_timer.mod.c`, `my_timer.o` 

- **Part 3**
    - How to run how program:
        1) **elevator_syscall**
            - directory created inside the `linux-4.19.98` kernel will run when the kernel is complied
        2) **kernel_module**
            - will compile elevator.c kernel module and create create the `elevator.ko` where you can run `sudo insmod elevator.ko` to install and `sudo rmmod elevator` to remove
         

# Group Member Contribution
- **Sam Anderson**
    - Created github repo
    - **Part 1** 
        - Formating 
    - **Part 2**
        - Researched and contributed to `procfile_read`
    - **Part 3**
        - Provided research for elevator_syscalls.c 
        - Formated code and provided research
        - Contributed to `print_elevator`, `passenger_name`, `elevator_state`, `elevator_move`, `can_board`, `thread_run`, `elevator_init`, `add_passenger_waiting`, `elevator_proc_open`, `elevator_proc_read`, `elevator_proc_release`, `unload_passengers`, `thread_init_parameter`
- **Luis Corps**
	- **Part 1**
        - Reviewed and tested
    - **Part 2**
        - Formated code and tested as well as providing psudeo code for `procfile_read`
    - **Part 3**   
        - Provided research for elevator_syscalls.c  
        - Formated code and provided research
        - Contributed to `unload_passengers`, `stop_elevator`, `start_elevator`, `thread_run`, `can_board`, `add_passenger_elevator`, `elevator_proc_open`, `elevator_proc_read`,  `elevator_proc_release`, `unload_passengers`,`thread_init_parameter`
- **James Kerrigan**
    - Part 1
        -  `Makefile`, `empty.c`, `part1.c`
    - Part 2    
        - Contributed to `procfile_read`, `timer_init`, `timer_cleanup`
    - Part 3
        - Implmented `elevator_syscalls.c` and `Makefile`
        - `Makefile` for elevator.c
        - Formated code and provided research
        - - Contributed to `can_board`, `thread_run`, `modify_elevator_types`, `elevator_init`, `unload_passengers`, `start_elevator`, `add_passenger_elevator`, `elevator_proc_open`,  `elevator_proc_read`, `elevator_proc_release`, `unload_passengers`, `thread_init_parameter`
        - 
# Bugs
There were many problems with our schedule() implementations and other similar kernel scheduling macros, so ultimately we chose to sleep the thread every so often when it became idle or thrown into an offline state.

# Comments
We had multiple problems trying to run similar code on each other's machine, and our final version would not execute on one of our group member's computer, but we made sure the version worked with a group majority.

Some Kernel components in our implementations may have been named differently, however, we saw no requirements for a naming process for these particular components and we realized that rebuilding these Kernels was time spent not working on the module.

# Special considerations
- Github log will not accurately represent contribution since majority of code was shared through discord. Also made us of zoom by having meetings to review/modify members contributions

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/linkage.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/wait.h> 
#include <linux/sched.h> 

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("project-2 | part3 - elevator");

/* gerneral info */
#define ENTRY_NAME "elevator"
#define ENTRY_SIZE 1000
#define PERMS 0644

/* boolean values */
#define TRUE 1
#define FALSE 0

/* elevator states */
#define IDLE 0
#define UP 1
#define DOWN 2
#define LOADING 3
#define OFFLINE 4
#define REQUESTING 5

/* passengers info */
#define NUM_PASSENGER_TYPES 3
#define GRAPE 0
#define SHEEP 1
#define WOLF 2

/* elevator info */
#define MAX_PASSENGER 10
#define MAX_FLOOR 10

/************ global variables *************/
static struct file_operations fops;

static char *message;

static int read_p;

static int stopped;

static int buffer_request = REQUESTING;

struct thread_parameter
{
    int id;
    struct task_struct *kthread;
    //struct mutex mutex;
};

struct thread_parameter elevator_thread;

struct
{
    int grapes_on;
    int wolves_on;
    int sheeps_on;
    int total_cnt;
    int serviced;
    int current_floor;
    int next;
    int state;
	int state_switch;
    struct list_head on_board; // passengers on board
} elevator;

struct
{
    int total_waiting;
    struct list_head waiting[10]; // passengers waiting on each floor
} wait_queue;

typedef struct passenger
{
    int type;
    int dest_floor;
    int start_floor;
    const char *name;
    int wait_flag;
    int elevator_flag;
    struct list_head list;
} Passenger;

struct mutex elevator_mutex;

wait_queue_head_t sleep_queue;

/************ function prototypes **********/
char *passenger_name(int type); // return name of passenger arrcording to their type

char *elevator_state(int state); // return string of elevator state IDLE, UP, etc.

void modify_elevator_types(Passenger *p, int i);

void elevator_move(void);

int can_board(Passenger *p);

long start_elevator(void);

long issue_request(int start_floor, int destination_floor, int type);

long stop_elevator(void);

int elevator_init(void);

void elevator_exit(void);

int add_passenger_waiting(int type, int start, int destination);

int add_passenger_elevator(void);

int print_elevator(void);

int elevator_proc_open(struct inode *sp_inode, struct file *sp_file);

ssize_t elevator_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset);

int elevator_proc_release(struct inode *sp_inode, struct file *sp_file);

void unload_passengers(void);

int check_waiting(void);

void check_requests(void);

int thread_run(void *data);

void thread_init_parameter(struct thread_parameter *parm);

// ********************************************
extern long (*STUB_start_elevator)(void);
extern long (*STUB_issue_request)(int, int, int);
extern long (*STUB_stop_elevator)(void);

// ********************************************
void modify_elevator_types(Passenger *p, int i)
{
    if (p->type == WOLF)
    {
        elevator.wolves_on += i;
    }
    else if (p->type == GRAPE)
    {
        elevator.grapes_on += i;
    }
    else
    {
        elevator.sheeps_on += i;
    }
}

void elevator_move()
{
    /* update state and next value */
    if (elevator.current_floor > elevator.next)
    {	
		if(elevator.state != OFFLINE)
		{
			elevator.state = DOWN;
		}
		
		ssleep(1);
		elevator.current_floor--;
    }
	
    else if (elevator.current_floor < elevator.next)
    {
        if(elevator.state != OFFLINE)
		{
			elevator.state = UP;
		}
		
		ssleep(1);
		elevator.current_floor++;
    }
	
	else
	{
		ssleep(1); //sleep for a second if elevator doesn't have a new destination;
	}
}

int check_waiting(void)
{	
	int i = 0;
	for(;i < 10; i++)
	{
		
		if (!list_empty(&wait_queue.waiting[i]))
		{	
			break;
		}
	}
	
	return i + 1;
}

void check_requests()
{
	if(buffer_request == OFFLINE || buffer_request == IDLE)
	{
		elevator.state_switch = buffer_request;
		buffer_request = REQUESTING;
	}
}

char *passenger_name(int type)
{
    char *name;
    if (type == GRAPE)
    {
        name = "G";
    }
    else if (type == SHEEP)
    {
        name = "S";
    }
    else if (type == WOLF)
    {
        name = "W";
    }
    else
    {
        name = " ";
    }
    return name;
}

char *elevator_state(int state)
{
    char *name;
    if (elevator.state == IDLE)
    {
        name = "IDLE";
    }
    else if (elevator.state == UP)
    {
        name = "UP";
    }
    else if (elevator.state == DOWN)
    {
        name = "DOWN";
    }
    else if (elevator.state == LOADING)
    {
        name = "LOADING";
    }
    else if (elevator.state == OFFLINE)
    {
        name = "OFFLINE";
    }
    else
    {
        name = " ";
    }
    return name;
}

int thread_run(void *data)
{
	set_current_state(TASK_INTERRUPTIBLE);
    printk(KERN_DEBUG "thread_run\n");

    Passenger* p;
    
    while (!kthread_should_stop())
    {
        check_requests();
        

        if(elevator.total_cnt > 0)
        {
            if(mutex_lock_interruptible(&elevator_mutex) == 0)
            {
                elevator.state = LOADING;
				unload_passengers();
                mutex_unlock(&elevator_mutex);
            }
			
			check_requests();
        }
        
        if(elevator.state_switch != OFFLINE)
        {
            if(mutex_lock_interruptible(&elevator_mutex) == 0)
            {
                elevator.state = LOADING;
                add_passenger_elevator();
                mutex_unlock(&elevator_mutex);
            }

            check_requests();
        }
        
        if(elevator.total_cnt > 0)
        {
            p = list_first_entry(&elevator.on_board, Passenger, list);
            elevator.next = p->dest_floor;
            elevator_move();
        }
            
        else if((wait_queue.total_waiting > 0) && (elevator.state_switch != OFFLINE))
        {
            if(mutex_lock_interruptible(&elevator_mutex) == 0)
            {
                elevator.next = check_waiting();
                mutex_unlock(&elevator_mutex);
            }

            elevator_move();
        }

        else
        {
            if(elevator.state_switch == OFFLINE)
            {
                elevator.state = OFFLINE;
            }
            
            else
            {
                elevator.state = IDLE;
				
            }
			
			ssleep(2);
            
        }

    }
	
    return 0;
}

void thread_init_parameter(struct thread_parameter *parm)
{
    printk(KERN_DEBUG "thread_init\n");

    static int id = 1;
    int i;
    parm->id = id++;
    parm->kthread = kthread_run(thread_run, parm, "thread example %d", parm->id);
}

long issue_request(int start_floor, int destination_floor, int type)
{
    printk("issue_request: %d, %d --> %d\n", type, start_floor, destination_floor);
	
    /* check if passenger is already on the correct floor */
    if (start_floor == destination_floor)
    {
        elevator.serviced++;
    }
    else
    {
		if(mutex_lock_interruptible(&elevator_mutex) == 0)
		{
        	add_passenger_waiting(type, start_floor, destination_floor);
		    mutex_unlock(&elevator_mutex);
		}
		
    }
    return 0;
}

long stop_elevator(void)
{
    printk(KERN_DEBUG "stop_elevator\n");
    
	if (elevator.state != OFFLINE)
    {
        printk("Setting elevator to OFFLINE\n");
        buffer_request = OFFLINE;
    }
	
    return 0;
}

long start_elevator(void)
{
    printk(KERN_DEBUG "start_elevator\n");
	
    /* check if elevator is already active */
	
	if (elevator.state != IDLE)
    {
        printk("Setting elevator to IDLE\n");
		buffer_request = IDLE;
    }


    return 0;
}

int elevator_init(void)
{
    printk(KERN_DEBUG "elevator_init\n");

    fops.open = elevator_proc_open;
    fops.read = elevator_proc_read;
    fops.release = elevator_proc_release;

    STUB_start_elevator = start_elevator;
    STUB_issue_request = issue_request;
    STUB_stop_elevator = stop_elevator;

    /* init elevator */
    elevator.total_cnt = 0;
    elevator.current_floor = 1;
    elevator.next = 1;
    elevator.serviced = 0;
    elevator.sheeps_on = 0;
    elevator.grapes_on = 0;
    elevator.wolves_on = 0;
    elevator.state = OFFLINE;
	elevator.state_switch = OFFLINE;
    INIT_LIST_HEAD(&elevator.on_board);
	mutex_init(&elevator_mutex);
	
	init_waitqueue_head(&sleep_queue); 

    /* init wait_queue */
    wait_queue.total_waiting = 0;
    int inx = 0;
    for (inx = 0; inx < MAX_FLOOR; ++inx)
    {
        /* init array of 10 linked list */
        INIT_LIST_HEAD(&wait_queue.waiting[inx]);
    }

    if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops))
    {
        printk("Error in proc_create");
        remove_proc_entry(ENTRY_NAME, NULL);
        return -ENOMEM;
    }

    thread_init_parameter(&elevator_thread);
    if (IS_ERR(elevator_thread.kthread))
    {
        printk(KERN_WARNING "error spawning thread");
        remove_proc_entry(ENTRY_NAME, NULL);
        return PTR_ERR(elevator_thread.kthread);
    }
    return 0;
}

void elevator_exit(void)
{
    printk(KERN_DEBUG "elevator_exit\n");
    kthread_stop(elevator_thread.kthread);
    remove_proc_entry(ENTRY_NAME, NULL);
    mutex_destroy(&elevator_mutex);
    printk(KERN_INFO "thread stopped");
    STUB_start_elevator = NULL;
    STUB_issue_request = NULL;
    STUB_stop_elevator = NULL;
}

int add_passenger_waiting(int type, int start, int destination)
{
    char *name = passenger_name(type);
    Passenger *p;

    p = kmalloc(sizeof(Passenger) * 1, __GFP_RECLAIM);
    if (p == NULL)
    {
        return -ENOMEM;
    }

    /* create passenger */
    p->type = type;
    p->name = name;
    p->dest_floor = destination;
    p->start_floor = start;

    /* add passenger to the wait queue */
    list_add_tail(&p->list, &wait_queue.waiting[p->start_floor - 1]); /* insert at back of list */
    wait_queue.total_waiting++;
    return 0;
}

int add_passenger_elevator(void)
{
    Passenger *p;
    struct list_head *tmp;
    struct list_head *cur;
    elevator.state = LOADING;
	
	if (elevator.total_cnt < MAX_PASSENGER)
	{

		/* safe to allow deletion from list */
		list_for_each_safe(cur, tmp, &wait_queue.waiting[elevator.current_floor - 1])
		{
			p = list_entry(cur, Passenger, list);
			if (can_board(p) == TRUE)
			{
				// ssleep(1);
				list_del(cur);
				list_add_tail(&p->list, &elevator.on_board); /* insert at back of on_board */

				modify_elevator_types(p, 1);
				wait_queue.total_waiting--;

				if (++elevator.total_cnt == MAX_PASSENGER)
				{
					break;
				}
			}
            else 
            {
                break;
            }
		}
	}

    return 0;
}

void unload_passengers(void)
{
    Passenger *p;
    struct list_head *tmp;
    struct list_head *cur;

    list_for_each_safe(cur, tmp, &elevator.on_board)
    {
        p = list_entry(cur, Passenger, list);
		
		int i = 0;
        // unload passegers from elevator
        if (p->dest_floor == elevator.current_floor)
        {
            ssleep(1);
            modify_elevator_types(p, -1);
            list_del(cur); // removes from linked list
            kfree(p);
            elevator.total_cnt--;
            elevator.serviced++;
        }
    }
}

int can_board(Passenger *p)
{
    return !((elevator.sheeps_on > 0 && p->type == GRAPE) || 
             (elevator.wolves_on > 0 && p->type == SHEEP));
}

int print_elevator(void)
{
    Passenger *p;
    struct list_head *temp;

    char *buf = kmalloc(sizeof(char) * 100, __GFP_RECLAIM);
    if (buf == NULL)
    {
        printk(KERN_WARNING "print_passengers");
        return -ENOMEM;
    }

    /* init message buffer */
    strcpy(message, "");

    /* headers, print to temporary then append to message buffer */
    sprintf(buf, "Elevator state: %s\n", elevator_state(elevator.state));
    strcat(message, buf);

    sprintf(buf, "Elevator state: ");
    strcat(message, buf);

    sprintf(buf, "%d wolves, ", elevator.wolves_on);
    strcat(message, buf);

    sprintf(buf, "%d sheep, ", elevator.sheeps_on);
    strcat(message, buf);

    sprintf(buf, "%d grape\n", elevator.grapes_on);
    strcat(message, buf);

    sprintf(buf, "Current Floor: %d\n", elevator.current_floor);
    strcat(message, buf);

    sprintf(buf, "Number of passengers: %d\n", elevator.total_cnt);
    strcat(message, buf);

    sprintf(buf, "Number passengers waiting: %d\n", wait_queue.total_waiting);
    strcat(message, buf);

    sprintf(buf, "Number passengers serviced: %d\n", elevator.serviced);
    strcat(message, buf);

    /* print floors backwards */
    int i;
    for (i = MAX_FLOOR; i > 0; --i)
    {
        if (elevator.current_floor == i)
        {
            sprintf(buf, "[%s] Floor %d: ", "*", i);
        }
        else
        {
            sprintf(buf, "[ ] Floor %d: ", i);
        }
        strcat(message, buf);

        list_for_each(temp, &wait_queue.waiting[i - 1])
        {
            p = list_entry(temp, Passenger, list);
            sprintf(buf, " %s", p->name);
            strcat(message, buf);
        }
        sprintf(buf, "\n");
        strcat(message, buf);
    }

    /* trailing newline to separate file from commands */
    strcat(message, "\n");
    kfree(buf);
    return 0;
}

int elevator_proc_open(struct inode *sp_inode, struct file *sp_file)
{
    read_p = 1;
    message = kmalloc(sizeof(char) * ENTRY_SIZE, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
    if (message == NULL)
    {
        printk(KERN_WARNING "elevator_proc_open");
        return -ENOMEM;
    }

    return print_elevator();
}

ssize_t elevator_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset)
{
    int len = strlen(message);
    read_p = !read_p;
    if (read_p)
    {
        return 0;
    }

    copy_to_user(buf, message, len);
    return len;
}

int elevator_proc_release(struct inode *sp_inode, struct file *sp_file)
{
    kfree(message);
    return 0;
}

// *****************************************
module_init(elevator_init);
module_exit(elevator_exit);

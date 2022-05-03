#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define BUF_LEN 100
#define TRUE 1
#define FALSE 0

/************ Prototypes *******************/
static ssize_t procfile_read(struct file* file, char * ubuf, size_t count, loff_t *ppos);

static int timer_init(void);

static void timer_cleanup(void);

/************ global variables *************/
static struct proc_dir_entry* proc_entry; // pointer to procentry

static char msg[BUF_LEN];                 // buffer to store read/write message

static int procfs_buf_len;                // variable to hold length of message

static struct timespec last_time;

static int first_time = TRUE;

static long long elapse_sec = 0;

static long elapse_nsec = 0;

static long long prev_sec = 0;

static long prev_nsec = 0;

static struct proc_dir_entry *ent;

static struct file_operations procfile_fops = 
{
    .owner = THIS_MODULE,
    .read = procfile_read
};

/************ Start of definitions ***********/

static ssize_t procfile_read(struct file* file, char * ubuf, size_t count, loff_t *ppos)
{
	if (*ppos > 0 || count < procfs_buf_len) // check if data already read and if space in user buffer
	{
		return 0;
	}
	/* get current time */
	struct timespec cur_time = current_kernel_time();

	if (first_time) 
	{
		/* first entry only print current time */	
        procfs_buf_len = snprintf(
            msg,
            BUF_LEN,
            "current_time: %lld.%.9ld\n",
            (long long)cur_time.tv_sec,
            cur_time.tv_nsec
        );
		
		prev_sec = (long long)cur_time.tv_sec;
		prev_nsec = cur_time.tv_nsec;
		first_time = FALSE; // set first time to false
    }
	else
	{
		/* calculate elapsed time */
		if ((cur_time.tv_nsec - prev_nsec) <= 0)
		{
			elapse_nsec = prev_nsec - cur_time.tv_nsec;
			elapse_sec = (((long long)cur_time.tv_sec - prev_sec) - 1);
		}
		else
		{
			elapse_nsec = cur_time.tv_nsec - prev_nsec;
			elapse_sec = (long long)cur_time.tv_sec - prev_sec;
		}
		/* debuging */
		// printk("cur_time is: %lld\n", (long long)cur_time.tv_sec);
		// printk("cur_ntime is: %.9ld\n", cur_time.tv_nsec);
		// printk("elapse_sec is: %lld\n", elapse_sec);
		// printk("elapse_nsec is: %.9ld\n", elapse_nsec);

		procfs_buf_len = snprintf(
			msg,
			BUF_LEN,
			"current_time: %lld.%.9ld\nelapsed time: %lld.%.9ld\n",
			(long long)cur_time.tv_sec,
			cur_time.tv_nsec,
			elapse_sec,
			elapse_nsec
		);
		/* set previous time */
		prev_sec = (long long)cur_time.tv_sec;
		prev_nsec = cur_time.tv_nsec;
	}

	if (copy_to_user(ubuf, msg, procfs_buf_len)) //send data to user buffer
	{
		return -EFAULT;
	}

	*ppos = procfs_buf_len; // update position
	printk(KERN_INFO "gave to user %s\n", msg);

	return procfs_buf_len; // return number of characters read
}

static int timer_init(void)
{
	proc_entry = proc_create("timer", 0444, NULL, &procfile_fops);
	return proc_entry == NULL ? -ENOMEM : 0;
}

static void timer_cleanup(void)
{
	proc_remove(proc_entry);
}
/************ End of definitions ***********/

module_init(timer_init);
module_exit(timer_cleanup)
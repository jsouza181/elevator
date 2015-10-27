#include <linux/time.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timekeeping.h>
#include <asm-generic/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("NAMES");
MODULE_DESCRIPTION("Hello World Test Module");

#define ENTRY_NAME "my_xtime"
#define PERMS 0644
#define PARENT NULL;

static struct file_operations fops;

static char *message;
static int read_p;
static struct timespec previousTime;

static int my_xtime_init(void);
static void my_xtime_exit(void);
int my_xtime_proc_open(struct inode *sp_inode, struct file *sp_file);
ssize_t my_xtime_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset);
int my_xtime_proc_release(struct inode *sp_inode, struct file *sp_file);
int timespec_subtract(struct timespec *result, struct timespec *x, struct timespec *y);

static int my_xtime_init(void) {
	previousTime.tv_sec = 0;
	printk("/proc/%s create\n", ENTRY_NAME); fops.open = my_xtime_proc_open;
	fops.read = my_xtime_proc_read;
	fops.release = my_xtime_proc_release;
	if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops)) { 
		printk("ERROR! proc_create\n"); 
		remove_proc_entry(ENTRY_NAME, NULL);
		return -ENOMEM;
	}
	return 0;
}

static void my_xtime_exit(void) {
	remove_proc_entry(ENTRY_NAME, NULL);
	printk("Removing /proc/%s.\n", ENTRY_NAME);
} 


int my_xtime_proc_open(struct inode *sp_inode, struct file *sp_file) { 
	char buf[128];
	struct timespec currentTime;
	struct timespec elapsedTime;
	printk("proc called open\n");
	read_p = 1;
	message = kmalloc(sizeof(char) * 128, __GFP_WAIT | __GFP_IO | __GFP_FS);
	if (message == NULL) {
		printk("ERROR, my_xtime_proc_open");
		return -ENOMEM;
	}
        
	currentTime = current_kernel_time();

	if(previousTime.tv_sec != 0) { // If previous read was made
	  // Subtract times
	  if(timespec_subtract(&elapsedTime, &currentTime, &previousTime)) {
	    printk("ERROR: negative elapsed time\n");
	    return 1;
 	  }
	  sprintf(buf, "current time: %lu.%lu\nelapsed time: %lu.%lu\n", 
		currentTime.tv_sec, currentTime.tv_nsec, elapsedTime.tv_sec, 
		elapsedTime.tv_nsec);
	}
	else // Otherwise, this was the first /proc read
	  sprintf(buf, "current time: %lu.%lu\n", currentTime.tv_sec, 
		currentTime.tv_nsec);

	strcpy(message, buf);
	previousTime.tv_sec = currentTime.tv_sec;
	previousTime.tv_nsec = currentTime.tv_nsec;
	return 0;
}

/* Function to subtract two timeval obtained from the GNU C Library Manual:
 * http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html#Elapsed-Time
 * Subtract the `struct timespec' values X and Y,
 * storing the result in RESULT.
 * Return 1 if the difference is negative, otherwise 0. 
 */
int timespec_subtract (struct timespec *result, struct timespec *x, struct timespec *y)
{
  	/* Perform the carry for the later subtraction by updating y. */
  	if (x->tv_nsec < y->tv_nsec) {
    	  int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
    	  y->tv_nsec -= 1000000000 * nsec;
    	  y->tv_sec += nsec;
  	}
  	if (x->tv_nsec - y->tv_nsec > 1000000000) {
    	int nsec = (x->tv_nsec - y->tv_nsec) / 1000000000;
    	y->tv_nsec += 1000000000 * nsec;
    	y->tv_sec -= nsec;
  	}

  	/* Compute the time remaining to wait.
     	tv_nsec is certainly positive. */
  	result->tv_sec = x->tv_sec - y->tv_sec;
  	result->tv_nsec = x->tv_nsec - y->tv_nsec;

  	/* Return 1 if result is negative. */
  	return x->tv_sec < y->tv_sec;
}

ssize_t my_xtime_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset) {
	int len = strlen(message);
	read_p = !read_p; 
	if (read_p) {
		return 0; }
	printk("proc called read\n"); copy_to_user(buf, message, len); return len;
}

int my_xtime_proc_release(struct inode *sp_inode, struct file *sp_file) {
	printk("proc called release\n"); kfree(message);
	return 0;
}



module_init(my_xtime_init);
module_exit(my_xtime_exit);


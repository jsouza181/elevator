#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <syscalls.h>
#include "elevator.h"
#include "elevatorproc.h"
#include "scheduler.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Souza, Ricardo Castilla, Philip Scott, Conor Stephen");
MODULE_DESCRIPTION("Simple module designed to illustrate scheduling");

// Thread to check the floor queue for floors to visit
static struct task_struct *requestServicer;
// might not be necessary: static struct task_struct *requestScheduler;

static int elevator_init(void) {
  printk("Inserting Elevator\n");
  elevator_syscalls_create();

  requestServicer = kthread_run(serviceRequests, NULL, "request servicer thread");
	if (IS_ERR(requestServicer)) {
		printk("ERROR! kthread_run, request servicer thread\n");
		return PTR_ERR(requestServicer);
	}

  return elevatorProcCreate();
}

static void elevator_exit(void) {
  int ret;

  printk("Removing elevator\n");
  elevator_syscalls_remove();

  ret = kthread_stop(requestServicer);
  	if(ret != -EINTR)
  		printk("Request servicer thread has stopped\n");

  elevatorProcRemove();
}

module_init(elevator_init);
module_exit(elevator_exit);

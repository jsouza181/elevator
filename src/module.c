#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <syscalls.h>
#include "elevator.h"
#include "elevatorproc.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Souza, ");
MODULE_DESCRIPTION("Simple module designed to illustrate scheduling");

static int elevator_init(void) {
  printk("Inserting Elevator\n"); 
  elevator_syscalls_create();  
  return elevatorProcCreate();
}

static void elevator_exit(void) {
  printk("Removing elevator\n");
  elevator_syscalls_remove();
  elevatorProcRemove();
}

module_init(elevator_init);
module_exit(elevator_exit);

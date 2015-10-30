#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <asm-generic/uaccess.h>
#include "elevator.h"
#include "elevatorproc.h"

#define ENTRY_NAME "elevator"
#define PERMS 0644
#define PARENT NULL;

static struct file_operations fops;
char message[2048];
static int read_p;
// Need to be able to access elevator and floor data for /proc reads

int elevatorProcOpen(struct inode *sp_inode, struct file *sp_file);
ssize_t elevatorProcRead(struct file *sp_file, char __user *buf, size_t size, loff_t *offset);
int elevatorProcRelease(struct inode *sp_inode, struct file *sp_file);

// Called when module is inserted.
int elevatorProcCreate(void) {
  fops.open = elevatorProcOpen;
  fops.read = elevatorProcRead;
  fops.release = elevatorProcRelease;

  if(!proc_create(ENTRY_NAME, PERMS, NULL, &fops)) {
    printk("ERROR: could not create proc entry\n");
    remove_proc_entry(ENTRY_NAME, NULL);
    return -ENOMEM;
  }

  return 0;
}

// Called when module is removed.
void elevatorProcRemove(void) {
  remove_proc_entry(ENTRY_NAME, NULL);
}


int elevatorProcOpen(struct inode *sp_inode, struct file *sp_file) {
  char numToString[16]; // Used to add ints to the message
  int i;

  read_p = 1;

  strcpy(message, "Elevator data:\n");
/*
  sprintf(numToString, "  State: %d\n  Current Floor: %d\n  Next Floor:  %d\n",
      osMagicElv.state, osMagicElv.currentFloor, osMagicElv.destFloor);
  strcat(message, numToString);
*/
  strcat(message, "\nFloor weights:\n");

  for(i = 0; i < 10; i++) {
    sprintf(numToString, "  Floor [%d]: %d\n", i + 1, osMagicFloors[i].totalWeight);
    strcat(message, numToString);
  }

  return 0;
}
ssize_t elevatorProcRead(struct file *sp_file, char __user *buf, size_t size, loff_t *offset) {
  int len = strlen(message);
  read_p = !read_p;

  if(read_p) {
    return 0;
  }

  copy_to_user(buf, message, len);
  return len;
}
int elevatorProcRelease(struct inode *sp_inode, struct file *sp_file) {
  //deallocate if needed later
  return 0;
}

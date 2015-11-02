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
  char numToString[128]; // Temp string used to add ints to the message
  int i;

  read_p = 1;

  strcpy(message, "Elevator data:\n");
  sprintf(numToString, "  State: %d\n  Current Floor: %d\n  Next Floor: %d\n",
      osMagicElv.state, osMagicElv.currentFloor + 1, osMagicElv.nextFloor + 1);
  strcat(message, numToString);

  sprintf(numToString, "  Total Weight Load: %d.%d\n  Total Passenger Load: %d\n",
      osMagicElv.totalWeightWhole, osMagicElv.totalWeightFrac, osMagicElv.totalPass);
  strcat(message, numToString);

  strcat(message, "Floor data:\n");
  for(i = 9; i >= 0; i--) {
    if(osMagicElv.currentFloor == i) {
      sprintf(numToString, " *Floor [%d]:\n    Total Weight Load: %d.%d\n",
        i + 1, osMagicFloors[i].totalWeightWhole, osMagicFloors[i].totalWeightFrac);
    }
    else {
      sprintf(numToString, "  Floor [%d]:\n    Total Weight Load: %d.%d\n",
        i + 1, osMagicFloors[i].totalWeightWhole, osMagicFloors[i].totalWeightFrac);
    }
    strcat(message, numToString);

    sprintf(numToString, "    Total Passenger Load: %d\n    Passengers Serviced: %d\n",
        osMagicFloors[i].totalPass, osMagicFloors[i].totalServed);
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

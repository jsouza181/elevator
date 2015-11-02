#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include "elevator.h"

// Threaded function to handle elevator movement.
int serviceRequests(void *data) {
  ssleep(1);

  while(!kthread_should_stop()) {

    mutex_lock_interruptible(&floor_mutex);
    // Determine the next floor to visit
    osMagicElv.nextFloor = scheduleNextFloor();
    mutex_unlock(&floor_mutex);

    // Sleep for movement
    ssleep(2);
    moveToFloor(osMagicElv.nextFloor);

    // Sleep for load/unload
    ssleep(1);
    unloadPassengers();
    loadPassengers();
  } // while

  osMagicElv.state = STOPPED;
  return 0;
}

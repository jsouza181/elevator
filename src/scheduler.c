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

  // Do not change any states if elevator is STOPPED.
  while(!kthread_should_stop()) {
<<<<<<< HEAD

    mutex_lock_interruptible(&floor_mutex);
    // Determine the next floor to visit
    osMagicElv.nextFloor = scheduleNextFloor();
    mutex_unlock(&floor_mutex);

    // Sleep for movement
=======
    // Elevator movement.
    if(osMagicElv.state != STOPPED)
      osMagicElv.state = osMagicElv.direction;

>>>>>>> bb99aba4f7c068466343ff5fd6a473c536e0fb1c
    ssleep(2);
    moveToFloor(osMagicElv.nextFloor);

    // Load/unload
    if(osMagicElv.state != STOPPED)
      osMagicElv.state = LOADING;

    ssleep(1);
    unloadPassengers();
<<<<<<< HEAD
    loadPassengers();
  } // while
=======
>>>>>>> bb99aba4f7c068466343ff5fd6a473c536e0fb1c

    // Do not load passengers if the elevator is STOPPED
    if(osMagicElv.state != STOPPED) {
      loadPassengers();
    }
  }
  return 0;
}

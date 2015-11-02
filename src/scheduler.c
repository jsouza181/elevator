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
  // ssleep(1);

  // Threaded function to handle elevator movement.
  while(!kthread_should_stop()) {
    //Elevator movement.
    if(osMagicElv.state != STOPPED)
      osMagicElv.state = osMagicElv.direction;

    ssleep(2);
    moveToFloor(osMagicElv.nextFloor);

    // Load/Unload
    if(osMagicElv.state != STOPPED)
      osMagicElv.state = LOADING;
    ssleep(1);
    // no locking needed because each does it on its own
    unloadPassengers();

    // Do not load passengers if the elevator is STOPPED
    if(osMagicElv.state != STOPPED)
      loadPassengers();

  } // while

  osMagicElv.state = STOPPED;
  return 0;
}

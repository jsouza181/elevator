#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "elevator.h"

// Threaded function to handle elevator movement.
int serviceRequests(void *data) {
  ssleep(1);

  // Do not change any states if elevator is STOPPED.
  while(!kthread_should_stop()) {
    // Elevator movement.
    if(osMagicElv.state != STOPPED)
      osMagicElv.state = osMagicElv.direction;

    ssleep(2);
    moveToFloor(osMagicElv.nextFloor);

    // Load/unload
    if(osMagicElv.state != STOPPED)
      osMagicElv.state = LOADING;

    ssleep(1);
    unloadPassengers();

    // Do not load passengers if the elevator is STOPPED
    if(osMagicElv.state != STOPPED) {
      loadPassengers();
    }
  }
  return 0;
}

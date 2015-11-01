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

  while(!kthread_should_stop()) {
      // Determine the next floor to visit
      osMagicElv.nextFloor = scheduleNextFloor();
      // Sleep for movement
      ssleep(2);
      moveToFloor(osMagicElv.nextFloor);

      // Sleep for load/unload
      ssleep(1);
      unloadPassengers();
      loadPassengers();

    /*
        elevatorStatus == STOPPED
        while(elevator is not empty)
          moveToFloor(next floor)
          unloadPassengers()
    */
  }
  return 0;

}

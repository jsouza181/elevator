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
    // Elevator Movement
    osMagicElv.state = osMagicElv.direction;
    ssleep(2);
    moveToFloor(osMagicElv.nextFloor);

    // Determine the next floor to visit
    osMagicElv.nextFloor = scheduleNextFloor();

    // Load/unload
    osMagicElv.state = LOADING;
    ssleep(1);
    unloadPassengers();
    loadPassengers();
  }

  osMagicElv.state = STOPPED;
  return 0;
}

    /*
        elevatorStatus == STOPPED
        while(elevator is not empty)
          moveToFloor(next floor)
          unloadPassengers()

          lastly, elevatorState == STOPPED
    */

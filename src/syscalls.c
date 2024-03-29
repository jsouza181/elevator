#include <syscalls.h>
#include <linux/kthread.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include "elevator.h"
#include "scheduler.h"

// Thread to move the elevator and handle loading/unloading passengers.
static struct task_struct *elevatorMover;

extern long (*STUB_start_elevator)(void);
long start_elevator(void) {
  /*
   * Do not attempt to start the elevator if it has been started.
   * Elevator movement can only begin if it has already been initialized and
   * is in the IDLE state, or if it has been STOPPED and is waiting to be started again.
   */
  printk("ELV STATE IS %d\n", osMagicElv.state);
  if(osMagicElv.state != IDLE && osMagicElv.state != STOPPED) {
    printk("ERROR: Attempting to start elevator that is already running.\n");
    return 1;
  }

  printk("Starting elevator\n");
  // Initialize elevator data
  elevatorInit();

  // Initialize thread for elevator movement.
  elevatorMover = kthread_run(serviceRequests, NULL, "Elevator mover thread");
	if (IS_ERR(elevatorMover)) {
		printk("ERROR! kthread_run, request servicer thread\n");
		return PTR_ERR(elevatorMover);
	}

  return 0;
}

extern long (*STUB_issue_request)(int,int,int);
long issue_request(int passenger_type, int start_floor, int destination_floor) {

  // Make sure the passenger type is valid
  if(passenger_type < ADULT || passenger_type > ROOM_SERVICE) {
    printk("Invalid request.\n");
    return 1;
  }

  // Producer makes requests from 1-10, not 0-9
  --start_floor;
  --destination_floor;

  if(start_floor < 0 || start_floor >= MAX_FLOOR) {
    printk("Invalid request.\n");
    return 1;
  }
  if(destination_floor < 0 || destination_floor >= MAX_FLOOR) {
    printk("Invalid request.\n");
    return 1;
  }

  if(start_floor == destination_floor) {
    printk("Invalid request.\n");
    return 1;
  }

  printk("New request: %d, %d => %d\n", passenger_type, start_floor, destination_floor);

  // Create a passenger, then add them to the start floor
  addToFloor(start_floor, createPassenger(passenger_type, start_floor, destination_floor));

  return 0;
}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void) {
  int ret;

  if(osMagicElv.state == STOPPED)
    return 1;

  printk("Stopping elevator\n");
  // Unload remaining passengers, then free dynamic data.
  elevatorRelease();

  // Kill thread
  ret = kthread_stop(elevatorMover);
  if(ret != -EINTR)
  	printk("Request servicer thread has stopped\n");

  return 0;
}

void elevator_syscalls_create(void) {
  STUB_start_elevator =& (start_elevator);
  STUB_issue_request =& (issue_request);
  STUB_stop_elevator =& (stop_elevator);
}

void elevator_syscalls_remove(void) {
  STUB_start_elevator = NULL;
  STUB_issue_request = NULL;
  STUB_stop_elevator = NULL;
}

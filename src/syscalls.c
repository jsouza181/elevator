#include <syscalls.h>
#include <linux/printk.h>
#include "elevator.h"

extern long (*STUB_start_elevator)(void);
long start_elevator(void) {
  printk("Starting elevator\n");

  //Initialize elevator and floors
  elevatorStart();

  return 0;
}

extern long (*STUB_issue_request)(int,int,int);
long issue_request(int passenger_type, int start_floor, int destination_floor) {

  printk("New request: %d, %d => %d\n", passenger_type, start_floor, destination_floor);

  // Create a passenger, then add them to the start floor
  //addToFloor(start_floor, createPassenger(passenger_type, destination_floor));
  addToFloor(2, createPassenger(passenger_type, destination_floor));

  // Send elevator to the new request if it is idle
/*
  if(osMagicElv.state == 0) {
    moveToFloor(start_floor);
    loadPassengers();
  }
*/

  return 0;
}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void) {
  printk("Stopping elevator\n");
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

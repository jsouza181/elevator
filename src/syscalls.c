#include <syscalls.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include "elevator.h"
#include "scheduler.h"

extern long (*STUB_start_elevator)(void);
long start_elevator(void) {
  printk("Starting elevator\n");

  //Initialize elevator and floors
  elevatorStart();

  return 0;
}

extern long (*STUB_issue_request)(int,int,int);
long issue_request(int passenger_type, int start_floor, int destination_floor) {

  // Make sure the passenger type is valid
  if(passenger_type < ADULT || passenger_type > ROOM_SERVICE) {
    printk("Invalid request.\n");
    return 1;
  }

  // Producer makes requests from 1-9, not 0-10
  --start_floor;
  --destination_floor;

  printk("New request: %d, %d => %d\n", passenger_type, start_floor, destination_floor);

  // Create a passenger, then add them to the start floor
  addToFloor(start_floor, createPassenger(passenger_type, destination_floor));

  // Add the passenger's destination to the list of scheduled requests
  scheduleRequest(destination_floor);

  return 0;
}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void) {
  struct list_head *temp, *ptr;
  PassengerNode *passenger;
  RequestNode *request;
  int i = 0;

  printk("Stopping elevator\n");

  printk("deallocating elevator\n");
  if(!list_empty(&osMagicElv.elvPassengers)) {
    list_for_each_safe(ptr, temp, &osMagicElv.elvPassengers) {
  		passenger = list_entry(ptr, PassengerNode, passengerList);
  		list_del(ptr);
  		kfree(passenger);
  	}
  }

  printk("deallocating request queue\n");
  if(!list_empty(&requestQueue)) {
    list_for_each_safe(ptr, temp, &requestQueue) {
      request = list_entry(ptr, RequestNode, requestList);
      list_del(ptr);
      kfree(request);
    }
  }

  for(i = 0; i < MAX_FLOOR; ++i) {
    printk("deallocating floors\n");
    if(list_empty(&osMagicFloors[i].floorPassengers))
      continue;

    list_for_each_safe(ptr, temp, &osMagicFloors[i].floorPassengers) {
  		passenger = list_entry(ptr, PassengerNode, passengerList);
  		list_del(ptr);
  		kfree(passenger);
  	}
  }

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

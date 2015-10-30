#include <linux/delay.h>
#include <linux/slab.h>
#include "elevator.h"

Elevator osMagicElv;
Floor osMagicFloors[];

// Initialize elevator and floors
void elevatorStart(void) {
  int i;

  // Initialize elevator
  osMagicElv.state = 0;
  osMagicElv.currentFloor = 0;
  osMagicElv.destFloor = 0;
  osMagicElv.totalPass = 0;
  osMagicElv.totalWeightWhole = 0;
  osMagicElv.totalWeightFrac = 0;
  osMagicElv.maxWeight = 8;
  osMagicElv.maxPass = 8;
  INIT_LIST_HEAD(&osMagicElv.elvPassengers);

  // Initialize floors
  for(i = 0; i < 10; i++) {
    osMagicFloors[i].totalWeightWhole = 0;
    osMagicFloors[i].totalWeightFrac = 0;
    osMagicFloors[i].totalPass = 0;
    osMagicFloors[i].totalServed = 0;
    INIT_LIST_HEAD(&osMagicFloors[i].floorPassengers);
  }
}

void elevatorStop(void) {
  osMagicElv.state = 4;
}

// Floor functions

void addToFloor(int floorNum, Passenger pgr) {
  PassengerNode *newPassengerNode;

  printk("TEST before allocating mem for node\n");
  // Add the passenger to the floor's list of waiting passengers
  newPassengerNode = kmalloc(sizeof(PassengerNode), __GFP_WAIT | __GFP_IO | __GFP_FS);

  if (newPassengerNode == NULL) {
    printk("ERROR: Failed to allocate passenger node\n");
    return;
  }

  newPassengerNode->passenger.destination = pgr.destination;
  newPassengerNode->passenger.weightWhole = pgr.weightWhole;
  newPassengerNode->passenger.weightFrac = pgr.weightFrac;
  newPassengerNode->passenger.size = pgr.size;

  printk("TEST before adding to floor's passengers");
  list_add_tail(&newPassengerNode->passengerList, &osMagicFloors[floorNum].floorPassengers);
  printk("TEST after adding to floor's passengers");

  osMagicFloors[floorNum].totalWeightWhole += newPassengerNode->passenger.weightWhole;
  osMagicFloors[floorNum].totalWeightFrac += newPassengerNode->passenger.weightFrac;
  osMagicFloors[floorNum].totalPass += newPassengerNode->passenger.size;
}

Passenger createPassenger(int passengerType, int destFloor) {
  Passenger newPassenger;

  newPassenger.destination = destFloor;

  switch(passengerType) {
    case 0:
      newPassenger.weightWhole = 1;
      newPassenger.weightFrac = 0;
      newPassenger.size = 1;
      break;
    case 1:
      newPassenger.weightWhole = 0;
      newPassenger.weightFrac = 5;
      newPassenger.size = 1;
      break;
    case 2:
      newPassenger.weightWhole = 2;
      newPassenger.weightFrac = 0;
      newPassenger.size = 2;
      break;
    case 3:
      newPassenger.weightWhole = 2;
      newPassenger.weightFrac = 0;
      newPassenger.size = 1;
      break;
  }

  return newPassenger;
}

// Elevator functions

void moveToFloor(int floorNum) {
  // Change status from IDLE to UP/Down
  osMagicElv.state = 1;
  ssleep(2);
  osMagicElv.currentFloor = floorNum;
}

// Load appropriate passengers from elevator's current floor until capacity
void loadPassengers(void) {
  PassengerNode *newPassengerNode;

  newPassengerNode = list_first_entry(&osMagicFloors[osMagicElv.currentFloor].floorPassengers,
	PassengerNode, passengerList);

  // Update floor data
  osMagicFloors[osMagicElv.currentFloor].totalServed++;
  osMagicFloors[osMagicElv.currentFloor].totalWeightWhole -= newPassengerNode->passenger.weightWhole;
  osMagicFloors[osMagicElv.currentFloor].totalWeightFrac -= newPassengerNode->passenger.weightFrac;
  osMagicFloors[osMagicElv.currentFloor].totalPass-= newPassengerNode->passenger.size;
  // Update elevator data
  osMagicElv.totalWeightWhole += newPassengerNode->passenger.weightWhole;
  osMagicElv.totalWeightFrac += newPassengerNode->passenger.weightFrac;
  osMagicElv.totalPass += newPassengerNode->passenger.size;

  list_move_tail(&newPassengerNode->passengerList, &osMagicElv.elvPassengers);
}

// Unload passengers whose destination is the current floor
void unloadPassengers(void) {
  PassengerNode *servicedPassenger;

  servicedPassenger = list_first_entry(&osMagicElv.elvPassengers, PassengerNode, passengerList);

  osMagicElv.totalWeightWhole -= servicedPassenger->passenger.weightWhole;
  osMagicElv.totalWeightFrac -= servicedPassenger->passenger.weightFrac;
  osMagicElv.totalPass -= servicedPassenger->passenger.size;

  list_del_init(&servicedPassenger->passengerList);
  kfree(servicedPassenger);
}

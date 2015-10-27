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
  osMagicElv.currentFloor = 1;
  osMagicElv.destFloor = 0;
  osMagicElv.passLoad = 0;
  osMagicElv.weightLoad = 0;
  osMagicElv.maxWeight = 8;
  osMagicElv.maxPass = 8;

  // Initialize floors
  for(i = 0; i < 10; i++) {
    osMagicFloors[i].totalWeight = 0;
    osMagicFloors[i].totalPass = 0;
    osMagicFloors[i].totalServed = 0;
  }
}

void elevatorStop(void) {
  osMagicElv.state = 4;
}

// Floor functions

void addToFloor(int floorNum, Passenger pgr) {
  PassengerNode *newPassenger;

  // Add the passenger to the floor's list of waiting passengers
  newPassenger = kmalloc(sizeof(PassengerNode), __GFP_WAIT | __GFP_IO | __GFP_FS);
  newPassenger->passenger = pgr;

  list_add_tail(&newPassenger->passengerList, &osMagicFloors[floorNum].floorPassengers);

  osMagicFloors[floorNum].totalWeight += pgr.weight;
  osMagicFloors[floorNum].totalPass += pgr.size;
}

Passenger createPassenger(int passengerType, int destFloor) {
  Passenger newPassenger;

  newPassenger.destination = destFloor;
  return newPassenger;

  switch(passengerType) {
    case 0:
      newPassenger.weight = 1;
      newPassenger.size = 1;
      break;
    case 1:
      newPassenger.weight = 0.5;
      newPassenger.size = 1;
      break;
    case 2:
      newPassenger.weight = 2;
      newPassenger.size = 2;
      break;
    case 3:
      newPassenger.weight = 2;
      newPassenger.size = 1;
      break;
  }
  
  return newPassenger;
}

// Elevator functions

void moveToFloor(int floorNum) {
  // Change status from IDLE to UP/Down
  osMagicElv.state = 1;
  ssleep(4); 
  osMagicElv.currentFloor = floorNum;
}

// Load appropriate passengers from elevator's current floor until capacity
void loadPassengers(void) {
  PassengerNode *newPassenger;

  newPassenger = list_first_entry(&osMagicFloors[osMagicElv.currentFloor].floorPassengers,
	PassengerNode, passengerList);

  // Update floor data
  osMagicFloors[osMagicElv.currentFloor].totalServed++;
  osMagicFloors[osMagicElv.currentFloor].totalWeight-= newPassenger->passenger.weight;
  osMagicFloors[osMagicElv.currentFloor].totalPass-= newPassenger->passenger.size;
  // Update elevator data
  osMagicElv.weightLoad += newPassenger->passenger.weight;
  osMagicElv.passLoad += newPassenger->passenger.size;

  list_move_tail(&newPassenger->passengerList, &osMagicElv.elvPassengers);
}

// Unload passengers whose destination is the current floor
void unloadPassengers(void) {
  PassengerNode *servicedPassenger;

  servicedPassenger = list_first_entry(&osMagicElv.elvPassengers, PassengerNode, passengerList);

  osMagicElv.weightLoad -= servicedPassenger->passenger.weight;
  osMagicElv.passLoad -= servicedPassenger->passenger.size;

  list_del_init(&servicedPassenger->passengerList);
  kfree(servicedPassenger);
}

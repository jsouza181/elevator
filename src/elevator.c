#include <linux/delay.h>
#include <linux/slab.h>
#include "elevator.h"

Elevator osMagicElv;
Floor osMagicFloors[];
struct list_head requestQueue;

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
  osMagicElv.maxWeight = MAX_WEIGHT;
  osMagicElv.maxPass = MAX_PASS;
  INIT_LIST_HEAD(&osMagicElv.elvPassengers);

  // Initialize floors
  for(i = 0; i < MAX_FLOOR; i++) {
    osMagicFloors[i].totalWeightWhole = 0;
    osMagicFloors[i].totalWeightFrac = 0;
    osMagicFloors[i].totalPass = 0;
    osMagicFloors[i].totalServed = 0;
    INIT_LIST_HEAD(&osMagicFloors[i].floorPassengers);
  }

  // Initialize queue of floors to visit
  INIT_LIST_HEAD(&requestQueue);

}

void elevatorStop(void) {
  osMagicElv.state = STOPPED;
}

/*
 * Utility functions
 */

// Convert our whole/frac to a whole.5 or whole.0
int findWeightWhole(int whole, int frac) {
  return whole + (frac / 10);
}
int findWeightFrac(int frac) {
  return frac % 10;
}

// Determine whether a given passenger type can fit on the elevator.
// Return 0 if it will not fit.
int willItFit(int passengerType) {
  int elvWhole = osMagicElv.totalWeightWhole;
  int elvFrac = osMagicElv.totalWeightFrac;
  int elvPass = osMagicElv.totalPass;

  // Check for weight overloads. These only occur at 6.5 and greater
  if(elvWhole >= MAX_WEIGHT) { // Already full
    return 0;
  }
  else if(elvWhole == MAX_WEIGHT - 1) {
    if(elvFrac == 5) { // 7.5
      if(passengerType != CHILD) // Only a child can fit
        return 0;
    }
    else { // 7.0
      if(passengerType >= BELLHOP) // Staff will not fit
        return 0;
    }
  }
  else if(elvWhole == MAX_WEIGHT - 2) {
    if(elvFrac == 5) { // 6.5
      if(passengerType >= BELLHOP) // Staff will not fit
        return 0;
    }
  }

  // Check for size differences.
  if(elvPass >= MAX_PASS) { // Already full
    return 0;
  }
  else if(elvPass == MAX_PASS - 1) {
    if(passengerType == BELLHOP)
      return 0;
  }

  return 1;
}

/*
 * Floor functions
 */

void addToFloor(int floorNum, Passenger pgr) {
  PassengerNode *newPassengerNode;

  printk("TEST before allocating mem for node\n");
  // Add the passenger to the floor's list of waiting passengers
  newPassengerNode = kmalloc(sizeof(PassengerNode), __GFP_WAIT | __GFP_IO | __GFP_FS);

  if (newPassengerNode == NULL) {
    printk("ERROR: Failed to allocate passenger node\n");
    return;
  }

  newPassengerNode->passenger.passengerType = pgr.passengerType;
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
    case ADULT:
      passengerType = ADULT;
      newPassenger.weightWhole = ADULT_WEIGHT_WHOLE;
      newPassenger.weightFrac = ADULT_WEIGHT_FRAC;
      newPassenger.size = ADULT_SIZE;
      break;
    case CHILD:
      passengerType = CHILD;
      newPassenger.weightWhole = CHILD_WEIGHT_WHOLE;
      newPassenger.weightFrac = CHILD_WEIGHT_FRAC;
      newPassenger.size = CHILD_SIZE;
      break;
    case BELLHOP:
      passengerType = BELLHOP;
      newPassenger.weightWhole = BELLHOP_WEIGHT_WHOLE;
      newPassenger.weightFrac = BELLHOP_WEIGHT_FRAC;
      newPassenger.size = BELLHOP_SIZE;
      break;
    case ROOM_SERVICE:
      passengerType = ROOM_SERVICE;
      newPassenger.weightWhole = ROOM_SERVICE_WEIGHT_WHOLE;
      newPassenger.weightFrac = ROOM_SERVICE_WEIGHT_FRAC;
      newPassenger.size = ROOM_SERVICE_SIZE;
      break;
  }

  return newPassenger;
}

/*
 * Elevator functions
 */

// Need a threaded function to continually check for when floors become populated
// Only runs when elevator is in IDLE state

// Threaded movement function. Ordered to stop when elevator is idle.
void moveToFloor(int floorNum) {
  if (osMagicElv.currentFloor < floorNum) {
    osMagicElv.state = DOWN;
  }
  else {
    osMagicElv.state = UP;
  }
  ssleep(2);
  osMagicElv.currentFloor = floorNum;
}

// Load appropriate passengers from elevator's current floor until capacity
void loadPassengers(void) {
  PassengerNode *newPassengerNode;
  newPassengerNode = list_first_entry(&osMagicFloors[osMagicElv.currentFloor].floorPassengers,
      PassengerNode, passengerList);

  // Loop while the next passenger will fit AND the list is not empty
  // start a mutex here
  while(willItFit(newPassengerNode->passenger.passengerType) &&
        !list_empty(&osMagicFloors[osMagicElv.currentFloor].floorPassengers)) {
    // Update floor data
    osMagicFloors[osMagicElv.currentFloor].totalServed++;
    osMagicFloors[osMagicElv.currentFloor].totalWeightWhole -=
        findWeightWhole(newPassengerNode->passenger.weightWhole, newPassengerNode->passenger.weightFrac);
    osMagicFloors[osMagicElv.currentFloor].totalWeightFrac -=
        findWeightFrac(newPassengerNode->passenger.weightFrac);
    osMagicFloors[osMagicElv.currentFloor].totalPass-= newPassengerNode->passenger.size;
    // Update elevator data
    osMagicElv.totalWeightWhole += findWeightWhole(newPassengerNode->passenger.weightWhole,
        newPassengerNode->passenger.weightFrac);
    osMagicElv.totalWeightFrac += findWeightFrac(newPassengerNode->passenger.weightFrac);
    osMagicElv.totalPass += newPassengerNode->passenger.size;

    list_move_tail(&newPassengerNode->passengerList, &osMagicElv.elvPassengers);
    newPassengerNode = list_first_entry(&osMagicFloors[osMagicElv.currentFloor].floorPassengers,
        PassengerNode, passengerList);
  } // while
  // end the mutex here
}

// Unload passengers whose destination is the current floor
void unloadPassengers(void) {
  PassengerNode *servicedPassenger;

  servicedPassenger = list_first_entry(&osMagicElv.elvPassengers, PassengerNode, passengerList);

  // check that the serviced Passenger is not NULL
  // start a mutex here
  osMagicElv.totalWeightWhole -= findWeightWhole(servicedPassenger->passenger.weightWhole,
      servicedPassenger->passenger.weightFrac);
  osMagicElv.totalWeightFrac -= findWeightFrac(servicedPassenger->passenger.weightFrac);
  osMagicElv.totalPass -= servicedPassenger->passenger.size;

  list_del_init(&servicedPassenger->passengerList);
  kfree(servicedPassenger);

  // end the mutex here
}

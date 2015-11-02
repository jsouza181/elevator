#include <linux/delay.h>
#include <linux/slab.h>
#include "elevator.h"

Elevator osMagicElv;
Floor osMagicFloors[];

/* Initialization and Deallocation functions */

// Initialize elevator and floors.
void elevatorInit(void) {
  int i;

  // Initialize elevator
  osMagicElv.state = IDLE;
  osMagicElv.direction = UP;
  osMagicElv.currentFloor = 0;
  osMagicElv.nextFloor = 1;
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

}

// Set the elevator to STOPPED, then free dynamic lists and unload remaining passengers.
void elevatorRelease(void) {
  struct list_head *temp, *ptr;
  PassengerNode *passenger;
  int i = 0;

  osMagicElv.state = STOPPED;

  if(!list_empty(&osMagicElv.elvPassengers)) {
    list_for_each_safe(ptr, temp, &osMagicElv.elvPassengers) {
  		passenger = list_entry(ptr, PassengerNode, passengerList);
  		list_del(ptr);
  		kfree(passenger);
  	}
  }

  for(i = 0; i < MAX_FLOOR; ++i) {
    if(list_empty(&osMagicFloors[i].floorPassengers))
      continue;

    list_for_each_safe(ptr, temp, &osMagicFloors[i].floorPassengers) {
  		passenger = list_entry(ptr, PassengerNode, passengerList);
  		list_del(ptr);
  		kfree(passenger);
  	}
  }
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

// Find the next floor to visit based on the elevator's current floor and its direction.
int scheduleNextFloor(void) {
  int nextFloor;

  if(osMagicElv.direction == UP) {
    if(osMagicElv.currentFloor + 1 < MAX_FLOOR)
      nextFloor = osMagicElv.currentFloor + 1;
    else {
      osMagicElv.direction = DOWN;
      nextFloor = osMagicElv.currentFloor - 1;
    }
  }
  else { // Elevator is going down
    if(osMagicElv.currentFloor - 1 >= MIN_FLOOR)
      nextFloor = osMagicElv.currentFloor - 1;
    else {
      osMagicElv.direction = UP;
      nextFloor = osMagicElv.currentFloor + 1;
    }
  }

  return nextFloor;
}

// Determine the direction that a newly added passenger is going.
int passengerDirection(int currentFloor, int nextFloor) {
  int direction;

  direction = currentFloor - nextFloor;

  if(direction > 0)
    return DOWN;
  else
    return UP;
}

/*
 * Floor functions
 */

void addToFloor(int floorNum, Passenger pgr) {
  PassengerNode *newPassengerNode;

  // Add the passenger to the floor's list of waiting passengers
  newPassengerNode = kmalloc(sizeof(PassengerNode), __GFP_WAIT | __GFP_IO | __GFP_FS);

  if (newPassengerNode == NULL) {
    printk("ERROR: Failed to allocate passenger node\n");
    return;
  }

  newPassengerNode->passenger.passengerType = pgr.passengerType;
  newPassengerNode->passenger.destination = pgr.destination;
  newPassengerNode->passenger.direction = pgr.direction;
  newPassengerNode->passenger.weightWhole = pgr.weightWhole;
  newPassengerNode->passenger.weightFrac = pgr.weightFrac;
  newPassengerNode->passenger.size = pgr.size;

  list_add_tail(&newPassengerNode->passengerList, &osMagicFloors[floorNum].floorPassengers);

  osMagicFloors[floorNum].totalWeightWhole += newPassengerNode->passenger.weightWhole;
  osMagicFloors[floorNum].totalWeightFrac += newPassengerNode->passenger.weightFrac;
  osMagicFloors[floorNum].totalPass += newPassengerNode->passenger.size;
}

Passenger createPassenger(int passengerType, int currentFloor, int nextFloor) {
  Passenger newPassenger;

  newPassenger.destination = nextFloor;
  newPassenger.direction = passengerDirection(currentFloor, nextFloor);

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

// Elevator movement function. Changes elevator's direction upon reaching 1 or 10.
void moveToFloor(int floorNum) {
  // Change the elevator's current floor, and find the next floor.
  osMagicElv.currentFloor = floorNum;
  osMagicElv.nextFloor = scheduleNextFloor();

  // Change direction if needed.
  if (osMagicElv.nextFloor < floorNum) {
    osMagicElv.state = DOWN;
  }
  else {
    osMagicElv.state = UP;
  }
}

// Load appropriate passengers from elevator's current floor until capacity
void loadPassengers(void) {
  PassengerNode *newPassengerNode;
  newPassengerNode = list_first_entry(&osMagicFloors[osMagicElv.currentFloor].floorPassengers,
      PassengerNode, passengerList);

  // Loop while the next passenger will fit AND the list is not empty
  // start a mutex here
  while(!list_empty(&osMagicFloors[osMagicElv.currentFloor].floorPassengers) &&
      willItFit(newPassengerNode->passenger.passengerType) &&
      osMagicElv.direction == newPassengerNode->passenger.direction) {

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
  struct list_head *temp, *ptr;

  if(list_empty(&osMagicElv.elvPassengers))
    return;

  // check that the serviced Passenger is not NULL
  // start a mutex here
  list_for_each_safe(ptr, temp, &osMagicElv.elvPassengers)
  {
     servicedPassenger = list_entry(ptr, PassengerNode, passengerList);

     if(osMagicElv.currentFloor == servicedPassenger->passenger.destination) {

     osMagicElv.totalWeightWhole -= findWeightWhole(servicedPassenger->passenger.weightWhole,
     servicedPassenger->passenger.weightFrac);
     osMagicElv.totalWeightFrac -= findWeightFrac(servicedPassenger->passenger.weightFrac);
     osMagicElv.totalPass -= servicedPassenger->passenger.size;

     list_del(ptr);
     kfree(servicedPassenger);
     // end the mutex here
     }
  }

}

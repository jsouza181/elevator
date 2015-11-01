#include <linux/delay.h>
#include <linux/slab.h>
#include "elevator.h"

Elevator osMagicElv;
Floor osMagicFloors[];

// Initialize elevator and floors
void elevatorStart(void) {
  int i;

  // Initialize elevator
  osMagicElv.state = IDLE;
  osMagicElv.direction = UP;
  osMagicElv.currentFloor = 0;
  osMagicElv.nextFloor = 0;
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

// Find the next floor to visit based on the elevator's current floor and its direction.
int scheduleNextFloor(void) {
  int nextFloor;

  if(osMagicElv.direction == UP) {
    if(osMagicElv.currentFloor + 1 < MAX_FLOOR)
      nextFloor = ++osMagicElv.currentFloor;
    else {
      osMagicElv.direction = DOWN;
      nextFloor = --osMagicElv.currentFloor;
    }
  }
  else { // Elevator is going down
    if(osMagicElv.currentFloor - 1 >= MIN_FLOOR)
      nextFloor = --osMagicElv.currentFloor;
    else {
      osMagicElv.direction = UP;
      nextFloor = ++osMagicElv.currentFloor;
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

  printk("TEST before allocating mem for node\n");
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

  printk("TEST before adding to floor's passengers");
  list_add_tail(&newPassengerNode->passengerList, &osMagicFloors[floorNum].floorPassengers);
  printk("TEST after adding to floor's passengers");

  osMagicFloors[floorNum].totalWeightWhole += newPassengerNode->passenger.weightWhole;
  osMagicFloors[floorNum].totalWeightFrac += newPassengerNode->passenger.weightFrac;
  osMagicFloors[floorNum].totalPass += newPassengerNode->passenger.size;
}

/*
      ADD 3RD ARGUMENT: CURRENT floor
      USE TO INITIALIZE PASSENGER'S DIRECTION VARIABLE
*/
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
  osMagicElv.currentFloor = floorNum;
}

// Load appropriate passengers from elevator's current floor until capacity
void loadPassengers(void) {
  PassengerNode *newPassengerNode;
  newPassengerNode = list_first_entry(&osMagicFloors[osMagicElv.currentFloor].floorPassengers,
      PassengerNode, passengerList);

  // Loop while the next passenger will fit AND the list is not empty
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
  }
}

// Unload passengers whose destination is the current floor
void unloadPassengers(void) {
  PassengerNode *servicedPassenger;
  struct list_head *temp, *ptr;

  if(list_empty(&osMagicElv.elvPassengers))
    return;

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
     }
  }

}

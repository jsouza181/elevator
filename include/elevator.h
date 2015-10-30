#ifndef __ELEVATOR_ELEVATOR_H
#define __ELEVATOR_ELEVATOR_H

#define IDLE 0
#define UP 1
#define DOWN 2
#define LOADING 3
#define STOPPED 4

typedef struct passenger {
  int destination;
  int weightWhole;
  int weightFrac;
  int size;
} Passenger;

typedef struct passengerNode {
  struct list_head passengerList;
  Passenger passenger;
} PassengerNode;

typedef struct floor {
  int totalWeightWhole;
  int totalWeightFrac;
  int totalPass;
  int totalServed;
  struct list_head floorPassengers;
} Floor;

typedef struct elevator {
  int state; //IDLE, UP, DOWN, LOADING, STOPPED
  int currentFloor;
  int destFloor;
  int totalWeightWhole;
  int totalWeightFrac;
  int totalPass;
  int maxWeight;
  int maxPass;
  struct list_head elvPassengers;
} Elevator;

extern Elevator osMagicElv;
extern Floor osMagicFloors[10];

// Floor functions
void addToFloor(int floorNum, Passenger pgr);
Passenger createPassenger(int passengerType, int destFloor);

// Elevator functions
void elevatorStart(void);
void elevatorStop(void);
void moveToFloor(int floorNum);
void loadPassengers(void);
void unloadPassengers(void);

#endif

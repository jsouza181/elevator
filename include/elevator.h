#ifndef __ELEVATOR_ELEVATOR_H
#define __ELEVATOR_ELEVATOR_H

/* elevator characteristics */
#define MIN_FLOOR 0
#define MAX_FLOOR 10
#define MAX_PASS 8
#define MAX_WEIGHT 8
/* elevator state definitions */
#define IDLE 0
#define UP 1
#define DOWN 2
#define LOADING 3
#define STOPPED 4
/* passenger type definitions */
#define ADULT 0
#define CHILD 1
#define BELLHOP 2
#define ROOM_SERVICE 3
/* weight definitions */
#define ADULT_WEIGHT_WHOLE 1
#define CHILD_WEIGHT_WHOLE 1
#define BELLHOP_WEIGHT_WHOLE 2
#define ROOM_SERVICE_WEIGHT_WHOLE 2
#define ADULT_WEIGHT_FRAC 0
#define CHILD_WEIGHT_FRAC 5
#define BELLHOP_WEIGHT_FRAC 0
#define ROOM_SERVICE_WEIGHT_FRAC 0
/* size definitions */
#define ADULT_SIZE 1
#define CHILD_SIZE 1
#define BELLHOP_SIZE 2
#define ROOM_SERVICE_SIZE 1

typedef struct passenger {
  int passengerType;
  int destination;
  int direction;
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
  int state; // IDLE, UP, DOWN, LOADING, STOPPED
  int direction; // Direction of movement
  int currentFloor;
  int nextFloor;
  int totalWeightWhole;
  int totalWeightFrac;
  int totalPass;
  int maxWeight;
  int maxPass;
  struct list_head elvPassengers;
} Elevator;

extern Elevator osMagicElv;
extern Floor osMagicFloors[MAX_FLOOR];
extern struct mutex floor_mutex;

// Utility functions
int findWeightWhole(int whole, int frac);
int findWeightFrac(int frac);
int scheduleNextFloor(void);
int passengerDirection(int currentFloor, int nextFloor);

// Floor functions
void addToFloor(int floorNum, Passenger pgr);
Passenger createPassenger(int passengerType, int currentFloor, int nextFloor);

// Elevator functions
void elevatorInit(void);
void elevatorRelease(void);
void moveToFloor(int floorNum);
void loadPassengers(void);
void unloadPassengers(void);

#endif

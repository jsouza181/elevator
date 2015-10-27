#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "elevator.h"

// Queue of floors that the elevator will visit in order
static struct list_head floorQueue;

typedef struct floorQueueItem {
  struct list_head list;
  int floorNum;
} FloorQueueItem;

// Add passenger's desired floor to the list of scheduled floors
void scheduleRequest(Passenger pgr) {
  FloorQueueItem *newScheduledFloor;

  // Add the floor the new request is on to the queue of floors to visit.
  newScheduledFloor = kmalloc(sizeof(FloorQueueItem), __GFP_WAIT | __GFP_IO | __GFP_FS);
  newScheduledFloor->floorNum = pgr.destination;
  list_add_tail(&newScheduledFloor->list, &floorQueue);
}

void serviceFloors(void) {
  printk("Scheduled floor queue status:  %d\n", list_empty(&floorQueue));
/*
  FloorQueueItem *nextFloorToVisit;

  while(osMagicElv.state != 4) { // While the elevator has not been STOPPED
    printk("list empty is %d\n", list_empty(&floorQueue));

    if(!list_empty(&floorQueue)) {
      // Check the head of the queue? (valid direction?)
      nextFloorToVisit = list_first_entry(&floorQueue, FloorQueueItem, list);

      // Move elevator to this request's current floor then service the floor
      moveToFloor(nextFloorToVisit->floorNum);
      unloadPassengers();
      loadPassengers();

      // Delete the head 
      list_del_init(&nextFloorToVisit->list);
      kfree(nextFloorToVisit);
    }
    else // No requests left, go IDLE
      osMagicElv.state = 0;
  }
  */
}

#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "elevator.h"

// Queue of floors that the elevator will visit in order
static struct list_head requestQueue;

typedef struct requestQueueItem {
  struct list_head list;
  int floorNum;
} RequestQueueItem;

// Add passenger's desired floor to the list of scheduled floors
void scheduleRequest(int requestFloor) {
  RequestQueueItem *newRequest;

  // Add the new request floor to the queue of floors to visit.
  newRequest = kmalloc(sizeof(RequestQueueItem), __GFP_WAIT | __GFP_IO | __GFP_FS);
  newRequest->floorNum = requestFloor;
  list_add_tail(&newRequest->list, &requestQueue);
}

int serviceRequests(void *data) {
  RequestQueueItem *nextFloorToVisit;

  while(!kthread_should_stop()) {

    if(list_empty(&requestQueue))
      printk("Checking List: list is empty\n");
    else
      printk("Checking List: list is not empty\n");

    if(!list_empty(&requestQueue)) {
      // Check the head of the queue? (valid direction?)
      nextFloorToVisit = list_first_entry(&requestQueue, RequestQueueItem, list);

      // Move elevator to this request's current floor then service the floor
      moveToFloor(nextFloorToVisit->floorNum);
      unloadPassengers();
      loadPassengers();

      // Delete the head
      list_del_init(&nextFloorToVisit->list);
      kfree(nextFloorToVisit);
    }
    else // No requests left, go IDLE
      osMagicElv.state = IDLE;
  }
  return 0;
}

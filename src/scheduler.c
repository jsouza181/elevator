#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "elevator.h"


// Add passenger's desired floor to the list of scheduled floors
void scheduleRequest(int requestFloor) {
  RequestNode *newRequest;

  // Add the new request floor to the queue of floors to visit.
  newRequest = kmalloc(sizeof(RequestNode), __GFP_WAIT | __GFP_IO | __GFP_FS);
  newRequest->floorNum = requestFloor;
  printk("Memory for newRequest successfully allocated\n");
  list_add_tail(&newRequest->requestList, &requestQueue);
  printk("newRequest added to request queue\n");
}

int serviceRequests(void *data) {
  //RequestQueueItem *nextFloorToVisit;

  while(!kthread_should_stop()) {
    if(list_empty(&requestQueue))
      printk("Checking List: list is empty\n");
    else
      printk("Checking List: list is not empty\n");

    ssleep(8);
  }

  /*
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
  */
  return 0;
}

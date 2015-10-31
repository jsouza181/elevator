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

    /*
    ssleep(4);
    if(list_empty(&requestQueue))
      printk("Checking List: list is empty\n");
    else
      printk("Checking List: list is not empty\n");
      */
int serviceRequests(void *data) {
  RequestNode *nextFloorToVisit;

  while(!kthread_should_stop()) {
    if(!list_empty(&requestQueue)) { // If there is a request to service
      nextFloorToVisit = list_first_entry(&requestQueue, RequestNode, requestList);

      // Delete the request
      // list_del_init(&nextFloorToVisit->requestList);
      // kfree(nextFloorToVisit);
    }
    printk("IM STILL ALIVE!!!\n");
    ssleep(2);
  }
  return 0;
}
    /*

      printk("freeing node\n");
    }
    else // No requests left, go IDLE
      osMagicElv.state = IDLE;
  }
    return 0;
}
*/
/*
      // Move elevator to this request's current floor then service the floor
      moveToFloor(nextFloorToVisit->floorNum);
      //unloadPassengers();
      //loadPassengers();
*/

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
  RequestNode *nextFloorToVisit;
  ssleep(1);

  while(!kthread_should_stop()) {
    if(!list_empty(&requestQueue)) { // If there is a request to service
      nextFloorToVisit = list_first_entry(&requestQueue, RequestNode, requestList);

      // Delete the request
      list_del_init(&nextFloorToVisit->requestList);
      kfree(nextFloorToVisit);
    }
    printk("IM STILL ALIVE!!!\n");
    ssleep(2);
  }
  return 0;
}

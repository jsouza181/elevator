MODULE_NAME = elevator
PWD := $(shell pwd)

#KDIR := /lib/modules/$(shell uname -r)/build
KDIR := /lib/modules/4.2.0/build
ccflags-y += -I$(src)/include

obj-y := src/sys_start_elevator.o
obj-y += src/sys_issue_request.o
obj-y += src/sys_stop_elevator.o

$(MODULE_NAME)-objs += src/syscalls.o
$(MODULE_NAME)-objs += src/elevatorproc.o
$(MODULE_NAME)-objs += src/elevator.o
$(MODULE_NAME)-objs += src/module.o
$(MODULE_NAME)-objs += src/scheduler.o
obj-m := $(MODULE_NAME).o

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

test: teststart testadd teststop

teststart: insert start
insert:
	sudo insmod elevator.ko
start:
	drivers/consumer.x --start

testadd:
	drivers/producer.x

testadd10:
	for i in `seq 1 10`; do drivers/producer.x; sleep 1; done;

teststop: stop remove
stop:
	drivers/consumer.x --stop
remove:
	sudo rmmod elevator

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

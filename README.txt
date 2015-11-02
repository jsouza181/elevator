# SHELL #

### Members ###
Jason Souza<br>
Ricardo Castilla<br>
Philip Scott<br>
Conor Stephen<br>

### Purpose ###
* Learn how to create a kernel module
* Learn how to compile a custom kernel
* Version: 1.0

### Tar file contents ###
elevator/

    -> README.txt

    -> README.md

    -> report.txt

    -> src/

        -> elevator.c

        -> elevatorproc.c

        -> module.c

        -> scheduler.c

        -> sys_issue_request.c

        -> sys_start_elevator.c

        -> sys_stop_elevator.c

    -> include/

        -> elevator.h

        -> elevatorproc.h

        -> scheduler.h

        -> syscall.h

    -> Makefile

    -> proc_view.sh

    -> part1/
        -> Makefile

    -> part2/

        -> Makefile

        -> my_xtime.c

### Compiling ###
* Use provided makefile to compile the source code
```
$> make
```

* In order to clean the environment, run
```
$> make clean
```

### Using the elevator ###
* In order to start the elevator, run
```
$> make teststart
```
* To create a request
```
$> make testadd
```
* To create 10 requests run
```
$> make testadd10
```
* To automate the start, add, and stop of one request
```
$> make test
```
* To stop the elevator module
```
$> make teststop
```

### Known Bugs ###

* Fractoinal numbers are not counted correctly sometimes, which causes
    the elevator to carry more weight than allowed
* Module hangs (dealocks?) when the elevator has people inside and is stopped.
    Fixed by doing sudo rmmod elevator

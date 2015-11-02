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

## Directory Structure ##
Because git does not commit empty directories, it might be necessary to manually create the necessary directories.

Directory structure should be:

shell/
    -> src/ : source code
    -> lib/ : header files
    -> output/ : generated output (testing output, executables, etc.)
    -> obj/ : generated object files (\*.o)

### Features ###

* Unlimited amount of pipes (unless one of the below buggy commands is used)
* Multiple redirection within one command
* Use any environment variable, it is supported

### Known Bugs ###

* Piping creates zombies processes. These are dealt with on the next command, or at exit
    Formatting is also a little off sometimes
* There is a great amount of memory leaks caused by getArg()
    I have no idea how to fix them besides redesigning the function. 80% of memory leaks are caused by this function
    This is not a bug. It is a feature
*  Can handle extra spaces, but won't parse apart unspaced commands

============================
	Problem statement
============================
	Design and implement a basic shell interface that supports input/output redirection,
	pipes, background processing, and a series of built in functions as specified below. The
	shell should be robust (e.g. it should not crash under any circumstance beyond machine
	failure). The required features should adhere to the operational semantics of the bash shell


===================================
	Steps taken to solve problem
===================================

1.	Looked at bash to understand how the shell we create should behave

2. 	Worked on function to show a prompt line to the user

3. 	Created function to get user input

4. 	Worked on parsing commands

5. 	Looked into how to create forks and handle spawning processes

6. 	Followed guidelines for implementing specific functions that were assigned (echo, cd, etimes, etc.)

7. 	Implemented redirection

8. 	Added functionality of supporting all other commands

============================
		Assumptions
============================
•	Redirection and piping would not be mixed within a single command
•	No more than three pipes per command
•	Limits and etimes will not be called in the background.

============================
		System calls
============================

unistd.h - fork(), execv(), access(), pipe(), dup2(), gethostname(), getcwd(), getenv(), setenv()
wait.h - waitpid()

============================
		Problems
============================
- cd not in a fork (We don't think it needs to be, since PWD is process-specific)
- getting the limits file after the child was executed
	- fixed by creating a temporary file
	- considered putting parent to sleep to force a context switch, but that seemed unreliable
- input redirection through us for a loop, because echo does not accept it
	- precious time was lost in this
- Linprog keeps getting overwhelmed and running extremely slowly. Sometimes confusing us as to whether our program broke or Linprog.

============================
Known Bugs
============================
- piping not working
- etimes, and limits does not run in the background
- path resolution does not work with absolute paths
- getArg() has memory leaks, accounting for more than 90% of all our memory leaks
- can handle extra spaces, but won't parse apart unspaced commands

============================
	Division of Labor
============================
Rebecca Powell:
	-input-parsing
	-process forking
	-limits, etimes

Ricardo Castilla
	-path-searching
	-input/output redirection
	-zombie termination

============================
	Slack Days Used
============================
Rebecca Powell used 1 slack day
Ricardo Castilla used 1 slack day

============================
	Log
============================
● Implementations:
	•  9/28/15 - limits,input/output redirection
	•  9/27/15 - etimes
	•  9/23/15 - cd,forking,parsing
	•  9/17/15 - getArg(), getPosition()
	•  9/14/15 - showPrompt()

● - project completed
	●  11/25/05

	●  - speed up: path-searching improved.

	●  11/24/05

	●  - completed: input redirection
	●  11/4/05

	●  - design: some areas unclear.

	●  - started implementation. ●

	●  11/3/05

	●  - researched existing shells.

	●  - started designing. ●

	●  

	●  - project assigned.

============================
Questions
============================
	●  1. Is it safe to try to open a file that does not exist?
	●  It can be unsafe if the program does not pay attention to the return

	●  value from the call to open(). If the program assumes that it has a

	●  pointer to a file that, and that file does not exist, it is possible

	●  that the program will crash before long, most likely with a

	●  segmentation fault.
	============================
	Additional Features
	============================

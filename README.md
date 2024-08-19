# Systems Programming Project: Threads and Simulations
### Authors:
Marco Toito, Kensley Schonauer

### Description of Program:
This program is a simulation where a ghost travels around a leaving evidence of what type of ghost it is. The four hunters will travel around each room each trying to detect a different ghost reading left in the room. All of the hunters and ghosts are making decisions and moving simultaneously using threads. The hunters can pass on the information it has gathered to other hunters if they are both in the same room. Once a hunter has collected three different forms of evidence of the ghost, either by themselves or being given information from another hunter, they win. If the ghost scares all the hunters out of the house or the hunters get too bored and leave then the ghost wins. The ghost can also get too bored and leave if they do not run into a hunter for too long. Once all the threads are complete a winner will be displayed, if the hunters win then the guessed ghost type based on the hunter evidence will be displayed.

### List of Files:
1. building.c
2. defs.h
3. Evidence.c
4. ghost.c
5. hunter.c
6. main.c
7. Makefile
8. room.c
	
### Compilation Command:
`make fp`
	
### Lauching intruction:
`./fp` \
No command line arguments required

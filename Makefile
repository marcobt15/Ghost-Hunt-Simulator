
fp: main.o room.o ghost.o hunter.o building.o Evidence.o
	gcc -g main.o room.o ghost.o hunter.o building.o Evidence.o -o fp
	
main.o: main.c defs.h
	gcc -g -c main.c defs.h
	
room.o: room.c defs.h
	gcc -g -c room.c defs.h
	
ghost.o: ghost.c defs.h
	gcc -g -c ghost.c defs.h
	
hunter.o: hunter.c defs.h
	gcc -g -c hunter.c defs.h
	
building.o: building.c defs.h
	gcc -g -c building.c defs.h
	
Evidence.o: Evidence.c defs.h
	gcc -g -c Evidence.c defs.h
	
clean:
	rm -f *.o fp

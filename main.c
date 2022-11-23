#include "defs.h"

int main(int argc, char *argv[])
{
    // Initialize a random seed for the random number generators
    srand(time(NULL));
    // kfewijfs[ofjs[e9fes]]
    // You may change this code; this is for demonstration purposes
    BuildingType building;
    initBuilding(&building);
    populateRooms(&building);

    //Initializing all the hunters
    char name[MAX_STR];
    for (int i = 0; i < MAX_HUNTERS; i++){
    	printf("Enter the name of the hunter: ");
	fgets(name, MAX_STR, stdin);

	HunterType* newHunter = malloc(sizeof(HunterType));
	EvidenceClassType evidence = i+1;

	//testing
	//RoomNodeType* head = building.rooms.head->room->rooms->head;
	//if (head == NULL) printf("NULL\n");
	//printf("%s", head->room->name);

	initHunter(&newHunter, building.rooms.head, evidence, name);
	addHunter(&building.hunters, newHunter);
    }

    for (int i = 0; i < MAX_HUNTERS; i++){
    	printf("%s", building.hunters.hunters[i]->name);
    }



    //Initializing the ghost
    GhostType ghost;

    //getting ghosttype
    int ghostClassNumber = randInt(1, 4);
    GhostClassType ghostClass = ghostClassNumber;

    //getting random room
    int roomNumber = randInt(1, 12); //13 is the amount of rooms given so move 12 times to get there
    RoomNodeType* currRoom = building.rooms.head;
    for (int i = 0; i < roomNumber; i++){
    	currRoom = currRoom->next;
    }
    ghost.room = currRoom->room; //ghost has its room

    currRoom->room->ghost = &ghost; //room has the ghost

    building.ghost = &ghost; //building has the ghost
<<<<<<< HEAD
   
   // ioSHQIUSBUNSQ
   
   
=======




>>>>>>> 949b59d4d2dc996806b248cea0608aa8ecb1b2a2
    cleanupBuilding(&building);
    return 0;
}


/*
  Function:  randInt
  Purpose:   returns a pseudo randomly generated number,
             in the range min to (max - 1), inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [min, max-1)
*/
int randInt(int min, int max)
{
    return rand() % (max - min) + min;
}

/*
  Function:  randFloat
  Purpose:   returns a pseudo randomly generated number,
             in the range min to max, inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [0, max-1)
*/
float randFloat(float a, float b) {
    // Get a percentage between rand() and the maximum
    float random = ((float) rand()) / (float) RAND_MAX;
    // Scale it to the range we want, and shift it
    return random * (b - a) + a;
}
